// testFnlog.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <thread>
#include <chrono>
#include <ratio>

#include "args.h"
#include "log4z.h"
//#include "Timer.h"

using namespace std;
using namespace std::chrono;

using namespace zsummer::log4z;
using namespace robin;


void ThreadFunc(uint32_t index, int32_t nSpeed, int64_t nPackets)
{
	std::stringstream ss;
	int i = 0;
	for (i=0; i<1024; i++)
	{
		ss << "a";
	}
	string line = ss.str();
	
	int count = 0;
	time_point<high_resolution_clock> tpStart = high_resolution_clock::now();
	int64_t n = 1;
	while (nPackets > 0)
	{
		LOGFMTE("thread%d: line %lld, %s", index, n++, line.c_str());

		nPackets--;
		count++;


		if (count >= nSpeed)
		{
			count = 0;
			time_point<high_resolution_clock> tpStop = high_resolution_clock::now();
			using ms = std::ratio<1, 1000>;
			duration<double, ms> tmDelta = duration<double, ms>(tpStop - tpStart);
			int leftMs = 1000 - (int)tmDelta.count();
			if (leftMs > 1)
				sleepForMs(leftMs);
			printf("thread %u left %lld to sort\n", index, nPackets);

			tpStart = high_resolution_clock::now();
		}
	}
	
}



void testLog4z(int nThreads, int nSpeed, int64_t nPackets)
{
	//start log4z  
	ILog4zManager::getRef().setLoggerPath(LOG4Z_MAIN_LOGGER_ID, "../log_test");
	ILog4zManager::getRef().setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, false);

	ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
	//ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_ERROR);
	ILog4zManager::getRef().setLoggerLimitsize(LOG4Z_MAIN_LOGGER_ID, 1000);   // 1000M bytes
	ILog4zManager::getRef().setLoggerReserveTime(LOG4Z_MAIN_LOGGER_ID, 3600);
	ILog4zManager::getRef().start();




	using ThreadPtr = std::shared_ptr< std::thread>;
	std::vector<ThreadPtr> vecThreads;

	for (int i = 0; i < nThreads; i++)
	{
		ThreadPtr thread = std::make_shared<std::thread>(ThreadFunc, i + 1, nSpeed, nPackets);
		vecThreads.emplace_back(thread);
	}

    sleepForMs(3000);

    while (true)
    {

        int speed = ILog4zManager::getRef().getDiskSpeed();
        size_t left = ILog4zManager::getRef().getSize();
        printf(" disk speed is %u, left in que is %zu\n", speed, left);


        //if (left == 0)
         //   break;

        sleepForMs(1000);

    }

	printf("waiting for thread finish job\n");
	for (int i = 0; i < nThreads; i++)
	{
		if (vecThreads[i]->joinable())
			vecThreads[i]->join();
	}
	printf(" waiting for writing all data\n");
	vecThreads.clear();

	// clean
	ILog4zManager::getRef().stop();
}


LoggerId logid_fromfile;
LoggerId logid_dynamic;

