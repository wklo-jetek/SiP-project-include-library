#include "extcode.h"
#ifdef __cplusplus
extern "C" {
#endif

/*!
 * LibDatalogIns_Bridge
 */
void __cdecl LibDatalogIns_Bridge(uint32_t bridge_num);
/*!
 * LibDatalogIns_LotEnd
 */
void __cdecl LibDatalogIns_LotEnd(void);
/*!
 * LibDatalogIns_create_ptr
 */
void __cdecl LibDatalogIns_create_ptr(uint32_t TEST_NUM, char TEST_TXT[], 
	char UNITS[]);
/*!
 * LibDatalogIns_log_dbl
 */
void __cdecl LibDatalogIns_log_dbl(uint8_t SITE_NUM, double result, 
	double lo, double hi, uint8_t fail, uint8_t nospec);
/*!
 * LibDatalogIns_write_log
 */
void __cdecl LibDatalogIns_write_log(void);
/*!
 * LibDatalogIns_log_int
 */
void __cdecl LibDatalogIns_log_int(uint8_t SITE_NUM, int32_t result, 
	int32_t lo, int32_t hi, uint8_t pf, uint8_t nospec);
/*!
 * LibDatalogIns_log_str
 */
void __cdecl LibDatalogIns_log_str(uint8_t SITE_NUM, char result[], 
	char lo[], char hi[], uint8_t pf, uint8_t nospec);
/*!
 * LibDatalogIns_lv_sot
 */
void __cdecl LibDatalogIns_lv_sot(void);
/*!
 * LibDatalogIns_lv_eot
 */
void __cdecl LibDatalogIns_lv_eot(void);

MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExecuteVIsInPrivateExecutionSystem(Bool32 value);

#ifdef __cplusplus
} // extern "C"
#endif

