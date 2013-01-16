#pragma once

#include "IPCKDGateWayVistor.h"
#include "LoginVistor.h"
#include "BuyVistor.h"
#include "IRobotDlg.h"

class CMyService
{
public:
	CMyService(void);
	~CMyService(void);

	void Init();
	void RegisterProtocal(IPCKDGateWayVistor *);
	void Run();

	CIRobotDlg * GetDlg() const { return m_pDlg; }
	void SetDlg(CIRobotDlg * val) { m_pDlg = val; }

private:
	CArray <IPCKDGateWayVistor*, IPCKDGateWayVistor*> m_ptServiceArray;
	CIRobotDlg *m_pDlg;

	int m_nTotalCaseCnt;
	int m_nSuccCaseCnt;
	int m_nFailCaseCnt;
	
	CLoginVistor LoginVistor;
	CBuyVistor BuyVistor;
};