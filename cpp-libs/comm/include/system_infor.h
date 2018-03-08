/* ===========================================================================
*   Description: ��ȡϵͳ����
*   Author:  huihai.shen 
*   Date :  
*   histroy:
* ============================================================================
*/
#ifndef  _SYSTEM_INFOR_H_
#define  _SYSTEM_INFOR_H_

using namespace std;



class CSystemInfor
{
public:
    CSystemInfor();
    ~CSystemInfor();

public:
    //��ȡCPU�����ʲ���
    static bool getCpuUse(long int & iAll, long int & iIdle);

    //����CPUʹ����
    static float calcCpuUse(long int & old_all, 
                            long int & old_idle, 
                            long int & new_all, 
                            long int & new_idle);

    //�������ʹ����
    static bool getMemUse(float & use_percent);

    //�������ʹ����
    static bool getDiskUse(float & use_percent);

};

#endif
