#include "stdafx.h"
#include "DataSocket.h"
#include"UDPClient_thd.h"
#include "UDPClient_thdDlg.h"

CDataSocket::CDataSocket(CUDPClient_thdDlg* pDlg)
{
	m_pDlg = pDlg;
}


CDataSocket::~CDataSocket()
{
}
//int CDataSocket::SendToEx(const void* lpBuf, int uBufLen, UINT nHostPort, LPCTSTR lpzHostAddress = NULL, int nFlags = 0)
//{
//
//}


void CDataSocket::OnReceive(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CSocket::OnReceive(nErrorCode);
	m_pDlg->ProcessReceive(this, nErrorCode);
}
