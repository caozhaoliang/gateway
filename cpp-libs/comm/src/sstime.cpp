#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sstime.h"

// �·ݵ����� :        1   2   3   4   5   6   7   8   9   10  11  12  ��2��
int DAYS_OF_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 29};


SSTime::SSTime() 
{

}


//����ַ����Ƿ��ǺϷ������ڸ�ʽ
bool SSTime::isValidDate(const char *szDate)
{
    if(szDate == NULL || (strlen(szDate) != 8 && strlen(szDate) != 14))
    {
        return false;
    }

    int iYear, iMonth, iDay;

    //����ȫ������
    for(int i = 0; i < (int)strlen(szDate); ++i)
    {
        if(szDate[i] < '0' || szDate[i] > '9')
        {
            return false;
        }
    }
    
    //�·ݱ�����[01, 12]֮��
    iMonth = 10 * (szDate[4] - '0') + szDate[5] - '0';
    if(iMonth < 1 || iMonth > 12)
    {
        return false;
    }

    //��������Ϊ��13��
    iYear = 100 * (szDate[1] - '0') + 10 * (szDate[2] - '0') + szDate[3] - '0';
    if(iMonth == 2 && (iYear % 4) == 0 && ((iYear % 400) == 0 || (iYear % 100) != 0))
    {
        iMonth = 13;
    }

    //�ж������Ƿ�Ϸ�
    iDay = 10 * (szDate[6] - '0') + szDate[7] - '0';
    if(iDay < 1 || iDay > DAYS_OF_MONTH[iMonth - 1])
    {
        return false;
    }

    return true;
}


//����ַ����Ƿ��ǺϷ������ڸ�ʽ
bool SSTime::isValidDateTime(const char *szDateTime)
{
    if(NULL == szDateTime || 14 != strlen(szDateTime) || !isValidDate(szDateTime))
    {
        return false;
    }

    int iHour, iMinute, iSecond;
    
    // Сʱ������[0, 23]֮��
    iHour = 10 * (szDateTime[8] - '0') + szDateTime[9] - '0';
    if(iHour < 0 || iHour > 23)
    {
        return false;
    }
    
    // ���ӱ�����[0, 59]֮��
    iMinute = 10 * (szDateTime[10] - '0') + szDateTime[11] - '0';
    if(iMinute < 0 || iMinute > 59)
    {
        return false;
    }
    
    // ����������[0, 59]֮��
    iSecond = 10 * (szDateTime[12] - '0') + szDateTime[13] - '0';
    if(iSecond < 0 || iSecond > 59)
    {
        return false;
    }
    
    return true;
}

//�ж��Ƿ�Ϊ����
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

//�Ƿ�Ϊ����
bool SSTime::isDigit(char c)
{
     if (c >= '0' && c <= '9')
     {
        return true;
     }

     return false;
}


//��ȡ���µ�һ�죨20170101���� ���һ�죨20170131��
bool SSTime::getFirstAndLastDay(std::string & sFirstDay, std::string & sLastDay, const std::string & sDate)
{
    //У�����ںϷ���
    if(!isValidDate(sDate.c_str()))
    {
        return false;
    }

    //��ȡ�·�
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

    //��ȡ�·����ڵ���
    int iLastDay = DAYS_OF_MONTH[iMonthIndex];

    //��װ��������
    char szYearMonth[32];
    memset(szYearMonth, 0x00, sizeof(szYearMonth));

    sprintf(szDays, "%d", iLastDay);
    strncpy(szYearMonth, sDate.c_str(), 6);
    sLastDay = std::string(szYearMonth) + std::string(szDays);

    sFirstDay = std::string(szYearMonth) + std::string("01");

    return true;
}
