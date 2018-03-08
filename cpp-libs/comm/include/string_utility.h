#ifndef     __STRING_UTILITY_H__
#define     __STRING_UTILITY_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>

// 不分大小写的字符串比较函数
#ifdef _WIN32
#ifndef STRNCASECMP
#define STRNCASECMP			_strnicmp
#endif
#ifndef STRCASECMP
#define STRCASECMP			_stricmp
#endif
#define snprintf            _snprintf
#else
#ifndef STRNCASECMP
#define STRNCASECMP			strncasecmp
#endif
#ifndef STRCASECMP
#define STRCASECMP			strcasecmp
#endif
#endif

#ifdef _WIN32
#define END_LINE "\r\n"
#else
#define END_LINE "\n"
#endif

#ifndef ISALPHA_DIGIT_SPACE

#define ISALPHA_DIGIT_SPACE
#define ISALPHA(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
#define ISDIGIT(c) (c >= '0' && c <= '9')
#define ISSPACE(c) (c == ' ' || c == '\t' || c == '\r' || c == '\n')
#endif

#ifdef __GNUC__
#define SU_DEPRECATED_BY(New)  __attribute__((deprecated))
#else
#ifdef _MSC_VER
#if _MSC_VER >= 1400
#define SU_DEPRECATED_BY(New) __declspec(deprecated(#New))
#else
#define SU_DEPRECATED_BY(New) __declspec(deprecated)
#endif
#endif //_MSC_VER
#endif

class StringUtility
{
public:

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 字符串内容处理

    /** 功能: 删除在strValue中左边的无效字符 */
    static std::string TrimLeft(const std::string& strValue, const std::string& strTarget);
    static std::string TrimLeft(const std::string& strValue);

    /** 功能: 删除在strValue中右边的无效字符 */
    static std::string TrimRight(const std::string& strValue, const std::string& strTarget);
    static std::string TrimRight(const std::string& strValue);

    /** 功能: 删除在strValue中两头的无效字符 */
    static std::string Trim(const std::string& strValue, const std::string& strTarget);
    static std::string Trim(const std::string& strValue);

    /** 请使用 Trim */
    SU_DEPRECATED_BY(Trim) static std::string TrimBothSides(const std::string& strValue, const std::string& strTarget = "\r\n \t")
    {
        return Trim(strValue, strTarget);
    }

    static void InplaceTrimLeft(std::string& strValue, const std::string& strTarget);
    static void InplaceTrimRight(std::string& strValue, const std::string& strTarget);
    static void InplaceTrim(std::string& strValue, const std::string& strTarget);
    static void InplaceTrimLeft(std::string& strValue);
    static void InplaceTrimRight(std::string& strValue);
    static void InplaceTrim(std::string& strValue);

    /** 功能：测试是否需要 trim */
    static bool TestTrimLeft(const std::string& strValue);
    static bool TestTrimRight(const std::string& strValue);
    static bool TestTrim(const std::string& strValue);
    static bool TestTrimLeft(const std::string& strValue, const std::string& strTarget);
    static bool TestTrimRight(const std::string& strValue, const std::string& strTarget);
    static bool TestTrim(const std::string& strValue, const std::string& strTarget);

    /** 功能: 删除在中文字符串两头的中文空格 */
    static std::string TrimChineseSpace(const std::string& strValue);

    /** 功能: 删除在字符串两边的所有中英文空格字符串 */
    static std::string TrimBothChEnSpace(const std::string& strValue);


    /** 功能: 把一个字符串划分成多个字符串 */
    static void Split(const std::string& strMain,
                      char chSpliter,
                      std::vector<std::string>& strList,
                      bool bReserveNullString = false);
    static void Split(const std::string& strMain,
                      const std::string& strSpliter,
                      std::vector<std::string>& strList,
                      bool bReserveNullString = false);

    /** 请使用 Split */
    SU_DEPRECATED_BY(Split) static void SplitString(
        const std::string& strMain,
        const std::string& strSpliter,
        std::vector<std::string>& strList,
        bool bReserveNullString = false)
    {
        Split(strMain, strSpliter, strList, bReserveNullString);
    }

    /** 功能:  简单的分词函数：把一个字符串按分隔符号划分成多个字符串 */
    static void SplitString(const std::string& strMain,
                            std::vector<std::string>& strList);

    /** 把字符串A中的子串B替换为C */
    static std::string ReplaceStr(const std::string& strValue,
                                  const std::string& oldStr,
                                  const std::string& newStr);

    /** 把一个集合中的字符转换为对应的另一个集合中字符
     *  例如：把 <变为[, > 变为]
     *        strNew = ReplaceCharSet(strOld, "<>", "[]");
     */
    static std::string ReplaceCharSet(const std::string& strOld,
                                      std::string strFromSet,
                                      std::string strToSet);

    /** 替换字符串中的某些子字符串 */
    static std::string ReplaceStrSet(const std::string& strRawData,
                                     std::map<std::string, std::string>& mSet);

    /** 把字符串A中的子符删除 */
    static std::string RemoveChar(std::string& strRawData, char ch);

