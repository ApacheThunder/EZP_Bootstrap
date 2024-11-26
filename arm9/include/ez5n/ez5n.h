#ifndef EZ5N_H
#define EZ5N_H

#ifdef __cplusplus
extern "C" {
#endif

#include <nds/arm9/dldi.h>


bool EZ5N_Startup(void);
bool EZ5N_IsInserted(void);
bool EZ5N_ReadSectors(uint32_t sector, uint32_t num_sectors, void* buffer);
bool EZ5N_WriteSectors(uint32_t sector, uint32_t num_sectors, const void* buffer);
bool EZ5N_ClearStatus(void);
bool EZ5N_Shutdown(void);

u32 EZ5N_GetVersion(void);

const DISC_INTERFACE io_ez5n = {
    0x455A354E, // "EZ5N"
    (FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_NDS),
    (FN_MEDIUM_STARTUP)&EZ5N_Startup,
    (FN_MEDIUM_ISINSERTED)&EZ5N_IsInserted,
    (FN_MEDIUM_READSECTORS)&EZ5N_ReadSectors,
    (FN_MEDIUM_WRITESECTORS)&EZ5N_WriteSectors,
    (FN_MEDIUM_CLEARSTATUS)&EZ5N_ClearStatus,
    (FN_MEDIUM_SHUTDOWN)&EZ5N_Shutdown
};


#ifdef __cplusplus
}
#endif

#endif // EZ5N_H

