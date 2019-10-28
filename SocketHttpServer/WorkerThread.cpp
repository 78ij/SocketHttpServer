#include "WorkerThread.h"
#include "main.h"
#include<time.h>

wxThread::ExitCode WorkerThread::Entry() {
	time_t curtime;
	time(&curtime);
	struct sockaddr_in sa;
	int len = sizeof(sa);
	getpeername(s->s_, (struct sockaddr *)&sa, &len);
	wxMessageOutputDebug().Printf("Thread Socket: %s:%d", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
	while (!TestDestroy())
	{
		if (time(NULL) - curtime >= 10) {
			s->Close();
			return 0;
		}
		std::string request;
		request = s->ReceiveBytes();
		if (request.size() != 0) {
			time(&curtime);
			http_message m(request);
			m.parse();

			bool keepalive;
			if (m.headers.at("Connection").compare("keep-alive") == 0) keepalive = true;
			else keepalive = false;
			string requestedpath = std::string(m_pHandler->pathtext->GetValue());
			requestedpath += m.URL;
			if (m.URL.compare("/") == 0) {
				requestedpath += "index.html";
			}
			if (!wxFile::Exists(requestedpath)) {

				http_message ret;
				ret.version = m.version;
				ret.status = "404";
				ret.statusdesc = "Not Found";
				ret.build();
				s->SendBytes(ret.message);
				wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, EVT_REQUEST);
				event.SetString(std::string(inet_ntoa(sa.sin_addr)) + ":" + std::to_string(ntohs(sa.sin_port)) + " - " +
					m.version + " " + m.method + " " + m.URL + " ---- " + ret.status);
				m_pHandler->GetEventHandler()->AddPendingEvent(event);
				s->Close();
				break;
			}
			else {
				http_message ret(true);
				ret.version = m.version;
				ret.status = "200";
				ret.statusdesc = "OK";

				wxFile fd;
				if (fd.Open(requestedpath)) {
					fd.Read(ret.content,fd.Length());
				}
				ret.headers.insert(std::make_pair("Connection", "Keep-Alive"));
				ret.headers.insert(std::make_pair("Content-Length", std::to_string(fd.Length())));
				ret.build();
				s->SendBytes(ret.message);
				wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, EVT_REQUEST);
				event.SetString(std::string(inet_ntoa(sa.sin_addr)) + ":" + std::to_string(ntohs(sa.sin_port)) + " - " +
					m.version + " " + m.method + " " + m.URL + " ---- " + ret.status);				m_pHandler->GetEventHandler()->AddPendingEvent(event);
			}
		}
		wxThread::This()->Sleep(20);
	}
	return (wxThread::ExitCode)0;     // success
}

WorkerThread::~WorkerThread() {
	wxMessageOutputDebug().Printf("Work Thread Exit");
	wxCriticalSectionLocker enter(m_plthread->workersCS);
	// the thread is being destroyed; make sure not to leave dangling pointers around
	int index = 0;
	for (WorkerThread * w : m_plthread->workers) {
		if (w == this) break;
		index++;
	}
	m_plthread->workers.erase(m_plthread->workers.begin() + index);
	delete s;
}