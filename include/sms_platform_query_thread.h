/* ===========================================================================
*   Description: 查询线程
*   Author:  huihai.shen@inin88.com yangyang.ding@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#ifndef _SMS_PLATFORM_QUERY_THREAD_H_
#define _SMS_PLATFORM_QUERY_THREAD_H_

#include <queue>
#include <string>
#include <memory.h>
#include "thread_base.h"
#include "inet_comm.h"
#include "datasvr_freetds.h"
#include "sms_platform_task.h"


using namespace std;


class CSmsPlatformThreadManager;
class CSmsPlatformQueryThread : public CThreadBase
{
public:
    //构造函数
    CSmsPlatformQueryThread(CSmsPlatformThreadManager* thread_mgr, const int thread_stack_size = I_PTHREAD_DEFAULT_STACK_SIZE);

    //析构函数
    virtual ~CSmsPlatformQueryThread();


public:
    struct STSmsInfor
    {
		int snd_id;
		int cp_id;
		char sms_spport[32];
		char user_mblphone[64];
		char sms_serviceid[32];
		char sms_feetype[4];
		char sms_feecode[32];
		int sms_msgformat;
		char sms_content[1024];
		int sms_operator;
		char sms_linkid[32];
		int sms_report;
		char sms_timer[32];
		int sms_momtflag;
		char sms_intime[32];
		int sms_clientid;

        STSmsInfor()
        {
            reset();
        }

        void reset()
        {
			snd_id = 0;
			cp_id = 0;
			memset(sms_spport, 0x00, sizeof(sms_spport));
			memset(user_mblphone, 0x00, sizeof(user_mblphone));
			memset(sms_serviceid, 0x00, sizeof(sms_serviceid));
			memset(sms_feetype, 0x00, sizeof(sms_feetype));
			memset(sms_feecode, 0x00, sizeof(sms_feecode));		
			sms_msgformat = 0;
			memset(sms_content, 0x00, sizeof(sms_content));
			sms_operator = 0;
			memset(sms_linkid, 0x00, sizeof(sms_linkid));
			sms_report = 0;
			memset(sms_timer, 0x00, sizeof(sms_timer));
			sms_momtflag = 0;
			memset(sms_intime, 0x00, sizeof(sms_intime));
			sms_clientid = 0;
        }

    };

public:
    //初始化线程
    virtual int init();

    //启动线程
    virtual int start();
private:
	// modify by czl for change splite way at 20180223 begin
	int groupSendPushTask(std::vector<STSmsInfor> sms_list,
									string m_Signature, int contenLen,
									int count);
	int singleSendPushTask(std::vector<STSmsInfor> sms_list,
									string m_Signature, int contenLen,
									int count);
	int spliteByLen(string s,int len,vector<string>& v_str);
	int spliteByWord(string s, vector<string>& v_str);
	// modify by czl for change splite way at 20180223 end
protected:
    //主处理流程
    virtual int process();

    //获取短信列表信息
    int getSmsList(std::vector<STSmsInfor> & sms_list);

	//
	bool GetLoadMessageSql(string &sqlstr);

	//
	bool FillLoadMessageData(string smsContent,STSmsInfor &smsinfo, STSmsTask *smstask);

	int buildTaskformList(STSmsTask* task, vector<STSmsInfor> sms_list, string signature,int iNum);

	
    CSmsPlatformThreadManager* thread_manager;

	CDataSvrFreeTDS* db_manager_;
};


#endif

