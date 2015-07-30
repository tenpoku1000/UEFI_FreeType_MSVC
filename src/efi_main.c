
#if !defined(_DEBUG)

#include<efi.h>
#include<efilib.h>

#include <string.h>
#include <stdbool.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define SCREEN_WIDTH 1024

static void draw_text(
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop, FT_Face face, UINTN x, UINTN y, CHAR16* text
);

static void init(
	EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable,
	EFI_GRAPHICS_OUTPUT_PROTOCOL** gop,
	FT_Library* library,
	FT_Byte** buffer, FT_Face* face
);

static void load_file(CHAR16* path, UINTN* buffer_size, FT_Byte** buffer);

static EFI_GRAPHICS_OUTPUT_BLT_PIXEL* conv_bitmap(
	unsigned char* buffer, UINTN width, int pitch, UINTN height
);

static void reset_system(EFI_STATUS status)
{
	EFI_STATUS local_status = EFI_SUCCESS;

	do{
		EFI_INPUT_KEY key;
		local_status = ST->ConIn->ReadKeyStroke(ST->ConIn, &key);
	} while (EFI_SUCCESS != local_status);

	RT->ResetSystem(EfiResetCold, status, 0, NULL);
}

static void error_print(CHAR16* msg)
{
	Print(msg);
	reset_system(EFI_SUCCESS);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;
	FT_Library library = NULL;
	FT_Byte* buffer = NULL;
	FT_Face face = NULL;

	init(ImageHandle, SystemTable, &gop, &library, &buffer, &face);

	if (FT_Set_Pixel_Sizes(face, 32, 64)){

		error_print(L"FT_Set_Pixel_Sizes Failed.\n");
	}

	Print(L"When you press any key, the system will reboot.\n");

	CHAR16* text[] = {
		L"Kanji",
		L"漢字",
		L"中国文字",
		L"中國文字",
		L"한자",
		NULL,
	};

	UINTN x = 0;
	UINTN y = 64;

	for (int i = 0; text[i]; ++i){

		draw_text(gop, face, x, y, text[i]);

		y += 64;
	}

	if (face){

		FT_Done_Face(face);
		face = NULL;
	}

	if (library){

		FT_Done_FreeType(library);
		library = NULL;
	}

	free(buffer);
	buffer = NULL;

	reset_system(EFI_SUCCESS);

	return EFI_SUCCESS;
}

static void draw_text(
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop, FT_Face face, UINTN x, UINTN y, CHAR16* text)
{
	int prev_glyph_index = 0;
	bool has_kerning = FT_HAS_KERNING(face);

	for (size_t i = 0; *text; ++i, ++text){

		FT_UInt glyph_index = FT_Get_Char_Index(face, *text);

		if (0 == glyph_index){

			Print(L"Undefined character code.\n");
			continue;
		}

		if ((0x00 <= *text) && (0xff >= *text)){

			(void)FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_BITMAP);
		}else{

			(void)FT_Load_Glyph(
				face, glyph_index,
				FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT
			);
		}

		if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD)){

			Print(L"FT_Render_Glyph Failed.\n");
			continue;
		}

		FT_Bitmap* bitmap = &(face->glyph->bitmap);

		if ((0 == bitmap->width) || (0 == bitmap->rows)){

			continue;
		}

		if ((0 != prev_glyph_index) && has_kerning){

			FT_Vector delta;

			FT_Get_Kerning(face,
				prev_glyph_index, glyph_index,
				ft_kerning_default, &delta
				);
			x += (delta.x >> 6);
		}

		prev_glyph_index = glyph_index;

		UINTN width = bitmap->width;
		UINTN height = bitmap->rows;

		EFI_GRAPHICS_OUTPUT_BLT_PIXEL* p = conv_bitmap(
			bitmap->buffer, width, bitmap->pitch, height
		);

		if (NULL == p){

			error_print(L"conv_bitmap Failed.\n");
		}

		int baseline = (face->height + face->descender) *
			face->size->metrics.y_ppem / face->units_per_EM;

		EFI_STATUS status = gop->Blt(
			gop, p,
			EfiBltBufferToVideo,
			0, 0,
			x + face->glyph->bitmap_left,
			y + baseline - face->glyph->bitmap_top,
			width, height,
			width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
		);

		x += (width + 8);

		free(p);
		p = NULL;

		if (EFI_ERROR(status)){

			error_print(L"Blt Failed.\n");
		}
	}
}

