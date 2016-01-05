#ifndef TYPES_H_
#define TYPES_H_

#define UINT64_MAX 0xffffffffffffffff

#define BYTE    unsigned char
#define WORD    unsigned short
#define DWORD   unsigned int
#define QWORD   unsigned long
#define BOOL    unsigned char
#define size_t DWORD

#define TRUE    1
#define FALSE   0
#define NULL    0

#define CANDY 0

// stddef.h
#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)

#define duplicated

#endif /* TYPES_H_ */
