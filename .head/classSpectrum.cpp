
#include "classSpectrum.h"
#include "libCustomizeItem.cpp"
#include "kpa_user_0_head.h"

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
    auto index = [&](double wl)
    { return (this->data.begin() + int((wl - this->start) / (this->step) + 0.5)); };
    SPECTRUM out;
    out.title = this->title;
    out.samp = this->samp;
    out.step = this->step;
    out.data = std::vector<float>(index(wl_start), index(wl_stop) + 1);
    out.start = wl_start;

    return out;
}
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
