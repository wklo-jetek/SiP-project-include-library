#ifndef __LIBTSMCFUNCTIONS_CPP__
#define __LIBTSMCFUNCTIONS_CPP__

#include "libCustomizeItem.cpp"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <windows.h>
#include <fstream>
#include "libSpectrumAnalysis.h"

class SEG_TIMER
{
private:
    clock_t t_init, t_seg;

public:
    SEG_TIMER();
    float life();
    int life_ms();
    float seg();
    int seg_ms();
};
SEG_TIMER::SEG_TIMER()
{
    t_init = clock();
    t_seg = t_init;
}
float SEG_TIMER::life()
{
    return (clock() - t_init) * 0.001;
}
int SEG_TIMER::life_ms()
{
    return (clock() - t_init);
}
float SEG_TIMER::seg()
{
    clock_t n = clock();
    float t = (n - t_seg) * 0.001;
    t_seg = n;
    return t;
}
int SEG_TIMER::seg_ms()
{
    clock_t n = clock();
    int t = (n - t_seg);
    t_seg = n;
    return t;
}
char *date_stamp()
{
    static char buf[5];
    if (buf[0] != 0)
        return buf;
    auto now = time(0);
    tm *ltm = localtime(&now);
    sprintf(buf, "%02d%02d", ltm->tm_mon + 1, ltm->tm_mday);
    return buf;
}
struct SUB_getDataFolder
{
private:
    std::string folder = "";

public:
    void reset()
    {
        folder = "";
    }
    std::string main()
    {
        if (folder.size() == 0)
        {
            std::string rpt(kpa::info::report_name.c_str());
            std::string no_subname(rpt.begin(), rpt.end() - 5);
            folder = str_format("%s\\%s", kpa::info::datalog_path.c_str(), no_subname.c_str());
            kpa::ins::msg.prefix("Create folder : ") << folder;
            mkdir(folder.c_str());
        }
        return folder;
    }
    std::string operator()()
    {
        return main();
    }
} getDataFolder;
// string getDataFolder()
// {
//     static string folder;
//     if (folder.size() == 0) {
//         folder = str_format("%s\\%s", kpa::info::datalog_path.c_str(), kpa::info::proc_name.c_str());
//         msg.prefix("Create folder : ") << folder;
//         mkdir(folder.c_str());
//     }
//     return folder;
// }
std::string checkFilePath(const std::string &f_path)
{
    int p_idx = f_path.find('.', f_path.size() - 4);
    std::string path_pre, path_post, out = f_path;

    if (p_idx == -1)
    {
        path_pre = f_path;
        path_post = "";
    }
    else
    {
        path_pre = std::string(f_path.begin(), f_path.begin() + p_idx);
        path_post = std::string(f_path.begin() + p_idx, f_path.end());
    }

    struct stat buf;
    for (int i = 1; i < 1000; i++)
    {
        if (stat(out.c_str(), &buf) != -1)
            out = str_format("%s(%d)%s", path_pre.c_str(), i, path_post.c_str());
    }
    return out;
}
std::string auto_si(double value, const char *word = "")
{
    const char *sis[] = {"f", "p", "n", "u", "m", "", "K", "M", "G", "T"};
    int idx_si = 5;
    while (true)
    {
        if (value > 1000.)
        {
            value *= 0.001;
            idx_si++;
        }
        else if (value < 1.)
        {
            value *= 1000.;
            idx_si--;
        }
        else
        {
            return str_format("%s%s", sis[idx_si], word);
        }
    }
    return std::string("");
}

struct SPECTRUM
{
public:
    struct PT
    {
        double wl, pw;
    };
    const char *default_title = "Wavelength(nm), Power(db) \n";
    double start;
    double step;
    std::vector<float> data;
    const char *title = default_title;
    uint16_t samp = 1;

    void save(std::ofstream &f_log);
    void save(const char *file);
    void save(const std::string &file);
    SPECTRUM segment(double wl_start, double wl_stop);
    double idxWaveLength(double wavelength);
    double bandwidth(float loss_db);

