/**
  * @file     SubPDU.h
  * @author   wudaozc
  * @brief    自定义子pdu
  * @date     2018-09-08
  **/
#ifndef SUBPDU_H
#define SUBPDU_H

namespace SubPDU
{
/*================================================================*/
/* Segment */
/*================================================================*/
const unsigned short
ProcDataBase1	= 0,		/**< */
Base1	= 30000;

/** Begin */
enum
{

    /*================================================================*/
    /* 数据处理模块 */
    /*================================================================*/
    ProcDataBase				= ProcDataBase1/*0*/ + 0,
    //获取状态文件列表
    getStatFilelist_REQ_TODBCOMM        =78,
    getStatFilelist_RSP_FROMDBCOMM      =79,

    //入库根据平滑参数得到的状态结果
    inStat_BaseSmoothPara_REQ_TODBCOMM          =80,
    inStat_BaseSmoothPara_RSP_FROMDBCOMM        =81,

    //获取未处理文件目录
    getUnprocDir_REQ_TODBCOMM           =82,
    getUnprocDir_RSP_FROMDBCOMM         =83,
};
}

#endif // SUBPDU_H
