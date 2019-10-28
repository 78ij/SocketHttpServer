// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".

#include "main.h"
wxDEFINE_EVENT(wxEVT_REQUEST_RECEIVED, wxThreadEvent);

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BUTTON(ID_PATH, MyFrame::OnClickPathButton)
EVT_BUTTON(ID_Save, MyFrame::OnSave)
EVT_BUTTON(ID_StartServer, MyFrame::OnStart)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_CLOSE(MyFrame::OnClose)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_COMMAND(EVT_REQUEST, wxEVT_COMMAND_TEXT_UPDATED, MyFrame::OnRequestReceviedEvent)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	MyFrame *frame = new MyFrame("Socket Http Server", wxPoint(300, 50), wxSize(800, 600));
	frame->Show(true);
	return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size,
		wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN)
{
	wxMenu *menuOpt = new wxMenu;
	menuOpt->Append(ID_Save, "&Save...\tCtrl-F");
	menuOpt->AppendSeparator();
	menuOpt->Append(ID_Save, "Start...\tCtrl-T");
	menuOpt->AppendSeparator();
	menuOpt->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuOpt, "&Options");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *linesizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *linesizer2 = new wxBoxSizer(wxHORIZONTAL);
	pathtext = new wxTextCtrl(this, -1, "����·������������߰��ұ߰�ťѡ��", wxDefaultPosition,
		wxSize(700, 20), 0, wxDefaultValidator, "AddrText");
	startbutton = new wxButton(this, ID_StartServer, "��ʼ����", wxDefaultPosition, wxSize(50, 20));
	stopbutton = new wxButton(this, ID_STOPSERVER, "ֹͣ����", wxDefaultPosition, wxSize(50, 20));
	pathbutton = new   wxButton(this, ID_PATH, "...", wxDefaultPosition, wxSize(10, 20));
	addrtext = new wxTextCtrl(this, -1, "������ַ��Ĭ��127.0.0.1)", wxDefaultPosition,
		wxSize(290, 20), 0, wxDefaultValidator, "AddrText");
	porttext = new wxTextCtrl(this, -1, "�����˿ڣ�Ĭ��45678)", wxDefaultPosition,
		wxSize(290, 20), 0, wxDefaultValidator, "PortText");
	linesizer->Add(
		addrtext,
		1,            // ��ֱ���������
		wxEXPAND |    // ˮƽ�����Զ����
		wxALL,        //   ���ܶ��б߿�
		10);         // �߿���Ϊ10

	linesizer->Add(
		porttext,
		1,            // ��ֱ���������
		wxEXPAND |    // ˮƽ�����Զ����
		wxALL,        //   ���ܶ��б߿�
		10);         // �߿���Ϊ10

	linesizer->Add(
		startbutton,
		1,            // ��ֱ���������
		wxEXPAND |    // ˮƽ�����Զ����
		wxALL,        //   ���ܶ��б߿�
		10);         // �߿���Ϊ10

	linesizer->Add(
		stopbutton,
		1,            // ��ֱ���������
		wxEXPAND |    // ˮƽ�����Զ����
		wxALL,        //   ���ܶ��б߿�
		10);         // �߿���Ϊ10
	linesizer2->Add(
		pathtext,
		1,
		wxEXPAND |
		wxALL,
		10
	);
	linesizer2->Add(
		pathbutton,
		1,
		wxEXPAND |
		wxALL,
		10
	);
	topsizer->Add(
		linesizer2,
		0,                // ���򲻿�����
		wxALIGN_CENTER); //�ޱ߿򣬾��ж���


	topsizer->Add(
		linesizer,
		0,                // ���򲻿�����
		wxALIGN_CENTER); //�ޱ߿򣬾��ж���
	
	messagelist = new wxListBox(this, -1,wxDefaultPosition,wxSize(700,400));
	topsizer->Add(
		messagelist,
		0,
		wxEXPAND | wxALL,
		10);
	SetSizer(topsizer);      // ����topsizer��Ϊ����


}

void MyFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("Socket Http Server V0.1\nWritten By Sun Jiamu",
		"About", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnSave(wxCommandEvent& event)
{
	wxLogMessage("Hello world from wxWidgets!");
}


void MyFrame::OnClickPathButton(wxCommandEvent& event) {
	wxDirDialog dirDlg(this, wxT("ѡ��һ��Ŀ¼"), wxT("C:\\"), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	int ret = dirDlg.ShowModal();
	if (ret == wxID_OK) {
		wxString path = dirDlg.GetPath();
		pathtext->WriteText(path);
	}
}

void MyFrame::OnClose(wxCloseEvent &event) {

	{
		wxCriticalSectionLocker enter(m_pThreadCS);
		if (m_plistenthread)         // does the thread still exist?
		{
			wxMessageOutputDebug().Printf("MYFRAME: deleting thread");
			if (m_plistenthread->Delete() != wxTHREAD_NO_ERROR)
				wxLogError("Can't delete the thread!");
		}
	}       // exit from the critical section to give the thread
			// the possibility to enter its destructor
			// (which is guarded with m_pThreadCS critical section!)
	while (1)
	{
		{ // was the ~MyThread() function executed?
			wxCriticalSectionLocker enter(m_pThreadCS);
			if (!m_plistenthread) break;
		}
		// wait for thread completion
		wxThread::This()->Sleep(1);
	}
	delete server;
	Destroy();
}

void MyFrame::StartListenThread() {
	m_plistenthread = new ListenThread(this);

	if (m_plistenthread->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError("Can't create the thread!");
		delete m_plistenthread;
		m_plistenthread = NULL;
	}

}

void MyFrame::OnRequestReceviedEvent(wxCommandEvent& event) {
	wxMessageOutputDebug().Printf("!!!!!!!!!!!");
	messagelist->InsertItems(1, &event.GetString(),messagelist->GetCount());
}

void MyFrame::OnStart(wxCommandEvent &event) {
	if (m_plistenthread != NULL) {
		wxMessageDialog errdlg(this, wxT("�������Ѿ���������������"), wxT("����"), wxOK);
		errdlg.ShowModal();
		return;
	}

	try {
		std::string host;
		std::string port;
		if (addrtext->GetLineText(0) == "������ַ��Ĭ��127.0.0.1)") host = "127.0.0.1";
		else host = addrtext->GetLineText(0);
		if (porttext->GetLineText(0) == "�����˿ڣ�Ĭ��45678)") port = "45678";
		else host = porttext->GetLineText(0);
		int portint;
		portint = std::stoi(port);
		server = new SocketServer(host, portint, 128,NonBlockingSocket);
		StartListenThread();
		wxMessageDialog errdlg(this, wxT("�����ɹ���\n������������\n" + host + ":" + port), wxT("�������Ѿ�������"), wxOK);
		errdlg.ShowModal();
	}catch (std::exception e) {
		wxMessageDialog errdlg(this, wxT("δ֪����"),wxT("����"), wxOK);
		errdlg.ShowModal();
		return;
	}
}