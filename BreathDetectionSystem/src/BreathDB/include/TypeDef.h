/**
  * Copyright (c) 2018, HangZhou Webcon Corporation.
  * @file     TypeDef.h
  * @author   shizc <shizc@webcon.com.cn>
  * @short    类型转换
  * @date     2018-07-02
  **/

#ifndef _TYPEDEF_H
#define _TYPEDEF_H

typedef int BOOL;
#define TRUE 1
#define FALSE 0

#define RetInit     0

typedef unsigned int    UINT;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;



#define _cpu32
//按位
#ifdef _cpu32
#define _64     long long
#elif defined(_cpu64)
#define _64     long
#endif
typedef signed char int8, *pint8;
typedef signed short int16, *pint16;
typedef signed int int32, *pint32;
typedef signed _64 int64, *pint64;
typedef unsigned char uint8, *puint8;
typedef unsigned short uint16, *puint16;
typedef unsigned int uint32, *puint32;
typedef unsigned _64 uint64, *puint64;

typedef const char  CCHAR;
typedef const void  CVOID;
typedef CCHAR* LPCSTR;
typedef CCHAR* LPCTSTR;


typedef char* LPSTR;
typedef char* LPTSTR;

typedef void* LPVOID;
typedef char TCHAR;

#endif
