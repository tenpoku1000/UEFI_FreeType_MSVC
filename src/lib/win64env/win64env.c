
// Copyright 2015 Shin'ichi Ichikawa. Released under the MIT license.

#include <windows.h>
#include "win64env.h"

static void get_last_error(void)
{
    LPVOID lpMsgBuf = NULL;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
    );

    MessageBox(NULL, (LPCTSTR)lpMsgBuf, L"Error", MB_OK | MB_ICONINFORMATION);

    LocalFree(lpMsgBuf);
    lpMsgBuf = NULL;
}

void* create_file(
    wchar_t* file_name, unsigned long long int flags,
    unsigned long long int efi_file_mode_read,
    unsigned long long int efi_file_mode_write,
    unsigned long long int efi_file_mode_create)
{
    DWORD dwDesiredAccess = 0;
    DWORD dwCreationDisposition = OPEN_EXISTING;

    if (efi_file_mode_read & flags) dwDesiredAccess |= GENERIC_READ;
    if (efi_file_mode_write & flags) dwDesiredAccess |= GENERIC_WRITE;
    if (efi_file_mode_create & flags) dwCreationDisposition = CREATE_NEW;

    HANDLE* handle = CreateFile(
        file_name,
        dwDesiredAccess, 0,
        NULL, dwCreationDisposition,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (INVALID_HANDLE_VALUE == handle){

#if 0
        get_last_error();
#endif

        return NULL;
    }

    return (void*)handle;
}

int read_file(void* hFile, unsigned char* buf, size_t* len)
{
    return (int)ReadFile(hFile, buf, (DWORD)*len, (LPDWORD)len, NULL);
}

int write_file(void* hFile, const unsigned char* buf, size_t* len)
{
    return (int)WriteFile(hFile, buf, (DWORD)*len, (LPDWORD)len, NULL);
}

int set_file_pointer_ex_seek(void* hFile, long long int off, int whence)
{
    BOOL status = TRUE;

    LARGE_INTEGER li = { 0 };

    switch (whence){
    case SEEK_SET:
        status = SetFilePointerEx(hFile, li, NULL, FILE_BEGIN);
        break;
    case SEEK_END:
        status = SetFilePointerEx(hFile, li, NULL, FILE_END);
        break;
    default:
        li.QuadPart = off;
        status = SetFilePointerEx(hFile, li, NULL, FILE_CURRENT);
        break;
    }

    return (int)status;
}

int set_file_pointer_ex_tell(void* hFile, UINT64* pos)
{
    LARGE_INTEGER li = { 0 };
    LARGE_INTEGER li_pos = { 0 };

    BOOL status = SetFilePointerEx(hFile, li, &li_pos, FILE_CURRENT);

    if (status){

        *pos = li_pos.QuadPart;
    }

    return (int)status;
}

void close_handle(void* hFile)
{
    (void)CloseHandle(hFile);
}

void* heap_alloc(size_t buffer_size)
{
    return HeapAlloc(GetProcessHeap(), 0, buffer_size);
}

void* heap_realloc(void* buffer, size_t new_size)
{
    return HeapReAlloc(GetProcessHeap(), 0, buffer, new_size);
}

void heap_free(void* buffer)
{
    (void)HeapFree(GetProcessHeap(), 0, buffer);
}

int alloc_console(void)
{
    return (int)AllocConsole();
}

int read_consle(char* buf, size_t* len)
{
    return (int)ReadConsoleA(
        GetStdHandle(STD_INPUT_HANDLE), buf, (DWORD)*len, (LPDWORD)len, NULL
    );
}

int write_consle(const char* buf, size_t* len)
{
    return (int)WriteConsoleA(
        GetStdHandle(STD_OUTPUT_HANDLE), buf, (DWORD)*len, (LPDWORD)len, NULL
    );
}

int free_console(void)
{
    return (int)FreeConsole();
}

void exit_process(unsigned int exit_code)
{
    ExitProcess(exit_code);
}

typedef struct data{
    int x;
    int y;
    int width;
    int height;
    size_t num;
    int pitch;
    unsigned char* buffer;
    struct data* next;
}bitmap_data;

static bitmap_data* data = NULL;
static bitmap_data* current = NULL;

void append_data(int x, int y, int width, int height, size_t num, int pitch, unsigned char* buffer)
{
    unsigned char* current_buffer = heap_alloc(num);

    if (NULL == current_buffer){

        return;
    }

    for (size_t i = 0; num > i; ++i){

        current_buffer[i] = buffer[i];
    }

    if (NULL == data){

        data = heap_alloc(sizeof(bitmap_data));

        if (data){

            current = data;
            goto next;
        }

        heap_free(current_buffer);
        current_buffer = NULL;

        return;
    }else{

        current->next = heap_alloc(sizeof(bitmap_data));

        if (current->next){

            current = current->next;
            goto next;
        }

        heap_free(current_buffer);
        current_buffer = NULL;

        return;
    }

next:
    current->x = x;
    current->y = y;
    current->width = width;
    current->height = height;
    current->num = num;
    current->pitch = pitch;
    current->buffer = current_buffer;
    current->next = NULL;
}

static void draw_text(HDC buffer)
{
    for (bitmap_data* ptr = data; NULL != ptr; ptr = ptr->next){

        for (int j = 0, y = ptr->y; ptr->height > j; ++j, ++y){

            unsigned char* p = NULL;
            
            if (ptr->pitch > 0){

                p = ptr->buffer + (ptr->pitch * j);
            }else{

                p = ptr->buffer + ((-(ptr->pitch) * ptr->height) - ptr->pitch * j);
            }

            for (int i = 0, x = ptr->x; ptr->width > i; ++i, ++x){

                unsigned char r = p[i];
                unsigned char g = p[i + 1];
                unsigned char b = p[i + 2];

                SetPixel(buffer, x, y, RGB(r, g, b));
            }
        }
    }
}

static void free_data(void)
{
    current = NULL;

    bitmap_data* ptr = data;
    
    while (NULL != ptr){

        bitmap_data* p = ptr;

        ptr = ptr->next;

        heap_free(p->buffer);
        heap_free(p);
    }
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // ディスプレイの幅
    static int disp_width = 0;
    // ディスプレイの高さ
    static int disp_height = 0;
    // BitBLT/PatBlt サポート有無
    static BOOL rc_bitblt = FALSE;

    static HBITMAP bitmap = NULL;
    static HDC buffer = NULL;

    switch (message){
    case WM_CREATE:{

        HDC hdc = GetDC(hWnd);

        // ディスプレイの幅
        disp_width = GetSystemMetrics(SM_CXSCREEN);
        // ディスプレイの高さ
        disp_height = GetSystemMetrics(SM_CYSCREEN);
        // BitBLT/PatBlt サポート有無
        rc_bitblt = RC_BITBLT & GetDeviceCaps(hdc, RASTERCAPS);

        if ((0 < disp_width) && (0 < disp_height) && rc_bitblt){

            bitmap = CreateCompatibleBitmap(hdc, disp_width, disp_height);
            buffer = CreateCompatibleDC(hdc);

            if (bitmap && buffer){

                SelectObject(buffer, bitmap);
                SelectObject(buffer, GetStockObject(NULL_PEN));

                PatBlt(buffer, 0, 0, disp_width, disp_height, BLACKNESS);

                draw_text(buffer);

                free_data();
            }
        }

        ReleaseDC(hWnd, hdc);
        break;
    }
    case WM_PAINT:{

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if ((0 < disp_width) && (0 < disp_height) && rc_bitblt){

            BitBlt(hdc, 0, 0, disp_width, disp_height, buffer, 0, 0, SRCCOPY);
        }

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:

        DeleteDC(buffer);
        buffer = NULL;

        DeleteObject(bitmap);
        bitmap = NULL;

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int init_instance(unsigned short* p)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = (HINSTANCE)GetModuleHandle(NULL);
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = p;
    wcex.hIconSm = NULL;

    RegisterClassEx(&wcex);

    HWND hWnd = CreateWindow(
        p, p, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, wcex.hInstance, NULL
    );

    if (!hWnd){

        return FALSE;
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    return TRUE;
}

int message_loop(void)
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)){

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