void testlog1()
{

	ILog4zManager::getRef().setLoggerName(LOG4Z_MAIN_LOGGER_ID, "main");
	ILog4zManager::getRef().setLoggerPath(LOG4Z_MAIN_LOGGER_ID, "d:\\log\\log_main");
	ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
	ILog4zManager::getRef().setLoggerFileLine(LOG4Z_MAIN_LOGGER_ID, false);
	ILog4zManager::getRef().setLoggerOutFile(LOG4Z_MAIN_LOGGER_ID, true);

	logid_dynamic = ILog4zManager::getRef().createLogger("dynamic");
	ILog4zManager::getRef().setLoggerPath(logid_dynamic, "d:\\log\\log_dyna");
	ILog4zManager::getRef().setLoggerDisplay(logid_dynamic, true);
	ILog4zManager::getRef().setLoggerLevel(logid_dynamic, LOG_LEVEL_DEBUG);
	//ILog4zManager::getRef().setLoggerMonthdir(logid_dynamic, true);
	ILog4zManager::getRef().setLoggerLimitsize(logid_dynamic, 1);
	ILog4zManager::getRef().setLoggerReserveTime(logid_dynamic, 4);

	logid_fromfile = ILog4zManager::getRef().createLogger("fromfile");
	ILog4zManager::getRef().setLoggerPath(logid_fromfile, "d:\\log\\log_file");
	ILog4zManager::getRef().setLoggerDisplay(logid_fromfile, true);
	ILog4zManager::getRef().setLoggerLevel(logid_fromfile, LOG_LEVEL_DEBUG);
	//ILog4zManager::getRef().setLoggerMonthdir(logid_fromfile, true);
	ILog4zManager::getRef().setLoggerLimitsize(logid_fromfile, 1);
	
	//start log4z
	ILog4zManager::getRef().start();


	LOG_TRACE(logid_fromfile, "fromfile LOG_TRACE");
	LOG_DEBUG(logid_fromfile, "fromfile LOG_DEBUG");
	LOG_INFO(logid_fromfile, "fromfile LOG_INFO");
	LOG_WARN(logid_fromfile, "fromfile LOG_WARN");
	LOG_ERROR(logid_fromfile, "fromfile LOG_ERROR");
	LOG_ALARM(logid_fromfile, "fromfile LOG_ALARM");
	LOG_FATAL(logid_fromfile, "fromfile LOG_FATAL");

	LOG_TRACE(logid_dynamic, "dynamic LOG_TRACE");
	LOG_DEBUG(logid_dynamic, "dynamic LOG_DEBUG");
	LOG_INFO(logid_dynamic, "dynamic LOG_INFO");
	LOG_WARN(logid_dynamic, "dynamic LOG_WARN");
	LOG_ERROR(logid_dynamic, "dynamic LOG_ERROR");
	LOG_ALARM(logid_dynamic, "dynamic LOG_ALARM");
	LOG_FATAL(logid_dynamic, "dynamic LOG_FATAL");

	sleepForMs(30 * 1000);

	LOG_TRACE(logid_fromfile, "fromfile LOG_TRACE");
	LOG_DEBUG(logid_fromfile, "fromfile LOG_DEBUG");
	LOG_INFO(logid_fromfile, "fromfile LOG_INFO");
	LOG_WARN(logid_fromfile, "fromfile LOG_WARN");
	LOG_ERROR(logid_fromfile, "fromfile LOG_ERROR");
	LOG_ALARM(logid_fromfile, "fromfile LOG_ALARM");
	LOG_FATAL(logid_fromfile, "fromfile LOG_FATAL");

	LOG_TRACE(logid_dynamic, "dynamic LOG_TRACE");
	LOG_DEBUG(logid_dynamic, "dynamic LOG_DEBUG");
	LOG_INFO(logid_dynamic, "dynamic LOG_INFO");
	LOG_WARN(logid_dynamic, "dynamic LOG_WARN");
	LOG_ERROR(logid_dynamic, "dynamic LOG_ERROR");
	LOG_ALARM(logid_dynamic, "dynamic LOG_ALARM");
	LOG_FATAL(logid_dynamic, "dynamic LOG_FATAL");

	// clean
	ILog4zManager::getRef().stop();

	

}

uint64_t parseReserveTime(const string & str)
{
	uint64_t  delta = 0;
	string timeStr = str;
	if (str.empty())
		return 0;

	auto pos = str.find("d");
	if (pos != string::npos)
	{
		string days = str.substr(0, pos);
		delta += std::atoi(days.c_str()) * 24 * 3600;
		timeStr = str.substr(pos + 1);
	}

	pos = str.find("h");
	if (pos != string::npos)
	{
		string hours = timeStr.substr(0, pos);
		delta += std::atoi(hours.c_str()) * 3600;
		timeStr = str.substr(pos + 1);
	}

	pos = str.find("m");
	if (pos != string::npos)
	{
		string minu = timeStr.substr(0, pos);
		delta += std::atoi(minu.c_str()) * 60;
		timeStr = str.substr(pos + 1);
	}


	delta += std::atoi(timeStr.c_str());
	timeStr = str.substr(pos + 1);
	

	return delta;

}

