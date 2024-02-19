#include "kpa_main_p.hpp"

#ifndef __KPA_USER_GLOBAL__
#define __KPA_USER_GLOBAL__
namespace user {
    // ***  Program start  *** //

    using namespace kpa::info;
    using namespace kpa::ins;
    using namespace kpa::type;
    using namespace std;

    bool fWaferStart = true; //* Wafer start flag
    int px, py;              //* Die Position (X, Y)
    int item_no = 1;         //* item number
    struct CONF
    {
        double center_wl = 1300.0;
        double laser_pw = 10.0;
        int fiber_height = 460;
    } conf;

    //! ==== Subfunctions ==== !//

#include "libTsmcFunction.cpp"

    std::string t_filename(char *name, const char *key)
    {
        return str_format("%s_%s_%s_X%d_Y%d_%s.csv", name, kpa::info::lotid.c_str(), kpa::info::waferid.c_str(), px, py, key);
    }
    std::string t_filename(std::string name, const char *key)
    {
        return str_format("%s_%s_%s_X%d_Y%d_%s.csv", name.c_str(), kpa::info::lotid.c_str(), kpa::info::waferid.c_str(), px, py, key);
    }

    //! ==== Low Level Hardware Connection ==== !//

#include "libLLDriver.cpp"

    //! ==== Laser Control ==== !//

#include "libCTPX.cpp"
    CTPX laser;

    //! ==== Polarization Synthesizer ==== !//

#include "libEPS1K.cpp" //! Check dll path
    EPS1000 polar;
    PM1K pm;

    //! ==== Photo Switch Control ==== !//

#include "libLTB12_SW.cpp"
    LTB12 lsw;

    //! ==== NI SMU Instructions ==== !//

#include "libNISMU.cpp"
    NISMU smu;

    //! ==== Prober Instructions ==== !//

#include "libCM300.cpp"
    CM300 probe;
// #include "libTS300.cpp"
//     TS300 probe;
#include "Prober_Read.cpp"
    Prober_Read Rd;

    //! ==== Define Functions ==== !//
    /*
    void funcRollAngle()
    {
        kpa::ins::MSG_INDENT __t;
        FORMAT::HW::FUNC_RollAngle set;
        ItemReader_Get_Data((uint8_t *)&set, sizeof(set));

        msg.prefix("Roll Angle (deg) = ") << set.probe_RollAngle;
        probe.hxpod.PitchAngle(set.probe_RollAngle);
    }
    void funcMoveToDie()
    {
        kpa::ins::MSG_INDENT __t;
        FORMAT::HW::FUNC_MoveToDie set;
        ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
        // probe.Separate();
        probe.MoveToDie(set.probe_PosX, set.probe_PosY, set.probe_SubDie);
        // probe.Contact();
    }
    void funcSetMenual()
    {
        kpa::port::panel_mode << 0;       //! Set Menual Mode
        kpa::port::f_stop_button << true; //! Set button stop
    }
*/

    //! Define Common Functions
    class SIPH
    {
    protected:
        struct COUPLE_METHOD
        {
        private:
            void __search_polar(EPS1000::MODE mode)
            {
                polar.feedback(true, mode); //! Search polarization
                Sleep(1000);
                polar.feedback(false);
                Sleep(200);
            }
            void __off()
            {
                msg << "Skip Coupling Search";
            }
            void __search_only()
            {
                msg << "Move to hexpod home position";
                probe.hxpod.MoveHomePosition();
                msg << "Search Coupling";
                probe.hxpod.OpticalAlignment(false);
            }
            void __analog_s()
            {
                msg << "Move to hexpod home position";
                probe.hxpod.MoveHomePosition();
                msg << "Search polarization (with power)";
                __search_polar(polar.analog); //! Search polarization
                msg << "Search coupling for scan spectrum";
                probe.hxpod.OpticalAlignment(false); //! Search coupling for Scan Spectrum
                Sleep(10);
            }
            void __analog()
            {
                msg << "Move to hexpod home position";
                probe.hxpod.MoveHomePosition();
                msg << "Search coupling for polarization";
                probe.hxpod.OpticalAlignment(false); //! Search coupling for polarization
                Sleep(10);
                msg << "Search polarization (with power)";
                __search_polar(polar.analog); //! Search polarization
                msg << "Search coupling for scan spectrum";
                probe.hxpod.OpticalAlignment(false); //! Search coupling for Scan Spectrum
                Sleep(10);
            }
            void __digital()
            {
                msg << "Move to hexpod home position";
                probe.hxpod.MoveHomePosition();
                msg << "Search polarization (with first polarization)";
                __search_polar(polar.digital); //! Search polarization
                msg << "Search coupling for scan spectrum";
                probe.hxpod.OpticalAlignment(false); //! Search coupling for Scan Spectrum
                Sleep(10);
            }
            void (COUPLE_METHOD::*__method)(void) = __off;

        public:
            void setMethod(char *method)
            {
                string _method = msg.prefix("xy_scan_mode    : ") << method;
                if (_method == "polar_analog_scan")
                    __method = __analog;
                // else if (_method == "polar_analog_simple_scan")
                //     __method = __analog_s;
                else if (_method == "polar_digital_scan")
                    __method = __digital;
                else if (_method == "no_polar_scan")
                    __method = __search_only;
                else if (_method == "no_xy_scan")
                    __method = __off;
                else if (_method == "gc_wi_power")
                    __method = __analog;
                // else if (_method == "gc_wi_power_simplify")
                //     __method = __analog_s;
                else if (_method == "gc_wi_polar_set")
                    __method = __digital;
                else if (_method == "gc_only")
                    __method = __search_only;
                else if (_method == "off")
                    __method = __off;
            }
            void run(void)
            {
                // kpa::ins::MSG_INDENT __t;
                (this->*__method)();
            }
        };
        struct SCAN_SPECTRUM
        {
        private:
            void (*fb_enable)(void);
            void (*fb_disable)(void);

        public:
            struct
            {
                double wl_start = 1260.0;
                double wl_stop = 1357.5;
                double wl_step = 0.1;
            } sptm;
            void setScanMethod(char *set_mode)
            {
                string _set = msg.prefix("spectrum_method : ") << set_mode;
                fb_disable = []() { Sleep(1000); polar.feedback(false); Sleep(200); };
                if (_set == "analog_fb") {
                    fb_enable = []() {kpa::ins::MSG_INDENT __t; msg << "Analog FB process"; polar.feedback(true, EPS1000::analog); Sleep(100); };
                } else if (_set == "digital_fb") {
                    fb_enable = []() {kpa::ins::MSG_INDENT __t; msg << "Digital FB process"; polar.feedback(true, EPS1000::digital); Sleep(100); };
                } else if (_set == "highloss_fb") {
                    fb_enable = []() {kpa::ins::MSG_INDENT __t;  msg << "High Loss FB process"; polar.feedback(true, EPS1000::analog_highloss); Sleep(100); };
                } else {
                    fb_enable = []() {};
                    fb_disable = []() {};
                }
            }
            void run()
            {
                msg << "Spectrum scan processing";
                fb_enable();
                probe.probing(probe.contact);
                laser.scan.start(sptm.wl_start).stop(sptm.wl_stop).step(sptm.wl_step).speed(100);
                laser.scan.single_mode().run().wait();
                laser.wavelength = conf.center_wl;
                fb_disable();
            }
        };
    };

