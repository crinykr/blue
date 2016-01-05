/*
 * printk.h
 *
 *  Created on: 2014. 8. 24.
 *      Author: criny
 */

#ifndef PRINTK_H_
#define PRINTK_H_

#include "types.h"
#include <stdarg.h>

void print_dec(QWORD value, QWORD width, char *buf, QWORD *ptr);
void print_hex(QWORD value, QWORD width, char *buf, QWORD *ptr);
size_t vasprintf(char * buf, const char *fmt, va_list args);
void printk(const char *str, ...);
int sprintf(char * buf, const char *fmt, ...);

#endif /* PRINTK_H_ */
