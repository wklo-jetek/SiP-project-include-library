#include "kpa_main_p.hpp"
#include "kpa_user_1_global.cpp"

void user::KPA_EVENT(const std::string &event)
{
    kpa::ins::MSG_INDENT __t;
    // ***  Program start  *** //

    if (event == "WAFEREND") {
        fWaferStart = true;
        getDataFolder.reset();
        return;
    }
    if (event == "LOTEND") {
        return;
    }
    if (event == "EOS") {
        return;
    }
    if (event == "EOT") {
        fWaferStart = false;
        laser.backToCenterWL(); //! laser.wavelength = conf.center_wl;
        if (kpa::info::panel_mode)
            probe.probing(probe.separate);
        return;
    }

    // ***  Program end    *** //
    return;
}
