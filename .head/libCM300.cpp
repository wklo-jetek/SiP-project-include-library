#ifndef LIBCM300_HPP
#define LIBCM300_HPP
#include "..\kpa_main_p.hpp"
#include <vector>
#include <sstream>

//! ==== Prober Instructions ==== !//

class CM300
{
public:
    enum STAT
    {
        none = 0,
        separate,
        contact,
        semi_cont
    };

private:
    char __rd_buf[512];
    char *query(const char *wt_buf, int TO_ms);
    bool __fValid = false;
    STAT cont_stat = none, __chuck = none, __fiber = none;

public:
    int __subdie = -1;
    class HEXAPOD
    {
    private:
        CM300 *up;

    public:
        uint16_t fiber_height = 40; // (um)
        struct
        {
            bool update = false; //
            double u, v, w;      // X(yaw), Y(pitch), Z(roll)
        } angle;
        struct
        {
            bool update = false;
            double x, y, z;
        } moter, pzt;
        HEXAPOD(CM300 *_up) : up(_up){};
        void MoveHomePosition();
        void MoveHexapodXY(double x_um, double y_um);
        void MovePztXY(double x_um, double y_um);
        void OpticalAlignment(bool RoughScanEn);
        void FindOpticalHeight();
        void ReadProbeAngle();
        void ReadProbePos();
        void RollAngle(double deg);
        void PitchAngle(double deg);
        void YawAngle(double deg);
    } hxpod{this};

    ~CM300();
    void Init(uint32_t bridge_num);
    void Term();
    void probing(STAT state);
    STAT getProbingState();
    void Separate();
    void Contact();
    void MoveToSubDie(uint16_t subdie_no);
    void MoveToDie(int posx, int posy, int _subdie = -1);
    std::vector<int> GetDiePosition(int *px_p = NULL, int *py_p = NULL, int *subdie_p = NULL);
};

CM300::~CM300()
{
    if (__fValid)
        Term();
}
void CM300::Init(uint32_t bridge_num)
{
    if (!__fValid)
    {
        LibCM300_INS_Init(bridge_num);
        __fValid = true;
        __chuck = none,
        __fiber = none;
        GetDiePosition();
    }
}
void CM300::Term()
{
    if (__fValid)
    {
        LibCM300_INS_Term();
        __fValid = false;
    }
}
char *CM300::query(const char *wt_buf, int TO_ms = 60000)
{
    if (__fValid)
    {
        LibCM300_INS_Write((char *)wt_buf);
        auto err = LibCM300_INS_Read(__rd_buf, 512, TO_ms);
        if (err != 0)
            kpa::ins::msg << "[Warning!] Message Receive Timeout in CM300 Module !";
    }
    return __rd_buf;
}
void CM300::probing(STAT state)
{
    if (cont_stat == STAT::none)
        getProbingState();
    if (cont_stat == state)
        return;
    auto chuck_contact = [&]()
    {
        if (__chuck == contact)
            return;
        query("MoveChuckContact");
        __chuck = contact;
    };
    auto chuck_seperate = [&]()
    {
        if (__chuck == separate)
            return;
        query("MoveChuckSeparation");
        __chuck = separate;
    };
    auto fiber_contact = [&]()
    {
        if (__fiber == contact)
            return;
        hxpod.FindOpticalHeight();
        __fiber = contact;
    };
    auto fiber_separate = [&]()
    {
        if (__fiber == separate)
            return;
        query("MoveOpticalSeparationHeight W");
        __fiber = separate;
    };
    switch (state)
    {
    case separate:
        chuck_seperate();
        fiber_separate();
        break;
    case contact:
        chuck_contact();
        fiber_contact();
        break;
    case semi_cont:
        chuck_contact();
        fiber_separate();
        break;
    default:
        break;
    }
    cont_stat = state;
}
CM300::STAT CM300::getProbingState()
{
    if (!__fValid)
        return cont_stat;
    auto split = [](const char *str) -> std::vector<std::string>
    {
        std::istringstream iss(str);
        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token)
            tokens.push_back(token);
        return tokens;
    };
    auto rlt = query("ReadChuckStatus"); //* 0: 7 7 0 0 T 1 C 2 0 P
    auto args = split(rlt);
    char c = args[7][0];
    switch (c)
    {
    case 'C':
        switch (cont_stat)
        {
        case STAT::contact:
            cont_stat = STAT::contact;
            break;
        case STAT::semi_cont:
        case STAT::separate:
        case STAT::none:
            cont_stat = STAT::semi_cont;
        }
        break;
    case 'S':
    case 'A':
    case '0':
        cont_stat = STAT::separate;
        break;
    default:
        std::cout << "[CM300] Warning! Failed to analysis chuck status." << std::endl;
    }
    return cont_stat;
}
void CM300::Separate()
{
    if (cont_stat == STAT::separate)
        return;
    query("MoveOpticalSeparationHeight W");
}
void CM300::Contact()
{
    if (cont_stat == STAT::contact)
        return;
    query("MoveChuckContact");
    hxpod.FindOpticalHeight();
}
void CM300::MoveToSubDie(uint16_t subdie_no)
{
    if (__subdie < 0)
        GetDiePosition();
    if (subdie_no == __subdie)
        return;
    probing(separate);
    query(str_format("StepNextSubDie %d", subdie_no).c_str());
    __subdie = subdie_no;
}
void CM300::MoveToDie(int posx, int posy, int _subdie)
{
    int num, px, py, subdie;
    if (__subdie < 0)
        GetDiePosition();
    probing(separate);
    subdie = (_subdie < 0) ? __subdie : _subdie;
    query(str_format("StepNextDie %d %d %d", posx, posy, subdie).c_str());
    __subdie = subdie;
}

