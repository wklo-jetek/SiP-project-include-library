

#include "libSiphModule.hpp"

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

#ifdef USE_libCM300
#include "libCM300.cpp"
CM300 probe;
#endif
#ifdef USE_libTS3K
#include "libTS300.cpp"
TS300 probe;
#endif

//! Define Common Functions
class SIPH
{
public:
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
            msg << "Search Coupling";
            probe.hxpod.OpticalAlignment(false);
        }
        void __analog_s()
        {
            // msg << "Search Coupling for Polarization";
            // probe.hxpod.OpticalAlignment(false); //! Search coupling for polarization
            // Sleep(10);
            msg << "Search polarization (with power)";
            __search_polar(polar.analog); //! Search polarization
            msg << "Search coupling for Scan Spectrum";
            probe.hxpod.OpticalAlignment(false); //! Search coupling for Scan Spectrum
            Sleep(10);
        }
        void __analog()
        {
            msg << "Search Coupling for Polarization";
            probe.hxpod.OpticalAlignment(false); //! Search coupling for polarization
            Sleep(10);
            msg << "Search polarization (with power)";
            __search_polar(polar.analog); //! Search polarization
            msg << "Search coupling for Scan Spectrum";
            probe.hxpod.OpticalAlignment(false); //! Search coupling for Scan Spectrum
            Sleep(10);
        }
        void __digital()
        {
            msg << "Search polarization (with first polarization)";
            __search_polar(polar.digital); //! Search polarization
            msg << "Search coupling for Scan Spectrum";
            probe.hxpod.OpticalAlignment(false); //! Search coupling for Scan Spectrum
            Sleep(10);
        }
        void (COUPLE_METHOD::*__method)(void) = __off;

    public:
        void setMethod(char *method)
        {
            string _method = msg.prefix("xy_scan_mode   : ") << method;
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
            kpa::ins::MSG_INDENT __t;
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
            string _set = msg.prefix("spectrum_method: ") << set_mode;
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
