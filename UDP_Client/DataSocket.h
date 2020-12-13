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
	//lpBuf  : 전송할 데이터가 들어있는 버퍼.
	//nBufLen : lpBuf에있는 데이터의 길이(바이트).
	//nHostPort : 소켓 응용 프로그램을 식별하는 포트입니다.
	//lpszHostAddress : 이 개체가 연결된 소켓의 네트워크 주소 : "ftp.microsoft.com"과 같은 컴퓨터 이름 또는 "128.56.22.8"과 같은 점으로 구분 된 번호.
	//nFlags : 호출 방법을 지정합니다.이 함수의 의미는 소켓 옵션과 nFlags 매개 변수에 의해 결정됩니다.후자는 다음 값 중 하나를 C ++ OR 연산자와 결합하여 생성됩니다.

	virtual void OnReceive(int nErrorCode);
};