void testParse()
{
	const char * vec[9] =
	{
		"30 s",
		"1d 2h 3m 4s",
		"1d",
		"2 h ",
		"2 h 3 m",
		"1d 2 h 3 m",
		"3 m 4 s",
		"3m 1d" , // wrong
		""
	};


	for (int i = 0; i < 9; i++)
	{
		uint64_t delta = parseReserveTime(vec[i]);
		printf("%s = %lld s \n", vec[i], delta);
	}
}
//////////////////////////////////////////////////////////////////////////////////////

static int64_t  countAll[10] = { 0 };
void ThreadFuncSpeed(uint32_t index, int32_t nSpeed, int64_t nPackets)
{
	std::stringstream ss;
	int i = 0;
	for (i = 0; i < 1024; i++)
	{
		ss << "a";
	}
	string line = ss.str();

	countAll[index] = 0;
	while (nPackets > 0)
	{
		LOGFMTE("thread%d: line %lld, %s", index, countAll[index], line.c_str());

		countAll[index] ++;
		if (countAll[index] >= nPackets)
			break;
	}
	printf("%u thread finish here\n", index);

}

void testLog4zSpeed(int nThreads, int nSpeed, int64_t nPackets)
{
	//start log4z  
	ILog4zManager::getRef().setLoggerPath(LOG4Z_MAIN_LOGGER_ID, "../log_test");
	ILog4zManager::getRef().setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, false);

	ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
	//ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_ERROR);
	ILog4zManager::getRef().setLoggerLimitsize(LOG4Z_MAIN_LOGGER_ID, 1000);   // 1000M bytes
	ILog4zManager::getRef().setLoggerReserveTime(LOG4Z_MAIN_LOGGER_ID, 3600);
	ILog4zManager::getRef().start();




	using ThreadPtr = std::shared_ptr< std::thread>;
	std::vector<ThreadPtr> vecThreads;

	for (int i = 0; i < nThreads; i++)
	{
		ThreadPtr thread = std::make_shared<std::thread>(ThreadFuncSpeed, i, nSpeed, nPackets);
		vecThreads.emplace_back(thread);
	}

	int64_t last = 0;
	int64_t cur = 0;
	while (true)
	{
		cur = 0;
		for (int i = 0; i < nThreads; i++)
		{
			cur += countAll[i];
		}

		if (last == 0)
		{
			last = cur;
		}
		else
		{
			int64_t delta = cur - last;
			last = cur;
			int speed = ILog4zManager::getRef().getDiskSpeed();
			size_t left = ILog4zManager::getRef().getSize();
			printf("log speed is %lld, disk speed is %u, left in que is %zu\n", delta, speed, left);

			
			if (left == 0)
				break;
		}

		sleepForMs(1000);

	}


	printf("waiting for thread finish job\n");
	for (int i = 0; i < nThreads; i++)
	{
		if (vecThreads[i]->joinable())
			vecThreads[i]->join();
	}
	printf(" waiting for writing all data\n");
	vecThreads.clear();

	// clean
	ILog4zManager::getRef().stop();
}

//////////////////////////////////////////////////////////////////////////////////////
void usage()
{
	printf("usage: testxxxx.exe [-n PacketsPerThread] [-s Speed] [-t nThreads]\n");
}

int main(int argc, char ** argv)
{
	int nThreads;
	int nSpeed;
	int64_t nPackets;
	
	int ret = parseArgs(argc, argv, usage);
	ret = getIntArg("-t", nThreads, 2, 1, 20);
	ret = getIntArg("-s", nSpeed, 1000, 1, 100000);
	ret = getInt64Arg("-n", nPackets, 10000, 1000, 10000 * 10000 * 100LL);

	printf("nThreads=%d, nSpeed=%d, nPackets=%lld\n", nThreads, nSpeed, nPackets);


    testLog4zSpeed(nThreads, nSpeed, nPackets);

    // testLog4z(nThreads, nSpeed, nPackets);
	//testlog1();



	return 0;
}

