#ifndef __LIBDATALOG_FLAG__
#define __LIBDATALOG_FLAG__

#include <sstream>
#include <string>
#include <vector>
#include ".\libDataBridgeActsite.hpp"
#include ".\libStdfLog.h"

namespace KPA
{
    struct JUDGE_BIN;
    class STDF_LOG;
    LVO_BOOL kpa_failstop{"Fail stop", true};
    LVO_STRING kpa_bin_method{"Bin Processing Method"};
    LVP_STRING kpa_bins{"Bins"};
}
struct KPA::JUDGE_BIN
{
public:
    static uint16_t good_bin_lim;
    using TYPE = void (JUDGE_BIN::*)(uint16_t);
    uint16_t bin = 1;
    static TYPE bin_cal_func;
    static bool auto_flash_acts_sys;

    JUDGE_BIN() { reset(); };
    void reset() { bin = 1; };
    void max(uint16_t _bin)
    {
        if (bin < _bin)
            bin = _bin;
    }
    void min(uint16_t _bin)
    {
        if (bin > _bin)
            bin = _bin;
    }
    void ffb(uint16_t _bin)
    {
        if (bin <= good_bin_lim)
            bin = _bin;
    }
    void lfb(uint16_t _bin)
    {
        if (bin > good_bin_lim)
            if (_bin <= good_bin_lim)
                return;
        bin = _bin;
    }
    bool is_pass_bin()
    {
        return (bin <= good_bin_lim);
    }
};
uint16_t KPA::JUDGE_BIN::good_bin_lim = 1;
KPA::JUDGE_BIN::TYPE KPA::JUDGE_BIN::bin_cal_func = KPA::JUDGE_BIN::max;
bool KPA::JUDGE_BIN::auto_flash_acts_sys = true;

class KPA::STDF_LOG : public KPA::JUDGE_BIN
{
public:
    const uint8_t site;

public:
    using FUNC_INIT = void (*)(uint32_t bridge_num);
    using FUNC_SOT = void (*)(void);
    using FUNC_EOT = void (*)(void);
    using FUNC_TERM = void (*)(void);
    using FUNC_EOS = void (*)(int s_idx);
    using FUNC_ESC = void (*)(void);

    using FUNC_PTR = void (*)(uint32_t t_num, char *t_name, char *format);
    using FUNC_DBL = void (*)(uint8_t site, double result, double lo, double hi, uint8_t pass, uint8_t nospec);
    using FUNC_INT = void (*)(uint8_t site, int result, int lo, int hi, uint8_t pass, uint8_t nospec);
    using FUNC_STR = void (*)(uint8_t site, char *result, char *lo, char *hi, uint8_t pass, uint8_t nospec);
    using FUNC_WRITE = void (*)(void);

    static FUNC_INIT log_init;
    static FUNC_SOT log_sot;
    static FUNC_EOT log_eot;
    static FUNC_TERM log_term;
    static FUNC_EOS event_eos;
    static FUNC_ESC event_esc;

    static FUNC_PTR wrt_ptr;
    static FUNC_DBL wrt_dbl;
    static FUNC_INT wrt_int;
    static FUNC_STR wrt_str;
    static FUNC_WRITE wrt_file;

