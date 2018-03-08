#include "time_utility.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>

#include <vector>

#ifdef _WIN32
// 包含这个是因为Sleep需要
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include "string_utility.h"

using namespace std;

/** 获得当前时间 */
string TimeUtility::GetCurTime()
{
    return GetStringTime(time(NULL));
}

/** 获得当前日期，不包含时间 */
string TimeUtility::GetCurDate(bool bIsEnglishFormat)
{
    time_t tTime = time (NULL);
    return GetDate(tTime, bIsEnglishFormat);
}

/** 获得当前年 */
int TimeUtility::GetCurYear()
{
    return GetYear(time(NULL));
}

/** 获得当前月 */
int TimeUtility::GetCurMonth()
{
    return GetMonth(time(NULL));
}

/** 获得当前天 */
int TimeUtility::GetCurDay()
{
    return GetDay(time(NULL));
}

/** 获得当前是星期几 */
int TimeUtility::GetCurWeek()
{
    return GetWeek(time(NULL));
}

/** 获得当前小时 */
int TimeUtility::GetCurHour()
{
    return GetHour(time(NULL));
}

/** 获得当前分钟 */
int TimeUtility::GetCurMinute()
{
    return GetMinute(time(NULL));
}

/** 获得当前秒 */
int TimeUtility::GetCurSecond()
{
    return GetSecond(time(NULL));
}

/** 获得当前毫秒 */
unsigned int TimeUtility::GetCurMilliSecond()
{
    time_t dwPreciseTime;
#ifdef _WIN32
    struct _timeb tbNow;
    _ftime(&tbNow);
    dwPreciseTime = tbNow.millitm;
#else
    struct timeval tvNow;
    gettimeofday(&tvNow, NULL);
    dwPreciseTime = tvNow.tv_usec / 1000;
#endif
    return (unsigned int)dwPreciseTime;
}

/** 获得当前微秒 */
unsigned long long TimeUtility::GetCurMicroSecond()
{
    unsigned long long dwPreciseTime;
#ifdef _WIN32
    struct _timeb	tbNow;
    _ftime(&tbNow);
    dwPreciseTime = (unsigned long long)tbNow.millitm * 1000;
#else
    struct timeval	tvNow;
    gettimeofday(&tvNow, NULL);
    dwPreciseTime = (unsigned long long)(tvNow.tv_sec*1000 + tvNow.tv_usec/1000);
#endif
    return dwPreciseTime;
}

/** 获得当前日期，不包含时间 */
string TimeUtility::GetDate(time_t tTime, bool bIsEnglishFormat)
{
    struct tm	*lptmNow;
    char szBuf[32];
#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    // linux下没有直接获得日期的函数
    struct tm	tmNow;
    lptmNow = localtime_r(&tTime, &tmNow);
#endif
    if (bIsEnglishFormat)
    {
        snprintf(szBuf, sizeof(szBuf), "%04d-%02d-%02d",
                 lptmNow->tm_year + 1900,
                 lptmNow->tm_mon + 1,
                 lptmNow->tm_mday);
    }
    else
    {
        snprintf(szBuf, sizeof(szBuf), "%04d年%02d月%02d日",
                 lptmNow->tm_year + 1900,
                 lptmNow->tm_mon + 1,
                 lptmNow->tm_mday);
    }
    return string(szBuf);
}

/** 获得年 */
int TimeUtility::GetYear(const time_t tTime)
{
    struct tm	*lptmNow;

#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    struct tm	tmNow;

    lptmNow = localtime_r(&tTime, &tmNow);
#endif
    return 1900+lptmNow->tm_year;
}

/** 获得月 */
int TimeUtility::GetMonth(const time_t tTime)
{
    struct tm	*lptmNow;

#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    struct tm	tmNow;

    lptmNow = localtime_r(&tTime, &tmNow);
#endif
    return lptmNow->tm_mon+1;
}

/** 获得天 */
int TimeUtility::GetDay(const time_t tTime)
{
    struct tm	*lptmNow;

#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    struct tm	tmNow;

    lptmNow = localtime_r(&tTime, &tmNow);
#endif
    return lptmNow->tm_mday;
}

