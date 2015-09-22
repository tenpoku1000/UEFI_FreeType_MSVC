
// Copyright 2015 Shin'ichi Ichikawa. Released under the MIT license.

#ifndef __WIN64ENV_H__
#define __WIN64ENV_H__

void* create_file(
    wchar_t* file_name, unsigned long long int flags,
    unsigned long long int efi_file_mode_read,
    unsigned long long int efi_file_mode_write,
    unsigned long long int efi_file_mode_create
);
int read_file(void* hFile, unsigned char* buf, size_t* len);
int write_file(void* hFile, const unsigned char* buf, size_t* len);
void close_handle(void* hFile);
int set_file_pointer_ex_seek(void* hFile, long long int off, int whence);
int set_file_pointer_ex_tell(void* hFile, UINT64* pos);

void* heap_alloc(size_t buffer_size);
void* heap_realloc(void* buffer, size_t new_size);
void heap_free(void* buffer);

int alloc_console(void);
int read_consle(char* buf, size_t* len);
int write_consle(const char* buf, size_t* len);
int free_console(void);
void exit_process(unsigned int exit_code);

void append_data(int x, int y, int width, int height, size_t num, int pitch, unsigned char* buffer);
int init_instance(unsigned short* p);
int message_loop(void);

#endif
