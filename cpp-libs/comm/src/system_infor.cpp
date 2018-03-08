/* ===========================================================================
*   Description: 获取系统参数
*   Author:  huihai.shen 
*   Date :  
*   histroy:
* ============================================================================
*/
#include <utility>
#include <vector>
#include <string>
#include <stdio.h>
#include <json/json.h>
#include "simple_log.h"
#include "system_infor.h"

using namespace std;



CSystemInfor::CSystemInfor()
{

}

CSystemInfor::~CSystemInfor()
{

}

//获取CPU利用率参数
bool CSystemInfor::getCpuUse(long int & iAll, long int & iIdle)
{
    FILE* fp = NULL;
    char buf[128];
    char cpu[5];
    long int user, nice, sys, idle, iowait, irq, softirq;
    //long int all1, all2, idle1, idle2;
    //float usage = 0.00;
    memset(buf, 0x00, sizeof(buf));
    memset(cpu, 0x00, sizeof(cpu));

    //while(1)
    //{
    fp = fopen("/proc/stat","r");
    if(fp == NULL)
    {
        return false;
    }

    fgets(buf, sizeof(buf), fp);
    sscanf(buf,"%s%ld%ld%ld%ld%ld%ld%ld",
        cpu, &user, &nice, &sys, &idle, &iowait, &irq, &softirq);

    iAll  = user + nice + sys + idle + iowait + irq + softirq;
    iIdle = idle;
    /*
    rewind(fp);

    //第二次取数据
    sleep(CK_TIME);
    memset(buf,0,sizeof(buf));
    cpu[0] = '\0';
    user = nice = sys = idle = iowait = irq = softirq = 0;
    fgets(buf, sizeof(buf), fp);
    sscanf(buf,"%s%d%d%d%d%d%d%d",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);

    all2 = user + nice + sys + idle + iowait + irq + softirq;
    idle2 = idle;

    usage = (float)(all2-all1-(idle2-idle1)) / (all2-all1)*100;


    printf("all=%d\n",all2-all1);
    printf("ilde=%d\n",all2-all1-(idle2-idle1));
    printf("cpu use = %.2f\%\n",usage);
    printf("=======================\n");
    */

    fclose(fp);

    //}

    return true;

}

//计算CPU使用率
float CSystemInfor::calcCpuUse(long int & old_all, 
                               long int & old_idle, 
                               long int & new_all, 
                               long int & new_idle)
{
    return (float)(new_all - old_all - (new_idle - old_idle)) / (new_all - old_all) * 100;
}

//计算内存使用率
bool CSystemInfor::getMemUse(float & use_percent)
{
    std::string szCmd("${KDS_YEAF_HOME}/bin/system_monitor.sh mem");
    FILE *pfp;
    if(NULL == (pfp = popen(szCmd.c_str(), "r")))
    {
        ERROR_LOG("Excute:<%s> error.\n", szCmd.c_str());
        return false;
    }

    char sBuf[512];
    memset(sBuf, 0x00, sizeof(sBuf));
    if(NULL == fgets(sBuf, sizeof(sBuf), pfp))
    {
        ERROR_LOG("Get pipe file content error.\n");
        pclose(pfp);
        return false;
    }

    pclose(pfp);
    use_percent = (float)100 - atof(sBuf) / 100;

    return true;
 }

//计算磁盘使用率
bool CSystemInfor::getDiskUse(float & use_percent)
{
    std::string szCmd("${KDS_YEAF_HOME}/bin/system_monitor.sh disk"); 
    FILE *pfp;
    if(NULL == (pfp = popen(szCmd.c_str(), "r")))
    {
        ERROR_LOG("Excute:<%s> error.\n", szCmd.c_str());
        return false;
    }

    char sBuf[512];
    memset(sBuf, 0x00, sizeof(sBuf));
    if(NULL == fgets(sBuf, sizeof(sBuf), pfp))
    {
        ERROR_LOG("Get pipe file content error.\n");
        pclose(pfp);
        return false;
    }

    pclose(pfp);
    use_percent = atof(sBuf);

    return true;
}


