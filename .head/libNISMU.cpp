#include "libCustomizeItem.cpp"
#include "libSiPhSmu.h"
#include <string>
#include <vector>

class NISMU;
class NISMU_CH;
class NISMU_CH
{
private:
    uint8_t &__fValid;
    std::string __pin;

public:
    std::vector<double> mv, mi;

    NISMU_CH(const char *pin_name, uint8_t &fValid) : __fValid(fValid), __pin(pin_name){};
    NISMU_CH(NISMU_CH &obj) : __fValid(obj.__fValid), __pin(obj.__pin){};
    ~NISMU_CH(){};
    NISMU_CH &ForceV(double vlevel, double ilimit, double vrange, double irange);
    NISMU_CH &VLevel(double vlevel);
    NISMU_CH &ForceI(double ilevel, double vlimit, double irange, double vrange);
    NISMU_CH &ILevel(double ilevel);
    NISMU_CH &Enable();
    NISMU_CH &Disable();
    NISMU_CH &Measure();
};
class NISMU
{
private:
    uint8_t __fValid = false;

public:
    NISMU_CH pin1{"pin1", __fValid};

    NISMU(){};
    ~NISMU();
    int Init();
    int Term();
    // NISMU_CH GetChannel(int ch);
};

NISMU::~NISMU()
{
    if (__fValid)
        Term();
}
int NISMU::Init()
{
    if (__fValid)
        return -2;
    int err = LibSmuNi_Init(kpa::info::proj_path);
    if (err) {
        kpa::ins::msg << "SMU Init Error!";
        return err;
    }
    kpa::info::actsite << LibSmuNi_SetActsite;
    __fValid = true;
    return 0;
}
int NISMU::Term()
{
    if (!__fValid)
        return -2;
    LibSmuNi_Term();
    __fValid = false;
    return 0;
}
// NISMU_CH NISMU::GetChannel(int ch)
// {
//     return NISMU_CH(ch, __fValid);
// }
NISMU_CH &NISMU_CH::ForceV(double vlevel, double ilimit, double vrange, double irange)
{
    LibSmuNi_ForceV((char *)__pin.c_str(), vlevel, ilimit, vrange, irange);
    return *this;
}
NISMU_CH &NISMU_CH::ForceI(double ilevel, double vlimit, double irange, double vrange)
{
    LibSmuNi_ForceI((char *)__pin.c_str(), ilevel, vlimit, irange, vrange);
    return *this;
}
NISMU_CH &NISMU_CH::VLevel(double vlevel)
{
    LibSmuNi_VLevel((char *)__pin.c_str(), vlevel);
    return *this;
}
NISMU_CH &NISMU_CH::ILevel(double ilevel)
{
    LibSmuNi_ILevel((char *)__pin.c_str(), ilevel);
    return *this;
}
NISMU_CH &NISMU_CH::Enable()
{
    LibSmuNi_Enable((char *)__pin.c_str());
    return *this;
}
NISMU_CH &NISMU_CH::Disable()
{
    LibSmuNi_Disable((char *)__pin.c_str());
    return *this;
}
NISMU_CH &NISMU_CH::Measure()
{
    mv.resize(kpa::info::n_site, 0.0);
    mi.resize(kpa::info::n_site, 0.0);
    LibSmuNi_Meas((char *)__pin.c_str(), kpa::ins::c_ptr(mv), mv.size(), kpa::ins::c_ptr(mi), mi.size());
    return *this;
}
