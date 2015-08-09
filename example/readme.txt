Based on the USB to UART bridge example

dfu-util -d 0x471:0xdf55 -c 0 -t 2048 -R -D /usr/local/lpcxpresso_7.5.0_254/lpcxpresso/bin/LPCXpressoWIN.enc
dfu-util -d 0x471:0xdf55 -c 0 -t 2048 -R -D  /usr/local/lpcxpresso_7.7.2_379/lpcxpresso/bin/LPCXpressoWIN.enc
This program is for the lpcexpresso Board. To be connected:

up to two can transceiver:
 - P0.4 and P0.5
 - P0.1 and P0.2
 
 one USB to Serial Converter at UART 0 - may also be used for intial programming i
  - P0.2 and P0.3 for programming P2.10 has to be low
  - P0.15 P0.16 as a connection to the Target
  
  We have up to 16 endpoints, this means we can make use of them:
   2 - UART to UART0
   2 - UART UART2 to Target
   2 - CAN0
   2 - CAN1

   