    STDF_LOG(int _site) : site(_site) {}
    void judge(double value, double lo, double hi, uint16_t bin)
    {
        bool test_fail = (value > hi) ? true : ((value < lo) ? true : false);
        (*wrt_dbl)(site, value, lo, hi, test_fail, 0);
        if (test_fail)
            (this->*bin_cal_func)(bin);
    }
    void judge(double value, uint16_t bin)
    {
        bool test_fail = false;
        (*wrt_dbl)(site, value, 0., 0., test_fail, 1);
        if (test_fail)
            (this->*bin_cal_func)(bin);
    }
    void judge(int value, int lo, int hi, uint16_t bin)
    {
        bool test_fail = (value > hi) ? true : ((value < lo) ? true : false);
        (*wrt_int)(site, value, lo, hi, test_fail, 0);
        if (test_fail)
            (this->*bin_cal_func)(bin);
    }
    void judge(int value, uint16_t bin)
    {
        bool test_fail = false;
        (*wrt_int)(site, value, 0, 0, test_fail, 1);
        if (test_fail)
            (this->*bin_cal_func)(bin);
    }
    void judge(const std::string &value, const char *lo, const char *hi, uint16_t bin)
    {
        bool test_fail = (value > hi) ? true : ((value < lo) ? true : false);
        (*wrt_str)(site, (char *)value.c_str(), (char *)lo, (char *)hi, test_fail, 0);
        if (test_fail)
            (this->*bin_cal_func)(bin);
    }
    void judge(const std::string &value, uint16_t bin)
    {
        bool test_fail = false;
        (*wrt_str)(site, (char *)value.c_str(), (char *)"", (char *)"", test_fail, 1);
        if (test_fail)
            (this->*bin_cal_func)(bin);
    }
    void eos_process()
    {
        if (this->is_pass_bin())
            return;
        if (!KPA::kpa_failstop)
            return;
        event_eos(site - 1);
        KPA::acts_sys[site - 1] = '0';
        return;
    }
};
KPA::STDF_LOG::FUNC_INIT KPA::STDF_LOG::log_init = LibDatalogIns_Bridge;
KPA::STDF_LOG::FUNC_SOT KPA::STDF_LOG::log_sot = LibDatalogIns_lv_sot;
KPA::STDF_LOG::FUNC_EOT KPA::STDF_LOG::log_eot = LibDatalogIns_lv_eot;
KPA::STDF_LOG::FUNC_TERM KPA::STDF_LOG::log_term = LibDatalogIns_LotEnd;
KPA::STDF_LOG::FUNC_EOS KPA::STDF_LOG::event_eos = [](int s_idx) {};
KPA::STDF_LOG::FUNC_ESC KPA::STDF_LOG::event_esc = []() {};

KPA::STDF_LOG::FUNC_PTR KPA::STDF_LOG::wrt_ptr = LibDatalogIns_create_ptr;
KPA::STDF_LOG::FUNC_DBL KPA::STDF_LOG::wrt_dbl = LibDatalogIns_log_dbl;
KPA::STDF_LOG::FUNC_INT KPA::STDF_LOG::wrt_int = LibDatalogIns_log_int;
KPA::STDF_LOG::FUNC_STR KPA::STDF_LOG::wrt_str = LibDatalogIns_log_str;
KPA::STDF_LOG::FUNC_WRITE KPA::STDF_LOG::wrt_file = LibDatalogIns_write_log;

//* instructions
namespace KPA
{
    std::vector<KPA::STDF_LOG> logs;

    void log_open(uint32_t bridge_num, JUDGE_BIN::TYPE bin_judge_func);
    void log_close();
    void log_sot();
    void log_eot();

    void judge(int t_num, const char *t_name, double *result, double lo, double hi, const char *format, uint16_t bin);
    void judge(int t_num, const char *t_name, const std::vector<double> &result, double lo, double hi, const char *format, uint16_t bin);
    void judge(int t_num, const char *t_name, int *result, int lo, int hi, const char *format, uint16_t bin);
    void judge(int t_num, const char *t_name, const std::vector<int> &result, int lo, int hi, const char *format, uint16_t bin);
    void judge(int t_num, const char *t_name, const std::vector<std::string> &result, const char *lo, const char *hi, const char *format, uint16_t bin);
    void log(int t_num, const char *t_name, double *result, const char *format);
    void log(int t_num, const char *t_name, const std::vector<double> &result, const char *format);
    void log(int t_num, const char *t_name, int *result, const char *format);
    void log(int t_num, const char *t_name, const std::vector<int> &result, const char *format);
    void log(int t_num, const char *t_name, const std::vector<std::string> &result, const char *format);