    PT peak()
    {
        PT out;
        int idx_peak = std::max_element(data.begin(), data.end()) - data.begin();
        out.wl = start + step * idx_peak;
        out.pw = data[idx_peak];
        return out;
    }
    PT cent(double wl)
    {
        PT out;
        out.wl = wl;
        int idx_cent = int(((wl * 1nm - start) / step) + 0.5);
        out.pw = data[idx_cent];
        return out;
    }
};
double SPECTRUM::idxWaveLength(double wavelength)
{
    // auto index = [&](double wl) { return (this->data.begin() + int((wl - this->start) / (this->step) + 0.5)); };
    return data[int((wavelength - this->start) / (this->step) + 0.5)];
}
void SPECTRUM::save(std::ofstream &f_log)
{
    auto wl = start;
    int l = 0;
    char line_buf[64];

    for (auto &value : data)
    {
        if (l == 0)
        {
            snprintf(line_buf, 63, "%10.4f, %10.6f\n", wl * 1E+9, value);
            f_log << line_buf;
        }
        wl = wl + step;
        l = ((++l) >= samp) ? 0 : l;
    }
}
void SPECTRUM::save(const char *file)
{
    std::ofstream f_log;
    std::string f_path = str_format("%s\\%s", getDataFolder().c_str(), file); //! Build full path to default folder
    f_path = kpa::ins::msg.prefix("Save File : ") << checkFilePath(f_path);   //! Check file name is unused

    f_log.open(f_path.c_str(), std::ios::out);
    f_log << title; //! Write title
    save(f_log);    //! Save table
    f_log.close();
}
void SPECTRUM::save(const std::string &file)
{
    this->save(file.c_str());
}
SPECTRUM SPECTRUM::segment(double wl_start, double wl_stop)
{
    auto index = [&](double wl) { return (this->data.begin() + int((wl - this->start) / (this->step) + 0.5)); };
    SPECTRUM out;
    out.title = this->title;
    out.samp = this->samp;
    out.step = this->step;
    out.data = std::vector<float>(index(wl_start), index(wl_stop) + 1);
    out.start = wl_start;

    return out;
}
double SPECTRUM::bandwidth(float loss_db)
{
    const auto &d = data;
    auto peak_itr = std::max_element(d.begin(), d.end());
    float level = *peak_itr - loss_db;
    auto itr = peak_itr;
    for (itr = peak_itr - 1; itr >= d.begin(); itr--)
        if (*(itr) <= level)
            break;
    auto lower = itr;
    itr = peak_itr;
    for (itr = peak_itr + 1; itr >= d.end(); itr++)
        if (*(itr) <= level)
            break;
    auto upper = itr;
    double bw = (upper - lower) * step;
    return bw;
}

struct MULTI_SPECTRUM
{
private:
    std::string __stringReplace(const std::string &origin, const std::string &search, const std::string &replace);
    std::string __checkTitle(const std::string &title);

public:
    struct DB_UNIT
    {
        std::string title;
        std::vector<float> data;
        DB_UNIT(const std::string &_t, const std::vector<float> &_d) : title(_t), data(_d) {}
        DB_UNIT(const std::string &_t, std::vector<float> &&_d) : title(_t), data(_d) {}
        DB_UNIT(std::string &&_t, const std::vector<float> &_d) : title(_t), data(_d) {}
        DB_UNIT(std::string &&_t, std::vector<float> &&_d) : title(_t), data(_d) {}
        DB_UNIT segment(std::vector<float>::iterator itr_start, std::vector<float>::iterator itr_stop);
    };
    std::vector<DB_UNIT *> db;
    struct SPEC_UNIT
    {
        struct PT
        {
            double wl, pw;
        };
        double &start, &step;
        DB_UNIT *pdb;

        SPEC_UNIT(double &_start, double &_step, DB_UNIT *_data) : start(_start), step(_step), pdb(_data) {}
        SPEC_UNIT(const SPEC_UNIT &src) = delete;
        void operator=(const SPEC_UNIT &src) = delete;
        PT peak();
        PT cent(double wl);
        double bandwidth(float loss_db);
    };
    double start, step;
    uint16_t samp = 1;

