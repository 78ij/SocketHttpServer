#pragma once
#include <wx/wx.h>
#include <wx/thread.h>
#include "Socket.h"
#include <vector>




class WorkerThread;
class MyFrame;

class ListenThread : public wxThread {
public:
	SocketServer *server;
	ListenThread(MyFrame *handler) :wxThread(wxTHREAD_DETACHED), m_pHandler(handler) {}
	~ListenThread();
private:
	virtual ExitCode Entry();
	MyFrame *m_pHandler;
protected:
	wxCriticalSection workersCS;
	std::vector<WorkerThread *> workers;
	friend class WorkerThread;
};