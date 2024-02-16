#include "extcode.h"
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * LibSiPhDriver_Init
 */
int32_t __cdecl LibSiPhDriver_Init(uint8_t deviceAddr, uint32_t *reference);
/*!
 * LibSiPhDriver_Term
 */
void __cdecl LibSiPhDriver_Term(uint32_t reference);
/*!
 * LibSiPhDriver_WriteStr
 */
void __cdecl LibSiPhDriver_WriteStr(uint32_t reference, char String[], 
	int32_t timeout_ms);
/*!
 * LibSiPhDriver_ReadStr
 */
void __cdecl LibSiPhDriver_ReadStr(uint32_t reference, char String[], 
	int32_t readByte, int32_t timeout_ms);
/*!
 * LibSiPhDriver_Init_test
 */
void __cdecl LibSiPhDriver_Init_test(char Output_File[], uint32_t *reference);
/*!
 * LibSiPhDriver_Init_visa
 */
int32_t __cdecl LibSiPhDriver_Init_visa(char resource[], uint32_t *reference);
/*!
 * lv_mkdir
 */
int32_t __cdecl lv_mkdir(char folder_path[]);

MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExecuteVIsInPrivateExecutionSystem(Bool32 value);

#ifdef __cplusplus
} // extern "C"
#endif