    class GOLDEN : public SIPH
    {
    protected:
        CTPX::SENSOR sensor = laser.sensor(2, 2);
        int spectrumStepToSampling(char *step)
        {
            const float std_step = 0.1;
            float inteval;
            sscanf(step, "%fpm", &inteval);
            return int((inteval / std_step) + 0.5);
        }
        CTPX::SENSOR selectModuleChannel(char *w)
        {
            int mod = 0, ch = 0;
            sscanf(w, "(%d,%d)", &mod, &ch);
            return laser.sensor(mod, ch);
        }
    };

    //! Define SIPH_CONFIG_SET
    class CLS_SIPH : GOLDEN
    {
    public:
        void config()
        {
            FORMAT::SIPH::SIPH_CONFIG set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            kpa::ins::MSG_INDENT __t;

            if (conf.center_wl != set.center_wavelength) {
                conf.center_wl = set.center_wavelength;
                laser.wavelength = conf.center_wl;
                msg.prefix("Laser Center Wavelength Change : ").format("%4.1f nm") << conf.center_wl;
            }
            if (conf.laser_pw != set.laser_pw) {
                conf.laser_pw = set.laser_pw;
                laser.power = conf.laser_pw;
                msg.prefix("Laser Power Change             : ").format("%+3.2f db") << conf.laser_pw;
            }
            if (conf.fiber_height != set.fiber_contact_height) {
                conf.fiber_height = set.fiber_contact_height;
                probe.hxpod.fiber_height = conf.fiber_height;
                msg.prefix("Fiber Array Contact Height     : ").format("%d um") << conf.fiber_height;
            }
        }
    };
    CLS_SIPH objSiph;

    //! Define CLS_ALIGN_METHOD
    class CLS_ALIGN_METHOD : public GOLDEN
    {
    private:
        struct SWEEP_LOG
        {
            struct PT
            {
                double angle, pw;
            };
            vector<double> ang, pw;
            void save(const string &filename)
            {
                ofstream f_log;
                string f_path = str_format("%s\\%s", getDataFolder().c_str(), filename.c_str());
                f_path = checkFilePath(f_path);
                f_log.open(f_path.c_str(), ios::out);
                f_log << "Angle(deg), IL_Center(db)\n";
                for (int l = 0; l < (int)ang.size(); l++) {
                    f_log << str_format("%+4.2f, %+4.2f\n", ang[l], pw[l]);
                }
                f_log.close();
            }
            PT max()
            {
                PT out;
                auto max_idx = std::max_element(pw.begin(), pw.end()) - pw.begin();
                out.angle = ang[max_idx];
                out.pw = pw[max_idx];
                return out;
            }
        };
        string __device;
        struct DEFAULT_ANGLE
        {
            double u = 0., v = 0., w = 0.;
        } __def;
        COUPLE_METHOD couple_method;
        SCAN_SPECTRUM scan_spectrum;

        template <typename SET>
        void __tune_angle_temp()
        {
            SET set;
            struct
            {
                const char *title;
                void (*set_angle)(double) = [](double ang) -> void {};
                const char *label_form;
                double *p_angle;
                const char *file_postfix;
                double center_angle;
            } a;
            if constexpr (std::is_same<SET, FORMAT::ALIGN_MODULE::AM_TUNE_YAW>::value) {
                a.title = "Yaw Angle Set = ";
                a.set_angle = [](double ang) -> void { probe.hxpod.YawAngle(ang); };
                a.label_form = "Y%+2.2f";
                a.p_angle = &probe.hxpod.angle.u;
                a.file_postfix = "_YA";
                a.center_angle = __def.u;
            }
            if constexpr (std::is_same<SET, FORMAT::ALIGN_MODULE::AM_TUNE_PITCH>::value) {
                a.title = "Pitch Angle Set = ";
                a.set_angle = [](double ang) -> void { probe.hxpod.PitchAngle(ang); };
                a.label_form = "P%+2.2f";
                a.p_angle = &probe.hxpod.angle.v;
                a.file_postfix = "_PA";
                a.center_angle = __def.v;
            }
            if constexpr (std::is_same<SET, FORMAT::ALIGN_MODULE::AM_TUNE_ROLL>::value) {
                a.title = "Roll Angle Set = ";
                a.set_angle = [](double ang) -> void { probe.hxpod.RollAngle(ang); };
                a.label_form = "R%+2.2f";
                a.p_angle = &probe.hxpod.angle.w;
                a.file_postfix = "_RA";
                a.center_angle = __def.w;
            }
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            kpa::ins::MSG_INDENT ___t;
            SEG_TIMER _t;
            MULTI_SPECTRUM spctms;
            SWEEP_LOG sweep_log;
            couple_method.setMethod(set.couple_method);
            scan_spectrum.setScanMethod(set.scan_method);
            spctms.samp = spectrumStepToSampling(set.spectrum_step);

            auto sub_proc = [&](double angle) {
                msg.prefix(a.title).format("%+2.2f") << angle; //! a.title = "Pitch Angle Set = "
                kpa::ins::MSG_INDENT __t;
                sweep_log.ang.push_back(angle);
                probe.probing(probe.semi_cont);
                a.set_angle(angle); //! probe.hxpod.PitchAngle(angle);
                probe.probing(probe.contact);
                Sleep(5);
                couple_method.run();
                msg.prefix("Time of Search Coupling: ").unit("sec") << _t.seg();
                Sleep(5);
                sweep_log.pw.push_back(sensor.power());

                if (set.save_spectrum) {
                    scan_spectrum.run();
                    msg.prefix("Time of Scan Spectrum: ").unit("sec") << _t.seg();

                    msg << "Receive scan data";
                    auto &trace = sensor.trace;
                    spctms.start = trace.start();
                    spctms.step = trace.step();

                    spctms.add(str_format(a.label_form, angle), trace.data()); //! a.label_form = "P%+2.2f"
                    auto peak = spctms[-1].peak();
                    auto cent = spctms[-1].cent(conf.center_wl);
                    msg.prefix("Peak Wave Length : ").unit("nm").format("%4.4f") << peak.wl;
                    msg.prefix("Peak Power       : ").unit("db") << peak.pw;
                    msg.prefix(str_format("Power in %4.0fnm  : ", conf.center_wl).c_str()).unit("db") << cent.pw;
                    msg.prefix("Time of Data Transfer: ").unit("sec") << _t.seg();
                }
            };
            probe.hxpod.ReadProbeAngle();
            double ang_origin = *a.p_angle; //! probe.hxpod.angle.v
            double ang = set.ang_start + a.center_angle;
            double d = (set.ang_end >= set.ang_start) ? (set.ang_step) : (-set.ang_step);
            int loop = int(((set.ang_end - set.ang_start) / set.ang_step) + 0.5) + 1;
            for (int i = 0; i < loop; i++) {
                sub_proc(ang);
                ang += d;
                if (kpa::flag::f_debug)
                    if (kpa::flag::f_stop_button)
                        break;
            }
            string end_pos(set.end_position);
            if (end_pos == "IL_MIN") {
                auto p = sweep_log.max();
                a.set_angle(p.angle); //! probe.hxpod.PitchAngle(p.angle);
            } else if (end_pos == "home") {
                a.set_angle(a.center_angle); //! probe.hxpod.PitchAngle(a.center_angle);
            } else {
                a.set_angle(ang_origin); //! probe.hxpod.PitchAngle(ang_origin);
            }
            if (set.save_spectrum)
                spctms.save(t_filename((string(date_stamp()) + proj_name.c_str()), set.file_name).c_str());
            string f_spec = __device + a.file_postfix; //! a.file_postfix = "_PA"
            sweep_log.save(t_filename((string(date_stamp()) + proj_name.c_str()), f_spec.c_str()).c_str());
            msg.prefix("Time of Data Save: ").unit("sec") << _t.seg();
            msg.prefix("Time of 1DGC processing: ").unit("sec") << _t.life();
        }

