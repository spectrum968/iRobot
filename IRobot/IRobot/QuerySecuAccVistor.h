#pragma once

#include "IPCKDGateWayVistor.h"

/************************************************************************/
/* 客户股东查询                                                        */
/************************************************************************/
class CQuerySecuAccVistor
	: public IPCKDGateWayVistor
{
public:
	CQuerySecuAccVistor(void);
	virtual ~CQuerySecuAccVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);

	// 向KCXP发送消息
	BOOL SendKcxpMsg(char *);

	BOOL TestCase_1();

	BOOL ChkData(MID_501_QUERY_SECU_ACC_RET_MSG *pMsg);

private:
	MID_501_QUERY_SECU_ACC_RET_MSG *m_pMsg;
};
