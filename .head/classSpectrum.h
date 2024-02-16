#ifndef CLASS_SPECTRUM
#define CLASS_SPECTRUM

#include <string>
#include <vector>

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
    PT peak();
    PT cent(double wl);
};

#endif //* CLASS_SPECTRUM
