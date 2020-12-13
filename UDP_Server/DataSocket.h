#pragma once
#include "afxsock.h"

class CUDPServer_thdDlg;

class CDataSocket :public CSocket
{
public:
	CDataSocket(CUDPServer_thdDlg *pDlg);
	CUDPServer_thdDlg *m_pDlg;
	~CDataSocket();
	void OnReceive(int nErrorCode);
	void OnClose(int nErrorCode);
	UINT Socket_Port = 8000;
	struct HEADER
	{

	};
};

