#ifndef LIBCUSTOMIZEITEM_CPP
#define LIBCUSTOMIZEITEM_CPP

#include "libKpaSystem.cpp"

namespace kpa
{
    namespace info
    {
        KPA::LVO_INT32 panel_mode{"panel.mode"};
        KPA::LVO_STRING proc_name{"Process name"};
        KPA::LVO_STRING datalog_path{"Datalog path"};
        KPA::LVO_STRING report_name{"Report name", true};
        KPA::LVO_STRING Path_XY{"Backup01"};
    } // namespace info
    namespace flag
    {
    }
    namespace ins
    {
    }
    namespace port
    {
        KPA::LVP_STRING posx("Position X");
        KPA::LVP_STRING posy("Position Y");
        KPA::LVP_BOOL f_stop_button{"STOP"};
        KPA::LVP_INT32 panel_mode{"panel.mode"};
    } // namespace port
} // namespace kpa

#endif //* LIBCUSTOMIZEITEM_CPP
