/* ===========================================================================
*   Description: 短信平台回执HTTP工作处理器
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#include <sstream>
#include "inet_comm.h"
#include "md5_packing.h"
#include "string_utility.h"
#include "comm_def.h"
#include "sms_platform_update_thread.h"
#include "sms_platform_accept_handler.h"
#include "sms_platform_receipt_handler.h"
#include <json/json.h>
#include "sms_platform_svr.h"

using namespace std;


CSmsPlatformReceiptHandler::CSmsPlatformReceiptHandler(int handle, 
                                                       TimerManager* manager,
                                                       CEPoller* epoller, 
                                                       CSmsPlatformThreadManager* thread_mgr)
{
    _sock_fd = handle;

    thread_manager = thread_mgr;
    timer_manager  = manager;
    bind_epoller   = epoller;

    time_id = INVALID_TIME_ID;

    //事件绑定
    attachEpoller(bind_epoller);
    setEvent(FD_RECV|FD_CLOSE|FD_ERROR);

    //每10秒检查是否完成处理
    time_id = timer_manager->setTimer(this, 6, true, 0) ;

}


CSmsPlatformReceiptHandler::~CSmsPlatformReceiptHandler()
{
    INFO_LOG("client_ip=%s:%d\n", inet::inet_ntoa(client_addr.sin_addr.s_addr),
        client_addr.sin_port);

    closeSocket();

    if(time_id != INVALID_TIME_ID)
    {
        timer_manager->killTimer(time_id);
        time_id = INVALID_TIME_ID;
    }

}

void CSmsPlatformReceiptHandler::closeSocket()
{
    if( _sock_fd > 0)
    {
        detachEpoller();
        close(_sock_fd);
        _sock_fd = -1;
    }
}

void CSmsPlatformReceiptHandler::setClientAddr(struct sockaddr_in & address)
{
    client_addr = address;
}


sockaddr_in CSmsPlatformReceiptHandler::getClientAddr()
{
    return client_addr;
}

void CSmsPlatformReceiptHandler::freeMyself()
{
    delete this;
}


int CSmsPlatformReceiptHandler::onClose()
{
    freeMyself();
    return 0;
}


int CSmsPlatformReceiptHandler::onError()
{
    freeMyself();
    return 0;
}

void CSmsPlatformReceiptHandler::onTimer(int id,time_t current_time)
{
    if( id == time_id )
    {
        //超时释放
        freeMyself();
    }
 
}

int CSmsPlatformReceiptHandler::onRecv()
{
    int recv_len;
    char sRecvBuf[1024];
    memset(sRecvBuf, 0x00, sizeof(sRecvBuf));

    do
    {
        recv_len = recv(_sock_fd, sRecvBuf, sizeof(sRecvBuf) - 1, 0);
    }
    while(recv_len < 0 && errno == EINTR );

    if( recv_len == 0 )
    {
        // 对端连接关闭
        INFO_LOG("socket closed by peer,errno:%d,desc:%s,client_ip=%s\n",
            errno,strerror(errno), inet::inet_ntoa(client_addr.sin_addr.s_addr));
        freeMyself();
        return -1;
    }

    if (recv_len < 0)
    {
        if( errno != EAGAIN )
        {
            //异常
            ERROR_LOG("socket recv error,errno:%d,desc:%s,client_ip=%s\n",
                errno,strerror(errno), inet::inet_ntoa(client_addr.sin_addr.s_addr));
            freeMyself();
            return -1;
        }
        else
        {
            //
        }

        return 0;
    }

    INFO_LOG("ip=%s:%d, fd:%d\n%s\n", 
        inet::inet_ntoa(client_addr.sin_addr.s_addr), 
        client_addr.sin_port, _sock_fd, sRecvBuf);

    int iRet = http_parser.ParseHttp(sRecvBuf, recv_len);
    if(I_RET_CODE_INCOMPLETE == iRet)
    {
        //未完成
        return 0;
    }
    else if(I_RET_CODE_SUCCESS == iRet)
    {
        iRet = processMsg(http_parser.GetHttpMessage());
        if(0 != iRet)
        {
            ERROR_LOG("Process error uri:%s, error code:%d.\n", 
                http_parser.GetHttpUri().c_str(), iRet);
            std::string rsp_msg;
            responeHttpMsg(rsp_msg.c_str(), rsp_msg.size());
            freeMyself();
        }
    }
    else
    {
        ERROR_LOG("recv http format error, client_ip=%s:%d; %s\n",
            inet::inet_ntoa(client_addr.sin_addr.s_addr), client_addr.sin_port, sRecvBuf);
        freeMyself();
        return -1;
    }

    return 0 ;
}

//处理消息
int CSmsPlatformReceiptHandler::processMsg(const STHttpMsg & http_msg)
{
	int iRet = 0;
    DEBUG_LOG("processMsg param http_msg.http_uri:%s\n",http_msg.http_uri.c_str());
	if (std::string(SMS_URI_RECEIPT_TASK) == http_msg.http_uri)
	{
		iRet = reqSmsReceipt(http_msg.http_body);
	}
	else if (std::string(SMS_URI_UPLINK_TASK) == http_msg.http_uri)
	{
		iRet = reqSmsUplink(http_msg.http_body);
	}

    
    return iRet;
}

//短信回执
int CSmsPlatformReceiptHandler::reqSmsReceipt(const std::string & req_msg)
{
	//解析回执报文件
	Json::Reader reader;
	Json::Value root;

	std::string sRspMsg;

	try {
		if (!reader.parse(req_msg.c_str(), root))
		{
			ERROR_LOG("Parse json text error: %s\n", req_msg.c_str());
			getErrodeCode(sRspMsg, 0);
			responeHttpMsg(sRspMsg.c_str(), sRspMsg.size());
			return -1;
		}
		std::string mobilestr = root["mobile"].asString();
		std::string	attachstr = root["attach"].asString();
		int trade_code = root["trade_code"].asInt();

		CSmsPlatformUpdateThread* pThread = (CSmsPlatformUpdateThread*)thread_manager->getUpdateThread();
		if (NULL == pThread)
		{
			ERROR_LOG("Get update thread failed.\n");
			return 1;
		}

		//组装回执任务传给更新线程，把回执更新到数据库中
		std::string msgid = mobilestr + attachstr;
		std::string operid = CSmsPlatformSvr::GetConfig()->oper_str;
		int sms_status = 0; //成功

		NEW(pThread->ptask, STSmsTask());
		pThread->ptask->task_type = SMS_RECEIPT_TASK;
		pThread->ptask->report_status = sms_status;
		pThread->ptask->msg_id = msgid;
		pThread->ptask->oper_id = atoi(operid.c_str());

		pThread->pushTask(pThread->ptask);

		getErrodeCode(sRspMsg, 1);
		responeHttpMsg(sRspMsg.c_str(), sRspMsg.size());
	}
	catch (exception &ex) {
        ERROR_LOG("reqSmsReceipt catch a exception ex.what():%s\n",ex.what());
		return -1;
	}

	return 0;
}

//上行短信
int CSmsPlatformReceiptHandler::reqSmsUplink(const std::string & req_msg)
{
	//解析回执报文件
	Json::Reader reader;
	Json::Value root;

	std::string sRspMsg;

	try {
		if (!reader.parse(req_msg.c_str(), root))
		{
			ERROR_LOG("Parse json text error: %s\n", req_msg.c_str());
			getErrodeCode(sRspMsg, 0);
			responeHttpMsg(sRspMsg.c_str(), sRspMsg.size());
			return -1;
		}
		std::string mobilestr = root["mobile"].asString();
		std::string	contentstr = root["content"].asString();

		CSmsPlatformUpdateThread* pThread = (CSmsPlatformUpdateThread*)thread_manager->getUpdateThread();
		if (NULL == pThread)
		{
			ERROR_LOG("Get update thread failed.\n");
			return 1;
		}

		//组装回执任务传给更新线程，把回执更新到数据库中
		//组msgid
		std::string msgid = mobilestr;
		std::string operid = CSmsPlatformSvr::GetConfig()->oper_str;

		NEW(pThread->ptask, STSmsTask());
		pThread->ptask->task_type = SMS_UPLINK_TASK;
		pThread->ptask->msg_id = msgid;
		pThread->ptask->oper_id = atoi(operid.c_str());
		pThread->ptask->mobile_from = mobilestr;
		pThread->ptask->content = contentstr;

		pThread->pushTask(pThread->ptask);

		getErrodeCode(sRspMsg, 1);
		responeHttpMsg(sRspMsg.c_str(), sRspMsg.size());
	}
	catch (exception &ex) {
        ERROR_LOG("reqSmsUplink catch a exception ex.what():%s\n",ex.what());
		return -1;
	}

	return 0;
}

int CSmsPlatformReceiptHandler::getErrodeCode(std::string &errstr, const int errflag)
{
	try {
		Json::Value  rvalue;
		Json::FastWriter writer;

		switch (errflag)
		{
		case 0:
			rvalue["msg"] = "faild";
			rvalue["code"] = -1;
			break;
		case 1:
			rvalue["msg"] = "success";
			rvalue["code"] = 0;
			break;
		default:
			break;
		}
		std::string jsonstr = writer.write(rvalue);
		int nsize = jsonstr.size();
		errstr = std::string(jsonstr, 0, nsize - 1);
	}
	catch (const std::exception& ex) {
		ERROR_LOG("getErrodeCode catch a exception ex.what():%s\n",ex.what());
	}

	return 0;
}

//应答HTTP消息
int CSmsPlatformReceiptHandler::responeHttpMsg(const char* buff, const int len)
{
    char sBuf[1024];
    int header_len = sprintf(sBuf, "HTTP/1.1 200 OK\r\n" \
        "Server: Apache-Coyote/1.1\r\n" \
        "Content-Type: text/html;charset=UTF-8\r\n" \
        "Content-Length: %d\r\n" \
        "Date: Wed, 10 Nov 2016 07:10:05 GMT\r\n\r\n",
        len);

    int send_len = 0;
    do
    {
        send_len = sendTcpMsg((char*)sBuf, header_len);
        if(send_len != header_len)
        {
            ERROR_LOG("Respone http header error, header_len:%d, send_len:%d, client_ip=%s:%d, errno:%d, %s.\n%s\n",
                header_len, send_len, inet::inet_ntoa(client_addr.sin_addr.s_addr), client_addr.sin_port, 
                errno,strerror(errno), sBuf);
            break;
        }

        if(len > 0)
        {
            //INFO_LOG("Response http body:%s\n", buff);
            send_len = sendTcpMsg((char*)buff, len);
            if(send_len != len)
            {
                ERROR_LOG("Respone http body error, len:%d, send_len:%d, client_ip=%s:%d, errno:%d, %s.\n%s\n",
                    len, send_len, inet::inet_ntoa(client_addr.sin_addr.s_addr), client_addr.sin_port,
                    errno,strerror(errno), buff);
                break;
            }
        }
		


    }while(0);

    return 0;
}




