#ifndef __TIME_UTILITY_H__
#define __TIME_UTILITY_H__

#include <string>

using namespace std ;

/** 时间格式类 */
class TimeUtility
{

/**************** 下面是通用的时间工具函数 *************/
public:

	/** 获得当前时间 */
	static std::string GetCurTime();

	/** 获得当前日期 */
    static std::string GetCurDate(bool bIsEnglishFormat = true);

	/** 获得当前年 */
	static int GetCurYear();

	/** 获得当前月 */
	static int GetCurMonth();

	/** 获得当前天 */
	static int GetCurDay();

	/** 获得当前是星期几 */
	static int GetCurWeek();

	/** 获得当前小时 */
	static int GetCurHour();

	/** 获得当前分钟 */
	static int GetCurMinute();

	/** 获得当前秒 */
	static int GetCurSecond();

    /** 获得当前毫秒 */
    static unsigned int GetCurMilliSecond();

    /** 获得当前微秒 */
    static unsigned long long GetCurMicroSecond();

	/** 获得日期 */
    static std::string GetDate(time_t tTime, bool bIsEnglishFormat = true);

	/** 获得年 */
	static int GetYear(const time_t tTime);

	/** 获得月 */
	static int GetMonth(const time_t tTime);

	/** 获得天 */
	static int GetDay(const time_t tTime);

	/** 获得星期几 */
	static int GetWeek(const time_t tTime);

	/** 获得小时 */
	static int GetHour(const time_t tTime);

	/** 获得分钟 */
	static int GetMinute(const time_t tTime);

	/** 获得当前秒 */
	static int GetSecond(const time_t tTime);

	/** 把整数时间转为字符串时间 */
	static std::string GetStringTime(const time_t tTime);

	/** 获得当前时间，精确到微秒 */
	static std::string GetCurPreciseTime();

    /** 获得整数时间，hhmmss */
    static int GetNowTime(const time_t tTime);

    /** 获得整数日期，yyyymmdd */
    static int GetNowDate(const time_t tTime);

    /** 填充当前的时分秒 */
    static time_t PadCurHMS(time_t tRawTime);

	/** 判断一年是否是闰年 */
	static bool IsLeapYear(const int dwYear);

	/** 随机数初始化 */
	static void InitRand();

	/** 产生一个随机整数 */
	static int GetRandInt(int dwMin, int dwMax);

	/** 线程睡眠，单位是微秒，精确度与操作系统相关 */
	static void USleep(unsigned long dwUSeconds);

/**************** 下面实现自动识别时间格式的方法 *************/
public:

	/** 自动的把英文格式时间转为time_t类型的 */
	static time_t NormalizeTime(const std::string& strRawTime);

	/** 自动的把时间转为string类型的标准格式 */
	static std::string NormalizeTimeAsString(const std::string& strRawTime);

    /** 自动的把中文格式的时间转为time_t类型的 */
	static time_t NormalizeChineseTime(const std::string& strDateTime, std::string& strRawTime);
	static time_t NormalizeChineseTime(const std::string& strDateTime);

	/** 解析 2009-03-24 00:00:00 这样格式的时间为 time_t，失败返回 -1 */
	static time_t ParseDateTime(const char* str);
	static inline time_t ParseDateTime(const std::string& str)
	{
		return ParseDateTime(str.c_str());
	}
};

#endif
