#ifndef _SS_TIME_H__DEF
#define _SS_TIME_H__DEF
#if defined(_COMPILE_ON_LINUX)
#include <sys/time.h>
#else
#include <time.h>
#endif

#include <string>

using namespace std;



#define STRLEN_DATE     8
#define STRLEN_DATETIME 14
#define STRLEN_TIME     6

#define STRLEN_YEAR     4
#define STRLEN_MON      2
#define STRLEN_DAY      2
#define STRLEN_HOUR     2
#define STRLEN_MINUTE   2
#define STRLEN_SECOND   2



class SSTime
{
public:
    SSTime();                                 
 
public:                  
    //检查字符串是否是合法的日期格式
    static bool isValidDate(const char * szDate);

    //检查字符串是否是合法的日期格式
    static bool isValidDateTime(const char * szDateTime);

    //判断是否为闰年
    static bool isLeapYear(const std::string & szDate);

    //是否为数字
    static bool isDigit(char c);

    //获取当月第一天（20170101）和 最后一天（20170131）
    static bool getFirstAndLastDay(std::string & sFirstDay, std::string & sLastDay, const std::string & sDate);
};


#endif
