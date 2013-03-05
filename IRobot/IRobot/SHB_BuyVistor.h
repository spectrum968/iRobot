#pragma once

#include "IPCKDGateWayVistor.h"

/************************************************************************/
/* 沪B股买入                                                         */
/************************************************************************/
class CSHB_BuyVistor
	: public IPCKDGateWayVistor
{
public:
	CSHB_BuyVistor(void);
	virtual ~CSHB_BuyVistor(void);

	virtual BOOL Vistor();
	virtual BOOL ResultStrToTable(char *);	

	// 在执行前，获取一把相关数据，与执行后数据进行比较
	BOOL InitUserData();
	BOOL UpdateUserData();
	BOOL GetMatchedData();

	// 向KCXP发送消息
	BOOL SendKcxpMsg(char *);

	BOOL TestCase_1(); 

	BOOL ChkData();

	BOOL SaveCapital();

private:
	MID_403_ORDER_RET_MSG *m_pMsg;

	char m_szSecu_Intl[DEFAULT_LEN];		// 证券代码
	char m_szSecu_Code[DEFAULT_LEN];		// 证券代码
	char m_szPrice[DEFAULT_LEN];			// 委托价格
	char m_szQty[DEFAULT_LEN];				// 委托数量

	char m_szMarket_Board[3];
	char m_szTrdId[3];
	char m_szCurrency[2]; // 币种

	// 检测数据
	float m_fMatched_OrderFrzAmt;	// 成交的交易冻结金额
	float m_fMatched_Price;	// 成交价格
	int m_nMatched_Qty;	// 成交数量
	float m_fMatchedAmt;	// 成交金额
	float m_fMatched_SettAmt; // 清算金额

	int m_nShareBln_Old;			// 股份余额
	int m_nShareAvl_Old;			// 股份可用
	int m_nShareTrdFrz_Old;			// 交易冻结
	int m_nShareOtd_Old;			// 在途数量

	float m_fCptlBln_Old;			// 资金余额
	float m_fCptlAvl_Old;			// 资金可用
	float m_fCptlTrdFrz_Old;		// 交易冻结
	float m_fCptlOutstanding_Old;	// 在途资金
	float m_fCptlOtdAvl_Old;		// 在途可用

	int m_nShareBln_New;			// 股份余额
	int m_nShareAvl_New;			// 股份可用
	int m_nShareTrdFrz_New;			// 交易冻结
	int m_nShareOtd_New;			// 在途数量

	float m_fCptlBln_New;			// 资金余额
	float m_fCptlAvl_New;			// 资金可用
	float m_fCptlTrdFrz_New;		// 交易冻结
	float m_fCptlOutstanding_New;	// 在途资金
	float m_fCptlOtdAvl_New;		// 在途可用
};
