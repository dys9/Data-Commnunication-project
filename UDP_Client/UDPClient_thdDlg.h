
// UDPClient_thdDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxcoll.h"
#define ACK 'A'
#define NACK 'N'
#define SEND 'S'
#define RESEND 'R'
//////////////////////////////////////////
//struct ThreadArg
//{
//	CStringList* pList;
//	CDialogEx* pDlg;
//	int Thread_run;
//};

typedef struct Frame
{
	int sqN = 0;
	char data[17];
	unsigned short checkSum;
	char flag_s;
	char end_game;
}Frame;

struct ThreadArg_Send
{
	CStringList* pList;
	CDialogEx* pDlg;
	int Thread_run;
	CList<Frame> *pAckList;
};

struct ThreadArg_Receive
{
	CList<Frame> *pStringList;
	CDialogEx* pDlg;
	int Thread_run;
	CList<Frame> *pAckList;
};


class CDataSocket;

///////////////////////////////////////////
// CUDPClient_thdDlg 대화 상자
class CUDPClient_thdDlg : public CDialogEx
{
// 생성입니다.
public:
	CUDPClient_thdDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDPCLIENT_THD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSend();
	afx_msg void OnBnClickedClose();
	CIPAddressCtrl m_ipaddr;
	CEdit m_tx_edit_short;
	CEdit m_tx_edit;
	CEdit m_rx_edit;
	CWinThread *pThread1, *pThread2;
	//ThreadArg arg1, arg2;
	ThreadArg_Send arg1;
	ThreadArg_Receive arg2;
	CDataSocket *m_pDataSocket;
	void ProcessReceive(CDataSocket* pSocket, int nErrorCode);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEdit m_tx_Thread;
	CEdit m_rx_Thread;
};
unsigned short Checksum(unsigned short *ptr, int size);
UINT TimeOutCheckThread(LPVOID arg);