    /** 把字符串A中的某个子字符串删除 */
    static std::string RemoveStr(std::string& strRawData, std::string strSubStr, bool bNeedSeparator = false);

    /** 删除字符串中的某些子字符串 */
    static std::string RemoveStrSet(const std::string& strRawData,
                                    std::set<std::string>& sStrSet, bool bNeedSeparator = false);

    /** 删除一个字符串中的一段子字符串 */
    static std::string DeleteSubstr(const std::string& strRawData, std::string strBegin, std::string strEnd);

    /** 从一个字符串中提取出关注的部分，例如"[*]" */
    static std::string ExtractSubstr(const std::string& strInput, std::string& strPattern);

    /** 把字符串中一些需要转义的字符做转义处理 */
    static std::string QuoteWild(const char* strRaw);
    static std::string QuoteWild(const char* strRaw, size_t size);
    static std::string QuoteWild(const std::string& strRaw);

    /** 字符串摘要：取前面若干个字符，防止出现半个汉字和英文以及数字的截断　*/
    static std::string AbstractString(std::string& strOldStr, int dwLen, std::string& strLeftStr);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 字符串内容判断

    /** 比较字符，不区分大小写 */
    static bool CmpNoCase(char c1, char c2);
    /*搜索子字符串，主字符串中可能含有‘\0’字符，也可能不以0字符结束*/
    static char * FindNoCaseEnStrWithLength(const char * pcBegin,const char * pcEnd,const char * pcSubStr);
    /** 在字符串中查找子符串，不区分大小写 */
    static std::string::size_type FindNoCase(const std::string& strMain, const std::string& strSub, size_t dwFrom = 0);

    /** 在字符串中查找子符串，不区分大小写 */
    static std::string::size_type RFindNoCase(const std::string& strMain, const std::string& strSub, size_t dwFrom);

    /** 在中文字符串中开始查找一个宽字节字 */
    static char* WStrChr(const char* pcMain, const char* pcWChar);

    /** 在中文字符串中开始反向查找一个宽字节字 */
    static char* RWStrChr(const char* pcMain, const char* pcWChar);

    /** 在中文字符串中开始反向查找一个宽字节字 */
    static std::string::size_type RWStrChr(const std::string& strMain, const std::string& strWChar);

    /** 在中文字符串中开始查找宽字节的子符串 */
    static char* WFind(const char* pcMain, const char* pcSub);

    /** 获得一个字符串的字符个数 */
    static int GetCharCount(const std::string& strWord);

    /** 获得一个字符串中某个字符的个数 */
    static int GetCharCount(const std::string& strWord, char c);

    /** 统计一个字符串中数字和字符等个数 */
    static int StatStr(const std::string& strWord, int& dwAlpha, int& dwDigit);

    /** 获得一个字符串中某个子字符串的个数 */
    static int GetSubStrCnt(const std::string& strMain, const std::string& strSubStr, bool bNoCase = true);

    /** 获得一个字符串中中文字符的个数 */
    static int GetChineseCharCount(const std::string& strWord);

    /** 返回当前位置开始的下一个字符 */
    static std::string GetNextChar(const std::string& str, size_t dwCurPos);

    /** 获得下一个有效的结束位置, 一般用于找html tag的结束标签 */
    static char* GetNextValidEndPos(char* pcCurPos, char* pcEnd, char cEndSign = '>');

    /** 检测字符串中是否包含了某些子字符串 */
    static bool IsSubStrContained(const std::string& strRawData, const std::set<std::string>& sSubStr);

    /** 是否是数字IP */
    static bool IsDigitIp(const std::string& strIp);

    /** 判断一个字符串是否是空白字符 */
    static bool IsWhiteString(char const *pStr);

    /** 判断一个字符串是否全是因为字母,  最后一个参数表示是否要把'_'算作字母 */
    static bool IsEnglishString(const char* pStr, bool bLowLineAsAlpha = false);

    /** 判断一个字符串是否全是数字组成 */
    static bool IsDigitString(const char* pStr);

    /** 判断一个字符串是否全是16进制数字组成 */
    static bool IsHexDigitString(const char* pStr);

    /** 计算GB2312汉字的序号,返回值是0到6767之间的数 */
    static int GetGB2312HanziIndex(const std::string& str);

    /** 判断一个字符串的前两个字节是否是GB2312汉字 */
    static bool IsGB2312Hanzi(const std::string& str);

    /** 判断一个字符串是否完全由GB2312的汉字组成 */
    static bool IsOnlyComposedOfGB2312Hanzi(const std::string& strWord);

    /** 判断字符是否是空格，windows下的isspace()有bug */
    static bool IsSpace(char c);

    /** 判断字符是否是数字或字母，windows下的isalnum()有bug */
    static bool IsAlnum(char c);

    /** 判断字符是否是数字，windows下的isdigit()有bug */
    static bool IsDigit(char c);

    /** 判断字符是否是字母，windows下的isalpha()有bug */
    static bool IsAlpha(char c);

    /** 判断字符是否是分界符 */
    static bool IsSeparator(char c);

    /** 判断字符串是否是正常的URL */
    static bool IsValidUrl(char* pcUrl);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 字符串大小写转换