    ~MULTI_SPECTRUM();
    void add(const std::string &_title, std::vector<float> &&_data);
    void add(const std::string &_title, const std::vector<float> &_data);
    void save(std::ofstream &f_log);
    void save(const char *file);
    void save(const std::string &file);
    MULTI_SPECTRUM segment(double wl_start, double wl_stop);
    size_t size();
    SPEC_UNIT operator[](int idx);
    void clear();
    int getDbSize();
};
MULTI_SPECTRUM::~MULTI_SPECTRUM()
{
    this->clear();
}
MULTI_SPECTRUM::DB_UNIT MULTI_SPECTRUM::DB_UNIT::segment(std::vector<float>::iterator itr_start, std::vector<float>::iterator itr_stop)
{
    DB_UNIT out(this->title, std::vector(itr_start, itr_stop));
    return out;
}
MULTI_SPECTRUM::SPEC_UNIT::PT MULTI_SPECTRUM::SPEC_UNIT::peak()
{
    PT out;
    int idx_peak = std::max_element(pdb->data.begin(), pdb->data.end()) - pdb->data.begin();
    out.wl = start + step * idx_peak;
    out.pw = pdb->data[idx_peak];
    return out;
}
MULTI_SPECTRUM::SPEC_UNIT::PT MULTI_SPECTRUM::SPEC_UNIT::cent(double wl)
{
    PT out;
    out.wl = wl;
    int idx_cent = int(((wl * 1nm - start) / step) + 0.5);
    out.pw = pdb->data[idx_cent];
    return out;
}
double MULTI_SPECTRUM::SPEC_UNIT::bandwidth(float loss_db)
{
    const auto &d = pdb->data;
    auto peak_itr = std::max_element(d.begin(), d.end());
    float level = *peak_itr - loss_db;
    auto itr = peak_itr;
    for (itr = peak_itr - 1; itr >= d.begin(); itr--)
        if (*(itr) <= level)
            break;
    auto lower = itr;
    itr = peak_itr;
    for (itr = peak_itr + 1; itr >= d.end(); itr++)
        if (*(itr) <= level)
            break;
    auto upper = itr;
    double bw = (upper - lower) * step;
    return bw;
}
void MULTI_SPECTRUM::add(const std::string &_title, std::vector<float> &&_data)
{
    auto p = new DB_UNIT(_title, std::move(_data));
    db.push_back(p);
}
void MULTI_SPECTRUM::add(const std::string &_title, const std::vector<float> &_data)
{
    auto p = new DB_UNIT(_title, _data);
    db.push_back(p);
}
void MULTI_SPECTRUM::save(std::ofstream &f_log)
{
    auto wl = start;
    int l = 0;

    int len = 0x0FFFFFFFl;
    for (auto &u : db)
        len = (len > (int)u->data.size()) ? u->data.size() : len;

    for (int r = 0; r < len; r++)
    {
        if (l == 0)
        {
            f_log << std::setprecision(8) << wl * 1.E9;
            for (auto &u : db)
                f_log << ", " << std::setprecision(6) << u->data[r];
            f_log << "\n";
        }
        wl += step;
        l = ((++l) >= samp) ? 0 : l;
    }
}
void MULTI_SPECTRUM::save(const char *file)
{
    if (step <= 1E-15)
        return;
    if (db.size() == 0)
        return;
    std::ofstream f_log;
    std::string f_path = str_format("%s\\%s", getDataFolder().c_str(), file); //! Build full path to default folder
    f_path = kpa::ins::msg.prefix("Save File : ") << checkFilePath(f_path);   //! Check file name is unused

    f_log.open(f_path.c_str(), std::ios::out);
    f_log << "Wavelength(nm)";                   //! Write title
    for (auto &p : db)                           //!
        f_log << ", " << __checkTitle(p->title); //!
    f_log << "\n";                               //!
    save(f_log);                                 //! Save table
    f_log.close();
}
void MULTI_SPECTRUM::save(const std::string &file)
{
    this->save(file.c_str());
}
MULTI_SPECTRUM MULTI_SPECTRUM::segment(double wl_start, double wl_stop)
{
    MULTI_SPECTRUM out;
    out.samp = this->samp;
    out.step = this->step;
    for (auto &su : this->db)
    {
        auto index = [&](double wl) { return (su->data.begin() + int((wl - this->start) / (this->step) + 0.5)); };
        out.add(su->title, std::vector(index(wl_start), index(wl_stop)));
    }
    out.start = wl_start;

    return out;
}
size_t MULTI_SPECTRUM::size()
{
    return db.size();
}
MULTI_SPECTRUM::SPEC_UNIT MULTI_SPECTRUM::operator[](int idx)
{
    if (idx >= 0)
        return SPEC_UNIT(start, step, db[idx]);
    else
        return SPEC_UNIT(start, step, db[db.size() + idx]);
}
void MULTI_SPECTRUM::clear()
{
    for (auto &p : db)
        delete p;
    db.clear();
    db.resize(0);
}
int MULTI_SPECTRUM::getDbSize()
{
    return (int)db.size();
}
std::string MULTI_SPECTRUM::__stringReplace(const std::string &origin, const std::string &search, const std::string &replace)
{
    std::string _copy = origin;
    size_t pos = _copy.find(search);
    while (pos != std::string::npos)
    {
        _copy.replace(pos, search.length(), replace);
        pos = _copy.find(search, pos + replace.length());
    }
    return _copy;
}
std::string MULTI_SPECTRUM::__checkTitle(const std::string &title)
{
    return __stringReplace(title, ",", ";");
}

