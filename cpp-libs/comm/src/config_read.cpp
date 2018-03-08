#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#include "config_read.h"

using namespace std ;

////////////////////////////////////////////////////////
//// 读取配置文件
CConfigRead::CConfigRead(const char * szFile)
{
    Load(szFile);
}

int CConfigRead::Load(const char * szFile)
{
    //int iRet = 0;
    if (szFile)
    {
        char szItem[2048] = {0};

        std::ifstream oFile(szFile, std::ios_base::in);
        while (oFile.getline(szItem, sizeof(szItem) - 1))
        {
            if(strlen(szItem) == 0)
            {
                continue;
            }
            char* pItem = szItem;
            while(*pItem == ' ' || *pItem == '\t') ++pItem;
            if (*pItem != ';') //';'开头的为注释
            {
                char * pRemark = strrchr(szItem, ';') ;
                if (pRemark)
                {
                    *pRemark = 0 ;
                }
                ParseItem(szItem);
            }
        }
    }
    return m_oData.size();
}

void CConfigRead::ParseItem(const char * szItem)
{
    const char * p = strchr(szItem, '=');
    if (p)
    {
        const char * pFrom = szItem;
        while (*pFrom == ' ' || *pFrom == '\t') ++pFrom;
        const char * pTo = p - 1;
        while (pTo >= pFrom && (*pTo == ' ' || *pTo == '\t')) --pTo;
        if (pTo >= pFrom)
        {
            std::string strKey(pFrom, pTo + 1);
            pFrom = p + 1;
            while (*pFrom == ' ' || *pFrom == '\t') ++pFrom;
            pTo = szItem + strlen(szItem) - 1;
            while (pTo >= pFrom && (*pTo == ' ' || *pTo == '\t')) --pTo;
            if (pTo >= pFrom)
            {
                m_oData[strKey] = std::string(pFrom, pTo + 1);
            }
        }
    }
}

const char * CConfigRead::ReadAsString(const char * szKey, char const * szDefualt)
{
    DATA::const_iterator it = m_oData.find(szKey);
    if (it != m_oData.end())
    {
        return it->second.c_str();
    }
    return szDefualt;
}

const char * CConfigRead::ReadAsString(int index, const char * szKey, char const * szDefault)
{
    std::ostringstream oss;
    oss << szKey << index;
    return ReadAsString(oss.str().c_str(), szDefault);
}

int CConfigRead::ReadAsInt(const char * szKey, int iDefault)
{
    DATA::const_iterator it = m_oData.find(szKey);
    if (it != m_oData.end())
    {
        return atoi(it->second.c_str());
    }

    return iDefault;
}

int CConfigRead::ReadAsInt(int index, const char * szKey, int iDefault)
{
    std::ostringstream oss;
    oss << szKey << index;
    return ReadAsInt(oss.str().c_str(), iDefault);
}

unsigned int CConfigRead::ReadAsUint(const char * szKey, unsigned int iDefault)
{
    DATA::const_iterator it = m_oData.find(szKey);
    if (it != m_oData.end())
    {
        return strtoul(it->second.c_str(), 0, 10);
    }

    return iDefault;
}

unsigned int CConfigRead::ReadAsUint(int index, const char * szKey, unsigned int iDefault)
{
    std::ostringstream oss;
    oss << szKey << index;
    return ReadAsUint(oss.str().c_str(), iDefault);
}


double CConfigRead::ReadAsDouble(const char * szKey, double dDefault)
{
    DATA::const_iterator it = m_oData.find(szKey);
    if (it != m_oData.end())
    {
        return atof(it->second.c_str());
    }

    return dDefault;
}

double CConfigRead::ReadAsDouble(int index, const char * szKey, double dDefault)
{
    std::ostringstream oss;
    oss << szKey << index;
    return ReadAsDouble(oss.str().c_str(), dDefault);
}

//szKey = 1f 32 00 48
bool CConfigRead::ReadAsHexBuf(const char * szKey, std::vector<unsigned char>& oBuf)
{
    DATA::const_iterator it = m_oData.find(szKey);
    if (it != m_oData.end())
    {
        oBuf.clear();
        const char * p = it->second.c_str();
        const char * pEnd = p + it->second.length();
        while (p + 1 < pEnd)
        {
            oBuf.push_back((Hex2Int(p[0]) << 4) + Hex2Int(p[1]));
            p += 3;
        }
        return true;
    }
    return false;
}

bool CConfigRead::ReadAsHexBuf(int index, const char * szKey, std::vector<unsigned char>& oBuf)
{
    std::ostringstream oss;
    oss << szKey << index;
    return ReadAsHexBuf(oss.str().c_str(), oBuf);
}


inline unsigned int CConfigRead::Hex2Int(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 0x0a;
    }
    else if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 0x0a;
    }
    return 0;
}


//解析型如 127.0.0.1:1234 192.168.19.1:443 的格式
int CConfigRead::ParserIPListConfig(IPCONFIGLIST& Addrs, const char* lpcszHost)
{
    if(lpcszHost == 0 || *lpcszHost ==0 )
    {
        return 0 ;
    }
    Addrs.clear();

    char* lpszHost = 0;
    int len = (int)strlen(lpcszHost);
    lpszHost = new char[len+1];
    memcpy(lpszHost, lpcszHost, len);
    lpszHost[len] = 0;

    for(int i = 0; i < len; i++)
    {
        //去掉前面的空格
        while(i < len &&
                (lpszHost[i] == ' ' ||  lpszHost[i] == '\t' )
             )
        {
            i++;
        }

        int start = i;

        //找到第一个合法字段的结束
        while(i < len &&
                lpszHost[i] != ' ' &&
                lpszHost[i] != '\t'&&
                lpszHost[i] != ':'
             )
        {
            i++;
        }

        if(i > len)
            break;

        char c = lpszHost[i];
        lpszHost[i] = 0;
        const char* lpszIP = lpszHost + start;
        unsigned short nPort = 0;
        if(c == ':')
        {
            //后面是端口号
            nPort = (unsigned short)atol(&lpszHost[i+1]);
        }

        printf("ip %s:%u.\n",lpszIP,nPort);

        IPCONFIG IPItem;
        memset(&IPItem,0,sizeof(IPCONFIG));
        IPItem.uiIP = inet_addr(lpszIP);
        IPItem.usPort = htons(nPort);
        Addrs.push_back(IPItem);
        lpszHost[i] = c; //恢复这个值
        //找到下一项的配置开始处,及从i开始的第一个空格
        for(; i < len; i++)
        {
            if( lpszHost[i] == ' ' ||
                    lpszHost[i] =='\t' ||
                    lpszHost[i] =='\r' ||
                    lpszHost[i] =='\n')
            {
                i--;
                break; //找到分隔符
            }
        }
    }

    delete [] lpszHost;
    return (int)Addrs.size();
}
/////////////////
