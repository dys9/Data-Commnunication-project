#pragma once
#include "afxsock.h"
class CUDPClient_thdDlg;
class CDataSocket :
	public CSocket
{
public:
	CDataSocket(CUDPClient_thdDlg* pDlg);
	~CDataSocket();
	CUDPClient_thdDlg* m_pDlg;
	UINT Socket_Port = 8000;
	//int SendToEx(const void* lpBuf, int uBufLen, UINT nHostPort, LPCTSTR lpzHostAddress = NULL, int nFlags = 0);
	//lpBuf  : ������ �����Ͱ� ����ִ� ����.
	//nBufLen : lpBuf���ִ� �������� ����(����Ʈ).
	//nHostPort : ���� ���� ���α׷��� �ĺ��ϴ� ��Ʈ�Դϴ�.
	//lpszHostAddress : �� ��ü�� ����� ������ ��Ʈ��ũ �ּ� : "ftp.microsoft.com"�� ���� ��ǻ�� �̸� �Ǵ� "128.56.22.8"�� ���� ������ ���� �� ��ȣ.
	//nFlags : ȣ�� ����� �����մϴ�.�� �Լ��� �ǹ̴� ���� �ɼǰ� nFlags �Ű� ������ ���� �����˴ϴ�.���ڴ� ���� �� �� �ϳ��� C ++ OR �����ڿ� �����Ͽ� �����˴ϴ�.

	virtual void OnReceive(int nErrorCode);
};

