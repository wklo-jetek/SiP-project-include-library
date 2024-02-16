#include "extcode.h"
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * ItemReader_Initialize
 */
void __cdecl ItemReader_Initialize(char ProjectPath[]);
/*!
 * ItemReader_Term
 */
void __cdecl ItemReader_Term(void);
/*!
 * ItemReader_NextFunction
 */
int8_t __cdecl ItemReader_NextFunction(char func_out[], int32_t datasize);
/*!
 * ItemReader_Get_Data
 */
void __cdecl ItemReader_Get_Data(uint8_t data[], int32_t len);
/*!
 * ItemReader_FunctionOutset
 */
void __cdecl ItemReader_FunctionOutset(int32_t Reload);
/*!
 * LibItemReader_SetRecipe
 */
void __cdecl LibItemReader_SetRecipe(char Recipe[]);
/*!
 * LibItemReader_SetEvent
 */
void __cdecl LibItemReader_SetEvent(char Event[]);

MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExecuteVIsInPrivateExecutionSystem(Bool32 value);

#ifdef __cplusplus
} // extern "C"
#endif

