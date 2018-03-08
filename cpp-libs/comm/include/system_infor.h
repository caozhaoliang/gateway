/* ===========================================================================
*   Description: 获取系统参数
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
    //获取CPU利用率参数
    static bool getCpuUse(long int & iAll, long int & iIdle);

    //计算CPU使用率
    static float calcCpuUse(long int & old_all, 
                            long int & old_idle, 
                            long int & new_all, 
                            long int & new_idle);

    //计算磁盘使用率
    static bool getMemUse(float & use_percent);

    //计算磁盘使用率
    static bool getDiskUse(float & use_percent);

};

#endif
