#ifndef _SIMPLE_LOG_H_
#define _SIMPLE_LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string>
#include <sys/shm.h>


using namespace std ;

enum LogLevel
{
    LOG_ERROR_LV = 0,
    LOG_WARN_LV = 1,
    LOG_DEBUG_LV = 2,
    LOG_TRACE_LV =3,
    LOG_INFO_LV =4

};

class CSimpleLog
{
private:
    string m_szLogBakDir;                            //备份日志存放目录
    string m_szLogName;                              //日志文件名称
    //string m_szLogNameBak;                           //备份日志文件名
    pthread_mutex_t  m_Lock;                         //锁:支持多线程写日志

    LogLevel m_log_level;                            // 日志级别
    char* level_buff;

private:

    //获取当前的时间
    string GetCurDateTimeStr()
    {
        char szTemp[ 256 ];
        memset( szTemp , 0 , sizeof( szTemp ) );

        time_t tNow = time( NULL );
        struct tm *tmNow;

        tmNow = localtime( &tNow );

        snprintf( szTemp ,sizeof(szTemp), "%d-%02d-%02d %02d:%02d:%02d" ,
                  tmNow->tm_year + 1900 , tmNow->tm_mon + 1 , tmNow->tm_mday ,
                  tmNow->tm_hour , tmNow->tm_min , tmNow->tm_sec );

        return szTemp;
    }

    //获取备份文件名
    void GetBakFileName(std::string & file_name)
    {
        char szTemp[ 256 ];
        memset( szTemp , 0 , sizeof( szTemp ) );

        time_t tNow = time( NULL );
        struct tm *tmNow;

        tmNow = localtime( &tNow );

        snprintf( szTemp ,sizeof(szTemp), "_%d%02d%02d%02d%02d%02d",
                  tmNow->tm_year + 1900 , tmNow->tm_mon + 1 , tmNow->tm_mday ,
                  tmNow->tm_hour , tmNow->tm_min , tmNow->tm_sec );

        file_name.append(m_szLogBakDir);
        file_name.append(m_szLogName);
        file_name.append(szTemp);
        
        return ;
    }

public:

    //构造函数
    CSimpleLog()
    {
        m_szLogBakDir =  "./logs/";
        m_szLogName =    "./log.txt";
        //m_szLogNameBak = "./log.txt.bak";
        Init();
    }

    //构造函数
    CSimpleLog(const char * szLogName)
    {
        m_szLogBakDir =  "./logs/";
        m_szLogName= szLogName;
        //m_szLogNameBak = string(szLogName) + ".bak" ;
        Init();
    }

    ~CSimpleLog()
    {
        shmdt(level_buff);
    }


    void Init()
    {
        pthread_mutex_init(&m_Lock,NULL);     //初始化锁

        m_log_level = LOG_ERROR_LV ;

        string home_pwd = getenv("HOME");
        home_pwd += "/ezDeviceCloud" ;
        key_t log_level_key = ftok(home_pwd.c_str(), 0) ;
        int shm_id = shmget(log_level_key, sizeof(int), IPC_CREAT | 0666) ;
        level_buff  = (char *)shmat(shm_id, NULL ,0);
        if(level_buff)
        {
            m_log_level = (LogLevel)(*(int*)level_buff);
        }

        printf("info:current log level:%s.\n",GetLevelDesc(m_log_level).c_str());

    }

    string GetLevelDesc( LogLevel level)
    {
        if( level == LOG_ERROR_LV )
        {
            return "[error]";
        }
        else if( level == LOG_WARN_LV )
        {
            return "[warn]";
        }
        else if( level == LOG_DEBUG_LV)
        {
            return  "[debug]";
        }
        else if( level == LOG_TRACE_LV)
        {
            return  "[trace]";
        }
        else if(level == LOG_INFO_LV )
        {
            return "[info]";
        }

        return "[unknown]";
    }