/** 获得星期几 */
int TimeUtility::GetWeek(const time_t tTime)
{
    struct tm	*lptmNow;

#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    struct tm	tmNow;

    lptmNow = localtime_r(&tTime, &tmNow);
#endif
    return lptmNow->tm_wday;
}


/** 获得小时 */
int TimeUtility::GetHour(const time_t tTime)
{
    struct tm	*lptmNow;

#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    struct tm	tmNow;

    lptmNow = localtime_r(&tTime, &tmNow);
#endif
    return lptmNow->tm_hour;
}


/** 获得分钟 */
int TimeUtility::GetMinute(const time_t tTime)
{
    struct tm	*lptmNow;

#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    struct tm	tmNow;

    lptmNow = localtime_r(&tTime, &tmNow);
#endif
    return lptmNow->tm_min;
}

/** 获得当前秒 */
int TimeUtility::GetSecond(const time_t tTime)
{
    struct tm	*lptmNow;

#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    struct tm	tmNow;

    lptmNow = localtime_r(&tTime, &tmNow);
#endif
    return lptmNow->tm_sec;
}

/** 把整数时间转为字符串时间 */
string TimeUtility::GetStringTime(const time_t tTime)
{
    if (tTime < 0)
        return GetCurTime();

    struct tm	*lptmNow;
    char		sTime[sizeof("2009-12-03 11:09:56")];

#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    struct tm	tmNow;

    lptmNow = localtime_r(&tTime, &tmNow);
#endif

    memset(sTime, 0, sizeof(sTime));
    size_t length = strftime(sTime, sizeof(sTime), "%F %T", lptmNow);
    return string(sTime, length);
}

/** 获得当前时间，精确到微秒 */
string TimeUtility::GetCurPreciseTime()
{
    struct tm	*lptmNow;
    time_t		nTime;
    int nPreciseTime;
    char		sTime[256];

#ifdef _WIN32
    struct _timeb	tbNow;

    _ftime(&tbNow);
    nTime = tbNow.time;
    nPreciseTime = tbNow.millitm;
#else
    struct timeval	tvNow;

    gettimeofday(&tvNow, NULL);
    nTime = tvNow.tv_sec;
    nPreciseTime = tvNow.tv_usec;
#endif

    memset(sTime, 0, sizeof(sTime));
#ifdef _WIN32
    lptmNow = localtime((const time_t *)&nTime);
    sprintf(sTime, "%04d-%02d-%02d %02d:%02d:%02d:%03d",
            1900+lptmNow->tm_year, lptmNow->tm_mon+1, lptmNow->tm_mday,
            lptmNow->tm_hour, lptmNow->tm_min, lptmNow->tm_sec, nPreciseTime);
#else
    struct tm	tmNow;
    lptmNow = localtime_r((const time_t *)&nTime, &tmNow);
    sprintf(sTime, "%04d-%02d-%02d %02d:%02d:%02d:%06d",
            1900+lptmNow->tm_year, lptmNow->tm_mon+1, lptmNow->tm_mday,
            lptmNow->tm_hour, lptmNow->tm_min, lptmNow->tm_sec, nPreciseTime);

#endif

    return string(sTime);
}

/** 判断一年是否是闰年 */
bool TimeUtility::IsLeapYear(const int dwYear)
{
    if((dwYear % 100) == 0)
    {
        if((dwYear % 400) == 0)
            return false;
        else
            return true;
    }
    if((dwYear % 4) == 0)
        return false;
    return true;
}

/** 随机数初始化 */
void TimeUtility::InitRand()
{
    int	dwSecond;
    int dwUSecond;
#ifdef _WIN32
    struct _timeb	tbNow;

    _ftime(&tbNow);
    dwSecond = (int)tbNow.time;
    dwUSecond = tbNow.millitm;
#else
    struct timeval	tvNow;

    gettimeofday(&tvNow, NULL);
    dwSecond = tvNow.tv_sec;
    dwUSecond = tvNow.tv_usec;
#endif

    srand(dwSecond * dwUSecond);
}

/** 产生一个随机整数 */
int TimeUtility::GetRandInt(int dwMin, int dwMax)
{
    return dwMin + (int) ((dwMax - dwMin)*1.0*rand()/(RAND_MAX+1.0));
}