    void judge(int t_num, const char *t_name, double *result, double lo, double hi, const char *format, const char *unit, uint16_t bin);
    void judge(int t_num, const char *t_name, const std::vector<double> &result, double lo, double hi, const char *format, const char *unit, uint16_t bin);
    void judge(int t_num, const char *t_name, int *result, int lo, int hi, const char *format, const char *unit, uint16_t bin);
    void judge(int t_num, const char *t_name, const std::vector<int> &result, int lo, int hi, const char *format, const char *unit, uint16_t bin);
    void judge(int t_num, const char *t_name, const std::vector<std::string> &result, const char *lo, const char *hi, const char *format, const char *unit, uint16_t bin);
    void log(int t_num, const char *t_name, double *result, const char *format, const char *unit);
    void log(int t_num, const char *t_name, const std::vector<double> &result, const char *format, const char *unit);
    void log(int t_num, const char *t_name, int *result, const char *format, const char *unit);
    void log(int t_num, const char *t_name, const std::vector<int> &result, const char *format, const char *unit);
    void log(int t_num, const char *t_name, const std::vector<std::string> &result, const char *format, const char *unit);

}
void KPA::log_open(uint32_t bridge_num, KPA::JUDGE_BIN::TYPE bin_judge_func = NULL)
{
    KPA::JUDGE_BIN::good_bin_lim = kpa_max_pass_bin;
    KPA::JUDGE_BIN::bin_cal_func = bin_judge_func;
    if (bin_judge_func == NULL)
    {
        std::string sel = kpa_bin_method.c_str();
        if (sel == "MAX")
            KPA::JUDGE_BIN::bin_cal_func = KPA::JUDGE_BIN::max;
        else if (sel == "MIN")
            KPA::JUDGE_BIN::bin_cal_func = KPA::JUDGE_BIN::min;
        else if (sel == "FFB")
            KPA::JUDGE_BIN::bin_cal_func = KPA::JUDGE_BIN::ffb;
        else if (sel == "LFB")
            KPA::JUDGE_BIN::bin_cal_func = KPA::JUDGE_BIN::lfb;
        else
            KPA::JUDGE_BIN::bin_cal_func = KPA::JUDGE_BIN::max;
    }

    STDF_LOG::log_init(bridge_num);
    if (KPA::logs.size() != uint32_t(kpa_qty_sites))
    {
        KPA::logs.clear();
        for (int i = 0; i < kpa_qty_sites; i++)
            KPA::logs.push_back(KPA::STDF_LOG(i + 1));
    }
}
void KPA::log_close()
{
    STDF_LOG::log_term();
}
void KPA::log_sot()
{
    STDF_LOG::log_sot();
    for (auto &log : logs)
        log.reset();
}
void KPA::log_eot()
{
    std::stringstream buf_bin;
    buf_bin << std::dec;
    for (int i = 0; i < kpa_qty_sites; i++)
        buf_bin << KPA::logs[i].bin << ",";
    kpa_bins << buf_bin.str();
    KPA::STDF_LOG::log_eot();
}

#define judge_nominal(len)                                    \
    STDF_LOG::wrt_ptr(t_num, (char *)t_name, (char *)format); \
    for (int i = 0; i < len; i++)                             \
        if (KPA::kpa_actsite.site(i))                         \
        {                                                     \
            KPA::logs[i].judge(result[i], lo, hi, bin);       \
            KPA::logs[i].eos_process();                       \
        }                                                     \
    STDF_LOG::wrt_file();                                     \
    if (!acts_sys)                                            \
        STDF_LOG::event_esc();

void KPA::judge(int t_num, const char *t_name, double *result,
                double lo, double hi, const char *format, uint16_t bin)
{
    judge_nominal(kpa_qty_sites);
}
void KPA::judge(int t_num, const char *t_name, const std::vector<double> &result,
                double lo, double hi, const char *format, uint16_t bin)
{
    int len = ((int)result.size() < kpa_qty_sites) ? result.size() : kpa_qty_sites;
    judge_nominal(len);
}
void KPA::judge(int t_num, const char *t_name, int *result,
                int lo, int hi, const char *format, uint16_t bin)
{
    judge_nominal(kpa_qty_sites);
}
void KPA::judge(int t_num, const char *t_name, const std::vector<int> &result,
                int lo, int hi, const char *format, uint16_t bin)
{
    int len = ((int)result.size() < kpa_qty_sites) ? result.size() : kpa_qty_sites;
    judge_nominal(len);
}
void KPA::judge(int t_num, const char *t_name, const std::vector<std::string> &result,
                const char *lo, const char *hi, const char *format, uint16_t bin)
{
    int len = ((int)result.size() < kpa_qty_sites) ? result.size() : kpa_qty_sites;
    judge_nominal(len);
}

