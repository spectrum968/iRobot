#include "StdAfx.h"
#include "SZ_LOF_Vistor.h"
#include "Cfg.h"
#include "MidConn.h"
#include "KcxpConn.h"
#include "loginterface.h"
#include "DBConnect.h"
#include "public.h"

extern CCfg *g_pCfg;
extern CMidConn *g_pMidConn;
extern CKcxpConn *g_pKcxpConn;
extern CLoginterface *g_pLog;
extern CDBConnect *g_pDBConn;

CSZLOFVistor::CSZLOFVistor(void)
{
	m_pMsg = NULL;
	m_nRowNum = 0;

	m_fOrder_RltFrzAmt = m_fOrder_RltFrzQty = 0;

	m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;

	m_fCptlBln_Old = m_fCptlAvl_Old = m_fCptlTrdFrz_Old = m_fCptlOutstanding_Old = m_fCptlOtdAvl_Old = 0;

	m_nShareBln_New = m_nShareAvl_New = m_nShareTrdFrz_New = m_nShareOtd_New = 0;
	
	m_fCptlBln_New = m_fCptlAvl_New = m_fCptlTrdFrz_New = m_fCptlOutstanding_New = m_fCptlOtdAvl_New = 0;

	strcpy_s(m_szServiceName, "���ڳ��ڻ���ҵ��");

	strcpy_s(m_szSecu_Code, "163406");
	strcpy_s(m_szSecu_Intl, "163406");
	strcpy_s(m_szQty, "10000");
	strcpy_s(m_szPrice, "1");

	// ��A
	strcpy_s(m_szMarket_Board, "00");
	strcpy_s(m_szTrdId, "7M");  // �깺
	strcpy_s(m_szCurrency, "0");
}

CSZLOFVistor::~CSZLOFVistor(void)
{
	DELCLS(m_pMsg);
	m_nRowNum = 0;
}

BOOL CSZLOFVistor::Vistor()
{
	BOOL bRet = TRUE;
	
	ExecTestCase(TestCase_1, "�깺 7M");
	ExecTestCase(TestCase_2, "��� 7N");
	ExecTestCase(TestCase_3, "�Ϲ� 7J");

	return bRet;
}

BOOL CSZLOFVistor::ResultStrToTable(char *pRetStr)
{
	m_nRowNum = m_pKDGateWay->GetRecNum();
	
	m_pMsg = new MID_403_ORDER_RET_MSG[m_nRowNum];
	memset(m_pMsg, 0x00, sizeof(MID_403_ORDER_RET_MSG)*m_nRowNum);

	int nLen = strlen(pRetStr);

	char *p = new char[nLen+1];

	strncpy(p, pRetStr, nLen);
	p[nLen] = '\0';

	// ��õڶ��У������ݿ�ʼ��
	char *q = m_pKDGateWay->GetNextLine(p);
	char *tmp = q;

	// ��ȡÿ�е�����
	for (DWORD nRow=0; nRow<m_nRowNum; nRow++)
	{
		// ��ȡÿ�еĳ���
		for (DWORD nCol=0; nCol<m_pKDGateWay->GetFieldNum(); nCol++)
		{
			while(tmp++)
			{
				if (*tmp == '|')
				{
					*tmp = '\0';					

					switch (nCol)
					{
					case 0:						
						SERVICE_STRNCPY(szBizNo);
						break;
					case 1:
						SERVICE_STRNCPY(szOrderID);
						break;
					case 2:						
						SERVICE_STRNCPY(szAccount);
						break;
					case 3:						
						SERVICE_STRNCPY(szPrice);
						break;
					case 4:						
						SERVICE_STRNCPY(szQty);
						break;
					case 5:						
						SERVICE_STRNCPY(szOrderAmt);
						break;
					case 6:						
						SERVICE_STRNCPY(szOrderFrzAmt);
						break;
					case 7:						
						SERVICE_STRNCPY(szSeat);
						break;
					case 8:						
						SERVICE_STRNCPY(szExtInst);
						break;
					case 9:						
						SERVICE_STRNCPY(szExtAcc);
						break;
					case 10:						
						SERVICE_STRNCPY(szExtSubAcc);
						break;
					case 11:						
						SERVICE_STRNCPY(szExtFrzAmt);
						break;					
					}

					q = tmp +1;					
					break;
				}
			}
		}		
	}

	return TRUE;
}