    /** 把字符转为小写 */
    static char CharToLower(char c);

    /** 把字符转为大写 */
    static char CharToUpper(char c);

    /** 把字符串转为小写 */
    static std::string ToLower(const char *pszValue);

    /** 把字符串转为大写 */
    static std::string ToUpper(const char *pszValue);

    ///////////////////////////////////////////////////////////////////////
    // 字符串和整数、浮点数、十六进制数之间的转换
    /** 把一个int值转换成为字符串*/
    static std::string Int2Str(int dwValue);

    /** 把一个unsigned int值转换成为字符串*/
    static std::string Int2Str(unsigned int  dwValue);

    /** 把字符串转为整型数 */
    static int Str2Int(const std::string& str);

    /** 把字符串转为无符号整型数 */
    static unsigned int Str2UInt(const std::string& str);

    /**把字符串转为long long型数*/
    static long long Str2LongLong(const std::string& str);

    /**把字符串转为unsigned long long型数*/
    static unsigned long long Str2ULongLong(const std::string& str);

    /** 把一个long long值转换成为字符串*/
    static std::string LongLong2Str(long long ddwValue);

    /** 把一个long long值转换成为字符串*/
    static std::string ULongLong2Str(unsigned long long ddwValue);

    /** 把字符串转为浮点数 */
    static double Str2Double(const std::string& str);

    /** 把浮点数转为字符串 */
    static std::string Double2Str(double);

    /** 把16进制的字符串转为整型数 */
    static int HexStr2long(const char* pcHex);

    /** 以十六进制的格式打印，调试时候使用 */
    static void Str2HexStr(char* pcHexStr, unsigned char * p,int len);

    /** 把一个字符转换成16进制的形式 */
    static std::string Char2Hex(unsigned char ch);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 调试时用到的字符串打印

    /** 以十六进制的格式打印，调试时候使用 */
    static void ToHxString(char* pcBinStr, char * title, unsigned char * p,int len);

    /** 以十六进制的格式打印，调试时候使用 */
    static void BinPrint(char * title, unsigned char * p,int len);

    /** 自动以可显示的格式打印，调试时候使用 */
    static void PrintWithAutoArrange(char* pText, int dwLineWidth = 80);

    /** 把一个set输出为一个字符串 */
    static std::string OutputSet(const std::set<std::string>& strSet);

    /** 把一个Vector输出为一个字符串 */
    static std::string OutputVector(const std::vector<std::string>& strVec);

    /** 把一个map输出为一个字符串 */
    static std::string OutputMap(const std::map<std::string, std::string>& strMap);

    /** 把一个map输出为一个字符串 */
    static std::string OutputMap(const std::map<std::string, int>& strMap);

    /** 把一个multimap输出为一个字符串 */
    static std::string OutputMultiMap(const std::multimap<std::string, std::string>& strMap);

    /** 判断一个字符是否在一个字符串中 */
    static bool IsCharInStr(unsigned char ch, const std::string& strTarget);

    /** 把一个字符串中的连续空格转为单个空格 */
    static std::string SingleBlank(const std::string& strRaw);

    /** 删除html或xml格式的注释 <!-- -->, 等价于DeleteSubstr(str, "<!--", "-->"), 但是性能要高 */
    static std::string StripComments(const std::string& strRawFile);

    /** 计算一个字符串的hash值 */
    static unsigned int GetStrHashValue(const char* pcStr);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 字符串模糊匹配实现（就是简单的正则表达式以及范围匹配功能）

    /** 检查一个字符串是否能匹配到一个通配符号
     *  MatchWildcard("he*o","hello"): true
     *  MatchWildcard("he*p","hello"): false
     *  MatchWildcard("he??o","hello"): true
     *  MatchWildcard("he?o","hello"): false
     *  MatchWildcard("[a-z][0-9]","h7"): true
     *  MatchWildcard("172.16.*","172.16.68.29"): true
     */
    static bool MatchWildcard(const std::string& strWild, const std::string& strMatch);

    /** 功能: 计算两个字符串的相似度
     *    输入参数 const std::string& strA      字符串1
     *    输入参数 const  std::string& strB     字符串2
     */
    static int CalcSimilar(const std::string& strA, const std::string& strB);
    static int CalcSimilarEx(const char* pcStrA, int dwLenA, const char* pcStrB, int dwLenB);

    // 生成CRC-16-CCITT算法签名
    static unsigned short crc16_ccitt(const char *ucbuf, int iLen);

    // 把16进制字符串还原成原始的字符串
    static std::string HexDigit2String(const char* pStr);

	//获取编译时间	
	static std::string GetBuildTime();

private:

    // 计算一个字符是否在一个集合中，例如'8'属于"0-9"
    static bool _MatchSet(const std::string& strPattern, char strMatch);

    // 匹配剩下的通配符，递归调用
    static bool _MatchRemainder(const std::string& strWild,
                                std::string::const_iterator itWild,
                                const std::string& strMatch,
                                std::string::const_iterator itMatch);

};

#undef SU_DEPRECATED_BY

#endif

