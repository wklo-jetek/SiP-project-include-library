#include "extcode.h"
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * LibDataBridge_ReturnRef
 */
uint32_t __cdecl LibDataBridge_ReturnRef(uint32_t bridge_num, 
	char var_name[]);
/*!
 * LibDataBridge_RefToInt
 */
int32_t __cdecl LibDataBridge_RefToInt(uint32_t Reference);
/*!
 * LibDataBridge_RefToBool
 */
uint8_t __cdecl LibDataBridge_RefToBool(uint32_t Reference);
/*!
 * LibDataBridge_RefToStr
 */
void __cdecl LibDataBridge_RefToStr(uint32_t Reference, LStrHandle *Value);
/*!
 * LibDataBridge_RefToDbl
 */
double __cdecl LibDataBridge_RefToDbl(uint32_t Reference);
/*!
 * LibDataBridge_RefFromInt
 */
void __cdecl LibDataBridge_RefFromInt(uint32_t Reference, int32_t Value);
/*!
 * LibDataBridge_RefFromBool
 */
void __cdecl LibDataBridge_RefFromBool(uint32_t Reference, uint8_t Value);
/*!
 * LibDataBridge_RefFromStr
 */
void __cdecl LibDataBridge_RefFromStr(uint32_t Reference, LStrHandle *Value);
/*!
 * LibDataBridge_RefFromDbl
 */
void __cdecl LibDataBridge_RefFromDbl(uint32_t Reference, double Value);

MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExecuteVIsInPrivateExecutionSystem(Bool32 value);

#ifdef __cplusplus
} // extern "C"
#endif