    public:
        void set_home()
        {
            FORMAT::ALIGN_MODULE::AM_SET_HOME set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            kpa::ins::MSG_INDENT __t;

            // __def.u = set.yaw_u;
            // msg.prefix("Yaw   (u) : ").format("%+2.2f deg") << __def.u;
            __def.v = set.pitch_v;
            probe.hxpod.PitchAngle(__def.v);
            msg.prefix("Pitch (v) : ").format("%+2.2f deg") << __def.v;
            __def.w = set.roll_w;
            probe.hxpod.RollAngle(__def.w);
            msg.prefix("Roll  (w) : ").format("%+2.2f deg") << __def.w;
        }
        void set_device()
        {
            FORMAT::ALIGN_MODULE::AM_SET_DEVICE set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            kpa::ins::MSG_INDENT ___t;

            __device = set.device;
            msg.prefix("Set Device     : ") << set.device;
            probe.MoveToSubDie(set.device_num);
            msg.prefix("Probing Subdie : ") << set.device_num;
            sensor = selectModuleChannel(set.device_pitch);
            laser.switch_ref(sensor);
            msg.prefix("Device pitch   : ") << set.device_pitch;
        }
        void tune_yaw()
        {
            __tune_angle_temp<FORMAT::ALIGN_MODULE::AM_TUNE_YAW>();
        }
        void tune_pitch()
        {
            __tune_angle_temp<FORMAT::ALIGN_MODULE::AM_TUNE_PITCH>();
        }
        void tune_roll()
        {
            __tune_angle_temp<FORMAT::ALIGN_MODULE::AM_TUNE_ROLL>();
        }
    };
    CLS_ALIGN_METHOD objAlignMethod;

    //! Define CLS_TEST_ALGRORITHM
    class CLS_TEST_ALGRORITHM : public GOLDEN
    {
    private:
        COUPLE_METHOD couple_method;
        SCAN_SPECTRUM scan_spectrum;
        SPECTRUM spctm[9];
        SPECTRUM __spctm;
        MULTI_SPECTRUM _spctms;
        struct
        {
            MULTI_SPECTRUM te;
            MULTI_SPECTRUM tm;
        } _2dgc_buf;
        struct IV_LOG
        {
            struct UNIT
            {
                double v, i;
            };
            vector<UNIT> d;
            void add(double v, double i)
            {
                d.push_back({v, i});
            }
            void clear()
            {
                d.clear();
                d.resize(0);
            }
            void save(const string &file_name)
            {
                ofstream f_log;
                // auto tmp3 = str_format("%s_%s", set.dev_name, "IV");
                auto _path = str_format("%s\\%s", getDataFolder().c_str(), t_filename((string(date_stamp()) + proj_name.c_str()), file_name.c_str()).c_str());
                f_log.open(_path.c_str(), ios::out);
                f_log << "Volt(V), Curr(A)\n";
                for (int l = 0; l < (int)d.size(); l++)
                    f_log << str_format("%3.03f, %6.06g\n", d[l].v, d[l].i);
                f_log.close();
            }
        };

        template <typename E>
        E vector_average(const vector<E> &data)
        {
            E sum = 0., avg = 0.;
            for (auto &e : data)
                sum += e;
            avg = (sum / data.size());
            return avg;
        };
        MULTI_SPECTRUM *subMultiSpctmBufSelect(char *w)
        {
            string s(w);
            if (s == "None")
                return &_spctms;
            if (s == "2DGC_TE")
                return &_2dgc_buf.te;
            if (s == "2DGC_TM")
                return &_2dgc_buf.tm;
            return &_spctms;
        }
        SPECTRUM *subSpctmBufSelect(char *w)
        {
            int ch;
            if (string(w) == "---")
                return &__spctm;
            sscanf(w, "spectrum%d", &ch);
            ch--;
            if ((ch >= 0) | (ch <= 8))
                return &spctm[ch];
            else
                return &__spctm;
        }

