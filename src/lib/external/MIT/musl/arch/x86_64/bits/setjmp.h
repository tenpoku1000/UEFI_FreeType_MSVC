
// @@@
//typedef unsigned long __jmp_buf[8];
#ifdef _MSC_VER
typedef unsigned long __jmp_buf[10];
#else
typedef unsigned long __jmp_buf[8];
#endif
// @@@
