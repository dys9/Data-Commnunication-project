
// UDPClient_thdDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "UDPClient_thd.h"
#include "UDPClient_thdDlg.h"
#include "afxdialogex.h"
#include "DataSocket.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
////////////////////////////////////////////
CCriticalSection tx_cs;
CCriticalSection rx_cs;
CWinThread *pTime_th;
int sq=0;
/////////////////////////////////////////////
// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUDPClient_thdDlg ��ȭ ����



CUDPClient_thdDlg::CUDPClient_thdDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UDPCLIENT_THD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUDPClient_thdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipaddr);
	DDX_Control(pDX, IDC_EDIT1, m_tx_edit_short);
	DDX_Control(pDX, IDC_EDIT3, m_tx_edit);
	//DDX_Control(pDX, IDC_EDIT2, m_rx_edit);
	DDX_Control(pDX, IDC_EDIT4, m_tx_Thread);
	DDX_Control(pDX, IDC_EDIT5, m_rx_Thread);
}

BEGIN_MESSAGE_MAP(CUDPClient_thdDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CUDPClient_thdDlg::OnBnClickedSend)
	ON_BN_CLICKED(IDC_CLOSE, &CUDPClient_thdDlg::OnBnClickedClose)
END_MESSAGE_MAP()

UINT TXThread(LPVOID arg)	//������ �κ�, �����ϱ� �� ������ �κ�//////////////////////////////
{
	ThreadArg_Send *pArg = (ThreadArg_Send *)arg;   //ThreadArg ��ü
	CStringList *plist = pArg->pList;
	CUDPClient_thdDlg *pDlg = (CUDPClient_thdDlg*)pArg->pDlg;

	CString message;
	pDlg->m_tx_edit.GetWindowTextW(message);
	CString SocketAddress = _T("127.0.0.1");	//addr
	UINT SocketPort = 8000;	//Port

	CStringList * stringlist;

	CList<Frame> *alist = pArg->pAckList;
	CList<Frame> *ack_list;
	while (pArg->Thread_run) {
		POSITION pos = plist->GetHeadPosition();
		POSITION current_pos;

		while (pos != NULL)
		{
			Frame temp;
			memset(&temp, 0, sizeof(temp));

			current_pos = pos;

			tx_cs.Lock();
			CString str = plist->GetNext(pos);
			tx_cs.Unlock();

			stringlist = new CStringList;
			stringlist->AddTail(str);

			int length;
			int length_w = str.GetLength();

			if (str.Find('`') >= 0 || str.GetLength() == 17)
			{
				str.Remove('`');
				temp.end_game = '`';
			}
			if (str.Find('/') >= 0)
			{
				str.Remove('/');
				temp.end_game = '/';
			}

			CString message;
			pDlg->m_tx_edit.GetWindowTextW(message);
			pDlg->m_tx_edit.SetWindowText(message);

			wchar_t* dataW = new wchar_t[length_w];
			dataW = (wchar_t*)(str.GetString());
			length = WideCharToMultiByte(CP_ACP, 0, dataW, -1, NULL, 0, NULL, NULL);

			char *dataMB = new char[length];
			memset(dataMB, 0, sizeof(dataMB));
			WideCharToMultiByte(CP_ACP, 0, dataW, -1, dataMB, length, 0, 0);
			/////////////////////////////////
			memcpy(temp.data, dataMB, 16);////
			////////////////////////////////

			temp.sqN = sq++;
			temp.flag_s = SEND;
			temp.checkSum = Checksum((unsigned short *)&temp, sizeof(Frame));

			CString tx;
			tx.Format(_T("seqNum:%d, checksum:%d, flag_s:%c\r\n"), (int)temp.sqN, temp.checkSum, temp.flag_s);

			CString TXmessage;
			pDlg->m_tx_edit.GetWindowText(TXmessage);

			pDlg->m_pDataSocket->SendToEx(&temp, sizeof(temp), SocketPort, SocketAddress);

			int len = pDlg->m_tx_Thread.GetWindowTextLengthW();
			pDlg->m_tx_Thread.SetSel(len, len);
			pDlg->m_tx_Thread.ReplaceSel(tx);
			pDlg->m_tx_Thread.SetWindowTextW(tx);


			pDlg->m_tx_Thread.LineScroll(pDlg->m_tx_Thread.GetLineCount());
			pDlg->m_tx_edit.LineScroll(pDlg->m_tx_edit.GetLineCount());
			


			Sleep(50);
			char status = ACK;
			POSITION ack_Pos = alist->GetHeadPosition();

			POSITION ack_Current_pos;
			bool flag_t = 0;
			pTime_th = AfxBeginThread(TimeOutCheckThread, (LPVOID)&flag_t);
			while (flag_t != 1) 
			{
				if (ack_Pos == NULL)
					AfxMessageBox(_T("��ũ ����Ʈ ���̴�"));
				if (ack_Pos != NULL)
				{
					ack_Current_pos = ack_Pos;
					Frame ack_temp = alist->GetNext(ack_Pos);
					CString ack_txt;
					ack_txt.Format(_T("Receive->ACK Sq : %d  flag : %c\r\n"), ack_temp.sqN, ack_temp.flag_s);
					pDlg->m_rx_Thread.SetWindowTextW(ack_txt);
					status = ack_temp.flag_s;
					alist->RemoveAt(ack_Current_pos);
				}
			}
			if (flag_t == 1 || status != ACK)
			{
				temp.flag_s = RESEND;
				tx_cs.Lock();
				pDlg->m_pDataSocket->SendToEx(&temp, sizeof(temp), SocketPort, SocketAddress);
				tx_cs.Unlock();
			}
			plist->RemoveAt(current_pos);
		}
		Sleep(10);
	}
	return 0;
}
static CString Buf = _T("");
UINT RXThread(LPVOID arg)//Receive (Rx)
{
	ThreadArg_Receive *pArg = (ThreadArg_Receive *)arg; //������ ���� ������ pArg�����Ͽ� �Ű����� arg�� �����Ѵ�.
	CList<Frame> *plist = pArg->pStringList; //����Ʈ ������ plist�� �����Ͽ� pArg->pList�� �����Ѵ�.
	CUDPClient_thdDlg *pDlg = (CUDPClient_thdDlg *)pArg->pDlg; //dlg������ pDlg�����Ͽ� pArg->pList�� �����Ѵ�.
	UINT port = 8000; CString addr = _T("127.0.0.1");

	CStringList * stringlist;

	while (pArg->Thread_run)
	{
		POSITION pos = plist->GetHeadPosition();
		POSITION current_pos;
		while (pos != NULL)
		{
			Frame ack_frame;
			current_pos = pos;

			rx_cs.Lock(); //Critical Section method Lock()�� �ٸ������尡 ���� �� �ϰ� ��
			Frame frame = plist->GetNext(pos); //pos�� ������ġ�� plist�� ����Ű���ϰ� �� ���� str������ ��������ش�.
			rx_cs.Unlock(); //�ٸ� �������� ������ �����ϰ� �Ѵ�.

			unsigned short kk = Checksum((unsigned short*)&frame, sizeof(Frame));
			
			if (kk ==0)//CHECKSUM OK
			{
				CString temp;

				ack_frame.sqN = frame.sqN + 1;
				ack_frame.flag_s = ACK;
				rx_cs.Lock();
				pDlg->m_pDataSocket->SendToEx(&ack_frame, sizeof(Frame), port, addr);
				rx_cs.Unlock();

				temp.Format(_T("seqNum: %d, flag_s :%c\r\n"), (int)ack_frame.sqN, ack_frame.flag_s);
				pDlg->m_tx_Thread.SetWindowTextW(temp);

				stringlist = new CStringList;
				stringlist->AddTail((CString)frame.data);

				int len = pDlg->m_rx_Thread.GetWindowTextLengthW();
			
				pDlg->m_rx_Thread.SetSel(len, len);
				temp.Format(_T("seqNum:%d, checksum:%d, flag_s:%c, end : %c\r\n"), (int)frame.sqN, frame.checkSum,frame.flag_s, frame.end_game);
				pDlg->m_rx_Thread.ReplaceSel(temp);

				CString message;
				pDlg->m_tx_edit.GetWindowText(message); //dialoge�� �����Ͽ� â�� ������ message�� ����

				POSITION fpos = stringlist->GetHeadPosition();
				POSITION fcurrent_pos;

				while (fpos != NULL)
				{
					fcurrent_pos = fpos;
					CString str = stringlist->GetNext(fpos);
					//AfxMessageBox(str);
					if ((frame.end_game != '`') && (frame.end_game != '/') && (str.GetLength() <= 16))
					{
						str = _T("���� : ") + str;
						message += str + _T("\r\n");
						message += "\n";
					}
					if ((frame.end_game == '`') )
					{
						if (str.GetLength() == 17)
						{
							str = str.Mid(0, 16);
						}
						Buf = Buf + str;
						//AfxMessageBox(Buf);
					}
					if (frame.end_game == '/')
					{
						Buf = Buf + str;
						//AfxMessageBox(Buf);
						str = Buf;
						Buf = _T("");
						str.Remove('`'); str.Remove('/');
						str = _T("���� : ") + str;
						message += str + _T("\r\n");
						message += "\n";
					}
					stringlist->RemoveAt(fcurrent_pos);
				}


				pDlg->m_tx_edit.SetWindowText(message); //�޼����� â�� ����Ѵ�
				pDlg->m_tx_edit.LineScroll(pDlg->m_tx_edit.GetLineCount()); //�ؽ�Ʈ ��ũ��

				plist->RemoveAt(current_pos); //����Ʈ���� ���� ������ ����
			}
			else
			{
				ack_frame.sqN = frame.sqN+1;
				ack_frame.flag_s = NACK;
				rx_cs.Lock();
				pDlg->m_pDataSocket->SendToEx(&ack_frame, sizeof(Frame), port, addr);
				break;
				rx_cs.Unlock();
			}
			
		}
		Sleep(10);
	}
	return 0;
}



