#include "kpa_main_p.hpp"
#include "kpa_user_1_global.cpp"

void user::KPA_TEST()
{
    kpa::ins::MSG_INDENT __t;
    LibItemReader_SetEvent((char *)"test");
    ItemReader_FunctionOutset(1);
    // ***  Program start  *** //

    auto printDie = [&]() {
        if (kpa::info::panel_mode) { //! Auto mode
            px = atoi(kpa::info::pos_x);
            py = atoi(kpa::info::pos_y);
        } else { //! Menual mode
            px = 0;
            py = 0;
            probe.GetDiePosition(&px, &py);
            char buf[16];
            sprintf(buf, "%d", px);
            kpa::port::posx << buf;
            sprintf(buf, "%d", py);
            kpa::port::posy << buf;
        }
        msg.prefix("(x,y) = ") << str_format("(%d,%d)", px, py);
        kpa::ins::log(item_no++, "Position X", vector<int>({px}), "%d");
        kpa::ins::log(item_no++, "Position Y", vector<int>({py}), "%d");
    };

    //! *** Program Start ***

    item_no = 1;
    printDie();
    laser.power = conf.laser_pw;
    laser.wavelength = conf.center_wl;
    probe.hxpod.fiber_height = conf.fiber_height;

    // auto switchInstruction = [](char *ins) {
    //     if (strncmp(ins, "TA_", 3) == 0) {
    //     }
    // };

    char buf[128];
    while (ItemReader_NextFunction(buf, 128)) {
        string func_name(buf);
        if (func_name == "TA_1DGC") {
            msg << "TEST_ALGRORITHM::TA_1DGC >> ";
            objTestAlgrorithm.dev1DGC();
        } else if (func_name == "TA_2DGC") {
            msg << "TEST_ALGRORITHM::TA_2DGC >> ";
            objTestAlgrorithm.dev2DGC();
        } else if (func_name == "TA_1IMO") {
            msg << "TEST_ALGRORITHM::TA_1IMO >> ";
            objTestAlgrorithm.dev1IMO();
        } else if (func_name == "TA_DC_BIAS") {
            msg << "TEST_ALGRORITHM::TA_DC_BIAS >> ";
            objTestAlgrorithm.devDCBias();
        } else if (func_name == "TA_2DGC_Normalize") {
            msg << "TEST_ALGRORITHM::TA_2DGC_Normalize >> ";
            objTestAlgrorithm.norm2DGC();
        } else if (func_name == "TA_GetSpctmDelta") {
            msg << "TEST_ALGRORITHM::TA_GetSpctmDelta >> ";
            objTestAlgrorithm.spctmDelta();
        } else if (func_name == "WG_Wavelength") {
            msg << "WGCutBack::WG_Wavelength >> ";
            objWGCUTBACK.WG_Wavelength();
        } else if (func_name == "WG_Subdie") {
            msg << "WGCutBack::WG_Subdie >> ";
            objWGCUTBACK.WG_Subdie();
        } else if (func_name == "WG_Analysis") {
            msg << "WGCutBack::WG_Analysis >> ";
            objWGCUTBACK.WG_Analysis();
        } else if (func_name == "Analysis") {
            msg << "Spectrum::Analysis >> ";
            objSpectrum.Analysis();
        } else if (func_name == "SIPH_CONFIG") {
            msg << "SIPH::SIPH_CONFIG >> ";
            objSiph.config();
        } else if (func_name == "AM_TUNE_PITCH") {
            msg << "ALIGN_MODULE::AM_TUNE_PITCH >> ";
            objAlignMethod.tune_pitch();
        } else if (func_name == "AM_TUNE_ROLL") {
            msg << "ALIGN_MODULE::AM_TUNE_ROLL >> ";
            objAlignMethod.tune_roll();
        } else if (func_name == "AM_TUNE_YAW") {
            msg << "ALIGN_MODULE::AM_TUNE_YAW >> ";
            objAlignMethod.tune_yaw();
        } else if (func_name == "AM_SET_DEVICE") {
            msg << "ALIGN_MODULE::AM_SET_DEVICE >> ";
            objAlignMethod.set_device();
        } else if (func_name == "AM_SET_HOME") {
            msg << "ALIGN_MODULE::AM_SET_HOME >> ";
            objAlignMethod.set_home();
        } else if (func_name == "REC_POLAR_STATE") {
            msg << "RECORD::REC_POLAR_STATE >> ";
            objRecord.rec_polar_state();
        } else if (func_name == "SET_POLAR_STATE") {
            msg << "RECORD::SET_POLAR_STATE >> ";
            objRecord.set_polar_state();
        } else if (func_name == "REC_WAFER_POSITION") {
            msg << "RECORD::REC_WAFER_POSITION >> ";
            objRecord.rec_wafer_position();
        } else if (func_name == "MOVE_WAFER_POSITION") {
            msg << "RECORD::MOVE_WAFER_POSITION >> ";
            objRecord.move_wafer_position();
        } else if (func_name == "SET_WAFER_POSITION") {
            msg << "RECORD::SET_WAFER_POSITION >> ";
            objRecord.set_wafer_position();
        } else if (func_name == "SET_CENTER_WL") {
            msg << "RECORD::SET_CENTER_WL >> ";
            objRecord.set_center_wl();
        }
        if (kpa::flag::f_debug)
            if (kpa::flag::f_stop_button)
                break;
    }

    // ***  Program end    *** //
    return;
}
