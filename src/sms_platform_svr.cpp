#include <signal.h>
#include "time_utility.h"
#include "file_utility.h"
#include "comm_def.h"
#include "thread_base.h"
#include "sms_platform_accept_handler.h"
#include "timer_manager.h"
#include "sms_platform_svr.h"
#include "version.h"

bool svr_run_flag = false;
CSmsPlatformSvr* g_sms_svr = NULL;
CSmsPlatformConfig* CSmsPlatformSvr::sms_config = NULL;

CSmsPlatformSvr::CSmsPlatformSvr(CSmsPlatformConfig* config) :
    timer_manager(NULL),
    thread_manager(NULL),
    receipt_accept_handler(NULL),
    total_msg_count(0)
{
    sms_config = config;
}

CSmsPlatformSvr::~CSmsPlatformSvr()
{
	sms_config = NULL;
}

//Æô¶¯
bool CSmsPlatformSvr::fire()
{
    if(NULL == sms_config)
    {
        ERROR_LOG("config null.\n");
        return false;
    }

    int iRet = -1;
    LOGLEVEL((LogLevel)sms_config->log_level);

    //³õÊ¼»¯epoll
    iRet = conn_epoll.create(sms_config->max_epoll_size);
    if(0 != iRet)
    {
        ERROR_LOG("Create epoll error.\n");
        return false;
    }

    //³õÊ¼»¯Ê±ÖÓµ¥Ôª
    NEW(timer_manager, TimerManager(sms_config->max_timer_count));
    if(NULL == timer_manager)
    {
        ERROR_LOG("New TimerManager error.\n");
        return false;
    }

    //Ïß³Ì¹ÜÀíÆ÷
    NEW(thread_manager, CSmsPlatformThreadManager(sms_config->thread_count));
    if(NULL == thread_manager)
    {
        ERROR_LOG("New CSmsPlatformThreadManager error.\n");
        return false;
    }

    //¼àÌýÆ÷
    NEW(receipt_accept_handler, CSmsPlatformAcceptHandler(sms_config->local_receipt_ip,
        sms_config->local_receipt_port, timer_manager, &conn_epoll, thread_manager));
    if(NULL == receipt_accept_handler)
    {
        ERROR_LOG("New CSmsPlatformAcceptHandler error.\n");
        return false;
    }

    //Æô¶¯·¢ËÍÏß³Ì
    iRet = thread_manager->fireSendThreads();
    if(iRet <= 0 )
    {
        ERROR_LOG("start send threads error.\n");
        return false;
    }

    //Æô¶¯¸üÐÂÏß³Ì
    iRet = thread_manager->fireUpdateThreads();
    if(iRet <= 0 )
    {
        ERROR_LOG("start update threads error.\n");
        return false;
    }

    //Æô¶¯²éÑ¯Ïß³Ì
    iRet = thread_manager->fireQueryThreads();
    if(iRet <= 0 )
    {
        ERROR_LOG("start query thread error.\n");
        return false;
    }

    return true;
}


void CSmsPlatformSvr::process()
{
    time_t current_time  = time(NULL) ;
    time_t last_check_time = current_time ;
    time_t last_report_time = current_time ;

    while(svr_run_flag)
    {
        // ´¦ÀíÍøÂçÏûÏ¢
        conn_epoll.waitForEvent(200);

        // ´¥·¢Ê±ÖÓ
        current_time = time(NULL);
        if(current_time - last_check_time >= 1)
        {
            if(current_time - last_report_time >= 30)
            {

                //¼à¿ØÉÏ±¨´¦ÊÇ
                //...

                last_report_time = current_time;
            }

            last_check_time = current_time;
            timer_manager->loopCheck(current_time); // ´¥·¢¶¨Ê±Æ÷
        }
    }
}


//ÓÃÐÅºÅ SIGUSR1ÖÕÖ¹³ÌÐò
void sig_usr1_handler(int sig)
{
    svr_run_flag = false ;
    FileUtility::WriteFile("signal.log","%s recv signal SIGUSR1,exit.\n",
        TimeUtility::GetCurTime().c_str());
}


CSimpleLog * g_pLog ;
int main(int argc,char** argv)
{
    if(argc < 2)
    {
        printf("usage:%s config_file.\n",argv[0]);
        return -1;
    }

    if(string(argv[1]) == "-v" || string(argv[1]) == "-V")
    {
        string build_time = StringUtility::GetBuildTime();
        string minVersionStr = MINOR_VERSION_STRING;
        if(minVersionStr.empty() || minVersionStr.length() ==0 ) {
            printf("警告！本副本在编译时使用的源代码没有纳入源代码版本控制系统。\n");
        }
        printf( "%s v%s.%s 版(%s,%s)\n", 
                KDS_PROGRAM_NAME_CN , 
                MAIN_VERSION_STRING,
                MINOR_VERSION_STRING,
                __DATE__,
                __TIME__);
        // printf("%s build on %s.\n",argv[0],build_time.c_str());
        return -1;
    }

    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGHUP ,SIG_IGN);
    signal(SIGCHLD,SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    struct sigaction act, oact;
    act.sa_handler = sig_usr1_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaddset(&act.sa_mask, SIGUSR1);
    if(sigaction(SIGUSR1, &act, &oact) < 0)
    {
        printf("sigaction(SIGUSR1, &act, &oact) error.\n");
        return -1;
    }

    svr_run_flag = true ;

    g_pLog = new CSimpleLog("../log/sms_platform.log");
    LOGLEVEL(LOG_INFO_LV);

    WRITELOG("sms_platform begin run...................\n");

    char* filename = argv[1];
    CSmsPlatformConfig oConfig;
    if(!oConfig.loadConfig(filename))
    {
        ERROR_LOG("sms_svr load config error:%s !\n", filename);
        return 1;
    }

    g_sms_svr = new CSmsPlatformSvr(&oConfig);
    if(!g_sms_svr->fire())
    {
        printf("sms_svr fired failed !\n");
        return 1;
    }

    g_sms_svr->process();

    delete g_sms_svr;

    WRITELOG("g_sms_svr exit...................\n");

    return 0;
}

