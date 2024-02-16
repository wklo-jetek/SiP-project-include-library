#include "kpa_main_p.hpp"
#include "kpa_user_1_global.cpp"

void user::KPA_INIT(uint32_t bridge_num)
{
    KPA::MSG_INDENT __t;
    // ***  Program start  *** //

    ItemReader_Initialize(proj_path);
    LibItemReader_SetEvent((char *)"init");
    ItemReader_FunctionOutset(1);

    char buf[128];
    while (ItemReader_NextFunction(buf, 128)) {
        string cmd(buf);
        if (cmd == "SIPH_HwControl") {
            // msg << "SIPH::SIPH_HwControl >> ";
            FORMAT::SIPH::SIPH_HwControl set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            if (set.Enable_Laser) {
                msg << "Enable Laser >>";
                kpa::ins::MSG_INDENT ___t;
                laser.Init();
            }
            if (set.Enable_Polarizer) {
                msg << "Enable Polarizer >>";
                kpa::ins::MSG_INDENT ___t;
                polar.Init();
            }
            if (set.Enable_OpticalSwitch) {
                msg << "Enable Optical Switch >>";
                kpa::ins::MSG_INDENT ___t;
                lsw.Init();
            }
            if (set.Enable_ProberCommunication) {
                msg << "Enable Prober Communication >>";
                kpa::ins::MSG_INDENT ___t;
                probe.Init(bridge_num);
                auto probeState = probe.getProbingState();
                if (probeState == probe.contact)
                    msg << "Set probing state to contact";
                else if (probeState == probe.semi_cont)
                    msg << "Set probing state to semi-contact";
                else if (probeState == probe.separate)
                    msg << "Set probing state to separate";
            }
            if (set.Enable_SMU) {
                msg << "Enable Prober Communication >>";
                kpa::ins::MSG_INDENT ___t;
                smu.Init();
            }
        }
    }

    // laser.Init();
    // polar.Init();
    // lsw.Init();
    // smu.Init();
    // pm.Init();

    // ***  Program end    *** //
    return;
}
