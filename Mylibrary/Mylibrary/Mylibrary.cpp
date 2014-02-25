// Mylibrary.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Mylibrary.h"
#include "XThreadBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

class  MyThread : XThreadBase
{
public:

	MyThread()
	{
		m_iCnt = 0;
		StartThread();
	}

protected:
	virtual void ThreadProcMain(void);
private:
	int m_iCnt;
};

void MyThread::ThreadProcMain( void )
{
	m_iCnt++;
	printf("\n%d\n",m_iCnt);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
	}

	MyThread  ss;
	Sleep(5000);

	return nRetCode;
}