    public:
        void dev1DGC()
        {
            FORMAT::TEST_ALGRORITHM::TA_1DGC set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            kpa::ins::MSG_INDENT ___t;
            SEG_TIMER _t;
            couple_method.setMethod(set.couple_method);
            scan_spectrum.setScanMethod(set.scan_method);
            SPECTRUM *spctm = subSpctmBufSelect(set.store_to_buffer);
            spctm->samp = spectrumStepToSampling(set.spectrum_step);

            msg.prefix("Set Device     : ") << set.device;
            if (set.device_num >= 0) {
                probe.MoveToSubDie(set.device_num);
                msg.prefix("Probing Subdie : ") << set.device_num;
            }
            sensor = selectModuleChannel(set.sensor);
            msg.prefix("Power Sensor   : ") << set.sensor;
            laser.switch_ref(sensor);
            Sleep(5);

            couple_method.run();
            msg.prefix("Time of Search Coupling: ").unit("sec") << _t.seg();
            scan_spectrum.run();
            msg.prefix("Time of Scan Spectrum: ").unit("sec") << _t.seg();

            msg << "Receive scan data";
            auto &trace = sensor.trace;
            spctm->data = trace.data();
            spctm->start = trace.start();
            spctm->step = trace.step();

            auto peak = spctm->peak();
            msg.prefix("Peak Wave Length          : ").unit("nm").format("%4.4f") << peak.wl;
            msg.prefix("Peak Power                : ").unit("db").format("%+4.2f") << peak.pw;
            auto cent = spctm->cent(conf.center_wl);
            msg.prefix(str_format("Power in %4.0fnm           : ", cent.wl).c_str()).unit("db") << cent.pw;
            if (set.save_spectrum) {
                spctm->save(t_filename((string(date_stamp()) + proj_name.c_str()), set.device).c_str());
                msg.prefix("Time of Data Transfer & Save: ").unit("sec") << _t.seg();
            }

            if (set.device_num >= 0) {
                kpa::ins::log(item_no++, str_format("%s_PEAK_WL", set.device).c_str(), vector({peak.wl}), "%4.4f", "nm");
                kpa::ins::log(item_no++, str_format("%s_PEAK_PW", set.device).c_str(), vector({peak.pw}), "%2.2f", "db");
                kpa::ins::log(item_no++, str_format("%s_%#.0f_PW", set.device, conf.center_wl).c_str(), vector({cent.pw}), "%2.2f", "db");
                if (set.meas_1db_bandwidth) {
                    double bw = spctm->bandwidth(1.0f);
                    kpa::ins::log(item_no++, str_format("%s_1db_BW", set.device).c_str(), vector({bw}), "%4.4f", "nm");
                }
            }
            // if (set.save_spectrum) {
            //     auto tmp = spctm->segment((conf.center_wl - 0.5) * 1nm, (conf.center_wl + 0.5) * 1nm);
            //     double avg = vector_average(tmp.data);
            //     msg.prefix("Center Power +/-0.5nm avg   : ").unit("db").format("%+4.2f") << avg;
            //     if (set.device_num >= 0)
            //         kpa::ins::log(item_no++, str_format("%s_%#.0f_AVG_PW", set.device, conf.center_wl).c_str(), vector({avg}), "%2.2f", "db");
            // }

            msg.prefix("Time of 1DGC processing: ").unit("sec") << _t.life();
        }
        void dev2DGC()
        {
            FORMAT::TEST_ALGRORITHM::TA_2DGC set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            kpa::ins::MSG_INDENT ___t;
            SEG_TIMER _t;
            MULTI_SPECTRUM *spctms = subMultiSpctmBufSelect(set.store_to_buf);
            spctms->clear();

            couple_method.setMethod(set.couple_method);
            scan_spectrum.setScanMethod(set.scan_method);
            spctms->samp = spectrumStepToSampling(set.spectrum_step);

            msg.prefix("Set Device     : ") << set.device;
            if (set.device_num >= 0) {
                msg.prefix("Probing Subdie : ") << set.device_num;
                probe.MoveToSubDie(set.device_num);
            }
            msg.prefix("Sensor1        : ") << set.sensor1;
            auto sensor1 = selectModuleChannel(set.sensor1);
            msg.prefix("Sensor2        : ") << set.sensor2;
            auto sensor2 = selectModuleChannel(set.sensor2);

            //! Alignment with 'ONE' output refernce
            // msg.prefix("Switch ref to  : ") << set.sensor1_ref;
            // laser.switch_ref(sensor1);
            // Sleep(5);
            // couple_method.run();
            // msg.prefix("Time of Search Coupling: ").unit("sec") << _t.seg();

            //! Alignment with 'TWO' output refernces
            auto readPzt = []() -> vector<double> {
                probe.hxpod.ReadProbePos();
                vector<double> pos(2, 0.0);
                pos[0] = probe.hxpod.pzt.x;
                pos[1] = probe.hxpod.pzt.y;
                return pos;
            };
            auto subAlignProc = [&](char *sensor_name, CTPX::SENSOR &sensor) -> vector<double> {
                msg.prefix("Switch ref to  : ") << sensor_name;
                laser.switch_ref(sensor);
                Sleep(5);
                couple_method.run();
                vector<double> pos = readPzt();
                msg.prefix("Alignment Pos. : ").format("%5.2f um") << pos;
                return pos;
            };
            auto pos1 = subAlignProc(set.sensor1, sensor1);
            auto pos2 = subAlignProc(set.sensor2, sensor2);
            vector<double> pos_avg(2, 0.0);
            for (int i = 0; i < 2; i++)
                pos_avg[i] = (pos1[i] + pos2[i]) / 2;
            msg.prefix("Average Pos.   : ").format("%5.2f um") << pos_avg;
            probe.hxpod.MovePztXY(pos_avg[0], pos_avg[1]);
            msg.prefix("Checl Pos.     : ").format("%5.2f um") << readPzt();
            msg.prefix("Time of Search Coupling: ").unit("sec") << _t.seg();

            //! Scan Spectrum
            scan_spectrum.run();
            msg.prefix("Time of Scan Spectrum: ").unit("sec") << _t.seg();

            msg << "Receive scan data";
            auto &trace1 = sensor1.trace;
            auto &trace2 = sensor2.trace;
            spctms->start = trace1.start();
            spctms->step = trace1.step();
            spctms->add(set.name_of_sens1, trace1.data());
            spctms->add(set.name_of_sens2, trace2.data());

            auto seg_spctm = spctms->segment((conf.center_wl - 0.5) * 1nm, (conf.center_wl + 0.5) * 1nm);
            auto findSpecific = [&](int ch, const char *name) {
                auto peak = (*spctms)[ch].peak();
                msg.prefix("Peak Wave Length          : ").unit("nm").format("%4.4f") << peak.wl;
                msg.prefix("Peak Power                : ").unit("db").format("%+4.2f") << peak.pw;
                auto cent = (*spctms)[ch].cent(conf.center_wl);
                msg.prefix(str_format("Power in %4.0fnm           : ", cent.wl).c_str()).unit("db") << cent.pw;
                if (set.device_num >= 0) {
                    kpa::ins::log(item_no++, str_format("%s_%s_PEAK_WL", set.device, name).c_str(), vector({peak.wl}), "%4.4f", "nm");
                    kpa::ins::log(item_no++, str_format("%s_%s_PEAK_PW", set.device, name).c_str(), vector({peak.pw}), "%2.2f", "db");
                    kpa::ins::log(item_no++, str_format("%s_%s_%#.0f_PW", set.device, name, conf.center_wl).c_str(), vector({cent.pw}), "%2.2f", "db");
                    // double avg = vector_average(seg_spctm[ch].pdb->data);
                    // kpa::ins::log(item_no++, str_format("%s_%s_%#.0f_AVG_PW", set.device, name, conf.center_wl).c_str(), vector({avg}), "%2.2f", "db");
                }
            };
            findSpecific(0, set.name_of_sens1);
            findSpecific(1, set.name_of_sens2);

            if (set.save_spectrum) {
                spctms->save(t_filename((string(date_stamp()) + proj_name.c_str()), set.device).c_str());
                msg.prefix("Time of Data Transfer & Save: ").unit("sec") << _t.seg();
            }

            msg.prefix("Time of 2DGC processing: ").unit("sec") << _t.life();
        }
        void dev1IMO()
        {
            FORMAT::TEST_ALGRORITHM::TA_1IMO set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            kpa::ins::MSG_INDENT ___t;
            SEG_TIMER _t;
            couple_method.setMethod(set.couple_method);
            scan_spectrum.setScanMethod(set.scan_method);
            MULTI_SPECTRUM spctms;
            spctms.samp = spectrumStepToSampling(set.spectrum_step);

            msg.prefix("Set Device     : ") << set.device;
            if (set.device_num >= 0) {
                probe.MoveToSubDie(set.device_num);
                msg.prefix("Probing Subdie : ") << set.device_num;
            }
            msg.prefix("Reference Sensor   : ") << set.ref_sensor;
            laser.switch_ref(selectModuleChannel(set.ref_sensor));
            Sleep(5);

            couple_method.run();
            msg.prefix("Time of Search Coupling: ").unit("sec") << _t.seg();
            scan_spectrum.run();
            msg.prefix("Time of Scan Spectrum: ").unit("sec") << _t.seg();

            auto receiveSpctmData = [&](char *sens, char *col) {
                if (string(sens) == "---")
                    return;
                msg << str_format("Receive data from sensor %s", sens);
                kpa::ins::MSG_INDENT ___t;
                auto s = selectModuleChannel(sens);
                spctms.start = s.trace.start();
                spctms.step = s.trace.step();
                spctms.add(((col[0] == '\0') ? sens : col), s.trace.data());
                auto peak = spctms[-1].peak();
                msg.prefix("Peak Wave Length          : ").unit("nm").format("%4.4f") << peak.wl;
                msg.prefix("Peak Power                : ").unit("db").format("%+4.2f") << peak.pw;
                auto cent = spctms[-1].cent(conf.center_wl);
                msg.prefix(str_format("Power in %4.0fnm           : ", cent.wl).c_str()).unit("db") << cent.pw;
            };
            if (set.save_spectrum) {
                receiveSpctmData(set.sensor1, set.column1);
                receiveSpctmData(set.sensor2, set.column2);
                receiveSpctmData(set.sensor3, set.column3);
                receiveSpctmData(set.sensor4, set.column4);
                receiveSpctmData(set.sensor5, set.column5);
                receiveSpctmData(set.sensor6, set.column6);
                receiveSpctmData(set.sensor7, set.column7);
                receiveSpctmData(set.sensor8, set.column8);
                spctm->save(t_filename((string(date_stamp()) + proj_name.c_str()), set.device).c_str());
                msg.prefix("Time of Data Transfer & Save: ").unit("sec") << _t.seg();
            }

            msg.prefix("Time of 1DGC processing: ").unit("sec") << _t.life();
        }
        void devDCBias()
        {
            const int module_num = 11;
            FORMAT::TEST_ALGRORITHM::TA_DC_BIAS set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            IV_LOG iv_log;
            kpa::ins::MSG_INDENT ___t;
            SEG_TIMER _t;
            couple_method.setMethod(set.couple_method);
            scan_spectrum.setScanMethod(set.scan_method);
            MULTI_SPECTRUM *spctms = &_spctms;
            spctms->clear();
            spctms->samp = spectrumStepToSampling(set.spectrum_step);
            smu.pin1.ForceV(0V, 10mA, 5V, 10mA);
            smu.pin1.Enable();

            msg.prefix("Set Device     : ") << set.device_name;
            if (set.device_num >= 0) {
                probe.MoveToSubDie(set.device_num);
                msg.prefix("Probing Subdie : ") << set.device_num;
            }
            sensor = selectModuleChannel(set.sensor);
            msg.prefix("Power Sensor   : ") << set.sensor;
            laser.switch_ref(sensor);
            Sleep(5);

            couple_method.run();
            msg.prefix("Time of Search Coupling: ").unit("sec") << _t.seg();

            auto getSpectrumOnes = [&](double v_bias) {
                scan_spectrum.run();
                msg.prefix("Time of Scan Spectrum: ").unit("sec") << _t.seg();

                msg << "Receive scan data";
                auto &trace = sensor.trace;
                spctms->start = trace.start();
                spctms->step = trace.step();
                auto buf = trace.data();
                spctms->add(str_format("V%+1.2f", v_bias), buf);

                auto peak = (*spctms)[-1].peak();
                msg.prefix("Peak Wave Length          : ").unit("nm").format("%4.4f") << peak.wl;
                msg.prefix("Peak Power                : ").unit("db").format("%+4.2f") << peak.pw;
                auto cent = spctm->cent(conf.center_wl);
                msg.prefix(str_format("Power in %4.0fnm           : ", cent.wl).c_str()).unit("db") << cent.pw;
            };
            auto gndLimit = [&](double i_limit, double t_limit) {
                SEG_TIMER _buf_t;
                for (int i = 0; i < 1000; i++) {
                    if (_buf_t.life() >= t_limit) {
                        msg << "GND wait over %#.2fsec";
                        return;
                    }
                    smu.pin1.Measure();
                    auto di = smu.pin1.mi[0];
                    if (abs(di) < i_limit) {
                        msg << str_format("DUT current (%5.02f) < %5.02fpA", (double)(di / 1pA), (double)(i_limit / 1pA));
                        return;
                    }
                    Sleep(100);
                }
            };
            double bias = set.bias_start;
            double d = (set.bias_end >= set.bias_start) ? (set.bias_step) : (-set.bias_step);
            int loop = int(((set.bias_end - set.bias_start) / set.bias_step) + 0.5) + 1;
            for (int i = 0; i < loop; i++) {
                msg.prefix("DC_BIAS = ").unit("V").format("%+3.2f") << bias;
                // if (i != 0)
                //     Sleep(3000); //! Wait for GND voltage stable
                smu.pin1.VLevel(bias);
                Sleep(3000);
                smu.pin1.Measure();
                iv_log.add(smu.pin1.mv[0], smu.pin1.mi[0]);
                msg.prefix("Volt").unit("V").format("%.02f") << smu.pin1.mv;
                msg.prefix("Curr").unit("pA").format("%.02f") << smu.pin1.mi;
                // kpa::ins::log(item_no++, "Volt", smu.pin1.mv, "%5.3f", "V");
                // kpa::ins::log(item_no++, "Curr", smu.pin1.mi, "%5.3f", "pA");
                if (set.save_spectrum)
                    getSpectrumOnes(bias);
                smu.pin1.VLevel(0V);
                gndLimit(50pA, 10S);
                bias += d;
                if (kpa::flag::f_debug)
                    if (kpa::flag::f_stop_button)
                        break;
            }

            iv_log.save(string(set.device_name) + "_IV");
            if (set.save_spectrum) {
                spctm->save(t_filename((string(date_stamp()) + proj_name.c_str()), set.device_name).c_str());
                msg.prefix("Time of Data Transfer & Save: ").unit("sec") << _t.seg();
            }
            msg.prefix("Time of 1DGC processing: ").unit("sec") << _t.life();
        }
        void norm2DGC()
        {
            FORMAT::TEST_ALGRORITHM::TA_2DGC_Normalize set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            kpa::ins::MSG_INDENT ___t;
            MULTI_SPECTRUM spctms;

            if (_2dgc_buf.te.size() == 0) {
                msg << "2DGC_TE buffer is vacuum, process stop!";
                return;
            }
            if (_2dgc_buf.tm.size() == 0) {
                msg << "2DGC_TM buffer is vacuum, process stop!";
                return;
            }

            spctms.start = _2dgc_buf.te.start;
            spctms.step = _2dgc_buf.te.step;
            spctms.samp = spectrumStepToSampling(set.spectrum_step);

            if (set.save_source_data) {
                auto subCopyData = [&](auto unit, const char *post) { spctms.add((unit.pdb->title + post), unit.pdb->data); };
                subCopyData(_2dgc_buf.te[0], "TE1");
                subCopyData(_2dgc_buf.te[1], "TE2");
                subCopyData(_2dgc_buf.tm[0], "TM1");
                subCopyData(_2dgc_buf.tm[1], "TM2");
            }
            auto to_watt = [](float db) -> float { return std::pow(10.f, db * 0.1f); };
            auto to_db = [](float watt) -> float { return 10.f * std::log10(watt); };
            auto calc = [&](float &te1, float &te2, float &tm1, float &tm2, float *tes, float *tms) {
                float te = to_db(to_watt(te1) + to_watt(te2));
                float tm = to_db(to_watt(tm1) + to_watt(tm2));
                *tes = (3.f * te - tm) / 4.f;
                *tms = (3.f * tm - te) / 4.f;
            };
            auto &te1 = _2dgc_buf.te[0].pdb->data, &te2 = _2dgc_buf.te[1].pdb->data;
            auto &tm1 = _2dgc_buf.tm[0].pdb->data, &tm2 = _2dgc_buf.tm[1].pdb->data;
            size_t len = te1.size();
            vector<float> tes(len, 0.), tms(len, 0.);
            for (int i = 0; i < (int)len; i++)
                calc(te1[i], te2[i], tm1[i], tm2[i], &tes[i], &tms[i]);
            spctms.add("TES", tes);
            spctms.add("TMS", tms);

            // auto seg_spctm = spctms.segment((conf.center_wl - 0.5) * 1nm, (conf.center_wl + 0.5) * 1nm);
            auto findSpecific = [&](int idx_d, const char *name) {
                auto peak = spctms[idx_d].peak();
                msg.prefix("Peak Wave Length          : ").unit("nm").format("%4.4f") << peak.wl;
                msg.prefix("Peak Power                : ").unit("db").format("%+4.2f") << peak.pw;
                auto cent = spctms[idx_d].cent(conf.center_wl);
                msg.prefix(str_format("Power in %4.0fnm           : ", cent.wl).c_str()).unit("db") << cent.pw;
                kpa::ins::log(item_no++, str_format("%s_%s_PEAK_WL", set.name, name).c_str(), vector({peak.wl}), "%4.4f", "nm");
                kpa::ins::log(item_no++, str_format("%s_%s_PEAK_PW", set.name, name).c_str(), vector({peak.pw}), "%2.2f", "db");
                kpa::ins::log(item_no++, str_format("%s_%s_%#.0f_PW", set.name, name, conf.center_wl).c_str(), vector({cent.pw}), "%2.2f", "db");
                if (set.meas_1db_bandwidth) {
                    double bw = spctms[idx_d].bandwidth(1.0f);
                    kpa::ins::log(item_no++, str_format("%s_1db_BW", set.name).c_str(), vector({bw}), "%4.4f", "nm");
                }
                // double avg = vector_average(seg_spctm[idx_d].pdb->data);
                // kpa::ins::log(item_no++, str_format("%s_%s_%#.0f_AVG_PW", set.name, name, conf.center_wl).c_str(), vector({avg}), "%2.2f", "db");
            };
            int idx_d = (set.save_source_data) ? 4 : 0;
            findSpecific(idx_d + 0, "TES");
            findSpecific(idx_d + 1, "TMS");

            if (set.save_spectrum) {
                spctms.save(t_filename((string(date_stamp()) + proj_name.c_str()), set.name).c_str());
                // msg.prefix("Time of Data Transfer & Save: ").unit("sec") << _t.seg();
            }

            if (set.range_limit_search) {
                auto limit_search = [&](MULTI_SPECTRUM::SPEC_UNIT ___spctms, const char *desc) {
                    double value = 0.;
                    auto &pdb = ___spctms.pdb;
                    if (set.IL_MAX) {
                        value = *std::min_element(pdb->data.begin(), pdb->data.end());
                        string item_name = str_format("%s_%s_%s_ILMAX", set.name, pdb->title.c_str(), desc);
                        kpa::ins::log(item_no++, item_name.c_str(), vector({value}), "%+2.3f", "db");
                    }
                    if (set.IL_MIN) {
                        value = *std::max_element(pdb->data.begin(), pdb->data.end());
                        string item_name = str_format("%s_%s_%s_ILMIN", set.name, pdb->title.c_str(), desc);
                        kpa::ins::log(item_no++, item_name.c_str(), vector({value}), "%+2.3f", "db");
                    }
                };
                auto spsg = spctms.segment(set.wl_lower, set.wl_upper);
                limit_search(spsg[idx_d + 0], "TES");
                limit_search(spsg[idx_d + 1], "TMS");
                // auto tesg = _2dgc_buf.te.segment(set.wl_lower, set.wl_upper);
                // limit_search(tesg[0], "TE1");
                // limit_search(tesg[1], "TE2");
                // auto tmsg = _2dgc_buf.tm.segment(set.wl_lower, set.wl_upper);
                // limit_search(tmsg[0], "TM1");
                // limit_search(tmsg[1], "TM2");
            }
        }
        void spctmDelta()
        {
            FORMAT::TEST_ALGRORITHM::TA_GetSpctmDelta set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            kpa::ins::MSG_INDENT ___t;
            SPECTRUM *spctm1 = subSpctmBufSelect(set.spctm1);
            SPECTRUM *spctm2 = subSpctmBufSelect(set.spctm2);
            SPECTRUM buf;
            auto spctmCompare = [](SPECTRUM *spctm1, SPECTRUM *spctm2) -> bool {
                if (spctm1->start != spctm2->start)
                    return false;
                if (spctm1->step != spctm2->step)
                    return false;
                if (spctm1->data.size() != spctm2->data.size())
                    return false;
                return true;
            };
            auto spctmReportTargetWL = [&](double wl) {
                if (wl < 1260.)
                    return;
                auto tar = buf.cent(wl);
                msg << str_format("Delta of %4.1fnm : %+2.5f db", tar.wl, tar.pw);
                kpa::ins::log(item_no++, str_format("%s_%#.0f_PW", set.log_name, tar.wl).c_str(), vector({tar.pw}), "%2.3f", "db");
            };

            if (!spctmCompare(spctm1, spctm2))
                return;
            buf.start = spctm1->start;
            buf.step = spctm1->step;
            buf.data.resize(spctm1->data.size());
            for (int i = 0; i < (int)buf.data.size(); i++)
                buf.data[i] = spctm1->data[i] - spctm2->data[i];
            spctmReportTargetWL(set.get_wavelength_1);
            spctmReportTargetWL(set.get_wavelength_2);
            spctmReportTargetWL(set.get_wavelength_3);
            spctmReportTargetWL(set.get_wavelength_4);
            spctmReportTargetWL(set.get_wavelength_5);
        }
    };
    CLS_TEST_ALGRORITHM objTestAlgrorithm;