static void init(
	EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable,
	EFI_GRAPHICS_OUTPUT_PROTOCOL** gop,
	FT_Library* library,
	FT_Byte** buffer, FT_Face* face)
{
	InitializeLib(ImageHandle, SystemTable);

	EFI_STATUS status = BS->LocateProtocol(
		&GraphicsOutputProtocol,
		NULL, gop
	);

	if (EFI_ERROR(status)){

		error_print(L"LocateProtocol GOP Failed.\n");
	}

	int mode_num = 0;
	int set_mode_num = 0;
	bool valid_query_mode = false;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* mode_info;
	UINTN size = 0;
	UINT32 HorizontalResolution = 0;

	for (; (status = (*gop)->QueryMode(*gop, mode_num, &size, &mode_info)) == EFI_SUCCESS;
		++mode_num){

		if (PixelBlueGreenRedReserved8BitPerColor != mode_info->PixelFormat){

			continue;
		}

		if (HorizontalResolution < mode_info->HorizontalResolution){

			HorizontalResolution = mode_info->HorizontalResolution;

			set_mode_num = mode_num;

			valid_query_mode = true;

			if (HorizontalResolution == SCREEN_WIDTH){

				break;
			}
		}
	}

	if (false == valid_query_mode){

		error_print(L"QueryMode GOP Failed.\n");
	}

	status = (*gop)->SetMode(*gop, set_mode_num);

	if (EFI_ERROR(status)){

		error_print(L"SetMode GOP Failed.\n");
	}

	if (FT_Init_FreeType(library)){

		error_print(L"FT_Init_FreeType Failed.\n");
	}

	CHAR16* path = L"\\efi\\boot\\fonts\\SourceHanSans-Normal.ttc";
#if 1
	FT_Long buffer_size = 0;

	load_file(path, (UINTN*)&buffer_size, buffer);

	FT_Error err = FT_New_Memory_Face(*library, (const FT_Byte*)*buffer, buffer_size, 0, face);
#else
	FT_Error err = FT_New_Face(library, path, 0, face);

	Print(L"FT_New_Face.\n");
#endif
	if (err){

		if (FT_Err_Unknown_File_Format == err){

			error_print(L"BAD FONT FILE.\n");
		}else{

			error_print(L"FT_New_Memory_Face or FT_New_Face Failed.\n");
		}
	}
}

static void load_file(CHAR16* path, UINTN* buffer_size, FT_Byte** buffer)
{
	EFI_FILE_IO_INTERFACE* efi_simple_file_system = NULL;
	EFI_FILE* efi_file_root = NULL;
	EFI_FILE* efi_file = NULL;

	EFI_STATUS status = BS->LocateProtocol(
		&FileSystemProtocol,
		NULL,
		&efi_simple_file_system
	);

	if (EFI_ERROR(status)){

		error_print(L"LocateProtocol FileSystemProtocol Failed.\n");
	}

	status = efi_simple_file_system->OpenVolume(
		efi_simple_file_system, &efi_file_root
	);

	if (EFI_ERROR(status)){

		error_print(L"OpenVolume Failed.\n");
	}

	status = efi_file_root->Open(
		efi_file_root, &efi_file, path,
		EFI_FILE_MODE_READ, EFI_FILE_ARCHIVE
		);

	if (EFI_ERROR(status)){

		error_print(L"Open Failed.\n");
	}

	status = efi_file->SetPosition(efi_file, 0xFFFFFFFFFFFFFFFF);

	if (EFI_ERROR(status)){

		error_print(L"SetPosition END Failed.\n");
	}

	UINT64 pos = 0;

	status = efi_file->GetPosition(efi_file, &pos);

	if (EFI_ERROR(status)){

		error_print(L"GetPosition Failed.\n");
	}

	*buffer_size = pos;

	status = efi_file->SetPosition(efi_file, 0);

	if (EFI_ERROR(status)){

		error_print(L"SetPosition SET Failed.\n");
	}

	FT_Byte* p = (FT_Byte*)malloc(*buffer_size);

	if (NULL == p){

		error_print(L"malloc Failed.\n");
	}

	*buffer = p;

	status = efi_file->Read(efi_file, buffer_size, *buffer);

	if (EFI_ERROR(status)){

		error_print(L"Read Failed.\n");
	}

	status = efi_file->Close(efi_file);

	if (EFI_ERROR(status)){

		error_print(L"Close Failed.\n");
	}
}

