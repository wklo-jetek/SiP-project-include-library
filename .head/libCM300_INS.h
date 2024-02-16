#include "extcode.h"
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * LibCM300_INS_Init
 */
void __cdecl LibCM300_INS_Init(uint32_t bridge_num);
/*!
 * StepNextSubdie
 */
void __cdecl StepNextSubdie(int32_t subdie);
/*!
 * LibCM300_INS_Term
 */
void __cdecl LibCM300_INS_Term(void);
/*!
 * LibCM300_INS_Write
 */
void __cdecl LibCM300_INS_Write(char str_in[]);
/*!
 * LibCM300_INS_Read
 */
int32_t __cdecl LibCM300_INS_Read(char str_out[], int32_t len, 
	int32_t timeout_ms);

MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExecuteVIsInPrivateExecutionSystem(Bool32 value);

#ifdef __cplusplus
} // extern "C"
#endif

