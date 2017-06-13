#pragma once
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
#include <memory>

#define LOGGER_FILE_NAME string("/game.log") 
using namespace std;

class Logger {
private:
	std::unique_ptr<std::ostream> output_stream{ nullptr };

public:
	Logger(std::string dir_path) :
		output_stream(std::make_unique<std::ofstream>(dir_path + LOGGER_FILE_NAME, ios::out | ios::app))
	{this->log("**** STARTING THE PROGRAM *****"); }

	// default destructor is OK

	template<typename T>
	void log(T info)
	{
		*output_stream << CurrentDateTime() << ":\t";
		*output_stream << info << std::endl;
	}
	static std::string Logger::CurrentDateTime();
	static const int TIME_BUF_SIZE = 100;
};


//https://cppcodetips.wordpress.com/2014/01/02/a-simple-logger-class-in-c/
//#define LOGGER CLogger::GetLogger()
///**
//*   Singleton Logger Class.
//*/
//class CLogger
//{
//public:
//	static void LogFirstMessage()
//	{
//		LOGGER->Log("**** STARTING THE PROGRAM *****");
//	}
//	/**
//	*   Logs a message
//	*   @param sMessage message to be logged.
//	*/
//	void Log(const std::string& sMessage);
//	/**
//	*   Variable Length Logger function
//	*   @param format string for the message to be logged.
//	*/
//	void Log(const char * format, ...);
//	/**
//	*   << overloaded function to Logs a message
//	*   @param sMessage message to be logged.
//	*/
//	CLogger& operator<<(const string& sMessage);
//	/**
//	*   Funtion to create the instance of logger class.
//	*   @return singleton object of Clogger class..
//	*/
//	static CLogger* CLogger::GetLogger(string dir_path);
//	static const std::string CLogger::CurrentDateTime();
//	static const int TIME_BUF_SIZE = 100;
//private:
//	/**
//	*    Default constructor for the Logger class.
//	*/
//	CLogger();
//	/**
//	*   copy constructor for the Logger class.
//	*/
//	CLogger(const CLogger&) {};             // copy constructor is private
//											/**
//											*   assignment operator for the Logger class.
//											*/
//	CLogger& operator=(const CLogger&) { return *this; };  // assignment operator is private
//														   /**
//														   *   Log file name.
//														   **/
//	static const std::string m_sFileName;
//	/**
//	*   Singleton logger class object pointer.
//	**/
//	static CLogger* m_pThis;
//	/**
//	*   Log file stream object.
//	**/
//	static ofstream m_Logfile;
//
//};