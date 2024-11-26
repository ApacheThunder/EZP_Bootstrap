// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2023
// SPDX-FileContributor: lifehackerhansol, 2024

#include <nds/ndstypes.h>

#include "ioezp.h"
#include "libtwl_card.h"

u8 TestBuffer[512];

// Initialize the driver. Returns true on success.
bool EZ5N_Startup(void) {
	if ((EZ5N_CardReadChipID() & 7) == 2)return EZ5N_SDReadSector0Test((void*)TestBuffer);
	return false;
}

// Returns true if a card is present and initialized.
bool EZ5N_IsInserted(void) {
    if ((EZ5N_CardReadChipID() & 7) == 2)return EZ5N_SDReadSector0Test((void*)TestBuffer);
	return false;
}

// Reads 512 byte sectors into a buffer that may be unaligned. Returns true on
// success.
bool EZ5N_ReadSectors(uint32_t sector, uint32_t num_sectors, void* buffer) {
    EZ5N_SDReadSectors(sector, num_sectors, buffer);
    return true;
}

// Writes 512 byte sectors from a buffer that may be unaligned. Returns true on
// success.
bool EZ5N_WriteSectors(uint32_t sector, uint32_t num_sectors, const void* buffer) {
    EZ5N_SDWriteSectors(sector, num_sectors, buffer);
    return true;
}

// Clear error flags from the card. Returns true on success.
bool EZ5N_ClearStatus(void) {
    return true;
}

// Shutdowns the card. This may never be called.
bool EZ5N_Shutdown(void) {
    return true;
}