#ifdef EN_SPECTRUM_LV_OBJECT

class LV_SPECTRUM_SYS_UNLOAD
{
public:
    LV_SPECTRUM_SYS_UNLOAD() {}
    ~LV_SPECTRUM_SYS_UNLOAD() { SysCleanMemory(); }
} lv_spectrum_sys_unload;

class LV_SG_SPCTM
{
private:
    uint32_t ref;

public:
};
class LV_SPECTRUM
{
private:
    std::vector<uint32_t> ref;

public:
    SPECTRUM SPECobject;
    MULTI_SPECTRUM MSPECobject;
    struct FSR_STRUCT
    {
        double FSR;
        double FWHM_raw;
        double FWHM_fit;
    };
    struct PEAK_STRUCT
    {
        double wl;
        double db;
    };
    std::vector<FSR_STRUCT> Log_FSR;
    std::vector<PEAK_STRUCT> Log_PEAK;
    std::vector<double> Log_Cent;
    std::vector<double> Log_Ng;
    std::vector<std::string> subdie;

    void CreatData(double wl_start, double wl_stop, const std::vector<float> &data);
    void CreatSPECM(SPECTRUM &spctm);         // testmode
    void CreatMLSPECM(MULTI_SPECTRUM &spctm); // testmode
    void CreatPath(std::string path);         // readfile
    ~LV_SPECTRUM();
    void modeMovingAverage(int mv_num);
    void modeRaw();
    void Getsubdie();
    // SPECTRUM getSpectrum();
    int GetREFsize();
    void getFSRFeat(int index, double wl, bool);
    void getNg(int index, double center_wl, double L, bool en);
    double findWavelength(int index, double wavelength);
    void findPeak(int index, bool en);
    void findcent(int index, double wavelength, bool en);
    void report(int *num, int index, std::vector<std::string> subdie, bool smooth);
    void PrintLoginfo();
    void LOG_clear();
    void clear();
};

