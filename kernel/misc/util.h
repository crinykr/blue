#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdarg.h>
#include "types.h"
#include "sys.h"

#define MIN( x, y )     ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#define MAX( x, y )     ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )

#define _BARRIER_; __asm__ __volatile__ ("cli"); while(1) { __asm__ __volatile__("hlt"); }
#define cli(); __asm__ __volatile__ ("cli");
#define sti(); __asm__ __volatile__ ("sti");
#define stop(); while(1) { }

void print_regs(regs_t *r);

unsigned char in_port_b(unsigned short _port);
void out_port_b(unsigned short _port, unsigned char _data);

void* memset(void * b, int val, size_t count);
void* memcpy(void * dest, const void * src, size_t count);
int memcmp(const char * a, const char * b, const int size);

char* strdup(const char *str);
int strcat(void* pvDeenable_intrnation, const void* pvSource);
char * strcpy(char * dest, const char * src);
int strlen(const char *str);

BOOL set_intr(BOOL bEnableInterrupt);
QWORD get_memsize(void);
void str_reverse(char* pcBuffer);
QWORD atoi(const char* str);
int itoa(long lValue, char* pcBuffer, int iRadix);
void sleep(QWORD qwMillisecond);

#endif /* UTILITY_H_ */
