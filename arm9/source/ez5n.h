#ifndef EZ5N_H
#define EZ5N_H

#ifdef __cplusplus
extern "C" {
#endif

bool ez_startup(void);
bool ez_is_inserted(void);
bool ez_read_sectors(uint32_t sector, uint32_t num_sectors, void *buffer);
bool ez_write_sectors(uint32_t sector, uint32_t num_sectors, const void *buffer);
bool ez_clear_status(void);
bool ez_shutdown(void);

const DISC_INTERFACE io_ez5n = {
    0x455A354E, // "EZ5N"
    (FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_SLOT_NDS),
    (FN_MEDIUM_STARTUP)&ez_startup,
    (FN_MEDIUM_ISINSERTED)&ez_is_inserted,
    (FN_MEDIUM_READSECTORS)&ez_read_sectors,
    (FN_MEDIUM_WRITESECTORS)&ez_write_sectors,
    (FN_MEDIUM_CLEARSTATUS)&ez_clear_status,
    (FN_MEDIUM_SHUTDOWN)&ez_shutdown
};

#ifdef __cplusplus
}
#endif

#endif // EZ5N_H