#include "extcode.h"
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * LibTS300_Init
 */
void __cdecl LibTS300_Init(uint32_t bridge_num);
/*!
 * LibTS300_Term
 */
void __cdecl LibTS300_Term(void);
/*!
 * LibTS300_Write
 */
void __cdecl LibTS300_Write(char buf_in[], int32_t timeout_ms);
/*!
 * LibTS300_Read
 */
int32_t __cdecl LibTS300_Read(char buf_out[], int32_t len, 
	int32_t timeout_ms);

MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExecuteVIsInPrivateExecutionSystem(Bool32 value);

#ifdef __cplusplus
} // extern "C"
#endif

