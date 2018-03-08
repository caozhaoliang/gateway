/* ===========================================================================
*   Description: 短信详情类
*   Author:  yangyang.ding@inin88.com
*   Date :
*   histroy:
* ============================================================================
*/

#ifndef _SMS_PLATFORM_TASK_H_
#define _SMS_PLATFORM_TASK_H_

#include <string>
#include <map>

#define SMS_URI_RECEIPT_TASK		"/sms_receipt"
#define SMS_URI_UPLINK_TASK			"/sms_uplink"

using namespace std;

enum ETaskType
{
	//心跳任务, 直接应答
	SMS_HEART_TASK = 0,

	//发送状态
	SMS_SEND_TASK = 1,

	//回执状态
	SMS_RECEIPT_TASK = 2,

	//上行短信
	SMS_UPLINK_TASK = 3
};


struct STSmsTask
{
	ETaskType task_type;
	string attach;
	string content;
	string mobile_to;
	string mobile_from;
	string snd_id;
	int snd_status;
	int report_status;
	string msg_id;
	int oper_id;
	map<string, string>	mp_snd_mobile;

	STSmsTask() {
		reset();
	}

	STSmsTask(const STSmsTask* task) {
		attach = task->attach;
		content = task->content;
		mobile_from = task->mobile_from;
		mobile_to = task->mobile_to;
		snd_id = task->snd_id;
		snd_status = task->snd_status;
		report_status = task->report_status;
		msg_id = task->msg_id;
		oper_id = task->oper_id;

	}

	void reset() {
		attach = "";
		content = "";
		mobile_from = "";
		mobile_to = "";
		snd_id = "";
		snd_status = 0;
		report_status = 0;
		msg_id = "";
		oper_id = 0;
		mp_snd_mobile.clear();
	}
};

#endif // !_SMS_PLATFORM_TASK_H_
