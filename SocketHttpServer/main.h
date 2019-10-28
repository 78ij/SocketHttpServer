#pragma once
#include <wx/wx.h>
#include "Socket.h"
#include "ListenThread.h"
#include <wx/thread.h>
#include <wx/file.h>

const int EVT_REQUEST = 100000;

wxDECLARE_EVENT(wxEVT_REQUEST_RECEIVED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_SOCKET_LISTENING, wxThreadEvent);

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
	wxTextCtrl *addrtext;
	wxTextCtrl *porttext;
	wxButton * startbutton;
	wxButton * stopbutton;
	wxButton * pathbutton;
	wxListBox *messagelist;
	void OnSave(wxCommandEvent& event);
	void OnStart(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnClickPathButton(wxCommandEvent &event);
	void OnClose(wxCloseEvent &event);
	void OnRequestReceviedEvent(wxCommandEvent& event);
protected:
	wxTextCtrl *pathtext;

	SocketServer *server;
	void StartListenThread();
	void StopListenThread();
	ListenThread *m_plistenthread;
	wxCriticalSection m_pThreadCS;
	friend class ListenThread;
	friend class WorkerThread;
	wxDECLARE_EVENT_TABLE();
};
enum
{
	ID_Save = 1,
	ID_StartServer,
	ID_STOPSERVER,
	ID_PATH
};