// �깺
BOOL CSZLOFVistor::TestCase_1()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "7M");

	SaveCapital();

	bRet = InitUserData();
	if (bRet == FALSE)
	{
		return FALSE;
	}

	// ��������
	char szTemp[2048] = {0};
	if (g_pCfg->GetTestMode() == USE_MID)
	{
		sprintf_s(szTemp,"403|%s|%s|%s|%s||%s|%s|%s|%s||||||||||||",
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSZA_BIND_SEAT());

		if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
		{
			ParseKcxpRetMsg();
		}
	}

	if (bRet == FALSE)
	{
		return FALSE;	
	}

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	bRet = ChkData();
	
	return bRet;
}

// �м�����: �嵵��ʱ�ɽ�ʣ�೷�� VB
BOOL CSZLOFVistor::TestCase_2()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "7N");

	SaveShares();

	bRet = InitUserData();
	if (bRet == FALSE)
	{
		return FALSE;
	}

	// ��������
	char szTemp[2048] = {0};
	if (g_pCfg->GetTestMode() == USE_MID)
	{
		sprintf_s(szTemp,"403|%s|%s|%s|%s||%s|%s|%s|%s||||||||||||",
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount(), m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���		
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSZA_BIND_SEAT());

		if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
		{
			ParseKcxpRetMsg();
		}
	}

	if (bRet == FALSE)
	{
		return FALSE;	
	}

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	bRet = ChkData();

	return bRet;
}

// �м�����: �嵵��ʱ�ɽ�ʣ��ת�� UB
BOOL CSZLOFVistor::TestCase_3()
{
	BOOL bRet = TRUE;
	strcpy_s(m_szTrdId, "UB");

	SaveCapital();

	bRet = InitUserData();
	if (bRet == FALSE)
	{
		return FALSE;
	}

	// ��������
	char szTemp[2048] = {0};
	if (g_pCfg->GetTestMode() == USE_MID)
	{
		sprintf_s(szTemp,"403|%s|%s|%s|%s||%s|%s|%s|%s||||||||||||",
			g_pCfg->GetCustID(), m_szMarket_Board, g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount(), m_szSecu_Code, m_szTrdId, m_szPrice, m_szQty);

		bRet = SendMidMsg(&szTemp[0]);
	}
	else
	{
		// ƴ�ӷ��͸�KCXP�������ַ���		
		sprintf_s(szTemp,"BEGIN:L0303001:23-11:00:22-576498  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=999999999999999&F_OP_BRANCH=%s&F_CHANNEL=0"
			"&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SERIAL_NO=-1&DELIST_CHANNEL=0&TRD_ID=%s&PRICE=%s&QTY=%s&SEAT=%s]",
			g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
			g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SHA(), g_pCfg->GetAccount().GetBuffer(),
			m_szSecu_Intl, m_szTrdId, m_szPrice, m_szQty, g_pCfg->GetSHA_BIND_SEAT());

		if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
		{
			ParseKcxpRetMsg();
		}
	}

	if (bRet == FALSE)
	{
		return FALSE;	
	}

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	bRet = ChkData();

	return bRet;
}

/*
 *	��ȡ��������
 	1. Shares��
		1.1 �ɷ����
		1.2 �ɷݿ���
		1.3 ���׶���
		1.4 ��;����
	2. Capitals��
		2.1 �ʽ����
		2.2 �ʽ����
		2.3 ���׶���
		2.4 ��;�ʽ�
		2.5 ��;����
 */
