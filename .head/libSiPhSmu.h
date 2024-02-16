#include "extcode.h"
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * LibSmuNi_Init
 */
int32_t __cdecl LibSmuNi_Init(char proj_path[]);
/*!
 * LibSmuNi_Term
 */
void __cdecl LibSmuNi_Term(void);
/*!
 * LibSmuNi_SetActsite
 */
void __cdecl LibSmuNi_SetActsite(char actsite[]);
/*!
 * LibSmuNi_ForceV
 */
void __cdecl LibSmuNi_ForceV(char pin_name[], double v_level, double i_limit, 
	double v_range, double i_range);
/*!
 * LibSmuNi_ForceI
 */
void __cdecl LibSmuNi_ForceI(char pin_name[], double i_level, double v_limit, 
	double i_range, double v_range);
/*!
 * LibSmuNi_Meas
 */
void __cdecl LibSmuNi_Meas(char pin_name[], double v_meas[], int32_t len_v, 
	double i_meas[], int32_t len_i);
/*!
 * LibSmuNi_Enable
 */
void __cdecl LibSmuNi_Enable(char pin_name[]);
/*!
 * LibSmuNi_Disable
 */
void __cdecl LibSmuNi_Disable(char pin_name[]);
/*!
 * LibSmuNi_VLevel
 */
void __cdecl LibSmuNi_VLevel(char pin_name[], double v_level);
/*!
 * LibSmuNi_ILevel
 */
void __cdecl LibSmuNi_ILevel(char pin_name[], double i_level);

MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExecuteVIsInPrivateExecutionSystem(Bool32 value);

#ifdef __cplusplus
} // extern "C"
#endif

