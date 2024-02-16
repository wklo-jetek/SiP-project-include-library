#ifndef LIBTS300_HPP
#define LIBTS300_HPP
#include "..\kpa_main_p.hpp"
#include <vector>

//! ==== Prober Instructions ==== !//

class TS300
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
    class HEXAPOD
    {
    private:
        TS300 *up;

    public:
        uint16_t fiber_height = 40; // (um)
        struct
        {
            bool update = false; //
            double u, v, w;      // X, Y(pitch?), Z(roll?)
        } angle;
        struct
        {
            bool update = false;
            double x, y, z;
        } moter, pzt;
        HEXAPOD(TS300 *_up) : up(_up){};
        void MoveHomePosition();
        void MoveHexapodXY(double x_um, double y_um);
        void OpticalAlignment(bool RoughScanEn);
        void FindOpticalHeight();
        void ReadProbeAngle();
        void ReadProbePos();
        void RollAngle(double deg);
        void PitchAngle(double deg);
        void YawAngle(double deg);
    } hxpod{this};

    ~TS300();
    void Init(uint32_t bridge_num);
    void Term();
    void probing(STAT state);
    void Separate();
    void Contact();
    void MoveToSubDie(int subdie_no);
    void MoveToDie(int posx, int posy, int _subdie);
    std::vector<int> GetDiePosition(int *px_p = NULL, int *py_p = NULL);
    std::vector<double> Gethexpad_Couple(double *px_p = NULL, double *py_p = NULL, double *pz_p = NULL, double *power_p = NULL);
    std::vector<double> Getncube_Couple(double *px_p = NULL, double *py_p = NULL, double *pz_p = NULL);
    STAT getProbingState() { return cont_stat; }
    void setFineScan(bool en);
    void setGradinSearch(bool en);
    void setCoarseSeach(bool en);
};

