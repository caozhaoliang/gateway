/* ===========================================================================
*   Description: ÅäÖÃÎÄ¼þ´¦ÀíÀà
*   Author:  huihai.shen@inin88.com
*   Date :  
*   histroy:
* ============================================================================
*/
#ifndef  _SMS_CONFIG_H_
#define  _SMS_CONFIG_H_

#include <utility>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <stdio.h>

#include "string_utility.h"
#include "config_read.h"
#include "simple_log.h"


using namespace std;



struct STDatabaseConn
{
    std::string    db_host;
    unsigned short db_port; 
    std::string    db_name;
    std::string    db_user; 
    std::string    db_pass;
    size_t		   db_count;

    STDatabaseConn()
    {
        reset();
    }

    void reset()
    {
        db_host.clear();
        db_port   = 0;
        db_name.clear();
        db_user.clear();
        db_pass.clear();
        db_count = 0;
    }
};   


//µ×²ãÍ¨ÐÅµÄÅäÖÃÐÅÏ¢
class CSmsPlatformConfig
{
public:
    CSmsPlatformConfig();
    ~CSmsPlatformConfig();

    //¼ÓÔØÅäÖÃÎÄ¼þ
    bool loadConfig(const char* szFilename);

public:
    //¹¤×÷Ïß³ÌÊý
    unsigned int thread_count;

    //×î´óÖ§³ÖµÄÃèÊö·û
    unsigned int max_epoll_size;

    //×î´ó¶¨Ê±Æ÷ÊýÁ¿
    unsigned int max_timer_count;

    //ÈÕÖ¾¼¶±ð
    int log_level;

    //½ÓÊÜ»ØÖ´µÄ¼àÌý¶Ë¿Ú
    std::string local_receipt_ip;
    unsigned short local_receipt_port;

    //Á¬½ÓSMSÖ÷»úºÍ¶Ë¿ÚÐÅÏ¢
	std::string sms_ip;
	unsigned short sms_port;
    
    //Êý¾Ý¿âÁ¬½ÓÐÅÏ¢
    STDatabaseConn  database_infor;

    //Êý¾Ý¿â×Ö·û¼¯
    std::string db_character_set;

	//HTTPÇëÇó·½·¨£¬POST»òGET
	std::string http_method;

    //add by czl for add config info at 20180130 begin
    bool groupsend;     //是否群发
    std::string oper_str;       //通道号/列表 
    std::string max_load_str;           //每次从数据库中获取数据最大条数
    std::string signature;              //短信签名
    int smslength;                      //短信长度
    std::string zxst_account;           //中兴视通账号
    std::string zxst_agentId;           //中兴视通代理渠道号
    std::string zxst_key;               //中兴视通MD5 key
    int sendRetry;
    int iIntervalLoadDB;                //扫描DB间隔时间
    int zxst_sendTimeOut;               //中兴视通请求超时时间

    //add by czl for add config info at 20180130 end

};

#endif


