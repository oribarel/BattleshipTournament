#include "Logger.h"
#include <time.h>
/*const string CLogger::m_sFileName = "game.log";
CLogger* CLogger::m_pThis = nullptr;
ofstream CLogger::m_Logfile;
CLogger::CLogger()
{

}
CLogger* CLogger::GetLogger(string dir_path) {
	if (m_pThis == nullptr) {
		m_pThis = new CLogger();
		m_Logfile.open((dir_path+m_sFileName).c_str(), ios::out | ios::app);
	}
	return m_pThis;
}

void CLogger::Log(const char * format, ...)
{
	char* sMessage = nullptr;
	int nLength = 0;
	va_list args;
	va_start(args, format);
	//  Return the number of characters in the string referenced the list of arguments.
	// _vscprintf doesn't count terminating '\0' (that's why +1)
	nLength = _vscprintf(format, args) + 1;
	sMessage = new char[nLength];
	vsprintf_s(sMessage, nLength, format, args);
	//vsprintf(sMessage, format, args);
	m_Logfile << CurrentDateTime() << ":\t";
	m_Logfile << sMessage << "\n";
	va_end(args);

	delete[] sMessage;
}

void CLogger::Log(const string& sMessage)
{
	m_Logfile << CurrentDateTime() << ":\t";
	m_Logfile << sMessage << "\n";
}

CLogger& CLogger::operator<<(const string& sMessage)
{
	m_Logfile << "\n" << CurrentDateTime() << ":\t";
	m_Logfile << sMessage << "\n";
	return *this;
}*/

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
std::string Logger::CurrentDateTime()
{
	time_t     now = time(nullptr);
	struct tm  tstruct;
	char       buf[TIME_BUF_SIZE];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}