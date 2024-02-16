#ifndef LIBLLDRIVER_CPP
#define LIBLLDRIVER_CPP

#include "SiPhDriver.h"
#include "libCustomizeItem.cpp"
#include <string>
#include <windows.h>

class LLDriverBase
{
protected:
    uint32_t session = 0;

    int __libClose();

public:
    int timeout;
    ~LLDriverBase();
    void write(const std::string &str);
    void write(const char *str);
    std::string read(int len_buf = 1024);
};
class llgpib : public LLDriverBase
{
public:
    llgpib(uint8_t dev_addr);
    ~llgpib() { __libClose(); }
};
class llvisa : public LLDriverBase
{
public:
    llvisa(const char *resource);
    ~llvisa() { __libClose(); }
};
class lltest : public LLDriverBase
{
public:
    lltest(const char *log_path);
    ~lltest() { __libClose(); }
    std::string read(int len_buf = 1024)
    {
        return std::string(len_buf, 0);
    }
};

LLDriverBase::~LLDriverBase()
{
    __libClose();
}
int LLDriverBase::__libClose()
{
    if (session)
        LibSiPhDriver_Term(session);
    session = 0;
    return 0;
}
void LLDriverBase::write(const std::string &str)
{
    // LibSiPhDriver_WriteStr(session, (char *)((str[str.size() - 1] != '\n') ? (str + "\n") : str).c_str(), timeout);
    LibSiPhDriver_WriteStr(session, (char *)str.c_str(), timeout);
}
void LLDriverBase::write(const char *str)
{
    // if (str[strlen(str) - 1] != '\n') {
    //     auto buf = string(str) + "\n";
    //     LibSiPhDriver_WriteStr(session, (char *)buf.c_str(), timeout);
    // } else {
    //     LibSiPhDriver_WriteStr(session, (char *)str, timeout);
    // }
    LibSiPhDriver_WriteStr(session, (char *)str, timeout);
}
std::string LLDriverBase::read(int len_buf)
{
    auto remove_return = [](std::string &str) {
        auto len = strlen(str.c_str());
        auto &c1 = str[len - 2], &c2 = str[len - 1];
        if (c1 == '\r' && c2 == '\n')
            c1 = c2 = 0;
    };
    std::string str_buf(len_buf, 0);
    if (len_buf > 100 * 1024)
        Sleep(5);
    LibSiPhDriver_ReadStr(session, (char *)str_buf.c_str(), str_buf.size() - 1, timeout);
    remove_return(str_buf);
    return str_buf;
}
llgpib::llgpib(uint8_t dev_addr)
{
    LibSiPhDriver_Init(dev_addr, &session);
    timeout = 3000;
}
llvisa::llvisa(const char *resource)
{
    int err = LibSiPhDriver_Init_visa((char *)resource, &session);
    if (err == 0) {
        kpa::ins::msg.prefix("Instrument Name : ") << resource;
    } else {
        kpa::ins::msg << str_format("Initialize Fail : %s", resource);
    }
    timeout = 3000;
}
lltest::lltest(const char *log_path)
{
    LibSiPhDriver_Init_test((char *)log_path, &session);
}

#endif //* LIBLLDRIVER_CPP