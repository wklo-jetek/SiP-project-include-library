// ======== ======== ======== ========
// Function serial run
// ======== ======== ======== ========

#ifndef __H_LIBKPADOMAINCONTROL__
#define __H_LIBKPADOMAINCONTROL__

#include <iostream>
#include <string>
#include "libKpaLevelMessage.hpp"
#include "libKpaDataPort.hpp"

namespace KPA
{
    const char *sysout = "KPA>> ";
}

// *** KPA_Serial Define *** //

namespace KPA
{
    template <class F>
    void Serial_Run(const char *desc = "s_idx", F func = NULL)
    {
        msg << "KPA::Serial_Mode";
        MSG_INDENT __ind;
        int s_qty = kpa_qty_sites;
        ACTS_BUF __o, __t;
        __o = kpa_actsite.getbuf();
        if (!__o)
            return;
        __t.fill('0');
        kpa_actsite.setbuf(__t);

        // actsite.__FLAG_SERIAL = true;
        for (int s_idx = 0; s_idx < s_qty; s_idx++)
            if (__o[s_idx] == '1')
            {
                __t[s_idx] = '1';     // ! Enable target site
                kpa_actsite.update(); // ! Link to new buffer and trigger actsite update
                func(s_idx);          // ! Run function
                // origin[s_idx] = actbuf[s_idx]; // ! Transfer the result of serial mode to parallel
                __t[s_idx] = '0'; // ! Disable target site
            }
        kpa_actsite.setbuf(__o);
        // actsite.__FLAG_SERIAL = false;
        // if (!acts_sys) // ! Check actsite is all disable
        //     STDF_LOG::event_esc();// ! Jump to KPA::TEST
    }
#define KPA_Serial_Start(s_idx) \
    KPA::Serial_Run(#s_idx, [&](int s_idx){
#define KPA_Serial_Stop() \
    });
}

// *** KPA_Serial Define *** //

namespace KPA
{
    template <typename F>
    void Repeat_Until_Pass(KPA::DATA_PORT_O &buffer, int loop, F func)
    {
        msg << "KPA::Repeat_Mode";
        MSG_INDENT __ind;
        int loop_remain = loop;
        ACTS_BUF __o, __t(kpa_actsite.getbuf());

        __o = kpa_actsite.getbuf();
        kpa_actsite.setbuf(__t);
        // msg << kpa::ins::format("Remain: %d, Acts: %s", loop_remain--, (char *)actsite);
        func(0);
        // actsite.__FLAG_SERIAL = true;
        for (int loop_idx = 1; loop_idx < loop; loop_idx++)
        {
            __t.assign(buffer.r_fail().c_str());
            if (!__t)
                break;
            kpa_actsite.update();
            // msg << kpa::ins::format("Remain: %d, Acts: %s", loop_remain--, (char *)actsite);
            func(loop_idx);
        }
        kpa_actsite.setbuf(__o);
        // actsite.__FLAG_SERIAL = false;
    }
#define KPA_Repeat_Start(data_port, loop, loop_idx) \
    KPA::Repeat_Until_Pass(data_port, loop, [&](int loop_idx){
#define KPA_Repeat_Stop() \
    });
}

namespace KPA
{
    template <typename F>
    void Active_Region(const char *active, F func)
    {
        msg << "KPA::Active_Mode";
        MSG_INDENT __ind;
        ACTS_BUF __o, __t;
        __o = kpa_actsite.getbuf();
        __t.assign(active);
        if (!__t)
            return;

        kpa_actsite.setbuf(__t);
        func();
        kpa_actsite.setbuf(__o);
    };
#define KPA_Active_Start(actsite) \
    KPA::Active_Region(actsite, [&](){
#define KPA_Active_Stop() \
    });
}

#endif // __H_LIBKPADOMAINCONTROL__