/** 线程睡眠，单位是微秒 */
void TimeUtility::USleep(unsigned long dwUSeconds)
{
#ifdef _WIN32
    Sleep(dwUSeconds / 1000);
#else
    struct timeval oDelay;
    oDelay.tv_sec = (unsigned long)dwUSeconds / 1000000;
    oDelay.tv_usec = (unsigned long)dwUSeconds % 1000000;
    select(0, 0, 0, 0, &oDelay);
#endif
}

/**************** 下面实现自动识别时间格式的方法 *************/

/** 自动的把时间转为time_t类型的, 输出的字符串应该是经过处理的 */
/** 目前不能处理这样的: Sat, 05 Jan 2008 15:35:21 GMT+8 */
time_t TimeUtility::NormalizeTime(const std::string& strRawTime)
{
    string strDate;
    string strTime;
    struct tm oTime = {};

    // 这里相当是缺省值
    oTime.tm_year = GetCurYear();
    oTime.tm_hour = 0;//GetCurHour();
    oTime.tm_min  = 0;//GetCurMinute();
    oTime.tm_sec  = 0;//GetCurSecond();

    string::size_type dwPos = strRawTime.find(" ");

    // 没有找到空格说明只有日期没有时间
    if (string::npos == dwPos)
    {
        strDate = strRawTime;
    }
    else
    {
        strDate = strRawTime.substr(0, dwPos);
        strTime = strRawTime.substr(dwPos+1);
    }

    vector<string> vDate;
    string         strTmp;
    for (size_t i = 0 ; i < strDate.size(); i++)
    {
        if (strDate[i] >= '0' && strDate[i] <= '9')
        {
            strTmp += strDate[i];
        }
        else
        {
            if (!strTmp.empty())
            {
                vDate.push_back(strTmp);
                strTmp = "";
            }
        }
    }
    if (!strTmp.empty())
    {
        vDate.push_back(strTmp);
        strTmp = "";
    }

    // 日期完整的情况
    if (3 == vDate.size())
    {
        // 2006-4-30 或 2006/4/30 的情况
        if (vDate[0].size() == 4)
        {
            oTime.tm_year = StringUtility::Str2Int(vDate[0]);
            oTime.tm_mon = StringUtility::Str2Int(vDate[1]);
            oTime.tm_mday = StringUtility::Str2Int(vDate[2]);
        }
        // 30/4/2006 的情况
        else if (vDate[2].size() == 4)
        {
            oTime.tm_year = StringUtility::Str2Int(vDate[2]);
            oTime.tm_mon = StringUtility::Str2Int(vDate[1]);
            oTime.tm_mday = StringUtility::Str2Int(vDate[0]);
        }
        // 30/4/06或06/4/30 的情况， 这个非常不好判断年月日的顺序
        else
        {
            int  dwYear = GetCurYear() - 2000;
            int  dwTmp0 = StringUtility::Str2Int(vDate[0]);
            int  dwTmp1 = StringUtility::Str2Int(vDate[1]);
            int  dwTmp2 = StringUtility::Str2Int(vDate[2]);
            bool bIsYMDOrder;
            if(dwTmp0 == dwYear)
                bIsYMDOrder = true;
            else if(dwTmp2 == dwYear)
                bIsYMDOrder = false;
            else if(abs(dwYear - dwTmp0) <= 2)
                bIsYMDOrder = true;
            else if(abs(dwYear - dwTmp2) <= 2)
                bIsYMDOrder = false;
            else
                bIsYMDOrder = true;

            if (bIsYMDOrder)
            {
                oTime.tm_year = dwTmp0 + 2000;
                oTime.tm_mon  = dwTmp1;
                oTime.tm_mday = dwTmp2;
            }
            else
            {
                oTime.tm_year = dwTmp2 + 2000;
                oTime.tm_mon  = dwTmp1;
                oTime.tm_mday = dwTmp0;
            }
        }
    }
    // 只有月和天的情况
    else if (2 == vDate.size())
    {
        oTime.tm_mon = StringUtility::Str2Int(vDate[0]);
        oTime.tm_mday = StringUtility::Str2Int(vDate[1]);
    }

    // 如果有时间
    if (!strTime.empty())
    {
        vector<string> vTime;
        bool bIsPM = false; // 是否是下午

        for (size_t i = 0 ; i < strTime.size(); i++)
        {
            if ((strTime[i] >= '0') && (strTime[i] <= '9'))
            {
                strTmp += strTime[i];
            }
            else if ((strTime[i] == ' ') || (strTime[i] == ':') || (strTime[i] == '/')
                     || (strTime[i] == '-') || (strTime[i] == '.'))
            {
                if (!strTmp.empty())
                {
                    vTime.push_back(strTmp);
                    strTmp = "";
                }
            }
            else if ((0 == strTime.compare(i, 2, "PM")) || (0 == strTime.compare(i, 2, "pm")))
            {
                bIsPM = true;
            }
            else
            {
                break;
            }
        }
        if (!strTmp.empty())
        {
            vTime.push_back(strTmp);
            strTmp = "";
        }

        // 时间完整的情况
        if (3 == vTime.size())
        {
            oTime.tm_hour = StringUtility::Str2Int(vTime[0]);
            oTime.tm_min  = StringUtility::Str2Int(vTime[1]);
            oTime.tm_sec  = StringUtility::Str2Int(vTime[2]);
        }
        // 只有小时和分的情况
        else if (2 == vTime.size())
        {
            oTime.tm_hour = StringUtility::Str2Int(vTime[0]);
            oTime.tm_min  = StringUtility::Str2Int(vTime[1]);
        }

        // 如果是下午
        if (bIsPM)
            oTime.tm_hour += 12;
    }
    oTime.tm_year -= 1900;
    oTime.tm_mon  -= 1;
    return mktime(&oTime);
}