#undef judge_nominal

#define judge_nospec(len)                                     \
    STDF_LOG::wrt_ptr(t_num, (char *)t_name, (char *)format); \
    for (int i = 0; i < len; i++)                             \
        if (KPA::kpa_actsite.site(i))                         \
        {                                                     \
            KPA::logs[i].judge(result[i], bin);               \
            KPA::logs[i].eos_process();                       \
        }                                                     \
    STDF_LOG::wrt_file();                                     \
    if (!acts_sys)                                            \
        STDF_LOG::event_esc();

#define log_nominal(len)                                      \
    STDF_LOG::wrt_ptr(t_num, (char *)t_name, (char *)format); \
    for (int i = 0; i < len; i++)                             \
        if (KPA::kpa_actsite.site(i))                         \
        {                                                     \
            KPA::logs[i].judge(result[i], 1);                 \
        }                                                     \
    STDF_LOG::wrt_file();

void KPA::log(int t_num, const char *t_name, double *result, const char *format)
{
    log_nominal(kpa_qty_sites);
}
void KPA::log(int t_num, const char *t_name, const std::vector<double> &result, const char *format)
{
    int len = ((int)result.size() < kpa_qty_sites) ? result.size() : kpa_qty_sites;
    log_nominal(len);
}
void KPA::log(int t_num, const char *t_name, int *result, const char *format)
{
    log_nominal(kpa_qty_sites);
}
void KPA::log(int t_num, const char *t_name, const std::vector<int> &result, const char *format)
{
    int len = ((int)result.size() < kpa_qty_sites) ? result.size() : kpa_qty_sites;
    log_nominal(len);
}
void KPA::log(int t_num, const char *t_name, const std::vector<std::string> &result, const char *format)
{
    int len = ((int)result.size() < kpa_qty_sites) ? result.size() : kpa_qty_sites;
    log_nominal(len);
}

#undef judge_nospec
#undef log_nominal

#define judge_unit()                            \
    char mix[256];                              \
    snprintf(mix, 255, "%s(%s)", format, unit); \
    judge(t_num, t_name, (result), lo, hi, (const char *)mix, bin);

void KPA::judge(int t_num, const char *t_name, double *result, double lo, double hi, const char *format, const char *unit, uint16_t bin)
{
    judge_unit()
}
void KPA::judge(int t_num, const char *t_name, const std::vector<double> &result, double lo, double hi, const char *format, const char *unit, uint16_t bin)
{
    judge_unit()
}
void KPA::judge(int t_num, const char *t_name, int *result, int lo, int hi, const char *format, const char *unit, uint16_t bin)
{
    judge_unit()
}
void KPA::judge(int t_num, const char *t_name, const std::vector<int> &result, int lo, int hi, const char *format, const char *unit, uint16_t bin)
{
    judge_unit()
}
void KPA::judge(int t_num, const char *t_name, const std::vector<std::string> &result, const char *lo, const char *hi, const char *format, const char *unit, uint16_t bin)
{
    judge_unit()
}

#undef judge_unit

#define judge_unit_nospec()                     \
    char mix[256];                              \
    snprintf(mix, 255, "%s(%s)", format, unit); \
    judge(t_num, t_name, (result), (const char *)mix, bin);

#define log_unit()                              \
    char mix[256];                              \
    snprintf(mix, 255, "%s(%s)", format, unit); \
    log(t_num, t_name, (result), (const char *)mix);

void KPA::log(int t_num, const char *t_name, double *result, const char *format, const char *unit)
{
    log_unit()
}
void KPA::log(int t_num, const char *t_name, const std::vector<double> &result, const char *format, const char *unit)
{
    log_unit()
}
void KPA::log(int t_num, const char *t_name, int *result, const char *format, const char *unit)
{
    log_unit()
}
void KPA::log(int t_num, const char *t_name, const std::vector<int> &result, const char *format, const char *unit)
{
    log_unit()
}
void KPA::log(int t_num, const char *t_name, const std::vector<std::string> &result, const char *format, const char *unit)
{
    log_unit()
}

#undef judge_unit_nospec
#undef log_unit

#endif //* __LIBDATALOG_FLAG__