BOOL CSZLOFVistor::InitUserData()
{
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡShares��
	CString strSql;
	strSql.Format("select * from  shares where account = %s and secu_intl = %s",
		g_pCfg->GetAccount().GetBuffer(), m_szSecu_Intl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet))
			return FALSE;

		// �ͻ���ǰû�гֲִ˹�Ʊ
		if (pRecordSet->adoEOF)
		{
			m_nShareBln_Old = m_nShareAvl_Old = m_nShareTrdFrz_Old = m_nShareOtd_Old = 0;
		}

		while(!pRecordSet->adoEOF)
		{	
			// 1.1 �ɷ����
			DB_GET_VALUE_INT("SHARE_BLN", m_nShareBln_Old);

			// 1.2 �ɷݿ���
			DB_GET_VALUE_INT("SHARE_AVL", m_nShareAvl_Old);

			// 1.3 ���׶���
			DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_nShareTrdFrz_Old);

			// 1.4 ��;����
			DB_GET_VALUE_INT("SHARE_OTD", m_nShareOtd_Old);

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		return FALSE;
	}
	

	// 2. ��ȡCapitals��
	try
	{
		strSql.Format("select * from  capital where account = %s and currency = %s",
			g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

		bstrSQL = strSql.AllocSysString();
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		if (pRecordSet->adoEOF)
		{
			// ���ݿⷵ�صĽ��Ϊ��
			pRecordSet->Close();

			g_pLog->WriteRunLogEx(__FILE__,__LINE__, "��ȡCapitals������ʧ��!");
			return FALSE;
		}

		while(!pRecordSet->adoEOF)
		{	
			// 2.1 �ʽ����
			DB_GET_VALUE_FLOAT("BALANCE", m_fCptlBln_Old);

			// 2.2 �ʽ����			
			DB_GET_VALUE_FLOAT("AVAILABLE", m_fCptlAvl_Old);

			// 2.3 ���׶���
			DB_GET_VALUE_FLOAT("TRD_FRZ", m_fCptlTrdFrz_Old);

			// 2.4 ��;�ʽ�
			DB_GET_VALUE_FLOAT("OUTSTANDING", m_fCptlOutstanding_Old);

			// 2.5 ��;����
			DB_GET_VALUE_FLOAT("OTD_AVL", m_fCptlOtdAvl_Old);

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		return FALSE;
	}

	return TRUE;
}


BOOL CSZLOFVistor::UpdateUserData()
{
	_variant_t TheValue; //VARIANT��������
	char szTmp[100] = {0};

	// 1. ��ȡShares��
	CString strSql;
	strSql.Format("select * from  shares where account = %s and secu_intl = %s",
		g_pCfg->GetAccount().GetBuffer(), m_szSecu_Intl);

	BSTR bstrSQL = strSql.AllocSysString();

	try
	{
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet))
			return FALSE;

		while(!pRecordSet->adoEOF)
		{	
			// 1.1 �ɷ����
			DB_GET_VALUE_INT("SHARE_BLN", m_nShareBln_New);

			// 1.2 �ɷݿ���
			DB_GET_VALUE_INT("SHARE_AVL", m_nShareAvl_New);

			// 1.3 ���׶���
			DB_GET_VALUE_INT("SHARE_TRD_FRZ", m_nShareTrdFrz_New);

			// 1.4 ��;����
			DB_GET_VALUE_INT("SHARE_OTD", m_nShareOtd_New);

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		return FALSE;
	}

	// 2. ��ȡCapitals��
	try
	{
		strSql.Format("select * from  capital where account = %s and currency = %s",
			g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

		bstrSQL = strSql.AllocSysString();
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		if (pRecordSet->adoEOF)
		{
			// ���ݿⷵ�صĽ��Ϊ��
			pRecordSet->Close();
			g_pLog->WriteRunLogEx(__FILE__,__LINE__,"��ȡCapitals������ʧ��!");
			return FALSE;
		}

		while(!pRecordSet->adoEOF)
		{	
			// 2.1 �ʽ����
			DB_GET_VALUE_FLOAT("BALANCE", m_fCptlBln_New);

			// 2.2 �ʽ����
			DB_GET_VALUE_FLOAT("AVAILABLE", m_fCptlAvl_New);

			// 2.3 ���׶���
			DB_GET_VALUE_FLOAT("TRD_FRZ", m_fCptlTrdFrz_New);

			// 2.4 ��;�ʽ�
			DB_GET_VALUE_FLOAT("OUTSTANDING", m_fCptlOutstanding_New);

			// 2.5 ��;����
			DB_GET_VALUE_FLOAT("OTD_AVL", m_fCptlOtdAvl_New);

			pRecordSet->MoveNext();
		}

		pRecordSet->Close();
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		return FALSE;
	}
	
	return TRUE;
}

BOOL CSZLOFVistor::GetOrderData()
{
	CTime tm=CTime::GetCurrentTime();
	CString strDate = tm.Format("%Y%m%d");
	BOOL bRet = TRUE;

	try
	{
		// 1.1 ����̨Orders���е������Ƿ���ί��һ��
		CString strSql;

		if (strcmp(m_szTrdId, "7M") == 0)
		{
			strSql.Format("select * from  orders where trd_date = %s and trd_id = '%s'and order_id = '%s'"
				" and account = %s and secu_intl = %s and order_qty = %s and order_amt = %s and order_frz_amt = %s and rlt_frz_amt = %s",
				strDate, m_szTrdId, m_pMsg[0].szOrderID, m_pMsg[0].szAccount, m_szSecu_Intl, m_szQty, m_szQty, m_szQty, m_szQty);
		}
		else if (strcmp(m_szTrdId, "7N") == 0)
		{
			strSql.Format("select * from  orders where trd_date = %s and trd_id = '%s'and order_id = '%s'"
				" and account = %s and secu_intl = %s and order_qty = %s and order_amt = %s and order_frz_amt = 0 and rlt_frz_qty = %s",
				strDate, m_szTrdId, m_pMsg[0].szOrderID, m_pMsg[0].szAccount, m_szSecu_Intl, m_szQty, m_szQty, m_szQty, m_szQty);
		}
		
		BSTR bstrSQL = strSql.AllocSysString();
		_RecordsetPtr pRecordSet;
		pRecordSet.CreateInstance(__uuidof(Recordset));

		if (!g_pDBConn->QueryData(bstrSQL, pRecordSet)) 
			return FALSE;

		_variant_t TheValue; //VARIANT��������
		char szTmp[100] = {0};

		if (pRecordSet->adoEOF)
		{
			// ���ݿⷵ�صĽ��Ϊ��
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.1 Fail!");
			pRecordSet->Close();
			return FALSE;
		}

		while(!pRecordSet->adoEOF)
		{								
			DB_GET_VALUE_FLOAT("RLT_FRZ_AMT", m_fOrder_RltFrzAmt);
			DB_GET_VALUE_FLOAT("RLT_FRZ_QTY", m_fOrder_RltFrzQty);
			pRecordSet->MoveNext();
		}
		pRecordSet->Close();	
	}
	catch(_com_error &e)
	{
		CString strMsg;
		strMsg.Format(_T("����������%s\n������Ϣ%s"), 
			(LPCTSTR)e.Description(),
			(LPCTSTR)e.ErrorMessage());

		g_pLog->WriteRunLogEx(__FILE__,__LINE__,strMsg.GetBuffer());

		bRet = FALSE;
	}

	return bRet;
}

BOOL CSZLOFVistor::ChkData()
{
	BOOL bRet = TRUE;

	// 1.1.����̨Orders���е������Ƿ���ί��һ��	
	if (!(bRet = GetOrderData()))
	{
		// ��ȡMatcing������ʧ��
		return bRet;
	}	

	// ��ȡ�ͻ����µĹɷݺ��ʽ�����
	if (!UpdateUserData())
	{
		g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "���²�������ʧ��!");
		bRet = FALSE;
	}
	else
	{
		// 1.2.���captial���С��ʽ�������
		if (m_fCptlBln_Old != m_fCptlBln_New)
		{
			bRet = FALSE;
			g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.2 Fail!");
		}
		
		if (strcmp(m_szTrdId, "7M") == 0)
		{
			/*
			�깺 ���������Ϣ
			1.1.����̨Orders���е������Ƿ���ί��һ��
			1.2.���captial���С��ʽ���
			1.3.���captial���С��ʽ���á�
			1.4.���captial���С����׶��᡿		
			*/

			// 1.3.���captial���С��ʽ���� AVAILABLE������ Ӧ�õ��� ������
			// ���ھ������⣬����1���ڵĲ���
			if (abs(CutFloatPnt(m_fCptlAvl_Old - m_fCptlAvl_New) - m_fOrder_RltFrzAmt) > 1)
			{				
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.3 Fail!");
			}

			// 1.4.���captial���С����׶��� TRD_FRZ������ Ӧ�õ��� ������
			if (abs(CutFloatPnt( m_fCptlTrdFrz_New - m_fCptlTrdFrz_Old) - m_fOrder_RltFrzAmt) > 1)
			{				
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.4 Fail!");
			}
		}
		else if (strcmp(m_szTrdId, "7N") == 0)
		{
			/*
			��� ���������Ϣ			
			1.3.���Shares���С��ɷ�������

			1.4.���Shares���С��ɷݿ��á����� ί������
			1.5.���Shares���С��ɷݶ��᡿���� ί������
			*/
			if (m_nShareBln_New != m_nShareBln_Old)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.3 Fail!");
			}
			
			if (abs(m_nShareAvl_New - m_nShareAvl_Old) != m_fOrder_RltFrzQty)
			{
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.4 Fail!");
			}

			if (abs(m_nShareTrdFrz_New - m_nShareTrdFrz_Old) != m_fOrder_RltFrzQty)
			{				
				bRet = FALSE;
				g_pLog->WriteRunLog(CHKPNT_MODE, LOG_WARN, "Chk 1.5 Fail!");
			}
		}
		else if (strcmp(m_szTrdId, "7J") == 0)
		{
			// �Ϲ�
		}
	}

	return bRet;
}

