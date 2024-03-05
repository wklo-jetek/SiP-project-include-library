namespace FORMAT {
#pragma pack(1)
    using str = char[128];
    using str_select = char[128];
    using i32 = int;
    using dbl = double;
    using boolean = char;

    namespace SIPH {
        struct SIPH_HwControl;
        struct SIPH_CONFIG;
    }
    namespace ALIGN_MODULE {
        struct AM_SET_HOME;
        struct AM_SET_DEVICE;
        struct AM_TUNE_YAW;
        struct AM_TUNE_PITCH;
        struct AM_TUNE_ROLL;
    }
    namespace TEST_ALGRORITHM {
        struct TA_1DGC;
        struct TA_2DGC;
        struct TA_1IMO;
        struct TA_DC_BIAS;
        struct TA_2DGC_Full;
        struct TA_2DGC_Normalize;
        struct TA_GetSpctmDelta;
    }
    namespace WGCutBack {
        struct WG_Wavelength;
        struct WG_Subdie;
        struct WG_Analysis;
    }
    namespace PostProcessing {
        struct PP_Subdie;
        struct PP_FWHMTable;
    }
    namespace RECORD {
        struct REC_WAFER_POSITION;
        struct MOVE_WAFER_POSITION;
        struct SET_WAFER_POSITION;
        struct REC_POLAR_STATE;
        struct SET_POLAR_STATE;
        struct SET_CENTER_WL;
    };
}

struct FORMAT::SIPH::SIPH_CONFIG
{
    dbl laser_pw = 10.0;             //* format="%2.2f db"
    dbl laser_step = 0.1;            //* format="%2.2f pm"
    str_select laser_mode = "Oband"; //* select="Oband|CLband", width=86
    dbl Oband_start = 1260.0;        //* upper=1357.5, lower=1260.0,width=70,format="%4.1f nm "
    dbl Oband_stop = 1357.5;         //* upper=1357.5, lower=1260.0,,width=70,x_ofs=80, y_ofs=-45,format="%4.1f nm"
    dbl Oband_cent = 1310.0;         //*upper=1357.5, lower=1260.0,width=70,x_ofs=160, y_ofs=-45,format="%4.1f nm"
    dbl CLband_start = 1502.5;       //*  upper=1637.5, lower=1502.5,width=70,format="%4.1f nm "
    dbl CLband_stop = 1637.55;       //* upper=1637.5, lower=1502.5,,width=70,x_ofs=80, y_ofs=-45,format="%4.1f nm"
    dbl CLband_cent = 1550.0;        //* upper=1637.5, lower=1502.5,,width=70,x_ofs=160, y_ofs=-45,format="%4.1f nm"
    i32 fiber_contact_height = 460;  //* format="%d um"
};
struct FORMAT::SIPH::SIPH_HwControl
{
    boolean Enable_Laser = true;
    boolean Enable_Polarizer = true;
    boolean Enable_OpticalSwitch = true;
    boolean Enable_ProberCommunication = true;
    boolean Enable_SMU = true;
};

