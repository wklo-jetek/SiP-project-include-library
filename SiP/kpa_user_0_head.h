// ------------------------- //
//     include head list     //
// ------------------------- //

#include <ctime>
#include <direct.h>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <ostream>
#include <sstream>
#include <thread>
#include <windows.h>

// * Define include library
#include "ItemReader_LV20.h"
#include "SiPhDriver.h"
#include "format.h"
#include "ftd2xx.h"
#include "libCM300_INS.h"
#include "libSiPhSmu.h"
#include "libTS300_ins.h"

#define EN_SPECTRUM_LV_OBJECT

#ifdef EN_SPECTRUM_LV_OBJECT
#include "libSpectrumAnalysis.h"
#endif

// * Define physical unit (_kpa_si_unit)
_kpa_si_unit(A);
_kpa_si_unit(V);
_kpa_si_unit(S);
_kpa_si_unit(Hz);
_kpa_si_unit(m);