static EFI_GRAPHICS_OUTPUT_BLT_PIXEL* conv_bitmap(unsigned char* buffer, UINTN width, int pitch, UINTN height)
{
	size_t num = width * height;

	EFI_GRAPHICS_OUTPUT_BLT_PIXEL* p = calloc(num, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

	if (NULL == p){

		return NULL;
	}

	int k = 0;

	for (int j = 0; height > j; ++j){

		unsigned char* q = NULL;

		if (pitch > 0){

			q = buffer + (pitch * j);
		}else{

			q = buffer + (((-pitch) * height) - (pitch * j));
		}

		for (int i = 0; width > i; ++i){

			p[k].Red = q[i];
			p[k].Green = q[i + 1];
			p[k].Blue = q[i + 2];
//			p[k].Reserved;
			++k;
		}
	}

	return p;
}

#else

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <ft2build.h>
#include <ftbitmap.h>
#include FT_FREETYPE_H

#include "win64env.h"

static void get_char(void)
{
	char buf[4] = { 0 };
	size_t len = 3;
	(void)read_consle(buf, &len);
}

static void error_print(char* msg)
{
	size_t len = strlen(msg);
	(void)write_consle(msg, &len);

	get_char();

	(void)free_console();
	exit_process(EXIT_FAILURE);
}

static void Print(char* msg)
{
	size_t len = strlen(msg);
	(void)write_consle(msg, &len);
}

static void draw_text(FT_Face face);

int __stdcall WinMain(void)
{
	(void)alloc_console();

	FT_Library library = NULL;

	if (FT_Init_FreeType(&library)){

		error_print("FT_Init_FreeType Failed.\n");
	}

	const char* path = "SourceHanSans-Normal.ttc";

#if 0
	FT_Error err = FT_New_Face(library, path, 0, &face);
#else
	FILE* fp = fopen(path, "rb");
	long l = 0;
	char* buf = NULL;
	if (fp){

		fseek(fp, 0, SEEK_END);
		l = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buf = (char*)malloc(l);
		if (buf){

			fread(buf, 1, l, fp);

			fclose(fp);
		}
	}
	FT_Face face = NULL;
	FT_Error err = FT_New_Memory_Face(library, (const FT_Byte*)buf, (FT_Long)l, 0, &face);
#endif

	if (err){

		if (FT_Err_Unknown_File_Format == err){

			error_print("BAD FONT FILE.\n");
		}
		else{

			error_print("FT_New_Memory_Face or FT_New_Face Failed.\n");
		}
	}

	if (FT_Set_Pixel_Sizes(face, 32, 64)){

		error_print("FT_Set_Pixel_Sizes Failed.\n");
	}

	draw_text(face);

	if (!init_instance(L"font_test")){

		error_print("init_instance Failed.\n");
	}

	int ret = message_loop();

	if (face){

		FT_Done_Face(face);
		face = NULL;
	}

	if (library){

		FT_Done_FreeType(library);
		library = NULL;
	}

	get_char();
	(void)free_console();

	return ret;
}

static void draw_text(FT_Face face)
{
	unsigned short* text = L"EFI,ひらがな,カタカナ,漢字";
	int x = 0;
	int y = 64;

	int prev_glyph_index = 0;
	bool has_kerning = FT_HAS_KERNING(face);

	for (size_t i = 0; *text; ++i, ++text){

		FT_UInt glyph_index = FT_Get_Char_Index(face, *text);

		if (0 == glyph_index){

			Print("Undefined character code.\n");
			continue;
		}

		if ((0x00 <= *text) && (0xff >= *text)){

			(void)FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_BITMAP);
		}else{

			(void)FT_Load_Glyph(
				face, glyph_index,
				FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT
			);
		}

		if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_LCD)){

			Print("FT_Render_Glyph Failed.\n");
			continue;
		}

		FT_Bitmap* bitmap = &(face->glyph->bitmap);

		if ((0 == bitmap->width) || (0 == bitmap->rows)){

			if (0 == bitmap->width) Print("bitmap->width = 0\n");
			if (0 == bitmap->rows) Print("bitmap->rows = 0\n");

			continue;
		}

		char tmp[200];
		sprintf(tmp, "width = %d, rows = %d\n", bitmap->width, bitmap->rows);
		Print(tmp);

		if ((0 != prev_glyph_index) && has_kerning){

			FT_Vector delta;

			FT_Get_Kerning(face,
				prev_glyph_index, glyph_index,
				ft_kerning_default, &delta
			);
			x += (delta.x >> 6);
		}

		prev_glyph_index = glyph_index;

		int baseline = (face->height + face->descender) *
			face->size->metrics.y_ppem / face->units_per_EM;

		size_t num = bitmap->pitch * bitmap->rows;

		append_data(
			x + face->glyph->bitmap_left,
			y + baseline - face->glyph->bitmap_top,
			bitmap->width, bitmap->rows, num, bitmap->pitch, bitmap->buffer
		);

		x += (bitmap->width + 8);
	}
}

#endif