struct FORMAT::ALIGN_MODULE::AM_SET_DEVICE
{
    str device = "sample1";            //* width=144
    i32 device_num = 0;                //* width=72, x_ofs=150, y_ofs=-45, upper=999, lower=-1
    str_select device_pitch = "(2,1)"; //* select="(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)", width=72
};
struct FORMAT::ALIGN_MODULE::AM_SET_HOME
{
    dbl yaw_u = 0.0;   //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg"
    dbl pitch_v = 0.0; //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg"
    dbl roll_w = 0.0;  //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg"
};
struct FORMAT::ALIGN_MODULE::AM_TUNE_YAW
{
    str_select couple_method = "no_xy_scan"; //* select="no_xy_scan|no_polar_scan|polar_analog_scan|polar_digital_scan", width=120
    dbl ang_start = -0.1;                    //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg"
    dbl ang_step = 0.1;                      //* upper=3.0, lower=0.01, width=72, format="%#.2f deg", x_ofs=84, y_ofs=-45
    dbl ang_end = 0.1;                       //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg", x_ofs=168, y_ofs=-45
    str_select end_position = "IL_MIN";      //* select="IL_MIN|origin|home"
    boolean save_spectrum = false;           //* y_ofs=24
    str_select scan_method = "analog_fb";    //* select="no_fb|analog_fb|digital_fb|highloss_fb", width=86
    str file_name = "YAO";                   //* width=108
    str_select spectrum_step = "10pm";       //* select="100pm|10pm|1pm|0.5pm|0.1pm", width=60
};
struct FORMAT::ALIGN_MODULE::AM_TUNE_PITCH
{
    str_select couple_method = "no_xy_scan"; //* select="no_xy_scan|no_polar_scan|polar_analog_scan|polar_digital_scan", width=120
    dbl ang_start = -0.1;                    //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg"
    dbl ang_step = 0.1;                      //* upper=3.0, lower=0.01, width=72, format="%#.2f deg", x_ofs=84, y_ofs=-45
    dbl ang_end = 0.1;                       //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg", x_ofs=168, y_ofs=-45
    str_select end_position = "IL_MIN";      //* select="IL_MIN|origin|home"
    boolean save_spectrum = false;           //* y_ofs=24
    str_select scan_method = "analog_fb";    //* select="no_fb|analog_fb|digital_fb|highloss_fb", width=86
    str file_name = "PAO";                   //* width=108
    str_select spectrum_step = "10pm";       //* select="100pm|10pm|1pm|0.5pm|0.1pm", width=60
};
struct FORMAT::ALIGN_MODULE::AM_TUNE_ROLL
{
    str_select couple_method = "no_xy_scan"; //* select="no_xy_scan|no_polar_scan|polar_analog_scan|polar_digital_scan", width=120
    dbl ang_start = -0.1;                    //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg"
    dbl ang_step = 0.1;                      //* upper=3.0, lower=0.01, width=72, format="%#.2f deg", x_ofs=84, y_ofs=-45
    dbl ang_end = 0.1;                       //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg", x_ofs=168, y_ofs=-45
    str_select end_position = "IL_MIN";      //* select="IL_MIN|origin|home"
    boolean save_spectrum = false;           //* y_ofs=24
    str_select scan_method = "analog_fb";    //* select="no_fb|analog_fb|digital_fb|highloss_fb", width=86
    str file_name = "RAO";                   //* width=108
    str_select spectrum_step = "10pm";       //* select="100pm|10pm|1pm|0.5pm|0.1pm", width=60
};

