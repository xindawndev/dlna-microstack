
#ifndef _CXL_YETI_MD5_H_
#define _CXL_YETI_MD5_H_

#include <string>
#include <stdio.h>

struct MD5Context {
    unsigned int buf[4];
    unsigned int bytes[2];
    unsigned in[16];
};

class Md5
{
public:
    Md5(void);
    ~Md5(void);

    void append(const void * inBuf, size_t inLen);
    void append(const std::string & str);
    void getDigest(unsigned char digest[16]);
    void getDigest(std::string & digest);

    static std::string GetMD5(const std::string & text);
private:
    MD5Context m_ctx;
};

#endif // _CXL_YETI_MD5_H_