TS300::~TS300()
{
    if (__fValid)
        Term();
}
void TS300::Init(uint32_t bridge_num)
{
    if (!__fValid)
    {
        LibTS300_Init(bridge_num);
        __fValid = true;
        __chuck = none,
        __fiber = none;
    }
}
void TS300::Term()
{
    if (__fValid)
    {
        LibTS300_Term();
        __fValid = false;
    }
}
char *TS300::query(const char *wt_buf, int TO_ms = 60000)
{
    if (__fValid)
    {
        LibTS300_Write((char *)wt_buf, 3000);
        auto err = LibTS300_Read(__rd_buf, 512, TO_ms);
        if (err != 0)
            kpa::ins::msg << "[Warning!] Message Receive Timeout in TS300 Module !";
    }
    return __rd_buf;
}
void TS300::probing(STAT state)
{
    auto chuck_contact = [&]() {
        if (__chuck == contact)
            return;
        query("move_chuck_contact");
        __chuck = contact;
    };
    auto chuck_seperate = [&]() {
        if (__chuck == separate)
            return;
        query("move_chuck_separation");
        __chuck = separate;
    };
    auto fiber_contact = [&]() {
        if (__fiber == contact)
            return;
        hxpod.FindOpticalHeight();
        __fiber = contact;
    };
    auto fiber_separate = [&]() {
        if (__fiber == separate)
            return;
        query("siph:move_separation West");
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
}
void TS300::Separate()
{
    // if (cont_stat == STAT::separate)
    //     return;
    // query("MoveOpticalSeparationHeight W");
}
void TS300::Contact()
{
    if (cont_stat == STAT::contact)
        return;
    query("move_chuck_contact");
    hxpod.FindOpticalHeight();
}
void TS300::MoveToSubDie(int subdie_no)
{
    if (subdie_no < 0)
        return;
    probing(separate);
    query(str_format("map:subsite:step %d", subdie_no).c_str());
}
void TS300::MoveToDie(int posx, int posy, int _subdie = -1)
{
    int num, px, py, subdie;
    probing(separate);
    auto r_buf = query("map:die:get_current_index");
    sscanf(r_buf, "0,0,%d,%d,%d", &num, &px, &py);
    auto r1_buf = query("map:die:get_current_subsite");
    sscanf(r1_buf, "0,0,%d", &subdie);
    subdie = (subdie < 0) ? 0 : subdie;
    query(str_format("map:step_die %d,%d,%d", posx, posy, (_subdie < 0) ? subdie : _subdie).c_str());
}

void TS300::HEXAPOD::MoveHomePosition()
{
    up->query("move_positioner_home West");
    this->moter.update = false;
}
void TS300::HEXAPOD::MoveHexapodXY(double x_um, double y_um)
{
    char buf[128];
    sprintf(buf, "move_positioner_xy West,Zero,%#.0f,%#.0f", x_um, y_um);
    auto rlt = up->query(buf, 11000);
}
void TS300::HEXAPOD::OpticalAlignment(bool RoughScanEn = false)
{
    up->query("siph:move_origin West");
    up->probing(contact);
    up->query("siph:fast_alignment", 90000);
}
void TS300::HEXAPOD::FindOpticalHeight()
{
    up->query("siph:move_hover West");
}
void TS300::HEXAPOD::ReadProbeAngle()
{
    // auto r_buf = up->query("ReadOpticalProbeAngle W Z");
    // sscanf(r_buf, "0: %lf %lf %lf", &angle.u, &angle.v, &angle.w);
    // angle.update = true;
    // return;
}
void TS300::HEXAPOD::ReadProbePos()
{
    up->query("get_positioner_xy West, Zero");
    sscanf(up->__rd_buf, "0,0,%lf,%lf", &moter.x, &moter.y);
    up->query("get_positioner_z West, zero");
    sscanf(up->__rd_buf, "0,0,%lf", &moter.z);
    up->query("get_nanocube_xy West");
    sscanf(up->__rd_buf, "0,0,%lf,%lf", &pzt.x, &pzt.y);
    up->query("get_nanocube_z West");
    sscanf(up->__rd_buf, "0,0,%lf", &pzt.z);
    moter.update = true;
    pzt.update = true;
    return;
}
void TS300::HEXAPOD::RollAngle(double deg)
{
    // up->probing(semi_cont);
    // if (!angle.update)
    //     ReadProbeAngle();
    // angle.w = deg;
    // up->query(str_format("RotateOpticalProbe W %f %f %f Z", angle.u, angle.v, angle.w).c_str());
}
void TS300::HEXAPOD::PitchAngle(double deg)
{
    // up->probing(semi_cont);
    // if (!angle.update)
    //     ReadProbeAngle();
    // angle.v = deg;
    // up->query(str_format("RotateOpticalProbe W %f %f %f Z", angle.u, angle.v, angle.w).c_str());
}
void TS300::HEXAPOD::YawAngle(double deg)
{
    // up->probing(semi_cont);
    // if (!angle.update)
    //     ReadProbeAngle();
    // angle.v = deg;
    // up->query(str_format("RotateOpticalProbe W %f %f %f Z", angle.u, angle.v, angle.w).c_str());
}
std::vector<int> TS300::GetDiePosition(int *px_p, int *py_p)
{
    int num, px = 0, py = 0, subdie;
    if (!__fValid)
        return std::vector<int>({px, py});

    auto r_buf = query("map:die:get_current_index");
    sscanf(r_buf, "0,0,%d,%d,%d", &num, &px, &py);
    auto r1_buf = query("map:die:get_current_subsite");
    sscanf(r1_buf, "0,0,%d", &subdie);
    if (px_p != NULL)
        *px_p = px;
    if (py_p != NULL)
        *py_p = py;
    return std::vector<int>({px, py});
}

std::vector<double> TS300::Gethexpad_Couple(double *px_p, double *py_p, double *pz_p, double *power_p)
{
    double px = 0, py = 0, pz = 0, power = 0;
    if (!__fValid)
        return std::vector<double>({px, py, pz, power});

    auto r_buf = query("siph:get_intensity");
    sscanf(r_buf, "0,0,%lf", &power);
    auto r1_buf = query("get_positioner_xy West, Zero");
    sscanf(r1_buf, "0,0,%lf,%lf", &px, &py);
    auto r2_buf = query("get_positioner_z West, zero");
    sscanf(r2_buf, "0,0,%lf", &pz);
    if (power_p != NULL)
        *power_p = power;
    if (px_p != NULL)
        *px_p = px;
    if (py_p != NULL)
        *py_p = py;
    if (pz_p != NULL)
        *pz_p = pz;
    return std::vector<double>({px, py, pz, power});
}

std::vector<double> TS300::Getncube_Couple(double *px_p, double *py_p, double *pz_p)
{
    double px = 0, py = 0, pz = 0;
    if (!__fValid)
        return std::vector<double>({px, py, pz});

    auto r1_buf = query("get_nanocube_xy West");
    sscanf(r1_buf, "0,0,%lf,%lf", &px, &py);
    auto r2_buf = query("get_nanocube_z West");
    sscanf(r2_buf, "0,0,%lf", &pz);

    if (px_p != NULL)
        *px_p = px;
    if (py_p != NULL)
        *py_p = py;
    if (pz_p != NULL)
        *pz_p = pz;
    return std::vector<double>({px, py, pz});
}
void TS300::setFineScan(bool en)
{
    auto b = [](char *str) -> bool { return str[0] == 'T'; };
    auto o = [](bool en) -> const char * { return (en) ? "ON" : "OFF"; };
    auto r_buf = query("siph:get_alignment West,Array");
    strtok(r_buf, ",");
    strtok(NULL, ",");
    auto Coarse_Seach = strtok(NULL, ",");
    auto Fine_Search = strtok(NULL, ",");
    auto G_Search = strtok(NULL, ",");
    auto cmd = str_format("siph:set_alignment West,Array,%s,%s,%s,OFF", o(b(Coarse_Seach)), o(en), o(b(G_Search)));
    query(cmd.c_str());
}
void TS300::setGradinSearch(bool en)
{
    auto b = [](char *str) -> bool { return str[0] == 'T'; };
    auto o = [](bool en) -> const char * { return (en) ? "ON" : "OFF"; };
    auto r_buf = query("siph:get_alignment West,Array");
    strtok(r_buf, ",");
    strtok(NULL, ",");
    auto Coarse_Seach = strtok(NULL, ",");
    auto Fine_Search = strtok(NULL, ",");
    auto G_Search = strtok(NULL, ",");
    auto cmd = str_format("siph:set_alignment West,Array,%s,%s,%s,OFF", o(b(Coarse_Seach)), o(b(Fine_Search)), o(en));
    query(cmd.c_str());
}
void TS300::setCoarseSeach(bool en)
{
    auto b = [](char *str) -> bool { return str[0] == 'T'; };
    auto o = [](bool en) -> const char * { return (en) ? "ON" : "OFF"; };
    auto r_buf = query("siph:get_alignment West,Array");
    strtok(r_buf, ",");
    strtok(NULL, ",");
    auto Coarse_Seach = strtok(NULL, ",");
    auto Fine_Search = strtok(NULL, ",");
    auto G_Search = strtok(NULL, ",");

    auto cmd = str_format("siph:set_alignment West,Array,%s,%s,%s,OFF", o(en), o(b(Fine_Search)), o(b(G_Search)));

    query(cmd.c_str());
}

#endif