struct FORMAT::TEST_ALGRORITHM::TA_1DGC
{
    str device = "sample1";                         //* width=96
    i32 device_num = 0;                             //* x_ofs=102, y_ofs=-45, upper=999, lower=-1, width=66
    str_select sensor = "(2,1)";                    //* select="(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)", width=72
    str_select couple_method = "polar_analog_scan"; //* select="no_xy_scan|no_polar_scan|polar_analog_scan|polar_digital_scan", width=120, y_ofs=1
    str_select scan_method = "analog_fb";           //* select="no_fb|analog_fb|digital_fb|highloss_fb", width=120
    boolean meas_1db_bandwidth = false;             //* y_ofs=12
    boolean save_spectrum = true;                   //* y_ofs=12
    str_select spectrum_step = "10pm";              //* select="100pm|10pm|1pm|0.5pm|0.1pm", width=60, y_ofs=-3
    str_select store_to_buffer = "spectrum1";       //* select="---|spectrum1|spectrum2|spectrum3|spectrum4|spectrum5|spectrum6|spectrum7|spectrum8|spectrum9", width=72, y_ofs=18
    boolean WG = false;                             //* y_ofs=12
    dbl WG_x = 0.0;                                 //* width=85
};
struct FORMAT::TEST_ALGRORITHM::TA_2DGC
{
    str device = "sample";                          //* width=96
    i32 device_num = 0;                             //* x_ofs=102, y_ofs=-45, upper=999, lower=-1, width=66
    str_select sensor1 = "(2,1)";                   //* select="(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)", width=72
    str_select sensor2 = "(2,1)";                   //* select="(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)", width=72, x_ofs=102, y_ofs=-45
    str name_of_sens1 = "sensor1";                  //* width=95
    str name_of_sens2 = "sensor2";                  //* width=95, x_ofs=102, y_ofs=-46
    str_select couple_method = "polar_analog_scan"; //* select="no_xy_scan|no_polar_scan|polar_analog_scan|polar_digital_scan", width=120, y_ofs=1
    str_select scan_method = "analog_fb";           //* select="no_fb|analog_fb|digital_fb|highloss_fb", width=120
    boolean save_spectrum = true;                   //* y_ofs=18
    str_select spectrum_step = "10pm";              //* select="100pm|10pm|1pm|0.5pm|0.1pm", width=60
    str_select store_to_buf = "None";               //* select="None|2DGC_TE|2DGC_TM"
};
struct FORMAT::TEST_ALGRORITHM::TA_1IMO
{
    str device = "sample1";                         //* width=96
    i32 device_num = 0;                             //* x_ofs=102, y_ofs=-45, upper=999, lower=-1, width=66
    str_select ref_sensor = "(2,1)";                //* width=60, select="(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)"
    str_select couple_method = "polar_analog_scan"; //* select="no_xy_scan|no_polar_scan|polar_analog_scan|polar_digital_scan", width=120, y_ofs=1
    str_select scan_method = "analog_fb";           //* select="no_fb|analog_fb|digital_fb|highloss_fb", width=120
    boolean save_spectrum = true;                   //* y_ofs=18
    str_select spectrum_step = "10pm";              //* select="100pm|10pm|1pm|0.5pm|0.1pm", width=60
    str_select sensor1 = "---";                     //* width=60, select="---|(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)"
    str column1 = "";                               //* width=84, x_ofs=90, y_ofs=-45
    str_select sensor2 = "---";                     //* width=60, select="---|(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)"
    str column2 = "";                               //* width=84, x_ofs=90, y_ofs=-45
    str_select sensor3 = "---";                     //* width=60, select="---|(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)"
    str column3 = "";                               //* width=84, x_ofs=90, y_ofs=-45
    str_select sensor4 = "---";                     //* width=60, select="---|(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)"
    str column4 = "";                               //* width=84, x_ofs=90, y_ofs=-45
    str_select sensor5 = "---";                     //* width=60, select="---|(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)"
    str column5 = "";                               //* width=84, x_ofs=90, y_ofs=-45
    str_select sensor6 = "---";                     //* width=60, select="---|(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)"
    str column6 = "";                               //* width=84, x_ofs=90, y_ofs=-45
    str_select sensor7 = "---";                     //* width=60, select="---|(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)"
    str column7 = "";                               //* width=84, x_ofs=90, y_ofs=-45
    str_select sensor8 = "---";                     //* width=60, select="---|(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)"
    str column8 = "";                               //* width=84, x_ofs=90, y_ofs=-45
};
struct FORMAT::TEST_ALGRORITHM::TA_2DGC_Normalize
{
    str name = "sample";                //* width=96
    boolean save_spectrum = true;       //* y_ofs=18
    boolean save_source_data = false;   //*
    str_select spectrum_step = "10pm";  //* select="100pm|10pm|1pm|0.5pm|0.1pm", width=60
    boolean meas_1db_bandwidth = false; //* y_ofs=12
    boolean range_limit_search = true;  //* y_ofs=12
    dbl wl_lower = 1260.0;              //* width=72, y_ofs=-3
    dbl wl_upper = 1360.0;              //* lower=0., width=72, format="%#.1f", x_ofs=84, y_ofs=-45
    boolean IL_MAX = true;              //*
    boolean IL_MIN = true;              //* x_ofs=84, y_ofs=-23
};
struct FORMAT::TEST_ALGRORITHM::TA_2DGC_Full
{
    str dev_1D_name = "dev1";              //* width=84
    i32 dev_1D_num = 0;                    //* width=72, x_ofs=90, y_ofs=-45
    str_select sens_1D = "(2,1)";          //* width=54, x_ofs=174, y_ofs=-45, select="(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)", width=72
    str dev_2D_name = "dev1";              //* width=84, y_ofs=12
    i32 dev_2D_num = 0;                    //* width=72, x_ofs=90, y_ofs=-45
    str_select sens1_2D = "(2,1)";         //* width=54, select="(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)", width=72
    str_select sens2_2D = "(2,1)";         //* width=54, x_ofs=90, y_ofs=-45, select="(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)", width=72
    boolean save_1D_data = true;           //* y_ofs=24
    boolean save_2D_raw_data = true;       //* y_ofs=0
    boolean save_2D_normalize_data = true; //* y_ofs=0
    str_select spectrum_step = "10pm";     //* select="100pm|10pm|1pm|0.5pm|0.1pm", width=60
};
struct FORMAT::TEST_ALGRORITHM::TA_DC_BIAS
{
    str device_name = "sample";                     //* width=84
    i32 device_num = 0;                             //* width=72, x_ofs=90, y_ofs=-45
    str_select sensor = "(2,1)";                    //* width=54, x_ofs=174, y_ofs=-45, select="(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)", width=72
    str_select couple_method = "polar_analog_scan"; //* select="no_xy_scan|no_polar_scan|polar_analog_scan|polar_digital_scan", width=120
    dbl bias_start = -0.1;                          //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg"
    dbl bias_step = 0.1;                            //* upper=3.0, lower=0.01, width=72, format="%#.2f deg", x_ofs=84, y_ofs=-45
    dbl bias_end = 0.1;                             //* upper=3.0, lower=-3.0, width=72, format="%#.2f deg", x_ofs=168, y_ofs=-45
    boolean save_spectrum = false;                  //* y_ofs=24
    str_select scan_method = "analog_fb";           //* select="no_fb|analog_fb|digital_fb|highloss_fb", width=86
    str_select spectrum_step = "10pm";              //* select="100pm|10pm|1pm|0.5pm|0.1pm", width=60
};
struct FORMAT::TEST_ALGRORITHM::TA_GetSpctmDelta
{
    str_select spctm1 = "spectrum1"; //* select="spectrum1|spectrum2|spectrum3|spectrum4|spectrum5|spectrum6|spectrum7|spectrum8|spectrum9", width=72
    str_select spctm2 = "spectrum2"; //* select="spectrum1|spectrum2|spectrum3|spectrum4|spectrum5|spectrum6|spectrum7|spectrum8|spectrum9", width=72, x_ofs=108, y_ofs=-45
    boolean log_raw = true;          //*
    boolean log_smooth = true;       //*
    dbl get_wavelength_1 = 0.0;      //* upper=1600.0, lower=0.0, format="%.1f nm", y_ofs=12
    dbl get_wavelength_2 = 0.0;      //* upper=1600.0, lower=0.0, format="%.1f nm"
    dbl get_wavelength_3 = 0.0;      //* upper=1600.0, lower=0.0, format="%.1f nm"
    dbl get_wavelength_4 = 0.0;      //* upper=1600.0, lower=0.0, format="%.1f nm"
    dbl get_wavelength_5 = 0.0;      //* upper=1600.0, lower=0.0, format="%.1f nm"
    str log_name = "delta";          //* width=96, y_ofs=12
};

