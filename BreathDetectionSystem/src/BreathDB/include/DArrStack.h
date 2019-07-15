#ifndef DARRSTACK_H
#define DARRSTACK_H

#include <assert.h>
#include <string.h>
#include "SockStream.h"

#define DAS_MAXSIZ	1
#define DAS_SUCCESS	0
#define DAS_ERROR	(-1)

class DArrStack
{
public:
    // Constructor: stack max len is 128
    DArrStack()
    {
        maxsiz_ = DAS_MAXSIZ;
        ar_ = new CSockStream[maxsiz_];
        for (size_t i=0; i<maxsiz_; i++)
            ar_[i].seek(0);

        m_top_ = -1;
    }

    ~DArrStack()
    {
        if (ar_)
            delete []ar_;
    }

    // ��һ��CSoskStream����ѹջ
    int push_back(CSockStream& item)
    {
        //assert(item.getData() != NULL);
        if (NULL == item.getData())
            return DAS_ERROR;
        if (IsFull())
        {
            if (maxsiz_ < 512)
                maxsiz_ *= 2;
            else
                maxsiz_ += 512;
            CSockStream *br = new CSockStream[maxsiz_];
            for (int i=0; i<=m_top_; i++)
                br[i].writeBytes(ar_[i].getData(), ar_[i].tell());
            if (ar_)
                delete []ar_;
            ar_ = br;
        }

        m_top_++;
        ar_[m_top_].seek(0);
        ar_[m_top_].writeBytes(item.getData(), item.tell());
        return DAS_SUCCESS;
    }

    // ������Ϊlen��bufѹ��ջ��
    int push_back(CVOID* buf, const size_t len)
    {
        //assert(buf != NULL);
        if (NULL == buf)
            return DAS_ERROR;
        if (IsFull())
        {
            if (maxsiz_ < 512)
                maxsiz_ *= 2;
            else
                maxsiz_ += 512;
            CSockStream *br = new CSockStream[maxsiz_];
            for (int i=0; i<=m_top_; i++)
                br[i].writeBytes(ar_[i].getData(), ar_[i].tell());
            if (ar_)
                delete []ar_;
            ar_ = br;
        }

        m_top_++;
        ar_[m_top_].seek(0);
        ar_[m_top_].writeBytes(buf, len);
        return DAS_SUCCESS;
    }

    // ��ջ������item����item����ָ��ָ��0
    int pop(CSockStream& item)
    {
        if (!IsEmpty())
        {
            assert(ar_[m_top_].getData() != NULL);
            item.seek(0);
            item.writeBytes(ar_[m_top_].getData(), ar_[m_top_].tell());
            ar_[m_top_].seek(0);
            m_top_--;
            return DAS_SUCCESS;
        }
        else
            return DAS_ERROR;
    }

    // ��ջ������buf
    int pop(void *&buf, const size_t len)
    {
        if (!IsEmpty())
        {
            assert(ar_[m_top_].getData() != NULL);
            size_t outlen = ar_[m_top_].tell();
            memcpy(buf, ar_[m_top_].getData(), len<outlen ? len : outlen);
            ar_[m_top_].seek(0);
            m_top_--;
            return DAS_SUCCESS;
        }
        else
            return DAS_ERROR;
    }

    //���ջ��CSoskStream����
    int GetTop(CSockStream& item)
    {
        if (!IsEmpty())
        {
            assert(ar_[m_top_].getData() != NULL);
            item.seek(0);
            item.writeBytes(ar_[m_top_].getData(), ar_[m_top_].tell());
            return DAS_SUCCESS;
        }
        else
            return DAS_ERROR;
    }

    bool IsFull()
    {
        if (m_top_ == (int)(maxsiz_-1))
            return true;
        else
            return false;
    }

    bool IsEmpty()
    {
        if (-1 == m_top_)
            return true;
        else
            return false;
    }

    int size()
    {
        return m_top_+1;
    }

    void clear()
    {
        for (size_t i=0; i<maxsiz_; i++)
            ar_[i].seek(0);
        m_top_ = -1;
    }

private:
    CSockStream *ar_;
    size_t maxsiz_;
    int m_top_;
};

#endif // DARRSTACK_H
