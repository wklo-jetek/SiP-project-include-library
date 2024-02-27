#include "libCustomizeItem.cpp"
#include "libLLDriver.cpp"
#include <fstream>
#include <string>
#include <vector>

template <typename F>
void recipe_repeat(F recipe, int times = 3, DWORD sleep_ms = 1000)
{
    for (int i = 0; i < times; i++)
        if (recipe())
            break;
        else
            Sleep(sleep_ms);
}

class CTPX
{
private:
    int __switch_ref_ch = 0, __switch_ref_sense = 0;

public:
    LLDriverBase *hw = NULL;
    double center_wavelength_nm = 1310.0;
    // llgpib hw{10};
    // lltest hw{"C:\\tmp\\ctps_log.txt"};

    class POWER
    {
    private:
        CTPX *r;
        LLDriverBase *hw;
        double var;

    public:
        POWER(CTPX *p_root, LLDriverBase *_hw) : r(p_root), hw(_hw) {}
        void operator=(double dbm);
        operator double();
    };
    POWER power{this, hw};
    class WAVELENGTH
    {
    private:
        CTPX *r;
        LLDriverBase *hw;
        double var;

    public:
        WAVELENGTH(CTPX *p_root, LLDriverBase *_hw) : r(p_root), hw(_hw) {}
        void operator=(double nm);
        operator double();
    };
    WAVELENGTH wavelength{this, hw};
    class SCAN
    {
    private:
        CTPX *r;
        const std::vector<int> ls_speed_select = {100, 67, 50, 40};
        int subSpeed(int sp);

    public:
        SCAN(CTPX *p_root) : r(p_root) {}
        SCAN &start(double nm);
        SCAN &stop(double nm);
        SCAN &step(int pm);
        SCAN &step(double pm);
        SCAN &speed(int nm_s);
        SCAN &single_mode();
        SCAN &continue_mode();
        SCAN &run();
        void wait(int timeout_ms = 60000);
    };
    SCAN scan{this};
    class SENSOR
    {
    private:
        CTPX *r;

    public:
        uint8_t sense, ch;
        struct TRACE
        {
        private:
            std::string inst(const char *inst_end);

        public:
            SENSOR *r;
            TRACE(SENSOR *root) : r(root) {}
            double start();
            double step();
            int length();
            std::vector<float> data();
            std::vector<float> data(const char *data_path);
        };
        TRACE trace{this};
        SENSOR(CTPX *p_root, uint8_t _sense = 2, uint8_t _ch = 1) : r(p_root), sense(_sense), ch(_ch) {}
        // SENSOR(const SENSOR &_src) = delete;
        // void operator=(const SENSOR &_src) = delete;
        SENSOR(const SENSOR &_dest) : r(_dest.r), sense(_dest.sense), ch(_dest.ch)
        {
            trace = TRACE(this);
        }
        void operator=(const SENSOR &_dest)
        {
            r = _dest.r;
            sense = _dest.sense;
            ch = _dest.ch;
            trace = TRACE(this);
        };
        double power();
    };
    SENSOR sensor(uint8_t sensor, uint8_t channel);

    CTPX() {}
    int Init()
    {
        hw = new llgpib{10};
        hw->write("*IDN?");
        auto rlt = hw->read();
        if (rlt[0] != 0)
        {
            kpa::ins::msg.prefix("Instrument Name : ") << rlt;
            return 0;
        }
        else
        {
            kpa::ins::msg.prefix("Initialize Fail : ") << rlt;
            return -1;
        }
        // hw = new lltest{"C:\\tmp\\ctps_log.txt"};
    }
    void Term()
    {
        delete hw;
        hw = NULL;
    }
    std::string IDN();
    int status();
    void wait(int timeout_ms = 3000);
    std::string readError();
    void switch_ref(const SENSOR &sensor);
    void setWavelength(double nm);
    void backToCenterWL();
};