struct FORMAT::RECORD::REC_WAFER_POSITION
{
    str_select memory; //* select="position1|position2|position3"
};
struct FORMAT::RECORD::MOVE_WAFER_POSITION
{
    boolean move_to_die = true;    //*
    i32 Die_X = 0;                 //* width=60
    i32 Die_Y = 0;                 //* width=60, y_ofs=-45, x_ofs=72
    boolean move_to_subdie = true; //* y_ofs=24
    i32 SubDie = 0;                //* width=60, lower=0
};
struct FORMAT::RECORD::SET_WAFER_POSITION
{
    str_select memory;                      //* select="position1|position2|position3"
    boolean move_back_die_position = true;  //*
    boolean move_back_subdie_number = true; //*
};
struct FORMAT::RECORD::REC_POLAR_STATE
{
    i32 ref_device_num = 0;                  //* upper=999, lower=-1, width=66
    str_select ref_sensor = "(2,1)";         //* select="(2,1)|(2,2)|(2,3)|(2,4)|(2,5)|(2,6)|(3,1)|(3,2)|(3,3)|(3,4)|(3,5)|(3,6)|(3,1)|(3,2)", width=72
    str_select record_polar_state = "---";   //* select="---|state1|state2|state3|state4|state5|state6"
    str_select virtical_polar_state = "---"; //* select="---|state1|state2|state3|state4|state5|state6"
};
struct FORMAT::RECORD::SET_POLAR_STATE
{
    str_select polar_state; //* select="state1|state2|state3|state4|state5|state6"
};
struct FORMAT::RECORD::SET_CENTER_WL
{
    str_select source = "SiPh_config"; //* select="SiPh_config|PeakWL(buffer)", width=96
    str_select buffer = "spectrum1";   //* select="spectrum1|spectrum2|spectrum3|spectrum4|spectrum5|spectrum6|spectrum7|spectrum8|spectrum9", width=96, y_ofs=18
};

