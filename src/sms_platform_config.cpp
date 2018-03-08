/* ===========================================================================
*   Description: ¶ÌÐÅÆ½Ì¨·þÎñÅäÖÃÎÄ¼þ´¦ÀíÀà
*   Author:  huihai.shen@inin88.com
*   Date :  
*   histroy:
* ============================================================================
*/
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
#include "inet_comm.h"
#include "sms_platform_config.h"

using namespace std;



CSmsPlatformConfig::CSmsPlatformConfig()
{
    //¹¤×÷Ïß³ÌÊý
    thread_count = 4;

    //×î´óÖ§³ÖµÄÃèÊö·û
    max_epoll_size = 100000;

    //Ö§³ÖµÄ×î´ó¶¨Ê±Æ÷ÊýÁ¿
    max_timer_count = 500000;

	sms_ip.clear();
    sms_port = 0;

    log_level = 0;
    db_character_set = "UTF8";
}

CSmsPlatformConfig::~CSmsPlatformConfig()
{

}


bool CSmsPlatformConfig::loadConfig(const char* szFilename)
{

    CConfigRead config(szFilename);

	thread_count    = config.ReadAsInt("thread_count", 1);
    max_epoll_size  = config.ReadAsInt("max_epoll_size", 10000);
    max_timer_count = config.ReadAsInt("max_timer_count", 10000);

    local_receipt_ip    = config.ReadAsString("local_receipt_ip", "");  
    local_receipt_ip    =  StringUtility::Trim(local_receipt_ip,"\r\n \t");
    local_receipt_port  = config.ReadAsInt("local_receipt_port", 10010);

    sms_ip    = config.ReadAsString("sms_ip", "");  
    sms_ip    =  StringUtility::Trim(sms_ip,"\r\n \t");
    sms_port  = config.ReadAsInt("sms_port", 80);
    //Add by czl for GetDomainIP at 20180203 begin
    if( StringUtility::IsAlpha(sms_ip[0]) ) {
        string sms_tmp_ip = inet::GetDomainIp(sms_ip);
        if( !sms_tmp_ip.empty() && sms_tmp_ip.length() > 0 ) {
            sms_ip = sms_tmp_ip;
        }
    }
    //Add by czl for GetDomainIP at 20180203 end 
    log_level = config.ReadAsInt("log_level", 4);

    WRITELOG("sms_ip: %s\n sms_port: %d\n"
        "   local_receipt_ip: %s\n local_receipt_port: %d\n"
        "   thread_count: %d\n"
        "   max_epoll_size: %d\n"
        "   max_timer_count: %d\n"
        "   log_level: %d\n",
		sms_ip.c_str(), sms_port,
        local_receipt_ip.c_str(), local_receipt_port,
        thread_count,
        max_epoll_size,
        max_timer_count,
        log_level
        );


    //¶ÁÈ¡Êý¾Ý¿âÁ¬½ÓÐÅÏ¢
    // STDatabaseConn database_conn;
    database_infor.db_host   = config.ReadAsString("db_ip", "");  
    database_infor.db_host   =  StringUtility::Trim(database_infor.db_host,"\r\n \t");
    database_infor.db_port = config.ReadAsInt("db_port",0);

    database_infor.db_name   = config.ReadAsString("db_name", "");  
    database_infor.db_name   =  StringUtility::Trim(database_infor.db_name,"\r\n \t");

    database_infor.db_user   = config.ReadAsString("db_user", "");  
    database_infor.db_user   =  StringUtility::Trim(database_infor.db_user,"\r\n \t");

    database_infor.db_pass   = config.ReadAsString("db_pass", "");  
    database_infor.db_pass   =  StringUtility::Trim(database_infor.db_pass,"\r\n \t");
    if(database_infor.db_host.empty() || 
        database_infor.db_name.empty() ||
        database_infor.db_user.empty())
    {
        ERROR_LOG("Load config error, must paramter null, db_ip:%s, db_name:%s, db_user:%s.\n", 
            database_infor.db_host.c_str(), database_infor.db_name.c_str(), database_infor.db_user.c_str());
        return false;
    }

    std::string read_character_set;
    read_character_set   = config.ReadAsString("db_character_set", "");  
    read_character_set   =  StringUtility::Trim(read_character_set,"\r\n \t");
    if(!read_character_set.empty())
    {
        db_character_set = read_character_set;
    }

    WRITELOG("db_character_set: %s\n"
        "   db_ip: %s\n"
        "   db_port: %d\n"
        "   db_name: %s\n"
        "   db_user: %s\n"
        "   db_pass: ****\n",
        db_character_set.c_str(),
	    database_infor.db_host.c_str(),
        database_infor.db_port,
        database_infor.db_name.c_str(),
        database_infor.db_user.c_str()/* ,database_infor.db_pass.c_str()*/);

    //add by czl for add config info at 20180130 begin
    groupsend = config.ReadAsInt("groupsend",1) > 0;
    oper_str = config.ReadAsString("oper_str","");
    oper_str = StringUtility::Trim(oper_str,"\r\n \t");
    signature = config.ReadAsString("Signature","");
    signature = StringUtility::Trim(signature,"\r\n \t");
    if(oper_str.empty() || oper_str.length() <= 0 /*|| signature.empty() || signature.length() <= 0*/) {
        return false;
    }
    max_load_str = config.ReadAsString("max_load_str","100");
    smslength   = config.ReadAsInt("smslength",0);

    http_method = config.ReadAsString("zxst_http_method","POST");
    zxst_account = config.ReadAsString("zxst_account","smscs");           //中兴视通账号
    zxst_agentId = config.ReadAsString("zxst_agentId","1171003030");           //中兴视通代理渠道号
    zxst_key = config.ReadAsString("zxst_key","5XhQFtXxi5J8sgiy");               //中兴视通MD5 key
    sendRetry = config.ReadAsInt("zxst_sendRetry",3);
    zxst_sendTimeOut = config.ReadAsInt("zxst_sendTimeOut",10);             //发送请求超时时间
    iIntervalLoadDB = config.ReadAsInt("intervalDbTime",5);
    //add by czl for add config info at 20180130 end

    return true;
 }




