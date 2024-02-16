#define __SC__
#define NIDL
#undef __STRICT_ANSI__ // for itoa not found issue
#define BYPASS_TOSTRING_ERROR

#ifndef __TESTPROGRAM_H__
#define __TESTPROGRAM_H__

#include "libCustomizeItem.cpp"
#include "libKpaSiUnit.h"
#include "libKpaSystem.cpp"
#include <extcode.h>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <windows.h>

// namespace kpa {
//     namespace info {
//         KPA::LVO_INT32 panel_mode{"panel.mode"};
//         KPA::LVO_STRING proc_name{"Process name"};
//         KPA::LVO_STRING datalog_path{"Datalog path"};
//         KPA::LVO_STRING report_name{"Report name", true};
//     }
//     namespace flag {
//     }
//     namespace ins {
//     }
//     namespace port {
//         KPA::LVP_STRING posx("Position X");
//         KPA::LVP_STRING posy("Position Y");
//         KPA::LVP_BOOL f_stop_button{"STOP"};
//         KPA::LVP_INT32 panel_mode{"panel.mode"};
//     }
// }

#include "kpa_user_0_head.h"
namespace user {
    void KPA_INIT(uint32_t bridge_num);
    void KPA_TEST();
    void KPA_EVENT(const std::string &event);
    void KPA_TERM();
}

#ifdef __cplusplus
extern "C"
{
#endif
    namespace KPA {
        using std::cout;
        using std::endl;

        void KPA_ENT_INIT(uint32_t bridge_num);
        void KPA_ENT_TEST();
        void KPA_ENT_EVENT(char kpa_event[], int length);
        void KPA_ENT_TERM();

    }
#ifdef __cplusplus
}
#endif

// ==== new databus struct ==== //

#endif