struct FORMAT::WGCutBack::WG_Wavelength
{
    i32 MV_point = 1000;
    dbl wavelength_01 = -1.0; //*
    dbl wavelength_02 = -1.0; //*
    dbl wavelength_03 = -1.0; //*
    dbl wavelength_04 = -1.0; //*
    dbl wavelength_05 = -1.0; //*
};
struct FORMAT::WGCutBack::WG_Analysis
{
    str Item_name = "NAME"; //*
};
struct FORMAT::WGCutBack::WG_Subdie
{
    str dev_name = "NAME"; //*
    dbl x = 0.0;           //* width=85
};

struct FORMAT::PostProcessing::PP_Subdie
{
    str dev_name = "NAME"; //*width=100
    // str condition = "NAME"; //*x_ofs=110,y_ofs=-47
    boolean Raw = true;            //*
    boolean Smooth = false;        //*x_ofs=60,y_ofs=-23
    i32 MV_point = 10;             //*width=70
    boolean PEAK = false;          //*y_ofs=20
    boolean CENT = false;          //*
    boolean FSR_raw = false;       //*
    boolean FSR_cosinefit = false; //*x_ofs=84, y_ofs=-23
    dbl Ng_lens = 0;               //*width=70,format="%4.1f nm"
    // boolean FWHM_Nor = false;       //*
    // boolean FWHM_cosinefit = false; //*x_ofs=84, y_ofs=-23
};
struct FORMAT::PostProcessing::PP_FWHMTable
{
    boolean Raw = true;           //*
    boolean Smooth = false;       //*x_ofs=60,y_ofs=-23
    i32 MV_point = 10;            //*width=70
    str_select FSR = "FSR_raw";   //*select="FSR_raw|FSR_cosinefit"
    dbl Ng_lens = 1000;           //*width=70,format="%4.1f nm"
    str_select FWHM = "FWHM_Nor"; //*select="FWHM_Nor|FWHM_cosinefit"
    i32 FWHMnum = 0;              //*
};
