/**
  * Copyright (c) 2004, HangZhou Webcon Corporation.

  * @file     ByteStream.h
  * @author   star<star@webcon.com.cn>
  * @short
  * @date     03/2004
  **/

#ifndef _BYTE_STREAM_H__
#define _BYTE_STREAM_H__

#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "TypeDef.h"

class CByteStream
{
public: //Constructor & destructor
    CByteStream(BYTE* pBuf,int bUseSwapLogic=TRUE)
    {
        m_pBuf = pBuf;
        m_dwCurPos = 0;
        m_bUseSwapLogic=bUseSwapLogic;
    }
    virtual ~CByteStream() {}

public:    // Operations
    inline DWORD SetCurPos(DWORD dwPos)
    {
        DWORD dwOld = m_dwCurPos;
        m_dwCurPos = dwPos;
        return dwOld;
    }
    inline DWORD GetCurPos()
    {
        return m_dwCurPos;
    }
    inline void Skip(long pos)
    {
        m_dwCurPos += pos;
    }
    inline BYTE* GetData()
    {
        return m_pBuf;
    }

    inline void ReadBytes(BYTE* pData, DWORD dwSize)
    {
        if (dwSize <= 0 || pData == NULL)
            return;

        memcpy(pData, m_pBuf+m_dwCurPos, dwSize);
        m_dwCurPos += dwSize;
    }
    inline void WriteBytes(BYTE* pData, DWORD dwSize)
    {
        if (dwSize <= 0 || pData == NULL)
            return;

        memcpy(m_pBuf+m_dwCurPos, pData, dwSize);
        m_dwCurPos += dwSize;
    }
    
    inline CCHAR * ReadString()
    {
        CCHAR * pStr = (CCHAR *)(m_pBuf+m_dwCurPos);
        m_dwCurPos += strlen(pStr)+1;
        return pStr;
    }
    inline void WriteString(CCHAR * pStr)
    {
        strcpy((char*)(m_pBuf+m_dwCurPos), pStr);
        m_dwCurPos += strlen(pStr)+1;
    }
    
    inline CByteStream& operator << (unsigned char ch)
    { m_pBuf[m_dwCurPos++] = ch; return *this; }
    inline CByteStream& operator << (char ch)
    { m_pBuf[m_dwCurPos++] = (unsigned char)ch; return *this; }
    inline CByteStream& operator << (short s)
    { s = htons( s ); WriteBytes((BYTE*)&s, (DWORD)sizeof(short)); return *this; }
    inline CByteStream& operator << (unsigned short s)
    { s = htons( s ); WriteBytes((BYTE*)&s, (DWORD)sizeof(unsigned short));	return *this; }
    inline CByteStream& operator << (int i)
    { i = htonl( i ); WriteBytes((BYTE*)&i, (DWORD)sizeof(int)); return *this; }
    inline CByteStream& operator << (unsigned int i)
    { i = htonl( i ); WriteBytes((BYTE*)&i, (DWORD)sizeof(unsigned int)); return *this; }
    inline CByteStream& operator << (long l)
    { l = htonl( l ); WriteBytes((BYTE*)&l, (DWORD)sizeof(long)); return *this; }
    inline CByteStream& operator << (unsigned long l)
    { l = htonl( l ); WriteBytes((BYTE*)&l, (DWORD)sizeof(unsigned long)); return *this; }
    inline CByteStream& operator << (float f)
    { Swap(&f, sizeof(float)); WriteBytes((BYTE*)&f, (DWORD)sizeof(float)); return *this; }
    inline CByteStream& operator << (double d)
    { Swap(&d, sizeof(double)); WriteBytes((BYTE*)&d, (DWORD)sizeof(double)); return *this; }

    inline CByteStream& operator >> (unsigned char& ch)
    { ch = m_pBuf[m_dwCurPos++]; return *this; }
    inline CByteStream& operator >> (char& ch)
    { ch = (char)m_pBuf[m_dwCurPos++]; return *this; }
    inline CByteStream& operator >> (short& s)
    { ReadBytes((BYTE*)&s, (DWORD)sizeof(short)); s = ntohs( s ); return *this; }
    inline CByteStream& operator >> (unsigned short& s)
    { ReadBytes((BYTE*)&s, (DWORD)sizeof(unsigned short)); s = ntohs( s ); return *this; }
    inline CByteStream& operator >> (int& i)
    { ReadBytes((BYTE*)&i, (DWORD)sizeof(int)); i = ntohl( i ); return *this; }
    inline CByteStream& operator >> (unsigned int& i)
    { ReadBytes((BYTE*)&i, (DWORD)sizeof(unsigned int)); i = ntohl( i ); return *this; }
    inline CByteStream& operator >> (long& l)
    { ReadBytes((BYTE*)&l, (DWORD)sizeof(long)); l = ntohl( l ); return *this; }
    inline CByteStream& operator >> (unsigned long& l)
    { ReadBytes((BYTE*)&l, (DWORD)sizeof(unsigned long)); l = ntohl( l ); return *this; }
    inline CByteStream& operator >> (float& f)
    { ReadBytes((BYTE*)&f, (DWORD)sizeof(float)); Swap(&f, sizeof(float)); return *this; }
    inline CByteStream& operator >> (double& d)
    { ReadBytes((BYTE*)&d, (DWORD)sizeof(double)); Swap(&d, sizeof(double)); return *this; }

    inline void Swap(void* pData, int nSize)
    {
        if (m_bUseSwapLogic)
        {
            if (!pData || nSize <= 0)
                return;
            BYTE* pHead = (BYTE*)pData;
            BYTE* pTail = (BYTE*)pData+nSize-1;
            BYTE byTmp;
            while (pTail - pHead >= 1)
            {
                byTmp = *pHead;
                *pHead = *pTail;
                *pTail = byTmp;
                pTail--;
                pHead++;
            }
        }
    }
    
protected:
    BYTE* m_pBuf;
    DWORD m_dwCurPos;
    int m_bUseSwapLogic;
};

#endif  //