void CM300::HEXAPOD::MoveHomePosition()
{
    up->query("MoveOpticalProbe W 0 0 H");
    this->moter.update = false;
}
void CM300::HEXAPOD::MoveHexapodXY(double x_um, double y_um)
{
    char buf[128];
    sprintf(buf, "MoveOpticalProbe W %5.2f %5.2f H", x_um, y_um);
    auto rlt = up->query(buf, 11000);
}
void CM300::HEXAPOD::MovePztXY(double x_um, double y_um)
{
    char buf[128];
    auto checkRange = [](double v) -> bool
    { return ((v > 0.) && (v < 100.)); };
    if (!checkRange(x_um))
        return;
    if (!checkRange(y_um))
        return;
    sprintf(buf, "MovePZT W %5.2f %5.2f 0. H", x_um, y_um);
    auto rlt = up->query(buf, 11000);
}
void CM300::HEXAPOD::OpticalAlignment(bool RoughScanEn = false)
{
    // up->probing(semi_cont); // up->Separate();
    // MoveHomePosition();     // up->query("MoveOpticalProbe W 0 0 H");
    up->probing(contact); // up->Contact();
    if (RoughScanEn)
        up->query("SearchFirstLight 1 0");
    up->query("AlignOpticalProbes");
}
void CM300::HEXAPOD::FindOpticalHeight()
{
    up->query(str_format("FindOpticalProbeHeight W %d", fiber_height).c_str());
}
void CM300::HEXAPOD::ReadProbeAngle()
{
    auto r_buf = up->query("ReadOpticalProbeAngle W Z");
    sscanf(r_buf, "0: %lf %lf %lf", &angle.u, &angle.v, &angle.w);
    angle.update = true;
    return;
}
void CM300::HEXAPOD::ReadProbePos()
{
    auto r_buf = up->query("ReadOpticalProbePos W Z");
    sscanf(r_buf, "0: %lf %lf %lf %lf %lf %lf", &moter.x, &moter.y, &moter.z, &pzt.x, &pzt.y, &pzt.z);
    moter.update = true;
    pzt.update = true;
    return;
}
void CM300::HEXAPOD::RollAngle(double deg)
{
    up->probing(semi_cont);
    if (!angle.update)
        ReadProbeAngle();
    angle.w = deg;
    up->query(str_format("RotateOpticalProbe W %f %f %f Z", angle.u, angle.v, angle.w).c_str());
}
void CM300::HEXAPOD::PitchAngle(double deg)
{
    up->probing(semi_cont);
    if (!angle.update)
        ReadProbeAngle();
    angle.v = deg;
    up->query(str_format("RotateOpticalProbe W %f %f %f Z", angle.u, angle.v, angle.w).c_str());
}
void CM300::HEXAPOD::YawAngle(double deg)
{
    up->probing(semi_cont);
    if (!angle.update)
        ReadProbeAngle();
    angle.u = deg;
    up->query(str_format("RotateOpticalProbe W %f %f %f Z", angle.u, angle.v, angle.w).c_str());
}
std::vector<int> CM300::GetDiePosition(int *px_p, int *py_p, int *subdie_p)
{
    int num, px = 0, py = 0;
    if (!__fValid)
        return std::vector<int>({px, py});
    auto r_buf = query("GetDieDataAsNum");
    sscanf(r_buf, "0:%d %d %d", &num, &px, &py); //! sscanf(r_buf, "0:%d %d %d %d", &num, &px, &py, &__subdie);
    if (px_p != NULL)
        *px_p = px;
    if (py_p != NULL)
        *py_p = py;
    // if (subdie_p != NULL)
    //     *subdie_p = __subdie;
    return std::vector<int>({px, py, __subdie});
}

#endif
