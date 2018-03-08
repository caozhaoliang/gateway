#include "time_utility.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>

#include <vector>

#ifdef _WIN32
// �����������ΪSleep��Ҫ
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include "string_utility.h"

using namespace std;

/** ��õ�ǰʱ�� */
string TimeUtility::GetCurTime()
{
    return GetStringTime(time(NULL));
}

/** ��õ�ǰ���ڣ�������ʱ�� */
string TimeUtility::GetCurDate(bool bIsEnglishFormat)
{
    time_t tTime = time (NULL);
    return GetDate(tTime, bIsEnglishFormat);
}

/** ��õ�ǰ�� */
int TimeUtility::GetCurYear()
{
    return GetYear(time(NULL));
}

/** ��õ�ǰ�� */
int TimeUtility::GetCurMonth()
{
    return GetMonth(time(NULL));
}

/** ��õ�ǰ�� */
int TimeUtility::GetCurDay()
{
    return GetDay(time(NULL));
}

/** ��õ�ǰ�����ڼ� */
int TimeUtility::GetCurWeek()
{
    return GetWeek(time(NULL));
}

/** ��õ�ǰСʱ */
int TimeUtility::GetCurHour()
{
    return GetHour(time(NULL));
}

/** ��õ�ǰ���� */
int TimeUtility::GetCurMinute()
{
    return GetMinute(time(NULL));
}

/** ��õ�ǰ�� */
int TimeUtility::GetCurSecond()
{
    return GetSecond(time(NULL));
}

/** ��õ�ǰ���� */
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

/** ��õ�ǰ΢�� */
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

/** ��õ�ǰ���ڣ�������ʱ�� */
string TimeUtility::GetDate(time_t tTime, bool bIsEnglishFormat)
{
    struct tm	*lptmNow;
    char szBuf[32];
#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    // linux��û��ֱ�ӻ�����ڵĺ���
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
        snprintf(szBuf, sizeof(szBuf), "%04d��%02d��%02d��",
                 lptmNow->tm_year + 1900,
                 lptmNow->tm_mon + 1,
                 lptmNow->tm_mday);
    }
    return string(szBuf);
}

/** ����� */
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

/** ����� */
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

/** ����� */
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

/** ������ڼ� */
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


/** ���Сʱ */
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


/** ��÷��� */
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

/** ��õ�ǰ�� */
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

/** ������ʱ��תΪ�ַ���ʱ�� */
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

/** ��õ�ǰʱ�䣬��ȷ��΢�� */
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

/** �ж�һ���Ƿ������� */
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

/** �������ʼ�� */
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

/** ����һ��������� */
int TimeUtility::GetRandInt(int dwMin, int dwMax)
{
    return dwMin + (int) ((dwMax - dwMin)*1.0*rand()/(RAND_MAX+1.0));
}

/** �߳�˯�ߣ���λ��΢�� */
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

/**************** ����ʵ���Զ�ʶ��ʱ���ʽ�ķ��� *************/

