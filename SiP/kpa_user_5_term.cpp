#include "kpa_main_p.hpp"
#include "kpa_user_1_global.cpp"

void user::KPA_TERM()
{
    KPA::MSG_INDENT __t;
    // ***  Program start  *** //

    ItemReader_Term();

    probe.Term();
    laser.Term();
    polar.Term();
    pm.Term();
    lsw.Term();

    // ***  Program end    *** //
    return;
}
