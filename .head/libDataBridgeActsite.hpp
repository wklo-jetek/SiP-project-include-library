#ifndef __LIBDATABRIDGEACTSITE_FLAG__
#define __LIBDATABRIDGEACTSITE_FLAG__

#include <iostream>
#include <vector>
#include <string>
#include "libDataBridge.hpp"

//*   KPA Resource Control class   *//
namespace KPA
{
    struct ACTS_BUF;
}
struct KPA::ACTS_BUF
{
private:
    LVO_INT32 &len = kpa_qty_sites;
    struct BUF
    {
        uint16_t ref;
        std::string act;
    };

private:
    BUF *p;

public:
    ACTS_BUF()
    {
        p = new BUF;
        p->ref = 0;
        p->act.resize(len);
    }
    ACTS_BUF(const char *str) : ACTS_BUF()
    {
        p->act.assign(str);
    }
    ACTS_BUF(char b) : ACTS_BUF()
    {
        this->fill(b);
    }
    ~ACTS_BUF()
    {
        if (p != NULL)
        {
            if (p->ref == 0)
                delete p;
            else
                (p->ref)--;
        }
    }
    ACTS_BUF(const ACTS_BUF &src) : ACTS_BUF()
    {
        p->act = src.p->act;
    }
    ACTS_BUF(ACTS_BUF &&src)
    {
        p = src.p;
        src.p = NULL;
    }
    void operator=(const ACTS_BUF &src)
    {
        this->~ACTS_BUF();
        p = src.p;
        p->ref++;
    }
    void operator=(ACTS_BUF &&src)
    {
        this->~ACTS_BUF();
        p = src.p;
        src.p = NULL;
    }

    //!   ACTS_BUF Method   !//
    char &operator[](int idx);
    ACTS_BUF &fill(char b);
    ACTS_BUF &assign(const char *str);
    ACTS_BUF &operator&=(const ACTS_BUF &src);
    ACTS_BUF &operator&=(const std::string &src);
    ACTS_BUF &operator|=(const ACTS_BUF &src);
    ACTS_BUF &operator|=(const std::string &src);
    ACTS_BUF operator^(const ACTS_BUF &src);

    //!   Type transfer   !//
    operator bool();
    const std::string &str();
    uint64_t toNumber();
};
char &KPA::ACTS_BUF::operator[](int idx)
{
    if (idx < 0)
        return p->act[0];
    if (idx >= int(p->act.size()))
        return p->act[p->act.size() - 1];
    return p->act[idx];
}
KPA::ACTS_BUF &KPA::ACTS_BUF::fill(char b)
{
    for (int i = 0; i < len; i++)
        p->act[i] = b;
    return *this;
}
KPA::ACTS_BUF &KPA::ACTS_BUF::assign(const char *str)
{
    p->act.assign(str, len);
    p->act.resize(len);
    return *this;
}
KPA::ACTS_BUF &KPA::ACTS_BUF::operator&=(const ACTS_BUF &src)
{
    for (int i = 0; i < len; i++)
        p->act[i] = (p->act[i] == '1' && src.p->act[i] == '1') ? '1' : '0';
    return *this;
}
KPA::ACTS_BUF &KPA::ACTS_BUF::operator&=(const std::string &src)
{
    int _len = ((int)src.size() < len) ? (int)src.size() : len;
    for (int i = 0; i < _len; i++)
        p->act[i] = (p->act[i] == '1' && src[i] == '1') ? '1' : '0';
    return *this;
}
KPA::ACTS_BUF &KPA::ACTS_BUF::operator|=(const ACTS_BUF &src)
{
    for (int i = 0; i < len; i++)
        p->act[i] = (p->act[i] == '1' || src.p->act[i] == '1') ? '1' : '0';
    return *this;
}
KPA::ACTS_BUF &KPA::ACTS_BUF::operator|=(const std::string &src)
{
    int _len = ((int)src.size() < len) ? (int)src.size() : len;
    for (int i = 0; i < _len; i++)
        p->act[i] = (p->act[i] == '1' || src[i] == '1') ? '1' : '0';
    return *this;
}
KPA::ACTS_BUF KPA::ACTS_BUF::operator^(const ACTS_BUF &src)
{
    ACTS_BUF out(*this);
    for (int i = 0; i < len; i++)
        out.p->act[i] = (p->act[i] != src.p->act[i]) ? '1' : '0';
    return out;
}
KPA::ACTS_BUF::operator bool()
{
    for (int i = 0; i < len; i++)
        if (p->act[i] == '1')
            return true;
    return false;
}
const std::string &KPA::ACTS_BUF::str()
{
    return p->act;
}
uint64_t KPA::ACTS_BUF::toNumber()
{
    uint64_t out = 0;
    for (int i = 0; i < len; i++)
        if (p->act[i] == '1')
            out |= (1ull << i);
    return out;
}

