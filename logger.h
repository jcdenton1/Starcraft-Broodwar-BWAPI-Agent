#pragma once

#include <fstream>

class Logger
{
public:
	enum e_logType { LOG_ERROR, LOG_WARNING, LOG_INFO };

	Logger(const char *filename = "ij_log.txt")
	{
		m_logFile.open(filename);
		
		if (!m_logFile.is_open())
		{
			// TODO: raise error.
			return;
		}
		m_logFile << "Log for Broodwar AI Agent!" << std::endl;
	}

	~Logger ()
	{
		if (m_logFile.is_open())
		{
			m_logFile << "Closing log file." << std::endl;
			m_logFile.close();
		}
	}

	void logLine(std::string message)
	{
		m_logFile << message << std::endl;
	}

private:
	static Logger* m_instance;

	std::ofstream m_logFile;
};
