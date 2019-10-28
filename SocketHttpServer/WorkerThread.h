#pragma once
#include <wx/wx.h>
#include <wx/thread.h>
#include "Socket.h"
#include "http_message.h"
#include "main.h"

class ListenThread;

class WorkerThread : public wxThread {
public:
	Socket *s;
	WorkerThread(MyFrame *handler, ListenThread *l, Socket *socket) :wxThread(wxTHREAD_DETACHED), m_pHandler(handler), m_plthread(l),s(socket) {}
	~WorkerThread();
private:
	virtual ExitCode Entry();
	MyFrame *m_pHandler;
	ListenThread *m_plthread;
};