/** 自动的把时间转为string类型的标准格式 */
string TimeUtility::NormalizeTimeAsString(const std::string& strRawTime)
{
    time_t t = NormalizeTime(strRawTime);
    string strRet;
    strRet.assign(ctime(&t));
    return strRet;
}

/** 自动的把中文格式的时间转为time_t类型的 */
time_t TimeUtility::NormalizeChineseTime(const std::string& strDateTime, std::string& strRawTime)
{
    string::size_type dwYearPos;
    string::size_type dwMonthPos;
    string::size_type dwDayPos;
    struct tm oTime = {};

    // 认为中文时间格式是:"yyyy年MM月DD日hh:mm"或"yyyy年MM月DD日hh:mm:ss"
    // "yyyy年MM月DD日hh时mm分"
    dwYearPos = strDateTime.find("年");
    dwMonthPos = strDateTime.find("月");
    dwDayPos = strDateTime.find("日");

    if(string::npos == dwYearPos || string::npos == dwMonthPos || string::npos == dwDayPos)
    {
        // "年月日"没有, 说明不存在中文时间
        return 0;
    }
    else
    {
        // 找到时间的开始地址
        string strYear = strDateTime.substr(0, dwYearPos);
        string strMonth = strDateTime.substr(dwYearPos+2, dwMonthPos-dwYearPos-2);
        string strDay = strDateTime.substr(dwMonthPos+2, dwDayPos-dwMonthPos-2);
        string strTime = strDateTime.substr(dwDayPos+2);

        // 把年份字符串前面的空格和中文字符过滤掉
        size_t dwBeginPos, dwEndPos = strYear.size()-1;
        while (dwEndPos > 0 && (strYear[dwEndPos] == ' '))
            dwEndPos--;
        dwBeginPos = dwEndPos;
        while (dwBeginPos > 0 && StringUtility::IsDigit(strYear[dwBeginPos]))
            dwBeginPos--;
        strYear = strYear.substr(dwBeginPos+1, dwEndPos - dwBeginPos+1);

        // 这里相当是缺省值
        oTime.tm_year = StringUtility::Str2Int(strYear) - 1900;
        oTime.tm_mon = StringUtility::Str2Int(strMonth) - 1;
        oTime.tm_mday = StringUtility::Str2Int(strDay);
        oTime.tm_hour = 0;
        oTime.tm_min  = 0;
        oTime.tm_sec  = 0;

        strTime = StringUtility::TrimLeft(strTime, " \t");

        string::size_type dwHourPos;
        string::size_type dwMinutePos;

        dwHourPos = strTime.find("时");
        dwMinutePos = strTime.find("分");
        if(string::npos != dwHourPos && string::npos != dwMinutePos)
        {
            string strHour   = strTime.substr(0, dwHourPos);
            string strMinute = strTime.substr(dwHourPos+2, dwMinutePos-dwHourPos-2);
            oTime.tm_hour    = StringUtility::Str2Int(strHour);
            oTime.tm_min     = StringUtility::Str2Int(strMinute);
        }
        else
        {
            // 把时、分、秒的字符串分离出来
            dwEndPos = 0;

            while ((dwEndPos < strTime.size() && (strTime[dwEndPos] == ':')) || StringUtility::IsDigit(strTime[dwEndPos]))
                dwEndPos++;
            strTime = strTime.substr(0, dwEndPos);

            // 把原始的时间字符串提取出来
            string::size_type dwRawTimeBeginPos = strDateTime.find(strYear);
            string::size_type dwRawTimeEndPos;
            if (!strTime.empty())
                dwRawTimeEndPos = strDateTime.find(strTime);
            else
                dwRawTimeEndPos = strDateTime.find("日");
            if (string::npos != dwRawTimeBeginPos && string::npos != dwRawTimeEndPos)
            {
                if (!strTime.empty())
                    strRawTime = strDateTime.substr(dwRawTimeBeginPos, dwRawTimeEndPos-dwRawTimeBeginPos+strTime.size());
                else
                    strRawTime = strDateTime.substr(dwRawTimeBeginPos, dwRawTimeEndPos-dwRawTimeBeginPos+2);
            }

            if (!strTime.empty())
            {
                vector<string> vTime;
                StringUtility::Split(strTime, ":", vTime);
                for (size_t i = 0; i < vTime.size() && i < 3; i++)
                {
                    if (0 == i)
                        oTime.tm_hour =  StringUtility::Str2Int(vTime[i]);
                    else if (1 == i)
                        oTime.tm_min =  StringUtility::Str2Int(vTime[i]);
                    else
                        oTime.tm_sec =  StringUtility::Str2Int(vTime[i]);
                } // end for
            } // end if
        }
    }

    time_t t = mktime(&oTime);
#ifdef _DEBUG
    //cout << "TIME: \t" << GetStringTime(t) << endl;
#endif
    return t;
}

