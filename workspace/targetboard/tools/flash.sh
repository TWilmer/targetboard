arm-none-eabi-objcopy -O binary ../Debug/targetboard ../Debug/targetboard.bin
cp ../Debug/targetboard.bin ../Debug/targetboard.org
if [ ! -r ./lpc-vector-checksum ]
 
then
pwd
gcc -o lpc-vector-checksum ../tools/lpc-vector-checksum.c
fi
./lpc-vector-checksum ../Debug/targetboard.bin
arm-none-eabi-objcopy -I binary -O ihex ../Debug/targetboard.bin ../Debug/targetboard.hex 
scp   ../Debug/targetboard.hex  pi@192.168.3.3:/tmp
scp   ../Debug/targetboard  pi@192.168.3.3:/tmp
scp   ../Debug/targetboard.bin  pi@192.168.3.3:/tmp
