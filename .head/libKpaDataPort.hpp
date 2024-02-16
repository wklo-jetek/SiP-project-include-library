#ifndef __LIBKPADATAPORT_H__
#define __LIBKPADATAPORT_H__

#include <vector>
#include <string>
#include ".\libDatalog.hpp"

// *** KPA DATA_PORT class *** //

namespace KPA
{
    struct DATA_PORT_O;
    template <typename U>
    struct DATA_PORT_U;
    template <typename... DPG>
    struct DATA_PORT_GROUP;
    using DATA_PORT = DATA_PORT_U<double>;
    using DATA_PORT_INT = DATA_PORT_U<int>;

    void judge(KPA::DATA_PORT &dp);
    void judge(KPA::DATA_PORT_INT &dp);
    template <typename... DPG>
    void judge(KPA::DATA_PORT_GROUP<DPG...> &dp);
}

// *** Class DATA_PORT_UNIT *** //

struct KPA::DATA_PORT_O
{
public:
    enum TYPE
    {
        NONE = 0,
        DBL = 1,
        INT = 2,
        GRP = 3,
    };
    TYPE type = NONE;
    virtual std::string r_pass() = 0;
    virtual std::string r_fail() = 0;
};

template <typename U>
struct KPA::DATA_PORT_U : public KPA::DATA_PORT_O
{
public:
    int t_num;
    std::string t_name;
    U lo;
    U hi;
    std::string format;
    uint16_t bin;
    std::vector<U> result;

private:
public:
    DATA_PORT_U(int _num, const char *_name, U _lo, U _hi, const char *_format, uint16_t _bin)
        : t_num(_num), t_name(_name), lo(_lo), hi(_hi), format(_format), bin(_bin)
    {
        if constexpr (std::is_same<U, double>::value)
            type = TYPE::DBL;
        else if constexpr (std::is_same<U, int>::value)
            type = TYPE::INT;
    }
    DATA_PORT_U() : DATA_PORT_U(0, "", (U)0, (U)0, "", 1) {}
    ~DATA_PORT_U() {}
    DATA_PORT_U &data(const std::vector<U> &_data);
    std::string r_pass();
    std::string r_fail();
};
template <typename U>
KPA::DATA_PORT_U<U> &KPA::DATA_PORT_U<U>::data(const std::vector<U> &_data)
{
    result.resize(KPA::kpa_qty_sites);
    uint32_t len = (result.size() < _data.size()) ? result.size() : _data.size();
    for (uint32_t s = 0; s < len; s++)
        if (KPA::kpa_actsite.site(s))
            result[s] = _data[s];
    return *this;
}
template <typename U>
std::string KPA::DATA_PORT_U<U>::r_pass()
{
    std::string out(KPA::kpa_qty_sites, '0');
    for (int s = 0; s < KPA::kpa_qty_sites; s++)
        if (KPA::kpa_actsite.site(s))
            out[s] = (result[s] <= hi && result[s] >= lo) ? '1' : '0';
    return out;
}
template <typename U>
std::string KPA::DATA_PORT_U<U>::r_fail()
{
    std::string out(KPA::kpa_qty_sites, '0');
    for (int s = 0; s < KPA::kpa_qty_sites; s++)
        if (KPA::kpa_actsite.site(s))
            out[s] = (result[s] > hi || result[s] < lo) ? '1' : '0';
    return out;
}

template <typename... DPG>
struct KPA::DATA_PORT_GROUP : public KPA::DATA_PORT_O
{
    std::vector<DATA_PORT_O *> dps;
    DATA_PORT_GROUP(DPG &...args)
    {
        dps = {&args...};
        type = TYPE::GRP;
    }
    std::string r_pass();
    std::string r_fail();
};
template <typename... DPG>
std::string KPA::DATA_PORT_GROUP<DPG...>::r_pass()
{
    KPA::ACTS_BUF out('1');
    out &= KPA::kpa_actsite.getbuf();
    for (auto &ele : dps)
        out &= ele->r_pass();
    return out.str();
}
template <typename... DPG>
std::string KPA::DATA_PORT_GROUP<DPG...>::r_fail()
{
    KPA::ACTS_BUF out('0');
    for (auto &ele : dps)
        out |= ele->r_fail();
    out &= KPA::kpa_actsite.getbuf();
    return out.str();
}

// * judge with DATA_PORT

void KPA::judge(KPA::DATA_PORT &dp)
{
    judge(dp.t_num, dp.t_name.c_str(), dp.result, dp.lo, dp.hi, dp.format.c_str(), dp.bin);
}
void KPA::judge(KPA::DATA_PORT_INT &dp)
{
    judge(dp.t_num, dp.t_name.c_str(), dp.result, dp.lo, dp.hi, dp.format.c_str(), dp.bin);
}
template <typename... DPG>
void KPA::judge(KPA::DATA_PORT_GROUP<DPG...> &dp)
{
    auto func_type = [&](auto ptr)
    {
        STDF_LOG::wrt_ptr(ptr->t_num, (char *)ptr->t_name.c_str(), (char *)ptr->format.c_str());
        for (int i = 0; i < KPA::kpa_qty_sites; i++)
            if (KPA::kpa_actsite.site(i))
                KPA::logs[i].judge(ptr->result[i], ptr->lo, ptr->hi, ptr->bin);
    };
    for (auto &ptr : dp.dps)
    {
        switch (ptr->type)
        {
        case DATA_PORT_O::TYPE::DBL:
            func_type((DATA_PORT_U<double> *)ptr);
            break;
        case DATA_PORT_O::TYPE::INT:
            func_type((DATA_PORT_U<int> *)ptr);
            break;
        case DATA_PORT_O::TYPE::GRP:
            judge(dp);
            break;
        default:
            break;
        }
    }
    STDF_LOG::wrt_file();
    for (int i = 0; i < KPA::kpa_qty_sites; i++)
        if (KPA::kpa_actsite.site(i))
            KPA::logs[i].eos_process();
    if (!KPA::acts_sys)
        STDF_LOG::event_esc();
}

#endif // * __LIBKPADATAPORT_H__
