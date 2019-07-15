/**
  * @file     TypeDef.h
  * @author   shizc
  * @short    自定义类型
  * @date     2018-07-04
  **/
#ifndef TYPEDEF_H
#define TYPEDEF_H

typedef int BOOL;
#define TRUE 1
#define FALSE 0

#define RetInit     0

//按位数
#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64)
#elif defined(bit64)
#define _64     long
#else
#define _64     long long
#endif

typedef unsigned int    UINT;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

typedef char int8, *pint8;
typedef short int16, *pint16;
typedef int int32, *pint32;
typedef _64 int64, *pint64;
typedef unsigned char uint8, *puint8;
typedef unsigned short uint16, *puint16;
typedef unsigned int uint32, *puint32;
typedef unsigned _64 uint64, *puint64;

typedef const char  CCHAR;
typedef CCHAR* LPCSTR;
typedef CCHAR* LPCTSTR;

typedef const void  CVOID;

typedef char* LPSTR;
typedef char* LPTSTR;

typedef void* LPVOID;
typedef char TCHAR;
#endif // TYPEDEF_H
