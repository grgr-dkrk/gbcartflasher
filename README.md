# gbcartflasher

PCB for the ATMega8515 / FT232RL based Gameboy Cart Flasher &amp; some mods to make it work easily under MacOS

## background

We needed to write a small text-based game, like Zork, for the gameboy. We wanted it to run on the original hardware and look like an original cartridge.

In order to flash a cartridge, you need to either mod an existing one by removing the ROM and replacing it with your own flash, but since the software was pretty simple, I figured I should make a PCB for a 32K flash that could be assembled easily. We had to make around 10 copies in a limited time.

One of the simplest and most used Gameboy Cart Flasher is the one designed by Kraku & Chroost. The schematics and software can be found on the [awesome website](http://www.reinerziegler.de/readplus.htm) maintained by Reiner Ziegler.

What you'll find in this repo are the eagle files ready to be sent to a fab house (I used [elecrow](https://www.elecrow.com/pcb-prototyping.html)), plus some instructions and a modified software to make it run easily on MacOS.

## BOM

Full BOM can be found on Mouser :
* [32K Cart](http://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=d0bee9d82e)
* [Cart Flasher](http://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=779e8e7766)
* [GB Connector](http://www.dx.com/p/repair-parts-replacement-gba-game-cart-slot-for-nds-lite-37787)

## notes on PCBs

The Gameboy Cartridge connector can be taken from a broken gameboy, but the easiest way to get one is to buy a Nintendo DS GBA Slot connector and [cutting some little plastic tabs](http://sudomod.com/old_forums/reply/771/) in order to get the original GB cartridges to fit.

In order to fit, the PCB for the cartridge needs to be 0.8mm thick, so be careful when ordering to check that your fab house can make it.

## programming the flasher

To program the ATmega8515, you'll need an ICSP programmer for AVR chips. I used an [USBasp](http://www.fischl.de/usbasp/) clone.

Using avrdude, fuses need to be set as low 0x1F and high 0xc9

```
avrdude -P usb -c usbasp -p ATmega8515 -qq -B 8 -U lfuse:w:0x1f:m -U hfuse:w:0xc9:m
````

I programmed my flasher using the .hex file [found here](http://www.reinerziegler.de/readplus.htm#GB_Flasher). Be careful to flash the USB version.

```
avrdude -P usb -c usbasp -p ATmega8515 -qq -B 8 -U flash:w:gbcf-fw-2.1-usb.hex
```

The firmware has been [rewritten in C in 2015 by Tauwasser](https://github.com/Tauwasser/GBCartFlasher), but I haven't tested that version personally.

## host software

On mac, you'll need to install Qt6 & libftdi :

```
brew install qt libftdi
```

Then, in the gbcflsh_1.1_libftdi folder :

```
qmake && make
```

You should find a gbcflsh.app in the release folder.

### references
 * [jrodrigo gb cart flasher](http://www.jrodrigo.net/es/project/gameboy-cart-flasher/)