// �ͻ��ʽ�����
BOOL CSZLOFVistor::SaveCapital()
{
	// �ʽ��������1W, �ʽ��������1W

	// ��������
	char szTemp[2048] = {0};
	BOOL bRet = TRUE;

	// ƴ�ӷ��͸�KCXP�������ַ���
	sprintf_s(szTemp,"BEGIN:L0107022:04-14:42:45-051781  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=00256497d99e&F_OP_BRANCH=%s&F_CHANNEL=0"
		"&ACC_USER=%s&ACC_USER_ROLE=1&ACCOUNT=%s&CURRENCY=%s&BLN_ADJUST_AMT=10000.00&AVL_ADJUST_AMT=10000.00&OP_REMARK=]",
		g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
		g_pCfg->GetCustID().GetBuffer(), g_pCfg->GetAccount().GetBuffer(), m_szCurrency);

	if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
	{
		ParseKcxpRetMsg();
	}

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	return bRet;
}


BOOL CSZLOFVistor::SaveShares()
{
	// ��������
	char szTemp[2048] = {0};
	BOOL bRet = TRUE;

	// ƴ�ӷ��͸�KCXP�������ַ���		
	sprintf_s(szTemp,"BEGIN:L0301009:04-10:03:27-577242  [_CA=2.3&_ENDIAN=0&F_OP_USER=%s&F_OP_ROLE=2&F_SESSION=%s&F_OP_SITE=00256497d99e&F_OP_BRANCH=%s&F_CHANNEL=0"
		"&OPER_FLAG=1&CUSTOMER=%s&MARKET=%c&BOARD=%c&SECU_ACC=%s&ACCOUNT=%s&SECU_INTL=%s&SEAT=%s&EXT_INST=0&CQLB=0--����ɷ�&QTY=%s&AVG_PRICE=%s&OP_REMARK=]",
		g_pCfg->GetOpId().GetBuffer(), g_pKcxpConn->GetSession(), g_pCfg->GetBranch().GetBuffer(), 
		g_pCfg->GetCustID().GetBuffer(), m_szMarket_Board[0], m_szMarket_Board[1], g_pCfg->GetSecu_Acc_SZA(), g_pCfg->GetAccount().GetBuffer(),
		m_szSecu_Intl, g_pCfg->GetSZA_BIND_SEAT(), m_szQty, m_szPrice);

	// TODO:����L0301009 ִ�гɹ���Ҳ����FALSE����˷���ֵ��Ч�� �˺�����Զ����TRUE
	if (TRUE == (bRet = SendKcxpMsg(&szTemp[0])) && m_nRowNum > 0)
	{
		ParseKcxpRetMsg();
	}

	// ���ߣ��ȴ����ݿ����
	Sleep(g_pCfg->GetRefreshDBGap());

	return bRet;
}

