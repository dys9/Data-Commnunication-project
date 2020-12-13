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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CSocket::OnReceive(nErrorCode);
	m_pDlg->ProcessReceive(this, nErrorCode);
}