    void SetLogLevel( LogLevel level )
    {
        m_log_level = level ;
        if( level_buff > 0 )
        {
            *(int*)level_buff  = (int)level;
        }
    }

    int GetLogLevel()
    {
        m_log_level = (LogLevel)(*(int*)level_buff);
        return (int)m_log_level;
    }

    void WriteLog(const char *format, ...)
    {

        pthread_mutex_lock(&m_Lock);

        FILE *fp = fopen(m_szLogName.c_str(), "a+");
        if(fp == NULL)
        {
            pthread_mutex_unlock(&m_Lock);
            return ;
        }

        struct stat st;
        if(stat(m_szLogName.c_str(), &st) < 0)
        {
            pthread_mutex_unlock(&m_Lock);
            return ;
        }

        //每100M切换一次
        if(  st.st_size > 200*1024*1024)
        {
            //rename(m_szLogName.c_str(), m_szLogNameBak.c_str());

            std::string bak_file_name;
            GetBakFileName(bak_file_name);
            rename(m_szLogName.c_str(), bak_file_name.c_str());
        }

        va_list  ap;
        va_start(ap, format);

        string pdata = GetCurDateTimeStr();
        fprintf(fp, "[%s] ", pdata.c_str());
        vfprintf(fp, format, ap);
        fflush(fp);
        fclose(fp);

        pthread_mutex_unlock(&m_Lock);

        return ;

    }


    //写日志
    void WriteLog(LogLevel level,const char *format, ...)
    {
        if( level_buff > 0 )
        {
            m_log_level = (LogLevel)(*(int*)level_buff);
        }

        if(level > m_log_level)
        {
            return ;
        }

        pthread_mutex_lock(&m_Lock);

        FILE *fp = fopen(m_szLogName.c_str(), "a+");
        if(fp == NULL)
        {
            pthread_mutex_unlock(&m_Lock);
            return ;
        }

        struct stat st;
        if(stat(m_szLogName.c_str(), &st) < 0)
        {
            pthread_mutex_unlock(&m_Lock);
            return ;
        }

        //每100M切换一次
        if(  st.st_size > 200*1024*1024)
        {
            //rename(m_szLogName.c_str(), m_szLogNameBak.c_str());
            std::string bak_file_name;
            GetBakFileName(bak_file_name);
            rename(m_szLogName.c_str(), bak_file_name.c_str());
        }

        va_list  ap;
        va_start(ap, format);

        string pdata = GetCurDateTimeStr();
        fprintf(fp, "[%s]%s ", pdata.c_str(),GetLevelDesc(level).c_str());
        vfprintf(fp, format, ap);
        fflush(fp);
        fclose(fp);

        pthread_mutex_unlock(&m_Lock);

        return ;
    }
};

//写日志全局指针

#define ERROR_LOG(fmt, args...)	DETAIL(LOG_ERROR_LV, fmt , ##args)
#define WARN_LOG(fmt, args...)	DETAIL(LOG_WARN_LV, fmt , ##args)
#define DEBUG_LOG(fmt, args...)	DETAIL(LOG_DEBUG_LV, fmt , ##args)
#define TRACE_LOG(fmt, args...)	DETAIL(LOG_TRACE_LV, fmt , ##args)
#define INFO_LOG(fmt, args...)  DETAIL(LOG_INFO_LV, fmt , ##args)

extern CSimpleLog * g_pLog ;

#define DETAIL(level,fmt, args...)   g_pLog->WriteLog(level,"[%s][%d]%s: " fmt, __FILE__, __LINE__, __FUNCTION__ , ##args)

#define WRITELOG(fmt, args...) g_pLog->WriteLog("[%s][%d]%s: " fmt, __FILE__, __LINE__, __FUNCTION__ , ##args)

#define LOGLEVEL(level)  g_pLog->SetLogLevel(level);

#define GETLOGLEVEL()  g_pLog->GetLogLevel();


#endif