    //! ==== WG_Cut_Back ==== !//
    class WGCUTBACK
    {
    private:
        struct WG_UNIT
        {
            double wavelength;
            vector<double> il_pw;
            vector<double> S_il_pw; // smooth
        };
        vector<WG_UNIT> db; // need to clear memory
        vector<double> X;   // need to clear memory

        vector<double> slope_; // need to clear memory
        vector<double> RSQ;    // need to clear memory
        vector<string> subdie_;
        vector<string> cond_;
        vector<double> slope_mean;
        vector<double> RSQ_mean;

        void linearFit(const vector<double> &x_values, const vector<double> &y_values, double &slope, double &RSQ)
        {
            if (x_values.size() != y_values.size() || x_values.size() == 0) {
                std::cerr << "Error: Invalid input data!" << std::endl;
                return;
            }

            int n = x_values.size();
            double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x_squared = 0;
            // print
            msg.prefix(" x = ") << x_values;
            msg.prefix(" y = ") << y_values;

            for (int i = 0; i < n; ++i) {
                sum_x += x_values[i];
                sum_y += y_values[i];
                sum_xy += x_values[i] * y_values[i];
                sum_x_squared += x_values[i] * x_values[i];
            }

            double x_mean = sum_x / n;
            double y_mean = sum_y / n;

            // double b = y_mean - a * x_mean;                                                // intercept

            // 计算协方差
            double covariance = 0;
            for (int i = 0; i < n; ++i) {
                covariance += (x_values[i] - x_mean) * (y_values[i] - y_mean);
            }
            // 计算标准差
            double stdDevX = 0;
            double stdDevY = 0;
            for (int i = 0; i < n; ++i) {
                stdDevX += std::pow(x_values[i] - x_mean, 2);
                stdDevY += std::pow(y_values[i] - y_mean, 2);
            }
            stdDevX = std::sqrt(stdDevX);
            stdDevY = std::sqrt(stdDevY);
            double correlation = covariance / (stdDevX * stdDevY);
            slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x_squared - sum_x * sum_x); // slope
            RSQ = covariance / (stdDevX * stdDevY);                                     // R
        };
        vector<double> cal_Predictions(const vector<double> &x_values, double slope, double intercept)
        {
            // 计算线性回归预测值
            vector<double> predictions;
            for (double value : x_values) {
                double predictedY = slope * value + intercept;
                predictions.push_back(predictedY);
            }
            return predictions;
        }
        double cal_Error(const vector<double> &actual, const vector<double> &predicted)
        {
            // 计算误差（残差）
            double sum_squared_error = 0;
            for (size_t i = 0; i < actual.size(); ++i) {
                double error = actual[i] - predicted[i];
                sum_squared_error += pow(error, 2);
            }
            return sqrt(sum_squared_error / actual.size());
        };