time_t TimeUtility::NormalizeChineseTime(const std::string& strDateTime)
{
    std::string strRaw;
    return TimeUtility::NormalizeChineseTime(strDateTime, strRaw);
}

time_t TimeUtility::ParseDateTime(const char* str)
{
    struct tm tm = {};

#ifdef _XOPEN_SOURCE
    const char* end = strptime(str, "%F %T", &tm);
    if (end && end - str >= int(sizeof("2009-03-24 00:00:00") - 1)) // 格式能完全匹配
        return mktime(&tm);
#else
    if (sscanf(str,
               "%u-%u-%u %u:%u:%u",
               &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
               &tm.tm_hour, &tm.tm_min, &tm.tm_sec
              ) == 6)
    {
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        return mktime(&tm);
    }
#endif

    return time_t(-1);
}

int TimeUtility::GetNowTime(const time_t tTime)
{
    time_t t = tTime;
    struct tm mt = {0};
    localtime_r(&t, &mt);
    //struct tm *mt = localtime(&t);//闈炵嚎绋嬪畨鍏?
    int time = mt.tm_hour * 10000 + mt.tm_min * 100 + mt.tm_sec;
    return time;
}

/** 获得整数日期，yyyymmdd */
int TimeUtility::GetNowDate(const time_t tTime)
{
    struct tm	*lptmNow;
    char szBuf[32];
#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    // linux下没有直接获得日期的函数
    struct tm	tmNow;
    lptmNow = localtime_r(&tTime, &tmNow);
#endif

   int date = ((lptmNow->tm_year + 1900) * 10000) 
       + ((lptmNow->tm_mon + 1) * 100) 
       + lptmNow->tm_mday;

    return date;
}


