# EZP Dual Storage HBMenu

This is a modified build of HBMenu with UI enhancements but setup to act as a kernel/firmware replacement to EZFlash Parellel.
This can not be used on other carts and is targed specifically for this cart and is designed to be flashed to it via EZP Recovery tool. 

The program will change boot behavior based on what button is held on boot:

* Holding no button on boot will show file browser.
* Holding A button will auto boot into GBA-Exploader. Specifically a new custom version with SuperCard and EZFlash Omega support.
* Holding B button will reboot console into GBA mode with gbaframe.bmp as the frame graphic.
* Holding Y button will boot GodMode9i.
* Holding X button will boot MaxMediaDock's slot1 rom for booting into MaxMediaDock devices on slot-2.

Also if an N-Card or N-Card clone USB slot-2 device is present on boot, the program will auto boot into nrio-usb-disk. A open source
file transfer program that makes use of the USB slot-2 device to connect flashcart's MicroSD storage to PC.

The repo this build of nrio-usb-disk can be found at: https://github.com/ApacheThunder/nrio-usb-disk
The original repo this build was forked from: https://github.com/asiekierka/nrio-usb-disk

This new app was created courtasy of Asiekirka.


Currently the gbaframe loader is setup to load GBA-Exploader compatible BMP files. Currently only setup to load frames from internal flash fat image.
Please refer to the fat image in NitroFS folder for that file if you wish to customize this.

This menu has a special feature in the filebrowser. You will notice that on boot it will show the file listing for the files found inside internal FAT image
stored in NitroFS. This is made possible with a special "NTRO" dldi driver that I wrote. This allows this cart to boot without a MicroSD card inserted!

You can find my source to that driver here:

https://github.com/ApacheThunder/DLDI/tree/master/source/ntro

However some actions will of coarse require the presense of a MicroSD card. (some of the included apps depend on that)

Assuming a MicroSD card is present on boot you can use shoulder buttons to navigate to MicroSD storage when wanting to boot files from MicroSD.


# Notes on how the EZFLash BIN file is constructed

The layout for the BIN file to be flashed via EZP Recovery Tool is reletively simple compared to other flashcarts.
This card does not support ntrboot due to hardcoded blowfish keys however you can flash entire 4MB flash chip with standard save commands.

EZP Recovery Tool can be found here: https://github.com/ApacheThunder/EZP_Recovery_Tool

This is a upgraded fork of NDS Backup Tool by Rudolph.
The full open sourced fork I've made for this tool can be found here: https://github.com/ApacheThunder/NDS_BACKUP_TOOL


As with other flashcarts. Arm9 secure area must be encrypted (though this program doesn't really use that region for anything).
Game code used in header is hardcoded as well due to hardcoded blowfish keys so do not attempt to change that.

Data at 0x1000 mostly pertain to the ntrboot payload this flashcart uses on 3DS.
Though the data found at 0x1200 is a copy of data found at found at 0x108400. (during normal rom reads)

EZP for some reason mirrors this data from 0x1200 into 0x108400 when rom reads happen instead of presenting any data orginally found there in the flash.

This is easy to account for. Anytime you build a custom bin file for this cart, just copy a 0x400 chunk of data from 0x108400 over to 0x1200.

It is possible data at 0x108400 instead appears else where in regions beyond the 4MB range of the flash chip.
(or it may have put it's exploit here and swaps it in after passing the white list checks)

But it's beyond the real data range of the internal flash chip so really not relevent anymore if you are using a custom rom.

I have not tested if the cart still functions correctly with the 3DS ntrboot stuff missing. I would recommend ensuring this is still present.

Fortunately though this cart can't really be bricked if you flash bad data to it. (I've ensured you can still use EZP Recovery Tool regardless of what state it's in).

Do note though if you have run official firmware v1.06 or newer on this card, 0x0 to 0x7FFF is write protected.

To undo this you must desolder WP# of eeprom flash chip. While it's lifted, run EZP_Recovery_Tool and perform a save initialize via the restore menu.

Doing so will make the recovery tool run a special command to reset the write protection registers on the chip which can't be done without WP# pin being lifted.

However from my tests the cart won't mount/read the flash contents while it's in this state. So after doing the save intialize to trigger the reset of write protection,
you must solder it back down. Then go back into EZP Recovery Tool and flash a new bin file. This should now properly write the intended data to the first couple of blocks.

Do not run official fw v1.06 or newer on the cart again in the future. Doing so will require a repeat of this process if you want to flash a new custom rom to it agian.


# License
Note: While the GPL license allows you to distribute modified versions of this program it would be appreciated if any improvements are contributed to devkitPro. Ultimately the community as a whole is better served by having a single official source for tools, applications and libraries.

The latest sources may be obtained from devkitPro git using the command: `git clone git@github.com:devkitPro/nds-hb-menu.git`

```
 Copyright (C) 2005 - 2017
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy

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
 ```
