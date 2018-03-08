#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sstime.h"

// 月份的天数 :        1   2   3   4   5   6   7   8   9   10  11  12  闰2月
int DAYS_OF_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 29};


SSTime::SSTime() 
{

}


//检查字符串是否是合法的日期格式
bool SSTime::isValidDate(const char *szDate)
{
    if(szDate == NULL || (strlen(szDate) != 8 && strlen(szDate) != 14))
    {
        return false;
    }

    int iYear, iMonth, iDay;

    //必须全是数字
    for(int i = 0; i < (int)strlen(szDate); ++i)
    {
        if(szDate[i] < '0' || szDate[i] > '9')
        {
            return false;
        }
    }
    
    //月份必须在[01, 12]之间
    iMonth = 10 * (szDate[4] - '0') + szDate[5] - '0';
    if(iMonth < 1 || iMonth > 12)
    {
        return false;
    }

    //闰年设置为第13月
    iYear = 100 * (szDate[1] - '0') + 10 * (szDate[2] - '0') + szDate[3] - '0';
    if(iMonth == 2 && (iYear % 4) == 0 && ((iYear % 400) == 0 || (iYear % 100) != 0))
    {
        iMonth = 13;
    }

    //判断天数是否合法
    iDay = 10 * (szDate[6] - '0') + szDate[7] - '0';
    if(iDay < 1 || iDay > DAYS_OF_MONTH[iMonth - 1])
    {
        return false;
    }

    return true;
}


//检查字符串是否是合法的日期格式
bool SSTime::isValidDateTime(const char *szDateTime)
{
    if(NULL == szDateTime || 14 != strlen(szDateTime) || !isValidDate(szDateTime))
    {
        return false;
    }

    int iHour, iMinute, iSecond;
    
    // 小时必须在[0, 23]之间
    iHour = 10 * (szDateTime[8] - '0') + szDateTime[9] - '0';
    if(iHour < 0 || iHour > 23)
    {
        return false;
    }
    
    // 分钟必须在[0, 59]之间
    iMinute = 10 * (szDateTime[10] - '0') + szDateTime[11] - '0';
    if(iMinute < 0 || iMinute > 59)
    {
        return false;
    }
    
    // 秒数必须在[0, 59]之间
    iSecond = 10 * (szDateTime[12] - '0') + szDateTime[13] - '0';
    if(iSecond < 0 || iSecond > 59)
    {
        return false;
    }
    
    return true;
}

//判断是否为闰年
bool SSTime::isLeapYear(const std::string & szDate)
{
    int i_szLen = szDate.size();
    char szYear[8];
    memset(szYear, 0x00, sizeof(szYear));

    int i_Year;
    if((4 == i_szLen) || (8 == i_szLen) || (16 == i_szLen))
    {
        strncpy(szYear, szDate.c_str(), 4);
        i_Year=atoi((char *)szYear);
        if((i_Year % 400) == 0 || ((i_Year % 4) == 0 && (i_Year % 100) != 0))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

//是否为数字
bool SSTime::isDigit(char c)
{
     if (c >= '0' && c <= '9')
     {
        return true;
     }

     return false;
}


//获取当月第一天（20170101）和 最后一天（20170131）
bool SSTime::getFirstAndLastDay(std::string & sFirstDay, std::string & sLastDay, const std::string & sDate)
{
    //校验日期合法性
    if(!isValidDate(sDate.c_str()))
    {
        return false;
    }

    //获取月份
    char szMonth[4];
    char szDays[4];
    memset(szMonth, 0x00, sizeof(szMonth));
    memset(szDays, 0x00, sizeof(szDays));

    strncpy(szMonth, sDate.c_str() + 4, 2);

    bool bIsLeapYear = false;
    bIsLeapYear = isLeapYear(sDate);

    int iMonthIndex = atoi(szMonth) - 1;
    if((1 == iMonthIndex) && bIsLeapYear)
    {
        iMonthIndex = 12;
    }

    //获取月份所在的天
    int iLastDay = DAYS_OF_MONTH[iMonthIndex];

    //组装返回数据
    char szYearMonth[32];
    memset(szYearMonth, 0x00, sizeof(szYearMonth));

    sprintf(szDays, "%d", iLastDay);
    strncpy(szYearMonth, sDate.c_str(), 6);
    sLastDay = std::string(szYearMonth) + std::string(szDays);

    sFirstDay = std::string(szYearMonth) + std::string("01");

    return true;
}
