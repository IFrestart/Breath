#ifndef __TESTSOCKEVENT_RTP__
#define __TESTSOCKEVENT_RTP__

#include "RTPConnector.h"

class TestSockEvent_rtp : public IRTPEventCallBack
{
public:
        //TestSockEvent_rtp(TestRtp* pServ) : pServ_(pServ) {}
        TestSockEvent_rtp() {}
        /**
         * inherit IRTPEventCallBack
         * @see IRTPEventCallBack
         */
    virtual int OnNewRTPSource ( unsigned long fromIP, unsigned short fromPort) ;

        /**
         * inherit IRTPEventCallBack
         * @see IRTPEventCallBack
         */
    virtual int OnRTPPacketReceived ( int sock, unsigned short pdutype, const void* data, unsigned long len,
                            unsigned long fromIP, unsigned short fromPort);

        /**
         * inherit IRTPEventCallBack
         * @see IRTPEventCallBack
         */
    virtual int OnErrorRTPSource ( int errCode, unsigned long fromIP, unsigned short fromPort) ;
protected:
//        TestRtp*        pServ_;
private:

} ;

#endif