/** �Զ��İ�ʱ��תΪtime_t���͵�, ������ַ���Ӧ���Ǿ�������� */
/** Ŀǰ���ܴ���������: Sat, 05 Jan 2008 15:35:21 GMT+8 */
time_t TimeUtility::NormalizeTime(const std::string& strRawTime)
{
    string strDate;
    string strTime;
    struct tm oTime = {};

    // �����൱��ȱʡֵ
    oTime.tm_year = GetCurYear();
    oTime.tm_hour = 0;//GetCurHour();
    oTime.tm_min  = 0;//GetCurMinute();
    oTime.tm_sec  = 0;//GetCurSecond();

    string::size_type dwPos = strRawTime.find(" ");

    // û���ҵ��ո�˵��ֻ������û��ʱ��
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

    // �������������
    if (3 == vDate.size())
    {
        // 2006-4-30 �� 2006/4/30 �����
        if (vDate[0].size() == 4)
        {
            oTime.tm_year = StringUtility::Str2Int(vDate[0]);
            oTime.tm_mon = StringUtility::Str2Int(vDate[1]);
            oTime.tm_mday = StringUtility::Str2Int(vDate[2]);
        }
        // 30/4/2006 �����
        else if (vDate[2].size() == 4)
        {
            oTime.tm_year = StringUtility::Str2Int(vDate[2]);
            oTime.tm_mon = StringUtility::Str2Int(vDate[1]);
            oTime.tm_mday = StringUtility::Str2Int(vDate[0]);
        }
        // 30/4/06��06/4/30 ������� ����ǳ������ж������յ�˳��
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
    // ֻ���º�������
    else if (2 == vDate.size())
    {
        oTime.tm_mon = StringUtility::Str2Int(vDate[0]);
        oTime.tm_mday = StringUtility::Str2Int(vDate[1]);
    }

    // �����ʱ��
    if (!strTime.empty())
    {
        vector<string> vTime;
        bool bIsPM = false; // �Ƿ�������

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

        // ʱ�����������
        if (3 == vTime.size())
        {
            oTime.tm_hour = StringUtility::Str2Int(vTime[0]);
            oTime.tm_min  = StringUtility::Str2Int(vTime[1]);
            oTime.tm_sec  = StringUtility::Str2Int(vTime[2]);
        }
        // ֻ��Сʱ�ͷֵ����
        else if (2 == vTime.size())
        {
            oTime.tm_hour = StringUtility::Str2Int(vTime[0]);
            oTime.tm_min  = StringUtility::Str2Int(vTime[1]);
        }

        // ���������
        if (bIsPM)
            oTime.tm_hour += 12;
    }
    oTime.tm_year -= 1900;
    oTime.tm_mon  -= 1;
    return mktime(&oTime);
}

/** �Զ��İ�ʱ��תΪstring���͵ı�׼��ʽ */
string TimeUtility::NormalizeTimeAsString(const std::string& strRawTime)
{
    time_t t = NormalizeTime(strRawTime);
    string strRet;
    strRet.assign(ctime(&t));
    return strRet;
}

/** �Զ��İ����ĸ�ʽ��ʱ��תΪtime_t���͵� */
time_t TimeUtility::NormalizeChineseTime(const std::string& strDateTime, std::string& strRawTime)
{
    string::size_type dwYearPos;
    string::size_type dwMonthPos;
    string::size_type dwDayPos;
    struct tm oTime = {};

    // ��Ϊ����ʱ���ʽ��:"yyyy��MM��DD��hh:mm"��"yyyy��MM��DD��hh:mm:ss"
    // "yyyy��MM��DD��hhʱmm��"
    dwYearPos = strDateTime.find("��");
    dwMonthPos = strDateTime.find("��");
    dwDayPos = strDateTime.find("��");

    if(string::npos == dwYearPos || string::npos == dwMonthPos || string::npos == dwDayPos)
    {
        // "������"û��, ˵������������ʱ��
        return 0;
    }
    else
    {
        // �ҵ�ʱ��Ŀ�ʼ��ַ
        string strYear = strDateTime.substr(0, dwYearPos);
        string strMonth = strDateTime.substr(dwYearPos+2, dwMonthPos-dwYearPos-2);
        string strDay = strDateTime.substr(dwMonthPos+2, dwDayPos-dwMonthPos-2);
        string strTime = strDateTime.substr(dwDayPos+2);

        // ������ַ���ǰ��Ŀո�������ַ����˵�
        size_t dwBeginPos, dwEndPos = strYear.size()-1;
        while (dwEndPos > 0 && (strYear[dwEndPos] == ' '))
            dwEndPos--;
        dwBeginPos = dwEndPos;
        while (dwBeginPos > 0 && StringUtility::IsDigit(strYear[dwBeginPos]))
            dwBeginPos--;
        strYear = strYear.substr(dwBeginPos+1, dwEndPos - dwBeginPos+1);

        // �����൱��ȱʡֵ
        oTime.tm_year = StringUtility::Str2Int(strYear) - 1900;
        oTime.tm_mon = StringUtility::Str2Int(strMonth) - 1;
        oTime.tm_mday = StringUtility::Str2Int(strDay);
        oTime.tm_hour = 0;
        oTime.tm_min  = 0;
        oTime.tm_sec  = 0;

        strTime = StringUtility::TrimLeft(strTime, " \t");

        string::size_type dwHourPos;
        string::size_type dwMinutePos;

        dwHourPos = strTime.find("ʱ");
        dwMinutePos = strTime.find("��");
        if(string::npos != dwHourPos && string::npos != dwMinutePos)
        {
            string strHour   = strTime.substr(0, dwHourPos);
            string strMinute = strTime.substr(dwHourPos+2, dwMinutePos-dwHourPos-2);
            oTime.tm_hour    = StringUtility::Str2Int(strHour);
            oTime.tm_min     = StringUtility::Str2Int(strMinute);
        }
        else
        {
            // ��ʱ���֡�����ַ����������
            dwEndPos = 0;

            while ((dwEndPos < strTime.size() && (strTime[dwEndPos] == ':')) || StringUtility::IsDigit(strTime[dwEndPos]))
                dwEndPos++;
            strTime = strTime.substr(0, dwEndPos);

            // ��ԭʼ��ʱ���ַ�����ȡ����
            string::size_type dwRawTimeBeginPos = strDateTime.find(strYear);
            string::size_type dwRawTimeEndPos;
            if (!strTime.empty())
                dwRawTimeEndPos = strDateTime.find(strTime);
            else
                dwRawTimeEndPos = strDateTime.find("��");
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
    if (end && end - str >= int(sizeof("2009-03-24 00:00:00") - 1)) // ��ʽ����ȫƥ��
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
    //struct tm *mt = localtime(&t);//非线程安�?
    int time = mt.tm_hour * 10000 + mt.tm_min * 100 + mt.tm_sec;
    return time;
}

/** ����������ڣ�yyyymmdd */
int TimeUtility::GetNowDate(const time_t tTime)
{
    struct tm	*lptmNow;
    char szBuf[32];
#ifdef _WIN32
    lptmNow = localtime(&tTime);
#else
    // linux��û��ֱ�ӻ�����ڵĺ���
    struct tm	tmNow;
    lptmNow = localtime_r(&tTime, &tmNow);
#endif

   int date = ((lptmNow->tm_year + 1900) * 10000) 
       + ((lptmNow->tm_mon + 1) * 100) 
       + lptmNow->tm_mday;

    return date;
}


