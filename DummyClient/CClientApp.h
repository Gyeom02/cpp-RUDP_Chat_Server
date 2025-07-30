#pragma once
#include "CLoginDialog.h"
class CClientApp : public CWinApp
{
public:
	BOOL InitInstance() override;
	int ExitInstance() override;
};

extern CClientApp ExLoginApp;

class AfxThreadManager
{
public:
	static AfxThreadManager GetInstance() { static AfxThreadManager manager; return manager; }
	void AddThread(CWinThread* thread);
	void StopThreads();

private:
	vector<CWinThread*> _threads;
	
};