void LV_SPECTRUM::CreatData(double wl_start, double wl_stop, const std::vector<float> &data)
{
    ref.push_back(NewData(wl_start, wl_stop, (float *)&data[0], data.size()));
}
void LV_SPECTRUM::CreatSPECM(SPECTRUM &spctm)
{
    ref.push_back(NewData(spctm.start, spctm.step, &spctm.data[0], spctm.data.size()));
}
void LV_SPECTRUM::CreatMLSPECM(MULTI_SPECTRUM &spctm)
{
    for (int i = 0; i < (int)spctm.getDbSize(); ++i)
    {
        MULTI_SPECTRUM::SPEC_UNIT specUnit = spctm[i];
        ref.push_back(NewData(spctm.start, spctm.step, specUnit.pdb->data.data(), specUnit.pdb->data.size()));
    }
}
void LV_SPECTRUM::CreatPath(std::string path)
{

    if (strlen(path.c_str()) == 0)
    {
        std::cout << "Path is empty!" << std::endl;
        return;
    }
    int ref_size;
    ref_size = SpOpenFile((char *)path.c_str());
    for (int i = 0; i < ref_size; i++)
    {
        this->ref.push_back(NewDataFromFile());
    }
}
LV_SPECTRUM::~LV_SPECTRUM()
{
    for (int i = 0; i < (int)ref.size(); i++)
    {
        DeleteData(ref[i]);
    }
    clear();
}
void LV_SPECTRUM::modeMovingAverage(int mv_num)
{
    SysModeMovingAverage(mv_num);
}
void LV_SPECTRUM::modeRaw()
{
    SysModeRaw();
}
// SPECTRUM LV_SPECTRUM::getSpectrum()
// {
// ReadData(ref, &SPECobject.start, &SPECobject.step, &SPECobject.data[0], SPECobject.data.size());
// return SPECobject;
//}
void LV_SPECTRUM::Getsubdie()
{
    int len = 30;
    char tmp[len];
    for (int i = 0; i < (int)ref.size(); i++)
    {
        // msg.prefix("ref.size()") << ref.size();
        GetSubdie(ref[i], tmp, len);
        std::string str(tmp);
        subdie.push_back(str);
    }
}
int LV_SPECTRUM::GetREFsize()
{
    return (int)ref.size();
}
void LV_SPECTRUM::getFSRFeat(int index, double wl, bool en)
{
    if (!en)
        return;

    FSR_STRUCT tmp;
    FineFSR(ref[index], wl, &tmp.FSR, &tmp.FWHM_raw, &tmp.FWHM_fit);
    Log_FSR.push_back(tmp);
}
void LV_SPECTRUM::getNg(int index, double center_wl, double L, bool en)
{
    if (!en)
        return;

    Log_Ng.push_back(Ng(this->Log_FSR[index].FSR, center_wl, L));
}
void LV_SPECTRUM::findPeak(int index, bool en)
{
    if (!en)
        return;
    PEAK_STRUCT tmp;

    FindPeak(ref[index], &tmp.wl, &tmp.db);
    Log_PEAK.push_back(tmp);
}
void LV_SPECTRUM::findcent(int index, double wavelength, bool en)
{
    if (!en)
        return;

    Log_Cent.push_back(FindWaveLength(ref[index], wavelength));
}
double LV_SPECTRUM::findWavelength(int index, double wavelength)
{
    double tmp;
    tmp = FindWaveLength(ref[index], wavelength);
    return tmp;
}
void LV_SPECTRUM::PrintLoginfo()
{
    // Peak
    for (const auto &peak : Log_PEAK)
    {
        std::cout << "wl: " << peak.wl << ", db: " << peak.db << std::endl;
    }
    // msg.prefix("Log_Cent") << Log_Cent;
    //  FSR
    for (const auto &FSR : Log_FSR)
    {
        std::cout << "FSR: " << FSR.FSR << ", FWHM_fit: " << FSR.FWHM_fit << ", FWHM_raw: " << FSR.FWHM_raw << std::endl;
    }
    // msg.prefix("Log_Ng") << Log_Ng;
}
void LV_SPECTRUM::report(int *num, int index, std::vector<std::string> subdie, bool smooth)
{
    auto mode = [](bool smooth) -> const char * { return (smooth) ? "S" : "R"; };
    const double nm = 1.E-9;

    if (Log_Cent.size() > 0)
    {
        kpa::ins::log((*num)++, str_format("%s_%s_Cent", mode(smooth), subdie[index].c_str()).c_str(), std::vector({Log_Cent[index]}), "%2.2f", "db");
    }
    if (Log_PEAK.size() > 0)
    {
        kpa::ins::log((*num)++, str_format("%s_%s_PEAK_wl", mode(smooth), subdie[index].c_str()).c_str(), std::vector({Log_PEAK[index].wl * nm}), "%4.4f", "nm");
        kpa::ins::log((*num)++, str_format("%s_%s_PEAK_db", mode(smooth), subdie[index].c_str()).c_str(), std::vector({Log_PEAK[index].db}), "%2.2f", "db");
    }
    if (Log_FSR.size() > 0)
    {
        kpa::ins::log((*num)++, str_format("%s_%s_FSR", mode(smooth), subdie[index].c_str()).c_str(), std::vector({Log_FSR[index].FSR * nm}), "%4.4f", "nm");
        kpa::ins::log((*num)++, str_format("%s_%s_FWHM_fit", mode(smooth), subdie[index].c_str()).c_str(), std::vector({Log_FSR[index].FWHM_fit * nm}), "%4.4f", "nm");
        kpa::ins::log((*num)++, str_format("%s_%s_FWHM_raw", mode(smooth), subdie[index].c_str()).c_str(), std::vector({Log_FSR[index].FWHM_raw * nm}), "%4.4f", "nm");
    }
    if (Log_Ng.size() > 0)
    {
        kpa::ins::log((*num)++, str_format("%s_%s_Ng", mode(smooth), subdie[index].c_str()).c_str(), std::vector({Log_Ng[index]}), "%4.4f", "");
    }
}
void LV_SPECTRUM::LOG_clear()
{
    Log_FSR.clear();
    Log_PEAK.clear();
    Log_Cent.clear();
    Log_Ng.clear();
}
void LV_SPECTRUM::clear()
{
    ref.clear();
    Log_FSR.clear();
    Log_PEAK.clear();
    Log_Cent.clear();
    Log_Ng.clear();
    subdie.clear();
}
#endif //* EN_SPECTRUM_LV_OBJECT

#endif //* __LIBTSMCFUNCTIONS_CPP__