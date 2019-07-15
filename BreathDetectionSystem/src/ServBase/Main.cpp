#include "Common.h"
#include "BaseServ.h"
#include "stdio.h"
#ifdef  __TXT_MODE__

int ACE_TMAIN(int argc, char** argv)
{
	ACE_ASSERT(0 != CBaseServ::getInstance());

	return CBaseServ::getInstance()->doMain(argc, argv);
}

#endif