void CTPX::POWER::operator=(double dbm)
{
    var = dbm;
    if (r->hw != NULL)
        r->hw->write(str_format(":INIT:LAS:POW %.2fDBM", var));
}
CTPX::POWER::operator double()
{
    return var;
}
void CTPX::WAVELENGTH::operator=(double nm)
{
    var = nm;
    if (r->hw != NULL && var <= 1357.5 && var >= 1260.0)
    {
        r->hw->write(str_format(":CTP:RLAS1:WAV %.2fNM", var));
        r->hw->write(":CTP:SENS:FBC 1");
    }
    if (r->hw != NULL && var <= 1502.5 && var >= 1637.5)
    {
        r->hw->write(str_format(":CTP:RLAS2:WAV %.2fNM", var));
        r->hw->write(":CTP:SENS:FBC 2");
    };
}
CTPX::WAVELENGTH::operator double()
{
    return var;
}
int CTPX::SCAN::subSpeed(int sp)
{
    for (auto &ele : ls_speed_select)
        if (ele <= sp)
            return ele;
    return ls_speed_select[ls_speed_select.size() - 1];
}
CTPX::SCAN &CTPX::SCAN::start(double nm)
{
    if (r->hw != NULL)
        r->hw->write(str_format(":INIT:WAV:STAR %.3fNM", nm));
    return *this;
}
CTPX::SCAN &CTPX::SCAN::stop(double nm)
{
    if (r->hw != NULL)
        r->hw->write(str_format(":INIT:WAV:STOP %.3fNM", nm));
    return *this;
}
CTPX::SCAN &CTPX::SCAN::step(double pm)
{
    if (r->hw != NULL)
        r->hw->write(str_format(":INIT:WAV:SAMP %5.1fPM", pm));
    return *this;
}
CTPX::SCAN &CTPX::SCAN::speed(int nm_s)
{
    if (r->hw != NULL)
        r->hw->write(str_format(":INIT:TLS1:SPE %d", subSpeed(nm_s)));
    return *this;
}
CTPX::SCAN &CTPX::SCAN::single_mode()
{
    if (r->hw != NULL)
        r->hw->write(":INIT:SMOD SING");
    return *this;
}
CTPX::SCAN &CTPX::SCAN::continue_mode()
{
    if (r->hw != NULL)
        r->hw->write(":INIT:SMOD CONT");
    return *this;
}
CTPX::SCAN &CTPX::SCAN::run()
{
    if (r->hw != NULL)
        r->hw->write(":INIT");
    return *this;
}
void CTPX::SCAN::wait(int timeout_ms)
{
    r->wait(timeout_ms);
}
std::string CTPX::SENSOR::TRACE::inst(const char *inst_end)
{
    return str_format(":TRAC:SENS%d:CHAN%d:TYPE1:DATA%s", r->sense, r->ch, inst_end);
}
double CTPX::SENSOR::TRACE::start()
{
    if (r->r->hw == NULL)
        return 0.0;
    std::string rlt;
    auto process = [&]() -> bool
    {
        r->r->hw->write(inst(":STAR?").c_str());
        rlt = r->r->hw->read();
        return (rlt[0] != 0x0);
    };

    recipe_repeat(process);

    return atof(rlt.c_str());
}
double CTPX::SENSOR::TRACE::step()
{
    if (r->r->hw == NULL)
        return 1.0;
    std::string rlt;
    auto process = [&]() -> bool
    {
        auto cmd = inst(":SAMP?");
        r->r->hw->write(cmd);
        rlt = r->r->hw->read();
        if (rlt[0] == 0x0)
        {
            kpa::ins::msg << cmd;
        }
        return (rlt[0] != 0x0);
    };

    recipe_repeat(process);

    return atof(rlt.c_str());
}
int CTPX::SENSOR::TRACE::length()
{
    if (r->r->hw == NULL)
        return 0.0;
    r->r->hw->write(inst(":LENG?").c_str());
    auto rlt = r->r->hw->read();
    return atoi(rlt.c_str());
}
std::vector<float> CTPX::SENSOR::TRACE::data()
{
    const uint8_t len_unit = sizeof(float);

    kpa::ins::msg << str_format("sensor(%d,%d).data()", r->sense, r->ch);
    kpa::ins::MSG_INDENT __t;
    if (r->r->hw == NULL)
        return std::vector<float>(1, 1.0);

    int p, len_data;
    std::string rlt;
    auto subReadData = [&]() -> bool
    {
        r->r->hw->write(inst("? BIN,DB").c_str());

        auto TO_BK = r->r->hw->timeout;
        r->r->hw->timeout = 30000;
        rlt = r->r->hw->read(10 * 1024 * 1024);
        r->r->hw->timeout = TO_BK;

        p = 1;
        int len_qty = atoi(rlt.substr(p, 1).c_str());
        p += 1;
        len_data = kpa::ins::msg.prefix("len_data: ") << (atoi(rlt.substr(p, len_qty).c_str()) / len_unit);
        p += len_qty;
        if (len_data == 0)
        {
            kpa::ins::MSG_INDENT __t;
            kpa::ins::msg.prefix("head16: ") << std::vector<uint8_t>(rlt.begin(), rlt.begin() + 16);
        }
        return (len_data != 0);
    };

    // subReadData();
    recipe_repeat(subReadData, 3, 5000);

    std::vector<float> out(len_data, 0.);
    uint32_t ele_buf;
    for (auto &ele : out)
    {
        for (uint8_t b = 0; b < len_unit; b++)
        {
            ele_buf <<= 8;
            ele_buf |= (rlt[p++] & 0xFF);
        }
        ele = *(float *)&ele_buf;
    }
    return out;
}
std::vector<float> CTPX::SENSOR::TRACE::data(const char *data_path)
{
    std::ofstream f_log;
    auto wl = start();
    auto wl_step = step();
    auto d = data();

    f_log.open(data_path);
    f_log << "Wavelength(nm), Power(db) \n";
    for (int i = 0; i < (int)d.size(); i++)
    {
        f_log << str_format("%10.6f", wl * 1E+9) << ", " << d[i] << "\n";
        wl = wl + wl_step;
    }
    f_log.close();
    return d;
}
double CTPX::SENSOR::power()
{
    if (r->hw == NULL)
        return 0.;

    double out;
    std::string rlt;
    auto process = [&]()
    {
        r->hw->write(str_format(":CTP:SENS%d:CHAN%d:POW?", sense, ch).c_str());
        rlt = r->hw->read();
        out = atof(rlt.c_str());
        if (out == 0.0)
            kpa::ins::msg << rlt;
    };

    process();
    for (int i = 0; i < 5; i++)
        if (rlt[0] == 0x0)
        {
            Sleep(1000);
            process();
        }
        else
            break;

    return out;
}
CTPX::SENSOR CTPX::sensor(uint8_t _sensor, uint8_t channel)
{
    return SENSOR(this, _sensor, channel);
}
std::string CTPX::IDN()
{
    if (hw == NULL)
        return std::string();
    hw->write("*IDN?");
    return hw->read();
}
int CTPX::status()
{
    if (hw == NULL)
        return 0;
    hw->write(":STAT:OPER:COND?");
    auto rlt = hw->read();
    return atoi(rlt.c_str());
}
void CTPX::wait(int timeout_ms)
{
    clock_t tend = clock() + timeout_ms;
    while (clock() < tend)
        if (status() == 0)
            return;
        else
            Sleep(10);
}
std::string CTPX::readError()
{
    if (hw == NULL)
        return std::string("");
    hw->write("SYST:ERR?");
    return hw->read();
}
void CTPX::switch_ref(const CTPX::SENSOR &sensor)
{
    if (!hw)
        return;
    if (sensor.sense == __switch_ref_sense)
        if (sensor.ch == __switch_ref_ch)
            return;
    hw->write(str_format(":SYST:ANLG:OUT1:SOUR %d,%d", sensor.sense, sensor.ch).c_str());
    hw->read();
    hw->write(str_format(":SYST:ANLG:OUT2:SOUR %d,%d", sensor.sense, sensor.ch).c_str());
    hw->read();
    __switch_ref_sense = sensor.sense;
    __switch_ref_ch = sensor.ch;
    Sleep(100);
}
void CTPX::setWavelength(double nm)
{
    if (hw != NULL)
        hw->write(str_format(":CTP:RLAS1:WAV %.2fNM", nm));
}
void CTPX::backToCenterWL()
{
    setWavelength(center_wavelength_nm);
}
