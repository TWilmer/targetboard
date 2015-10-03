Targetboard

Warning1: v1 of the board has a bug in the VDD/VSS connection of the CAN drivers. To fix the board lift the power supply pins of the can transceiver chips
Warning2: v1 of the board has a bug in IR receiver footprint pin 1 and 3 are mixed up
Warning3: v1 of the board has the USB connector too near to the Serial Port


What works:
- IR sender
- Leds
- Button
- Serial Port 3
- WIFI port (no SW support yet)
- JTAG, I'm using a recent openocd on RPi B+




this is my hobby project. It supports a lot of features where I want me an my daughter to play with:
- Simulate a Traffic light
- USB to Serial communication
- USB to CAN-Bus (High-Speed only), you need a second board to make some use
- 4 motor drivers
- JTAG probe

The software part is released under GPL2 additionally you have to follow
the original Software's license - e.g. the firmware may be used only
for LPC parts. To compile you need also to install the LPCopen deliver for LPC1769 devices.

The CAD Files are released under GPL2, too. It contains a ZIP
file, which can be used to oder the PCB and if you want also assembly
from SeedStudio - including a proper BOM list.

http://www.seeedstudio.com/service/index.php?r=pcb

So far I have not received any Assembled PCB - so everything is at
your own risk.

This board will use a LPC1768 - as this is availble at SeedStudio, but a LPC1769 should work, too.  A higher clock frequency is possible with LPC1769, but I don't plan to use it.


A set of two assembled board cost around 140$, where you get 3 blank PCBs extra. As SeedStudio is assemblying manually - it should be possible to populate this board also manually.
