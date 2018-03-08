#ifndef _CONFIG_READ_H
#define _CONFIG_READ_H
#include <string>
#include <map>
#include <vector>


//解析型如 127.0.0.1:1234 192.168.19.1:443 的格式
struct  tagIPCONFIG
{
    unsigned int  uiIP ;
    unsigned short usPort ;
} ;
typedef tagIPCONFIG IPCONFIG ;

typedef std::vector<IPCONFIG>  IPCONFIGLIST ;

class CConfigRead
{
    typedef std::map<std::string, std::string> DATA;

public:
    CConfigRead(const char * szFile);

    const char * ReadAsString(const char * szKey, char const * szDefualt = "");
    const char * ReadAsString(int index, const char * szKey, char const * szDefualt = "");
    int ReadAsInt(const char * szKey, int iDefault = 0);
    int ReadAsInt(int index, const char * szKey, int iDefault = 0);
    unsigned int ReadAsUint(const char * szKey, unsigned int iDefault = 0);
    unsigned int ReadAsUint(int index, const char * szKey, unsigned int iDefault = 0);
    double ReadAsDouble(const char * szKey, double dDefault = 0);
    double ReadAsDouble(int index, const char * szKey, double dDefault = 0);
    bool ReadAsHexBuf(const char * szKey, std::vector<unsigned char>& oBuf);
    bool ReadAsHexBuf(int index, const char * szKey, std::vector<unsigned char>& oBuf);
    int  ParserIPListConfig(IPCONFIGLIST& Addrs, const char* lpcszHost);


private:
    int Load(const char * szFile);
    void ParseItem(const char * szItem);
    static unsigned int Hex2Int(char c);

private:
    DATA m_oData;
};

#endif
