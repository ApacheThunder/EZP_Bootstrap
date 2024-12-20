/*-----------------------------------------------------------------
 Copyright (C) 2005 - 2013
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy
	Claudio "sverx"
	Michael "mtheall" Theall

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

------------------------------------------------------------------*/

#include <nds.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>

#include "args.h"
#include "hbmenu_banner.h"
#include "font6x8.h"

#define TITLE_POS_X			(13*8)
#define TITLE_POS_Y			(2)
#define TITLE_POS_OFFSET_Y	(10)

#define ICON_POS_X	26
// #define ICON_POS_Y	79
#define ICON_POS_Y	81

#define TEXT_WIDTH				((22-4)*8/6)
#define TEXT_WIDTH_VERSTRING	((26-4)*8/6)

#define VERSTRING_LENGTH 26

static int bg2, bg3;
static u16 *sprite;
static tNDSBanner banner;

// static char EZPHardwareVersion[11] = { 'H', 'W', ' ', 'V', 'e', 'r', ':', ' ', '0', '0', '\0' };
static char EZPHardwareVersion[VERSTRING_LENGTH];
static u32 EZPHWVerBuffer[1] = { 0xFFFFFFFF };

extern DTCM_DATA tNDSBanner hbNoIcon_bin;


static inline void writecharRS (int row, int col, u16 car) {
	// get map pointer
	u16 *gfx   = bgGetMapPtr(bg2);
	// get old pair of values from VRAM
	u16 oldval = gfx[row*(512/8/2)+(col/2)];

	// clear the half we will update
	oldval &= (col%2) ? 0x00FF : 0xFF00;
	// apply the updated half
	oldval |= (col%2) ? (car<<8) : car;

	// write back to VRAM
	gfx[row*(512/8/2)+col/2] = oldval;
}

// Modified to use TITLE_POS_OFFSET_Y as offset since ver string needs to be printed at 0 position outside of main text box.
static inline void writeRow (int rownum, const char* text) {
	int i, len, r = (rownum + TITLE_POS_OFFSET_Y), p = 0;
	len=strlen(text);

	if (len>TEXT_WIDTH)len=TEXT_WIDTH;

	// clear left part
	for (i=0;i<(TEXT_WIDTH-len)/2;i++)writecharRS (r, i, 0);

	// write centered text
	for (i=(TEXT_WIDTH-len)/2;i<((TEXT_WIDTH-len)/2+len);i++)writecharRS (r, i, text[p++]-' ');

	// clear right part
	for (i=((TEXT_WIDTH-len)/2+len);i<TEXT_WIDTH;i++)writecharRS (r, i, 0);
}

static inline void writeVerString (const char* text) {
	int i,len,p=0;
	len=strlen(text);

	if (len>TEXT_WIDTH_VERSTRING)len=TEXT_WIDTH_VERSTRING;

	// clear left part
	for (i=0;i<(TEXT_WIDTH_VERSTRING-len)/2;i++)writecharRS (0, i, 0);

	// write centered text
	for (i=(TEXT_WIDTH_VERSTRING-len)/2;i<((TEXT_WIDTH_VERSTRING-len)/2+len);i++)writecharRS (0, i, text[p++]-' ');

	// clear right part
	for (i=((TEXT_WIDTH_VERSTRING-len)/2+len);i<TEXT_WIDTH_VERSTRING;i++)writecharRS (0, i, 0);
}

static inline void clearIcon (void) { dmaFillHalfWords(0, sprite, sizeof(banner.icon)); }

void iconTitleInit (void) {
	// initialize video mode
	videoSetMode(MODE_4_2D);

	// initialize VRAM banks
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_SPRITE);

	// initialize bg2 as a rotation background and bg3 as a bmp background
	// http://mtheall.com/vram.html#T2=3&RNT2=96&MB2=3&TB2=0&S2=2&T3=6&MB3=1&S3=1
	bg2 = bgInit(2, BgType_Rotation, BgSize_R_512x512,   3, 0);
	bg3 = bgInit(3, BgType_Bmp16,    BgSize_B16_256x256, 1, 0);

	// initialize rotate, scale, and scroll
	bgSetRotateScale(bg3, 0, 1<<8, 1<<8);
	bgSetScroll(bg3, 0, 0);
	bgSetRotateScale(bg2, 0, 8*(1<<8)/6, 1<<8);
	bgSetScroll(bg2, -TITLE_POS_X, -TITLE_POS_Y);

	// clear bg2's map: 512x512 pixels is 64x64 tiles is 4KB
	dmaFillHalfWords(0, bgGetMapPtr(bg2), 4096);
	// load compressed font into bg2's tile data
	decompress(font6x8Tiles, bgGetGfxPtr(bg2), LZ77Vram);

	// load compressed bitmap into bg3
	decompress(hbmenu_bannerBitmap, bgGetGfxPtr(bg3), LZ77Vram);

	// load font palette
	dmaCopy(font6x8Pal, BG_PALETTE, font6x8PalLen);

	// apply the bg changes
	bgUpdate();

	// initialize OAM
	oamInit(&oamMain, SpriteMapping_1D_128, false);
	sprite = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_16Color);
	dmaFillHalfWords(0, sprite, sizeof(banner.icon));
	oamSet(&oamMain, 0, ICON_POS_X, ICON_POS_Y, 0, 0,
	       SpriteSize_32x32, SpriteColorFormat_16Color, sprite,
	       -1, 0, 0, 0, 0, 0);

	// oam can only be updated during vblank
	swiWaitForVBlank();
	oamUpdate(&oamMain);

	// Load Default Icon.
	DC_FlushAll();
	dmaCopy(hbNoIcon_bin.icon,    sprite,         sizeof(hbNoIcon_bin.icon));
	dmaCopy(hbNoIcon_bin.palette, SPRITE_PALETTE, sizeof(hbNoIcon_bin.palette));

	// everything's ready :)
	writeRow (1,"===>>> HBMenu+ <<<===");
}