// CUDPClient_thdDlg �޽��� ó����

BOOL CUDPClient_thdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

									// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
									//arg1�� trans��
	CStringList* strList = new CStringList;
	arg1.pList = strList;
	arg1.Thread_run = 1;
	arg1.pDlg = this;
	CList<Frame>* ackList1 = new CList<Frame>;
	arg1.pAckList = ackList1;


	CList<Frame>* frameList = new CList<Frame>;
	arg2.pStringList = frameList;
	arg2.Thread_run = 1;
	arg2.pDlg = this;
	CList<Frame>* ackList2 = new CList<Frame>;
	arg2.pAckList = ackList2;

	m_pDataSocket = NULL;
	m_ipaddr.SetWindowTextW(_T("127.0.0.1"));


	pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1);
	pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2);


	if (m_pDataSocket == NULL) 
	{
		m_pDataSocket = new CDataSocket(this);

		CString addr;
		m_ipaddr.GetWindowText(addr);
		if (m_pDataSocket->Create(8001, SOCK_DGRAM))
		{
			m_tx_edit_short.SetFocus();
		}
		else {
			delete m_pDataSocket;
			m_pDataSocket = NULL;
		}
	}
	else {
		//MessageBox(_T("������ �̹� ���ӵ�!"), _T("�˸�"), MB_ICONINFORMATION);
		m_tx_edit_short.SetFocus();
	}

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CUDPClient_thdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CUDPClient_thdDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CUDPClient_thdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUDPClient_thdDlg::OnBnClickedSend()
{
	CString tx_Smessage = NULL;
	m_tx_edit_short.GetWindowText(tx_Smessage);

	int length = 0;
	CString *temp;

	length = tx_Smessage.GetLength();


	if ((length <= 16)) {	//���࿡ ���ڿ����̰� 16����Ʈ�̰ų�
							//�Ǵ� 16����Ʈ ���� ������ ���̸� 1�� �־�����.
		CString *temp = new CString[1];
		temp[0] = tx_Smessage.Mid(0, 16);
		tx_cs.Lock();
		arg1.pList->AddTail(temp[0]);
		tx_cs.Unlock();
	}
	else if ((length > 16) && (length % 16) != 0) {	//���� ���ڿ� ���̰� 16���� Ŭ ��� ���̰� 
		CString *temp = new CString[(length / 16) + 1];

		for (int i = 0; i < (length / 16) + 1; i++)
		{
			if (i == (length / 16))
				temp[i] = tx_Smessage.Mid(i * 16, i * 16 + 16) + _T("/");
			else
				temp[i] = tx_Smessage.Mid(i * 16, i * 16 + 16) + _T("`");
		}

		tx_cs.Lock();
		for (int i = 0; i < (length / 16) + 1; i++) {
			arg1.pList->AddTail(temp[i]);
		}
		tx_cs.Unlock();
	}
	else {
		CString *temp = new CString[(length / 16) + 1];
		for (int i = 0; i < (length / 16) + 1; i++)
		{
			if (i == (length / 16))
				temp[i] = tx_Smessage.Mid(i * 16, i * 16 + 16) + _T("/");
			else
				temp[i] = tx_Smessage.Mid(i * 16, i * 16 + 16) + _T("`");
		}

		tx_cs.Lock();
		for (int i = 0; i < (length / 16) + 1; i++) {
			arg1.pList->AddTail(temp[i]);
		}
		tx_cs.Unlock();
	}
	m_tx_edit_short.SetWindowText(_T(""));//m_tx_edit_short ����
	m_tx_edit_short.SetFocus();//�Է� ���·� �����


	tx_Smessage = _T("Ŭ�� : ") + tx_Smessage;
	tx_Smessage += "\r\n";

	int len = m_tx_edit.GetWindowTextLengthW();
	m_tx_edit.SetSel(len, len);
	m_tx_edit.ReplaceSel(tx_Smessage);//���� tx_Smessage ����
}


