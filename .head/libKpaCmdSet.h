#include "extcode.h"
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Pause
 */
void __cdecl Pause(char message[]);
/*!
 * LoopRun
 */
uint8_t __cdecl LoopRun(char Message[]);
/*!
 * LibCppCmdSet_Bridge
 */
void __cdecl LibCppCmdSet_Bridge(uint32_t bridge_num);
/*!
 * LibCppCmdSet_sot
 */
void __cdecl LibCppCmdSet_sot(void);
/*!
 * LibCppCmdSet_NextGroup
 */
int32_t __cdecl LibCppCmdSet_NextGroup(char group[], int32_t len);
/*!
 * LibCppCmdSet_NextItem
 */
int32_t __cdecl LibCppCmdSet_NextItem(char group[], int32_t len);

MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExecuteVIsInPrivateExecutionSystem(Bool32 value);

#ifdef __cplusplus
} // extern "C"
#endif

