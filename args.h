#pragma once
#include <string>
#include <sstream>
#include <map>
#include <functional>
#include <inttypes.h>
#include <chrono>
#include <time.h>
#include <thread>
#include <limits.h>

using namespace std;
using UsageHandle = std::function<void (void)>;

static std::map<string, string> params;
namespace robin
{


	int inline parseArgs(int argc, char ** argv, UsageHandle usage)
	{

		params.clear();

		for (int i = 1; i < argc; )
		{
			string key = argv[i];
			i++;
			if (i >= argc)
			{
				if (usage)
					usage();
				break;
			}
			string val = argv[i];
			i++;
			params.insert(std::make_pair(key, val));

		}

		return (int)params.size();

	}

	int inline getIntArg(const char * name, int& val, int init_val = 1, int lower = INT_MIN, int upper = INT_MAX)
	{
		string tmp = params[name];
		if (!tmp.empty())
		{
			val = std::atoi(tmp.c_str());
			if (val < lower)
				val = lower;
			if (val > upper)
				val = upper;
			return 1;
		}
		else
		{
			val = init_val;
			return 0;
		}
	}

	int inline getInt64Arg(const char * name, int64_t& val, int64_t init_val = 1, int64_t lower = LLONG_MIN, int64_t upper = LLONG_MAX)
	{
		string tmp = params[name];
		if (!tmp.empty())
		{
			val = std::atoi(tmp.c_str());
			if (val < lower)
				val = lower;
			if (val > upper)
				val = upper;
			return 1;
		}
		else
		{
			val = init_val;
			return 0;
		}
	}

	int inline getStringArg(const char * name, string & val, const char * init_val)
	{
		val = params[name];
		if (val.empty())
		{
			val = init_val;
			return 0;
		}

		return 1;
	}

	static inline  void sleepForMs(int msecs)
	{
#if defined (_WIN32) || defined (_WIN64)
		std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
#else
		static struct timespec req;
		req.tv_sec = msecs / 1000;
		req.tv_nsec = msecs * 1000000 % 1000000000;   // ns
		int ret = nanosleep(&req, NULL);
		//printf("--------nano-----------\n");
#endif
	}

	static inline  void sleepForUs(int usecs)
	{
#if defined (_WIN32) || defined (_WIN64)
		std::this_thread::sleep_for(std::chrono::microseconds(usecs));
#else
		static struct timespec req;
		req.tv_sec = usecs / 1000000;
		req.tv_nsec = usecs * 1000 % 1000000000;   // ns
		int ret = nanosleep(&req, NULL);
		//printf("--------nano-----------\n");
#endif

	}


}// endof namespace robin


