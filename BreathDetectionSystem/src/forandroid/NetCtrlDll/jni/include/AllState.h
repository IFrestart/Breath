#ifndef	__ALLSTATE_H__
#define	__ALLSTATE_H__


#include "BaseState.h"


namespace  LOGINSTATE
{
	enum
	{	SAUTHREQUEST = 1, SLOGINAUTH = 2, SJOINSTRANS = 3, SLOGOUTAUTH = 4, SLEAVETRAS = 5,SNORMAL = 6,};
};

class CSLoginAuthRequest : public CBaseState
{
public: 
	int startObject(void*  obj);
	int SendData(void*  obj)	;
	int  State() const { return (int)LOGINSTATE::SAUTHREQUEST ;}
};

class CSLoginAuth : public CBaseState
{
public: 	
	int  startObject(void*  obj);
	int SendData(void*  obj)	;
	int  State() const { return (int)LOGINSTATE::SLOGINAUTH ;}
	//bool m_bNotInPoint;
};


class CSLogoutAuth : public CBaseState
{
public: 
	int startObject(void*  obj);
	int SendData(void*  obj);
	int  State() const { return (int)LOGINSTATE::SLOGOUTAUTH ;}
};

class CSLoginNormal: public CBaseState
{
public: 
	int startObject(void*  obj){return 0;}
	int SendData(void*  obj){return 0;}
	int  State() const { return (int)LOGINSTATE::SNORMAL ;}

};

class CSTransJoin : public CBaseState
{
public: 
	int startObject(void*  obj);
	int SendData(void*  obj)	;
	int State() const { return (int)LOGINSTATE::SJOINSTRANS ;}
};


class CSTransLeave : public CBaseState
{
public: 	
	int  startObject(void*  obj);
	int SendData(void*  obj)	;
	int  State() const { return (int)LOGINSTATE::SLEAVETRAS ;}
	//bool m_bNotInPoint;
};


#endif
