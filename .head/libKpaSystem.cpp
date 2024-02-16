#ifndef LIBKPASYSTEM_CPP
#define LIBKPASYSTEM_CPP

#include <vector>
#include <string>

#include ".\libDataBridge.hpp"
#include ".\libDataBridgeActsite.hpp"
#include ".\libDatalog.hpp"
#include ".\libKpaLevelMessage.hpp"
#include ".\libKpaDataPort.hpp"
#include ".\libKpaDomainControl.hpp"
#include ".\libKpaExtend.hpp"

namespace kpa
{
    namespace info
    {
        auto &n_site = KPA::kpa_qty_sites;
        auto &actsite = KPA::kpa_actsite;
        auto &max_pass_bin = KPA::kpa_max_pass_bin;
        KPA::LVO_STRING lotid{"Lot ID"};
        KPA::LVO_STRING waferid{"Wafer ID"};
        KPA::LVO_STRING die_no{"Die number"};
        KPA::LVO_STRING pos_x{"Position X"};
        KPA::LVO_STRING pos_y{"Position Y"};
        KPA::LVO_STRING proj_path{"Project path"};
        KPA::LVO_STRING proj_name{"Project name"};
        KPA::LVO_STRING stdf_name{"Report name"};
    } // namespace info
    namespace ins
    {
        using KPA::c_ptr;
        using KPA::judge;
        using KPA::log;
        using KPA::LoopRun;
        using KPA::msg;
        using KPA::MSG_INDENT;
        using KPA::Pause;
        // auto &next_group = KPA::LibCppCmdSet_NextGroup;
        // auto &next_item = KPA::LibCppCmdSet_NextItem;
        KPA::ITEM_SELECT i_group(KPA::LibCppCmdSet_NextGroup);
        KPA::ITEM_SELECT i_item(KPA::LibCppCmdSet_NextItem);
    } // namespace ins
    namespace type
    {
        using KPA::DATA_PORT;
        using KPA::DATA_PORT_GROUP;
        using KPA::DATA_PORT_INT;
    } // namespace type
    namespace flag
    {
        auto &f_failstop = KPA::kpa_failstop;
        KPA::LVO_BOOL f_stop_button{"STOP", true};
        KPA::LVO_BOOL f_debug{"Debug", true};
        KPA::LVO_BOOL f_pause{"Pause", true};
        KPA::LVO_BOOL f_summary{"Summary", true};
        KPA::LVO_BOOL f_terminal{"Terminal", true};
    } // namespace flag
    namespace port
    {
        auto &bins = KPA::kpa_bins;
        KPA::LVP_STRING sys_cmd{"system.cmd"};
        KPA::LVP_STRING sys_msg{"system.msg"};
    } // namespace port
} // namespace kpa

#endif //* LIBKPASYSTEM_CPP
