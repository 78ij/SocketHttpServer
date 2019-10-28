#include "ListenThread.h"
#include "main.h"
#include "WorkerThread.h"
wxThread::ExitCode ListenThread::Entry() {
	server = m_pHandler->server;
	while (!TestDestroy())
	{
		Socket * s = server->Accept();
		if (s != 0) {
			WorkerThread *pw = new WorkerThread(m_pHandler, this, s);
			if (pw->Run() != wxTHREAD_NO_ERROR)
			{
				wxLogError("Can't create the thread!");
				delete pw;
				pw = NULL;
			}
			else {
				struct sockaddr_in sa;
				int len = sizeof(sa);
				if (!getpeername(s->s_, (struct sockaddr *)&sa, &len))
				{
					
					wxMessageOutputDebug().Printf("New Thread init");

				}
				workers.push_back(pw);
			}
		}
		wxThread::This()->Sleep(20);

	}

	return (wxThread::ExitCode)0;     // success
}

ListenThread::~ListenThread() {
	{
		wxCriticalSectionLocker enter(workersCS); 
		for (WorkerThread * a : workers) {
			if(a->Delete() != wxTHREAD_NO_ERROR)
				wxLogError("Can't delete the thread!");
		}
		
	}
	while (1)
	{
		{ // was the ~MyThread() function executed?
			wxCriticalSectionLocker enter(workersCS);
			if (workers.size() == 0) break;
		}
		// wait for thread completion
		wxThread::This()->Sleep(1);
	}
	{
		wxCriticalSectionLocker enter(m_pHandler->m_pThreadCS);
		// the thread is being destroyed; make sure not to leave dangling pointers around
		m_pHandler->m_plistenthread = NULL;
	}

}