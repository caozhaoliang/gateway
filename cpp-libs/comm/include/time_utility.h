#ifndef __TIME_UTILITY_H__
#define __TIME_UTILITY_H__

#include <string>

using namespace std ;

/** ʱ���ʽ�� */
class TimeUtility
{

/**************** ������ͨ�õ�ʱ�乤�ߺ��� *************/
public:

	/** ��õ�ǰʱ�� */
	static std::string GetCurTime();

	/** ��õ�ǰ���� */
    static std::string GetCurDate(bool bIsEnglishFormat = true);

	/** ��õ�ǰ�� */
	static int GetCurYear();

	/** ��õ�ǰ�� */
	static int GetCurMonth();

	/** ��õ�ǰ�� */
	static int GetCurDay();

	/** ��õ�ǰ�����ڼ� */
	static int GetCurWeek();

	/** ��õ�ǰСʱ */
	static int GetCurHour();

	/** ��õ�ǰ���� */
	static int GetCurMinute();

	/** ��õ�ǰ�� */
	static int GetCurSecond();

    /** ��õ�ǰ���� */
    static unsigned int GetCurMilliSecond();

    /** ��õ�ǰ΢�� */
    static unsigned long long GetCurMicroSecond();

	/** ������� */
    static std::string GetDate(time_t tTime, bool bIsEnglishFormat = true);

	/** ����� */
	static int GetYear(const time_t tTime);

	/** ����� */
	static int GetMonth(const time_t tTime);

	/** ����� */
	static int GetDay(const time_t tTime);

	/** ������ڼ� */
	static int GetWeek(const time_t tTime);

	/** ���Сʱ */
	static int GetHour(const time_t tTime);

	/** ��÷��� */
	static int GetMinute(const time_t tTime);

	/** ��õ�ǰ�� */
	static int GetSecond(const time_t tTime);

	/** ������ʱ��תΪ�ַ���ʱ�� */
	static std::string GetStringTime(const time_t tTime);

	/** ��õ�ǰʱ�䣬��ȷ��΢�� */
	static std::string GetCurPreciseTime();

    /** �������ʱ�䣬hhmmss */
    static int GetNowTime(const time_t tTime);

    /** ����������ڣ�yyyymmdd */
    static int GetNowDate(const time_t tTime);

    /** ��䵱ǰ��ʱ���� */
    static time_t PadCurHMS(time_t tRawTime);

	/** �ж�һ���Ƿ������� */
	static bool IsLeapYear(const int dwYear);

	/** �������ʼ�� */
	static void InitRand();

	/** ����һ��������� */
	static int GetRandInt(int dwMin, int dwMax);

	/** �߳�˯�ߣ���λ��΢�룬��ȷ�������ϵͳ��� */
	static void USleep(unsigned long dwUSeconds);

/**************** ����ʵ���Զ�ʶ��ʱ���ʽ�ķ��� *************/
public:

	/** �Զ��İ�Ӣ�ĸ�ʽʱ��תΪtime_t���͵� */
	static time_t NormalizeTime(const std::string& strRawTime);

	/** �Զ��İ�ʱ��תΪstring���͵ı�׼��ʽ */
	static std::string NormalizeTimeAsString(const std::string& strRawTime);

    /** �Զ��İ����ĸ�ʽ��ʱ��תΪtime_t���͵� */
	static time_t NormalizeChineseTime(const std::string& strDateTime, std::string& strRawTime);
	static time_t NormalizeChineseTime(const std::string& strDateTime);

	/** ���� 2009-03-24 00:00:00 ������ʽ��ʱ��Ϊ time_t��ʧ�ܷ��� -1 */
	static time_t ParseDateTime(const char* str);
	static inline time_t ParseDateTime(const std::string& str)
	{
		return ParseDateTime(str.c_str());
	}
};

#endif