    public:
        int log_num;
        int mv_point;

        SPECTRUM spctm_; // for readfile test

        void addWGPoint(std::string dev_name, double ref_x)
        {
            LV_SPECTRUM lv_spctm;
            string *tar = NULL;
            for (auto &p : Rd.Path) {

                if (p.find(dev_name + ".") != string::npos)
                    tar = &p;
            }
            if (tar == NULL)
                return;
            msg.prefix("dev_name") << dev_name;
            lv_spctm.CreatPath(*tar);
            lv_spctm.modeRaw();
            for (auto &u : db)
                u.il_pw.push_back(lv_spctm.findWavelength(0, u.wavelength));
            lv_spctm.modeMovingAverage(mv_point);
            for (auto &u : db)
                u.S_il_pw.push_back(lv_spctm.findWavelength(0, u.wavelength));
            X.push_back(ref_x);
        }
        void addWGUnit(double wl)
        {
            WG_UNIT db_temp;
            if (wl != -1) {
                db.push_back(WG_UNIT({wl}));
            }
        }
        void addSubdie(char *device_name)
        {
            if (strcmp(device_name, (char *)"NAME") != 0) {
                subdie_.push_back(device_name);
            }
        }
        void addSubdieComb(char *device_name, char *cond)
        {
            string subdie_comb;
            if (strcmp(device_name, (char *)"NAME") != 0 && strcmp(cond, (char *)"NAME") != 0) {
                subdie_comb = std::string(device_name) + std::string("_") + std::string(cond);
                subdie_.push_back(subdie_comb);
            }
        }
        void Get_loss(SPECTRUM &spctmtemp)
        {
            if (db.size() == 0)
                return;
            for (int i = 0; i < (int)db.size(); i++) {
                db[i].il_pw.push_back(spctmtemp.idxWaveLength(db[i].wavelength * (1E-9)));
            };
        };
        void report(int *num_p, bool smooth, std::string item_name)
        {
            if (smooth == false || (smooth == true && mv_point > 0)) {
                auto mode = [](bool smooth) -> const char * { return (smooth) ? "S" : "R"; };
                kpa::ins::log((*num_p)++, str_format("%s_%s_slope", mode(smooth), item_name.c_str()).c_str(), slope_mean, "%.4f", "");
                kpa::ins::log((*num_p)++, str_format("%s_%s_RSQ", mode(smooth), item_name.c_str()).c_str(), RSQ_mean, "%.4f", "");
            }
        };
        void slope(bool smooth)
        {
            // linear fit
            double ss = 0;
            double ee = 0;
            slope_.clear();
            slope_mean.clear();
            RSQ.clear();
            RSQ_mean.clear();
            msg.prefix("X") << X;
            msg.prefix("db[0].il_pw") << db[0].il_pw;
            for (int i = 0; i < (int)db.size(); ++i) {
                double S;
                double R;
                msg.prefix("<wavelength>") << db[i].wavelength;
                if (smooth && mv_point > 0) {
                    linearFit(X, db[i].S_il_pw, S, R);
                    slope_.push_back(S);
                    RSQ.push_back(R);
                } else {
                    linearFit(X, db[i].il_pw, S, R);
                    slope_.push_back(S);
                    RSQ.push_back(R);
                }

                ss = ss + slope_[i];
                ee = ee + RSQ[i];
            }
            ss = ss / (int)db.size();
            slope_mean.push_back(ss);
            ee = ee / (int)db.size();
            RSQ_mean.push_back(ee);
            Printinfo();
        };
        void Printinfo()
        {
            msg.prefix("======================================== ") << "";
            msg.prefix("slope= ") << slope_;
            msg.prefix("RSS= ") << RSQ;
            msg.prefix("slope_mean= ") << slope_mean;
            msg.prefix("RSS_mean= ") << RSQ_mean;
        }
        void clear()
        {
            X.clear();
            db.clear();
            slope_.clear();
            RSQ.clear();
            subdie_.clear();
            slope_mean.clear();
            RSQ_mean.clear();
            cond_.clear();
        };
    };
    class CLS_WG_CUTBACK
    {
    private:
        WGCUTBACK WG;

    public:
        void WG_Wavelength()
        {
            msg << "WG_Wavelength >> ";
            kpa::ins::MSG_INDENT a;
            FORMAT::WGCutBack::WG_Wavelength set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            WG.clear();
            Rd.clear();
            Rd.path_lab = kpa::info::Path_XY;
            Rd.GetFile(Rd.path_lab);

            WG.mv_point = set.MV_point;
            WG.addWGUnit(set.wavelength_01);
            WG.addWGUnit(set.wavelength_02);
            WG.addWGUnit(set.wavelength_03);
            WG.addWGUnit(set.wavelength_04);
            WG.addWGUnit(set.wavelength_05);
        }
        void WG_Subdie()
        {
            msg << "WG_Subdie >> ";
            kpa::ins::MSG_INDENT a;
            FORMAT::WGCutBack::WG_Subdie set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            WG.addWGPoint(set.dev_name, set.x);
        }
        void WG_Analysis()
        {
            msg << "WG_Analysis >> ";
            kpa::ins::MSG_INDENT a;
            FORMAT::WGCutBack::WG_Analysis set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            WG.slope(false);
            WG.report(&item_no, false, set.Item_name);
            WG.slope(true);
            WG.report(&item_no, true, set.Item_name);
            WG.clear();
            Rd.clear();
        }
    };
    CLS_WG_CUTBACK objWGCUTBACK;

    //! Define Spectrum//
    class CLS_Spectrum
    {
    public:
        LV_SPECTRUM LVobj;
        string dev;

        void Analysis()
        {
            FORMAT::Spectrum::Analysis set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            kpa::ins::MSG_INDENT __t;
            Rd.path_lab = kpa::info::Path_XY;
            Rd.GetFile(Rd.path_lab);

            // Getsubdie();
            LVobj.clear();
            for (int i = 0; i < (int)Rd.Path.size(); i++) {
                LVobj.CreatPath(Rd.Path[i]);
            }
            LVobj.Getsubdie();
            msg.prefix("dev") << set.dev_name;
            if (strcmp(set.dev_name, (char *)"NAME") != 0 && strcmp(set.condition, (char *)"NAME") != 0) {
                dev = std::string(set.dev_name) + std::string("_") + std::string(set.condition);
            } else {
                dev = set.dev_name;
            }

            // main();
            if (set.Raw) {
                for (int k = 0; k < LVobj.GetREFsize(); k++) {
                    LVobj.modeRaw();
                    LVobj.findPeak(k, set.PEAK);
                    LVobj.findcent(k, conf.center_wl, set.CENT);
                    LVobj.getFSRFeat(k, conf.center_wl, set.FSR);
                    LVobj.getNg(k, conf.center_wl, set.Ng_lens, set.FSR);
                }
                for (int k = 0; k < LVobj.GetREFsize(); k++) {
                    // if (dev == LVobj.subdie[k]) {
                    if (LVobj.subdie[k].find(dev) != string::npos) {
                        LVobj.report(&item_no, k, LVobj.subdie, false);
                    }
                }
                // LVobj.PrintLoginfo();
                LVobj.LOG_clear();
            }

            // smooth
            if (set.Smooth && set.MV_point > 0) {
                for (int k = 0; k < LVobj.GetREFsize(); k++) {
                    LVobj.modeMovingAverage(set.MV_point);
                    LVobj.findPeak(k, set.PEAK);
                    LVobj.findcent(k, conf.center_wl, set.CENT);
                    LVobj.getFSRFeat(k, conf.center_wl, set.FSR);
                    LVobj.getNg(k, conf.center_wl, set.Ng_lens, set.FSR);
                }
                for (int k = 0; k < LVobj.GetREFsize(); k++) {
                    // if (dev == LVobj.subdie[k]) {
                    if (LVobj.subdie[k].find(dev) != string::npos) {
                        LVobj.report(&item_no, k, LVobj.subdie, true);
                    }
                }
                // LVobj.PrintLoginfo();
                LVobj.LOG_clear();
            }
            LVobj.clear();

            Rd.clear();
        }
    };
    CLS_Spectrum objSpectrum;

    //! Define CLS_RECORD
    class CLS_RECORD : public GOLDEN
    {
    private:
        struct
        {
            int die_x = 0, die_y = 0, subdie = 0;
        } probe_position[3];
        struct
        {
            vector<float> state[9];
        } _polar_state;
        vector<float> &__polar_state(char *w)
        {
            int idx = -1;
            int rlt = sscanf(w, "state%d", &idx);
            if (--idx < 0)
                return _polar_state.state[0];
            else
                return _polar_state.state[idx];
        }
        void __dieInfoUpdate()
        {
            probe.GetDiePosition(&px, &py);
            msg << str_format("Move to die (%+02d,%+02d)", px, py);
        }

    public:
        void rec_wafer_position()
        {
            kpa::ins::MSG_INDENT __t;
            FORMAT::RECORD::REC_WAFER_POSITION set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            int idx_pos;

            sscanf(set.memory, "position%d", &idx_pos);
            idx_pos--;
            if (idx_pos >= 0 && idx_pos <= 2) {
                auto &t_pos = probe_position[idx_pos];
                auto pos_s = probe.GetDiePosition();
                t_pos.die_x = pos_s[0];
                t_pos.die_y = pos_s[1];
                t_pos.subdie = pos_s[2];
            } else {
                msg << "[Warning!!] memory position is invalid !";
            }
        }
        void move_wafer_position()
        {
            kpa::ins::MSG_INDENT __t;
            FORMAT::RECORD::MOVE_WAFER_POSITION set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            int idx_pos;

            if (set.move_to_die) {
                if (set.move_to_subdie)
                    probe.MoveToDie(set.Die_X, set.Die_Y, set.SubDie);
                else
                    probe.MoveToDie(set.Die_X, set.Die_Y);
            } else {
                if (set.move_to_subdie)
                    probe.MoveToSubDie(set.SubDie);
                else
                    return;
            }
            __dieInfoUpdate();
        }
        void set_wafer_position()
        {
            kpa::ins::MSG_INDENT __t;
            FORMAT::RECORD::SET_WAFER_POSITION set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            int idx_pos;

            sscanf(set.memory, "position%d", &idx_pos);
            idx_pos--;
            if (idx_pos >= 0 && idx_pos <= 2) {
                auto &t_pos = probe_position[idx_pos];
                if (set.move_back_die_position) {
                    if (set.move_back_subdie_number)
                        probe.MoveToDie(t_pos.die_x, t_pos.die_y, t_pos.subdie);
                    else
                        probe.MoveToDie(t_pos.die_x, t_pos.die_y);
                } else {
                    if (set.move_back_subdie_number)
                        probe.MoveToSubDie(t_pos.subdie);
                    else
                        return;
                }
                __dieInfoUpdate();
            } else {
                msg << "[Warning!!] memory position is invalid !";
            }
        }
        void rec_polar_state()
        {
            kpa::ins::MSG_INDENT __t;
            FORMAT::RECORD::REC_POLAR_STATE set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            auto polarSearch = [](EPS1000::TRACE target) {
                polar.feedback(true, polar.analog, target); //! Search polarization
                Sleep(1000);
                polar.feedback(false);
                Sleep(200);
            };

            if (set.ref_device_num >= 0) {
                probe.MoveToSubDie(set.ref_device_num);
                msg.prefix("Probing Subdie : ") << set.ref_device_num;
            }
            msg.prefix("Power Sensor   : ") << set.ref_sensor;
            laser.switch_ref(selectModuleChannel(set.ref_sensor));
            Sleep(5);

            msg << "Search coupling for polarization";
            probe.hxpod.OpticalAlignment(false); //! Search coupling for polarization
            Sleep(10);
            msg << "Search polarization (with power)";
            polarSearch(polar.IL_MIN);
            msg << "Search coupling for scan spectrum";
            probe.hxpod.OpticalAlignment(false); //! Search coupling for Scan Spectrum
            Sleep(10);

            polarSearch(polar.IL_MIN);
            if (pm.Init() == 0) {
                auto buf = pm.ReadSOP_R();
                if (string(set.record_polar_state) != "---") {
                    __polar_state(set.record_polar_state) = buf;
                    msg.prefix("Save Polar State to : ") << set.record_polar_state;
                    msg.prefix("SOP setting         : ").format("%+4.3f") << buf;
                }
                if (string(set.virtical_polar_state) != "---") {
                    auto inv = buf;
                    for (auto &e : inv)
                        e = -e;
                    __polar_state(set.virtical_polar_state) = inv;
                    msg.prefix("Save Polar State to : ") << set.virtical_polar_state;
                    msg.prefix("SOP setting         : ").format("%+4.3f") << inv;
                }
                pm.Term();
            }
        }
        void set_polar_state()
        {
            kpa::ins::MSG_INDENT __t;
            FORMAT::RECORD::SET_POLAR_STATE set;
            ItemReader_Get_Data((uint8_t *)&set, sizeof(set));
            int idx = -1;
            int rlt = sscanf(set.polar_state, "state%d", &idx);
            if (--idx < 0)
                return;
            if (pm.Init() == 0) {
                msg.prefix("Enable Polar Setting : ") << set.polar_state;
                msg.prefix("SOP setting          : ").format("%+4.3f") << _polar_state.state[idx];
                pm.WriteSOP(_polar_state.state[idx]);
                pm.Term();

                msg << "Polarization adjust";
                polar.feedback(true, polar.digital);
                Sleep(200);
                polar.feedback(false);
            }
        }
    };
    CLS_RECORD objRecord;

    //! ==== Define Commands ==== !//
    // void command_process()
    // {
    //     char buf[128];
    //     while (ItemReader_NextFunction(buf, 128)) {
    //         string func_name(buf);
    //         if (func_name == "TA_1DGC") {
    //             msg << "TEST_ALGRORITHM::TA_1DGC >> ";
    //             objTestAlgrorithm.dev1DGC();
    //         } else if (func_name == "TA_2DGC") {
    //             msg << "TEST_ALGRORITHM::TA_2DGC >> ";
    //             objTestAlgrorithm.dev2DGC();
    //         } else if (func_name == "SIPH_CONFIG") {
    //             msg << "SIPH::SIPH_CONFIG >> ";
    //             objSiph.config();
    //         } else if (func_name == "AM_TUNE_PITCH") {
    //             msg << "ALIGN_MODULE::AM_TUNE_PITCH >> ";
    //             objAlignMethod.tune_pitch();
    //         } else if (func_name == "AM_TUNE_ROLL") {
    //             msg << "ALIGN_MODULE::AM_TUNE_ROLL >> ";
    //             objAlignMethod.tune_roll();
    //         } else if (func_name == "AM_TUNE_YAW") {
    //             msg << "ALIGN_MODULE::AM_TUNE_YAW >> ";
    //             objAlignMethod.tune_yaw();
    //         } else if (func_name == "AM_SET_DEVICE") {
    //             msg << "ALIGN_MODULE::AM_SET_DEVICE >> ";
    //             objAlignMethod.set_device();
    //         } else if (func_name == "AM_SET_HOME") {
    //             msg << "ALIGN_MODULE::AM_SET_HOME >> ";
    //             objAlignMethod.set_home();
    //         } else if (func_name == "REC_POLAR_STATE") {
    //             msg << "RECORD::REC_POLAR_STATE >> ";
    //             objRecord.rec_polar_state();
    //         } else if (func_name == "SET_POLAR_STATE") {
    //             msg << "RECORD::SET_POLAR_STATE >> ";
    //             objRecord.set_polar_state();
    //         }
    //         if (kpa::flag::f_debug)
    //             if (kpa::flag::f_stop_button)
    //                 return;
    //     }
    // }

    // ***  Program end    *** //
}
#endif // __KPA_USER_GLOBAL__
