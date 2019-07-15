/** 
 * Copyright (c) 2004, HangZhou Webcon Corporation.  
  
 * @file        CommonInc.h
 * @author      lujnan <suping@cvsserver>
     
 * @short  	common struction
 **/ 
 
#ifndef __COMMONINC_H__
#define __COMMONINC_H__

typedef struct tagBaseFunctionServerID {
	unsigned short areaindex;
	unsigned short servertype;
	unsigned short serverindex;
}BASE_FUNCTIONSERVERID;

typedef struct tagBaseGroupID {
	BASE_FUNCTIONSERVERID f_serverid;
	unsigned short groupindex;
}BASE_GROUPID;

#endif // 


