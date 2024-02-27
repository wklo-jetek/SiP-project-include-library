#include "extcode.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef uint16_t  Enum;
#define Enum_Raw 0
#define Enum_CosineFit 1

/*!
 * NewData
 */
uint32_t __cdecl NewData(double wl_start_nm, double wl_step_nm, 
	float power[], int32_t len);
/*!
 * SpOpenFile
 */
int32_t __cdecl SpOpenFile(char filepath[]);
/*!
 * NewDataFromFile
 */
uint32_t __cdecl NewDataFromFile(void);
/*!
 * DeleteData
 */
void __cdecl DeleteData(uint32_t reference);
/*!
 * FindPeak
 */
void __cdecl FindPeak(uint32_t reference, double *wavelength, double *power);
/*!
 * FindWaveLength
 */
double __cdecl FindWaveLength(uint32_t reference, double wavelength);
/*!
 * FindFsrDips
 */
void __cdecl FindFsrDips(uint32_t reference, double wavelengths[], 
	int32_t len);
/*!
 * FindFsrPeaks
 */
void __cdecl FindFsrPeaks(uint32_t reference, double wavelengths[], 
	int32_t len);
/*!
 * CompareDifference
 */
double __cdecl CompareDifference(uint32_t target_ref, uint32_t compare_ref);
/*!
 * SysCleanMemory
 */
void __cdecl SysCleanMemory(void);
/*!
 * SysModeMovingAverage
 */
void __cdecl SysModeMovingAverage(uint32_t MovingAverage);
/*!
 * SysModeRaw
 */
void __cdecl SysModeRaw(void);
/*!
 * LoadSpectrumFromFolder
 */
int16_t __cdecl LoadSpectrumFromFolder(char folderpath[], int32_t len);
/*!
 * NextPlot
 */
uint32_t __cdecl NextPlot(void);
/*!
 * GetPlotInfo
 */
void __cdecl GetPlotInfo(uint32_t reference, int32_t *X, int32_t *Y, 
	char dev_name[], int32_t len);
/*!
 * ReadData
 */
void __cdecl ReadData(uint32_t reference, double *wl_start_nm, 
	double *wl_step_nm, float power[], int32_t len);
/*!
 * GetSubdie
 */
void __cdecl GetSubdie(uint32_t reference, char subdie[], int32_t len);
/*!
 * FindBW
 */
double __cdecl FindBW(uint32_t reference, double loss);
/*!
 * FindFsrCent
 */
double __cdecl FindFsrCent(uint32_t reference, double cent_wl, Enum Datamode);
/*!
 * FindFwhmCent
 */
void __cdecl FindFwhmCent(uint32_t reference, double cent_wl, double *R_FWHM, 
	double *Fit_FWHM);
/*!
 * ALGOCosFit
 */
void __cdecl ALGOCosFit(uint32_t reference, double cent_wl);

MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExecuteVIsInPrivateExecutionSystem(Bool32 value);

#ifdef __cplusplus
} // extern "C"
#endif