void CUDPClient_thdDlg::OnBnClickedClose()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_pDataSocket == NULL) //m_pDataSocket �� NULL���¶��
	{
		MessageBox(_T("������ ���� ����!"), _T("�˸�"), MB_ICONINFORMATION);
	}
	else
	{
		arg1.Thread_run = 0; //To disconnect set Thread_run = 1
		arg2.Thread_run = 0; //To disconnect set Thread_run = 1

		m_pDataSocket->Close();
		delete m_pDataSocket;
		m_pDataSocket = NULL;

		int len = m_tx_edit.GetWindowTextLengthW();
		CString message = _T("\n### ���� ���� ###\r\n\r\n");
		m_tx_edit.SetSel(len, len); //Set Cursor place
		m_tx_edit.ReplaceSel(message);
	}
}


void CUDPClient_thdDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	Frame get;
	Frame* Data = &get;

	int nbytes;
	CString SocketAddress = _T("127.0.0.1");
	UINT SocketPort = 8000;

	nbytes = pSocket->ReceiveFromEx(&get, sizeof(Frame), SocketAddress, SocketPort, 0);

	if (get.flag_s == ACK || get.flag_s == NACK)
	{
		tx_cs.Lock();
		arg1.pAckList->AddTail(*Data);
		CString temp;
		temp.Format(_T("AckFrame's Seq.No : %d, Flag_s : %c"), get.sqN, get.flag_s);
		AfxMessageBox(temp);
		tx_cs.Unlock();
	}
	else if (get.sqN == SEND || get.flag_s == RESEND)
	{
		rx_cs.Lock();
		arg2.pStringList->AddTail(*Data);
		rx_cs.Unlock();
	}
	else {
		rx_cs.Lock();
		arg2.pStringList->AddTail(*Data);
		arg2.pAckList->AddTail(*Data);
		rx_cs.Unlock();
	}
}
BOOL CUDPClient_thdDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	return CDialogEx::PreTranslateMessage(pMsg);
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			OnBnClickedSend();
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			OnOK();
		}
	}
}


unsigned short Checksum(unsigned short *ptr, int size)
{
	Frame *frame = (Frame *)ptr; //checksum�� ������ ����
	unsigned short checksum = 0;
	unsigned short oddbyte;
	register short result;
	while (size > 1)
	{
		checksum += *ptr++;
		size -= 2;
	}

	if (size == 1)
	{
		oddbyte = 0;
		*((u_char*)&oddbyte) = *((u_char*)ptr);//oddbyte�� ����Ű�� �ּҿ� ����ִ� �ſ� ptr�� ����Ű�� ������ ����
		checksum += oddbyte;
	}
	checksum = (checksum >> 16) + checksum; //carryó��
	checksum = ~checksum; //compliment

	return checksum;
}
UINT TimeOutCheckThread(LPVOID arg)
{
	int *flag_t = (int *)arg;

	Sleep(5000);
	(*flag_t) = 1 - (*flag_t);
	//*(bool*)arg = false;
	return 0;
}
