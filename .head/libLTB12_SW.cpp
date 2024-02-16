#include "libCustomizeItem.cpp"
#include "libLLDriver.cpp"
#include <string>
#include <vector>

class LTB12
{
protected:
    struct CAL
    {
        double lstart;
        double lstep;
        std::vector<float> comp;

        bool fLoaded()
        {
            return (comp.size() != 0);
        }
        void readfile() {}
        float comp_value(double wl)
        {
            int idx = int(((wl - lstart) / lstep) + 0.5);
            if (idx >= (int)comp.size())
                return 0.0;
            return comp[idx];
        }
    } cals[16];
    CAL *p_cal = cals;

public:
    LLDriverBase *hw = NULL;

    ~LTB12()
    {
        if (hw == NULL)
            Term();
    }
    int Init();
    void Term();
    void channel(int ch);
    std::vector<float> compensate(double start, double step, const std::vector<float> &data);
};
int LTB12::Init()
{
    if (hw != NULL)
        return -2;
    hw = new llgpib{12};
    hw->write("*IDN?\n");
    auto rlt = hw->read();
    if (rlt[0] != 0) {
        kpa::ins::msg.prefix("Instrument Name : ") << rlt;
        return 0;
    } else {
        kpa::ins::msg.prefix("Initialize Fail : ") << rlt;
        return -1;
    }
}
void LTB12::Term()
{
    if (hw == NULL)
        return;
    delete hw;
    hw = NULL;
}
void LTB12::channel(int ch)
{
    hw->write(str_format("LINS0:ROUT:SCAN %d\n", ch));
    hw->write("LINS0:ROUT:SCAN?\n");
    auto buf = hw->read();
    if (atoi(buf.c_str()) != ch) {
        kpa::ins::msg << "LTB12::channel >> ";
        kpa::ins::MSG_INDENT __t;
        kpa::ins::msg << "[Warning!] Data double check is failure!";
        return;
    }
    p_cal = cals + ch;
    if (!p_cal->fLoaded())
        p_cal->readfile();
}
std::vector<float> LTB12::compensate(double start, double step, const std::vector<float> &data)
{
    std::vector<float> out(data);
    int i = 0;
    for (auto &e : out)
        e = e - p_cal->comp_value(start + step * (i++));
    return out;
}