void CSZLOFVistor::ParseKcxpRetMsg()
{
	m_pMsg = new MID_403_ORDER_RET_MSG[m_nRowNum];
	memset(m_pMsg, 0x00, sizeof(MID_403_ORDER_RET_MSG)*m_nRowNum);

	//ȡ���������
	int iRetCode = KCBP_MSG_OK;
	char szTemp[512] = {0};

	if (iRetCode = m_pKcxpConn->RsMore() == KCBP_MSG_OK)
	{
		int nRow = 0;
		while(nRow < m_nRowNum)
		{
			if(m_pKcxpConn->RsFetchRow() != KCBP_MSG_OK)
			{

				break;
			}

			SERVICE_KCXP_STRNCPY("BIZ_NO", szBizNo);
			SERVICE_KCXP_STRNCPY("ORDER_ID", szOrderID);
			SERVICE_KCXP_STRNCPY("ACCOUNT", szAccount);
			SERVICE_KCXP_STRNCPY("PRICE", szPrice);
			SERVICE_KCXP_STRNCPY("QTY", szQty);
			SERVICE_KCXP_STRNCPY("ORDER_AMT", szOrderAmt);
			SERVICE_KCXP_STRNCPY("ORDER_FRZ_AMT", szOrderFrzAmt);
			SERVICE_KCXP_STRNCPY("SEAT", szSeat);
			SERVICE_KCXP_STRNCPY("EXT_INST", szExtInst);
			SERVICE_KCXP_STRNCPY("EXT_ACC", szExtAcc);
			SERVICE_KCXP_STRNCPY("EXT_SUB_ACC", szExtSubAcc);
			SERVICE_KCXP_STRNCPY("EXT_FRZ_AMT", szExtFrzAmt);		
			nRow++;
		}		
	}
}