static void loadDefaultIcon() {
	DC_FlushAll();
	dmaCopy(hbNoIcon_bin.icon,    sprite,         sizeof(hbNoIcon_bin.icon));
	dmaCopy(hbNoIcon_bin.palette, SPRITE_PALETTE, sizeof(hbNoIcon_bin.palette));
}


void iconTitleUpdate (int isdir, const std::string& name) {
	
	if (EZPHWVerBuffer[0] == 0xFFFFFFFF) {
		cardParamCommand (0x3E, 0, CARD_ACTIVATE | CARD_nRESET | CARD_BLK_SIZE(7) | CARD_SEC_CMD | CARD_DELAY2(24) | CARD_SEC_EN | CARD_SEC_DAT, (u32*)EZPHWVerBuffer, 1);
		if (EZPHWVerBuffer[0] != 0xFFFFFFFF) {
			for (int i = 0; i < VERSTRING_LENGTH; i++)EZPHardwareVersion[i] = '\0';
			sprintf(EZPHardwareVersion, "          EZP HW VER   %02X", (u8)EZPHWVerBuffer[0]);
			writeVerString(EZPHardwareVersion);
		}
	}
	
	writeRow (0, name.c_str());
	writeRow (1, "");
	writeRow (2, "");
	writeRow (3, "");

	if (isdir) {
		// text
		writeRow (2, "[directory]");
		// icon
		clearIcon();
		loadDefaultIcon();
	} else {
		std::string ndsPath;
		if (!argsNdsPath(name, ndsPath)) {
			writeRow(2, "(invalid argv or NDS file!)");
			clearIcon();
			loadDefaultIcon();
			return;
		}

		unsigned int Icon_title_offset;

		// open file for reading info
		FILE *fp = fopen (ndsPath.c_str(), "rb");

		if (!fp) {
			// text
			writeRow (2,"(can't open file!)");
			// icon
			clearIcon();
			loadDefaultIcon();
			fclose (fp);
			return;
		}

		if (fseek (fp, offsetof(tNDSHeader, bannerOffset), SEEK_SET) != 0 || fread (&Icon_title_offset, sizeof(int), 1, fp) != 1) {
			// text
			writeRow (2, "(can't read file!)");
			// icon
			clearIcon();
			loadDefaultIcon();
			fclose (fp);
			return;
		}

		if (Icon_title_offset == 0) {
			// text
			writeRow (2, "(no title/icon)");
			// icon
			clearIcon();
			loadDefaultIcon();
			fclose (fp);
			return;
		}

		if (fseek (fp, Icon_title_offset, SEEK_SET) != 0 || fread (&banner, sizeof(banner), 1, fp) != 1) {
			// text
			writeRow (2,"(can't read icon/title!)");
			// icon
			clearIcon();
			loadDefaultIcon();
			fclose (fp);
			return;
		}

		// close file!
		fclose (fp);

		// turn unicode into ascii (kind of)
		// and convert 0x0A into 0x00
		char *p = (char*)banner.titles[1];
		int rowOffset = 1;
		int lineReturns = 0;
		for (size_t i = 0; i < sizeof(banner.titles[1]); i = i+2) {
			if ((p[i] == 0x0A) || (p[i] == 0xFF)) {
				p[i/2] = 0;
				lineReturns++;
			} else {
				p[i/2] = p[i];
			}
		}
		
		if (lineReturns < 2)rowOffset = 2; // Recenter if bennar has less 2 or less rows of text maintaining empty row gap between nds file name and nds banner.

		// text
		for (size_t i = 0; i < 3; ++i) {
			writeRow(i+rowOffset, p);
			p += strlen(p) + 1;
		}

		// icon
		DC_FlushAll();
		dmaCopy(banner.icon,    sprite,         sizeof(banner.icon));
		dmaCopy(banner.palette, SPRITE_PALETTE, sizeof(banner.palette));
	}
}

