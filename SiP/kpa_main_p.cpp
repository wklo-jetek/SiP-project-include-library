#include "kpa_main_p.hpp"
#include "kpa_user_1_global.cpp"
#include "kpa_user_2_init.cpp"
#include "kpa_user_3_test.cpp"
#include "kpa_user_4_event.cpp"
#include "kpa_user_5_term.cpp"
#include <setjmp.h>

//* ===== subfunctions ===== *//

jmp_buf ESC_POINT;

void kpa_ent_eos(int s_idx)
{
    KPA::ACTS_BUF __o, __t('0');
    __o = kpa::info::actsite.getbuf();
    __t[s_idx] = '1';
    kpa::info::actsite.setbuf(__t);
    user::KPA_EVENT(KPA::msg.prefix("KPA::EVENT::") << std::string("EOS"));
    kpa::info::actsite.setbuf(__o);
}

//* ===== mainfunction ===== *//

void KPA::KPA_ENT_INIT(uint32_t bridge_num)
{
    std::cout << endl;

    KPA::STDF_LOG::event_eos = kpa_ent_eos;
    KPA::STDF_LOG::event_esc = []() {};

    KPA::bridge_linkInfo(bridge_num);
    KPA::LibCppCmdSet_Bridge(bridge_num);
    KPA::bridge_updateInfo();
    KPA::acts_sot();
    KPA::log_open(bridge_num, NULL); //! [Important] Bin judgement method setting

    KPA::msg << "KPA::INIT";
    user::KPA_INIT(bridge_num);

    return;
}

void KPA::KPA_ENT_TEST()
{
    KPA::STDF_LOG::event_esc = []() {
        longjmp(ESC_POINT, 1); //* Escape to top level point
    };

    KPA::bridge_updateInfo();
    KPA::LibCppCmdSet_sot();
    KPA::acts_sot();
    KPA::log_sot();

    int err_code = setjmp(ESC_POINT);
    switch (err_code) {
    case 0:
        KPA::msg << "KPA::TEST";
        user::KPA_TEST();
        break;
    case 1:
        KPA::MSG_INDENT::reset();
        msg << "[SYS] All sites is nonactive, procedure stop !!";
        break;
    default:
        KPA::MSG_INDENT::reset();
        msg << "[SYS] Unknown error happened !!";
        break;
    }

    user::KPA_EVENT(msg.prefix("KPA::EVENT::") << std::string("EOT"));
    KPA::log_eot();

    return;
}

void KPA::KPA_ENT_EVENT(char kpa_event[], int length)
{
    std::string event(kpa_event);
    KPA::bridge_updateInfo();
    KPA::acts_sot();

    if (event == "LOTEND")
        KPA::log_close();
    user::KPA_EVENT(msg.prefix("KPA::EVENT::") << event);

    return;
}

void KPA::KPA_ENT_TERM()
{

    KPA::msg << "KPA::TERM";
    user::KPA_TERM();

    return;
}