//*   KPA LVO_ACTSITE DEFINE   *//namespace KPA

namespace KPA
{
    LVO_STRING acts_tstart{"Active site"};
    ACTS_BUF acts_sys;
    class INT_DELEGATE;
    class KPA_ACTSITE;
}
class KPA::INT_DELEGATE
{
private:
    void *sync_int_ptr = NULL;
    int sync_int_len = 0;
    const char **str_hdl = NULL;

protected:
    void __sync_int(char *);
    void __sync_str(char *);
    template <typename T>
    void __sync(T *buf_ptr);

public:
    void sync(uint8_t *buf_ptr);
    void sync(uint16_t *buf_ptr);
    void sync(uint32_t *buf_ptr);
    void sync(uint64_t *buf_ptr);
    void sync(const char **str_hdl);
};
#define SYNC_TYPE(T) \
    void KPA::INT_DELEGATE::sync(T *buf_ptr) { __sync(buf_ptr); }
SYNC_TYPE(uint8_t)
SYNC_TYPE(uint16_t)
SYNC_TYPE(uint32_t)
SYNC_TYPE(uint64_t)
#undef SYNC_TYPE
void KPA::INT_DELEGATE::sync(const char **_str_hdl)
{
    str_hdl = _str_hdl;
}
template <typename T>
void KPA::INT_DELEGATE::__sync(T *buf_ptr)
{
    sync_int_len = sizeof(T);
    sync_int_ptr = buf_ptr;
}
void KPA::INT_DELEGATE::__sync_int(char *acts)
{
    if (sync_int_ptr == NULL)
        return;

    uint64_t out = 0;
    for (int s = 0; s < KPA::kpa_qty_sites; s++)
        if (acts[s] == '1')
            out |= (1ull << s);
    switch (sync_int_len)
    {
    case 1:
        *(uint8_t *)sync_int_ptr = *(uint8_t *)&out;
        break;
    case 2:
        *(uint16_t *)sync_int_ptr = *(uint16_t *)&out;
        break;
    case 4:
        *(uint32_t *)sync_int_ptr = *(uint32_t *)&out;
        break;
    case 8:
        *(uint64_t *)sync_int_ptr = out;
        break;
    default:
        break;
    }
}
void KPA::INT_DELEGATE::__sync_str(char *acts)
{
    if (str_hdl == NULL)
        return;
    *str_hdl = acts;
}

class KPA::KPA_ACTSITE
{
private:
    ACTS_BUF __acts_buf;
    std::vector<void (*)(char *)> _act_func_list;

public:
    KPA_ACTSITE() {}
    ~KPA_ACTSITE() {}

    // * DATA/BUFFER CHANGE
    KPA_ACTSITE &update();
    ACTS_BUF &getbuf();
    void setbuf(ACTS_BUF &acts_buf);

    // * OBJECT IO
    char *c_str();
    operator char *();
    bool site(uint32_t s_idx);
    KPA_ACTSITE &operator<<(void string_func(char *)); //* Add delegate function
    uint64_t toNumber();
};
KPA::ACTS_BUF &KPA::KPA_ACTSITE::getbuf()
{
    return __acts_buf;
}
void KPA::KPA_ACTSITE::setbuf(KPA::ACTS_BUF &acts_buf)
{
    __acts_buf = acts_buf;
    update();
}
KPA::KPA_ACTSITE &KPA::KPA_ACTSITE::update()
{
    for (auto &func : _act_func_list)
        (*func)((char *)__acts_buf.str().c_str());
    return *this;
}

char *KPA::KPA_ACTSITE::c_str()
{
    return (char *)__acts_buf.str().c_str();
}
KPA::KPA_ACTSITE::operator char *()
{
    return c_str();
}
bool KPA::KPA_ACTSITE::site(uint32_t s_idx)
{
    return (s_idx < (uint32_t)KPA::kpa_qty_sites) ? (__acts_buf[s_idx] == '1') : false;
}
KPA::KPA_ACTSITE &KPA::KPA_ACTSITE::operator<<(void string_func(char *))
{
    for (int i = 0, len = _act_func_list.size(); i < len; i++)
        if (_act_func_list[i] == string_func)
            return *this;
    _act_func_list.push_back(string_func);
    string_func(this->c_str());
    return *this;
}
uint64_t KPA::KPA_ACTSITE::toNumber()
{
    return __acts_buf.toNumber();
}

namespace KPA
{
    KPA_ACTSITE kpa_actsite;
    void acts_sot();
};
void KPA::acts_sot()
{
    acts_sys.assign(acts_tstart);
    kpa_actsite.setbuf(acts_sys);
}

#endif // * __LIBDATABRIDGEACTSITE_FLAG__
