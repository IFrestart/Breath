/**
  * Copyright (c) 2004, HangZhou Webcon Corporation.

  * @file     BaseArray.h
  * @author   star <star@webcon.com.cn>
  * @short    »ù´¡Êý×é
  * @date     03/2004
  **/

#ifndef _COMMONTOOL_H
#define _COMMONTOOL_H

#include <string.h>
#include <assert.h>
#include "TypeDef.h"

template<class ItemType, int GrowSize>

class CToolArrayBase
{
public :
    CToolArrayBase()
    {
        m_ppHead = NULL;
        m_nAlloc = 0;
        m_nUsed  = 0;
    }

    CToolArrayBase(const CToolArrayBase& other)
    {
        m_ppHead = NULL;
        m_nAlloc = 0;
        m_nUsed  = 0;

        *this = other;
    }

    virtual ~CToolArrayBase()
    {
        RemoveAll();
    }

public :
    int GetCount()
    {
        return m_nUsed;
    }

    int Add(ItemType item, int bCheckDuplicate = FALSE)
    {
        if(Expand())
        {
            if(bCheckDuplicate && GetIndex(item) >= 0)
                return FALSE;

            m_ppHead[m_nUsed++] = item;
            return TRUE;
        }
        return FALSE;
    }

    int InsertAt(int nIndex, ItemType item)
    {
        assert(nIndex >= 0 && nIndex <= m_nUsed);

        if(nIndex >= 0 && nIndex <= m_nUsed)
        {
            if(!Expand())
                return FALSE;

            if(nIndex < m_nUsed)
                memmove(&m_ppHead[nIndex + 1], &m_ppHead[nIndex],
                        sizeof(ItemType)*(m_nUsed - nIndex));

            m_ppHead[nIndex] = item;
            m_nUsed++;

            return TRUE;
        }
        return FALSE;
    }

    int Remove(ItemType item)
    {
        int i;
        for(i = 0; i < m_nUsed; i++)
        {
            if( Compare(m_ppHead+i, &item) )
            {
                break;
            }
        }

        if (i < m_nUsed)
        {
            // found
            if(i < m_nUsed - 1)
                memcpy(&m_ppHead[i], &m_ppHead[i+1],
                       sizeof(ItemType)*(m_nUsed - i - 1));
            m_nUsed--;
            return  TRUE;
        }

        return FALSE;
    }

    int RemoveAt(int i)
    {
        if(i < m_nUsed)
        {
            if(i < m_nUsed - 1)
                memcpy(&m_ppHead[i], &m_ppHead[i+1],
                       sizeof(ItemType)*(m_nUsed - i - 1));
            m_nUsed--;
            return  TRUE;
        }
        return FALSE;
    }

    void RemoveAll()
    {
        if(m_ppHead)
            delete []m_ppHead;
        m_ppHead = NULL;
        m_nAlloc = 0;
        m_nUsed = 0;
    }

    CToolArrayBase& operator = (const CToolArrayBase& other)
    {
        RemoveAll();

        if(other.m_nUsed > 0)
        {
            m_ppHead = new ItemType[other.m_nUsed];
            if(m_ppHead)
            {
                memcpy(m_ppHead, other.m_ppHead, sizeof(ItemType)*other.m_nUsed);
                m_nAlloc = m_nUsed = other.m_nUsed;
            }
        }

        return *this;
    }

    ItemType& operator [](int i)
    {
        assert(m_ppHead != NULL);
        static	ItemType retVal ;

        if(i >= 0 && i < m_nUsed)
        {
            retVal = m_ppHead[i];
            return m_ppHead[i];
        }
        return retVal;
    }

    int SetAt(int nIndex, ItemType item)
    {
        assert(nIndex >= 0 && nIndex < m_nUsed);

        if (nIndex >= 0 && nIndex < m_nUsed)
        {
            m_ppHead[nIndex] = item;

            return TRUE;
        }

        return FALSE;
    }

    int GetIndex(ItemType item)
    {
        for(int i = 0; i < m_nUsed; i++)
        {
            if( Compare(m_ppHead+i, &item) )
                return	i;
        }

        return -1;
    }

    ItemType* GetHead()
    {
        return m_ppHead;
    }

protected :
    int Compare(ItemType* item1, ItemType* item2)
    {
        if( memcmp(item1, item2, sizeof(ItemType))==0 )
            return	TRUE;

        return	FALSE;
    }

    int Expand()
    {
        if(m_nUsed >= m_nAlloc)
        {
            ItemType* ppNew = new ItemType[m_nAlloc + GrowSize];
            if(ppNew)
            {
                if(m_ppHead)
                {
                    memcpy(ppNew, m_ppHead, sizeof(ItemType)*m_nAlloc);
                    delete []m_ppHead;
                }
                m_ppHead = ppNew;
                m_nAlloc += GrowSize;
                return TRUE;
            }
            return FALSE;
        }
        return TRUE;
    }

protected :
    ItemType*   m_ppHead;
    int         m_nAlloc;
    int         m_nUsed;
};

#endif
