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
    //����ַ����Ƿ��ǺϷ������ڸ�ʽ
    static bool isValidDate(const char * szDate);

    //����ַ����Ƿ��ǺϷ������ڸ�ʽ
    static bool isValidDateTime(const char * szDateTime);

    //�ж��Ƿ�Ϊ����
    static bool isLeapYear(const std::string & szDate);

    //�Ƿ�Ϊ����
    static bool isDigit(char c);

    //��ȡ���µ�һ�죨20170101���� ���һ�죨20170131��
    static bool getFirstAndLastDay(std::string & sFirstDay, std::string & sLastDay, const std::string & sDate);
};


#endif
