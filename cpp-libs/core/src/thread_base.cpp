/* ===========================================================================
*   Description: �̻߳���
*   Author:  huihai.shen@inin88.com
*   Date : 
*   histroy:
* ============================================================================
*/
#include "simple_log.h"
#include "thread_base.h"


//���캯��
CThreadBase::CThreadBase(const int stack_size) : 
    thread_stack_size(stack_size),
    runing_flag(false)
{

}

//��������
CThreadBase::~CThreadBase()
{
}

//��ʼ���߳���
int CThreadBase::initPthreadLock(pthread_mutex_t* pthread_lock)
{
    pthread_mutexattr_t mat;
	int result = 0;

	if ((result = pthread_mutexattr_init(&mat)) != 0)
	{
		ERROR_LOG("call pthread_mutexattr_init() fail, " \
			"errno: %d, error info: %s", \
			result, STRERROR(result));
		return result;
	}

	if ((result = pthread_mutexattr_settype(&mat, \
			PTHREAD_MUTEX_ERRORCHECK_NP)) != 0)
	{
		ERROR_LOG("call pthread_mutexattr_settype() fail, " \
			"errno: %d, error info: %s", \
		     result, STRERROR(result));
		return result;
	}

	if ((result = pthread_mutex_init(pthread_lock, &mat)) != 0)
	{
		ERROR_LOG("call pthread_mutex_init() fail, " \
			"errno: %d, error info: %s", \
			result, STRERROR(result));
		return result;
	}

	if ((result = pthread_mutexattr_destroy(&mat)) != 0)
	{
		ERROR_LOG("call thread_mutexattr_destroy() fail, " \
			"errno: %d, error info: %s", \
			result, STRERROR(result));
		return result;
	}

    return result;
}

//��ʼ���߳���
int CThreadBase::initPthreadAttr(pthread_attr_t* pattr, int const stack_size)
{
    size_t old_stack_size;
	size_t new_stack_size;
	int result = 0;

	if ((result = pthread_attr_init(pattr)) != 0)
	{
		ERROR_LOG("call pthread_attr_init() fail, " \
			"errno: %d, error info: %s", \
			result, STRERROR(result));
		return result;
	}

	if ((result = pthread_attr_getstacksize(pattr, &old_stack_size)) != 0)
	{
		ERROR_LOG("call pthread_attr_getstacksize() fail, " \
			"errno: %d, error info: %s", \
			result, STRERROR(result));
		return result;
	}

	if (stack_size > 0)
	{
		if (old_stack_size != stack_size)
		{
			new_stack_size = stack_size;
		}
		else
		{
			new_stack_size = 0;
		}
	}
	else if (old_stack_size < 1 * 1024 * 1024)
	{
		new_stack_size = 1 * 1024 * 1024;
	}
	else
	{
		new_stack_size = 0;
	}

	if (new_stack_size > 0)
	{
		if ((result = pthread_attr_setstacksize(pattr, \
				new_stack_size)) != 0)
		{
			ERROR_LOG("call pthread_attr_setstacksize() fail, " \
				"errno: %d, error info: %s", \
				result, STRERROR(result));
			return result;
		}
	}

	if ((result = pthread_attr_setdetachstate(pattr, \
			PTHREAD_CREATE_DETACHED)) != 0)
	{
		ERROR_LOG("call pthread_attr_setdetachstate() fail, " \
			"errno: %d, error info: %s", \
			result, STRERROR(result));
		return result;
	}

    return result;
}


//�����߳�
int CThreadBase::start()
{
    return 0;
}

//����������
int CThreadBase::process()
{
    return 0;
}

//ֹͣ�߳�
int CThreadBase::stop()
{
    int result = 0;
    if ((result = pthread_cancel(thread_id)) != 0)
	{
		ERROR_LOG("call pthread_cancel() fail, " \
			"errno: %d, error info: %s", \
			result, STRERROR(result));
		return result;
	}

    runing_flag = false;

    return result;
}


//�����߳�
void* CThreadBase::runThread(void* arg)
{
    CThreadBase * pThread = (CThreadBase * )arg;
    pThread->process();

    return NULL;
}

