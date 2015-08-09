CAN driver for LPC1769 Target board
================================================

Note
----
THIS SOFTWARE COMES WITHOUT ANY WARRANTY!


License
-------
GPL v2


How to build
------------
* Make sure you have kernel version >= 2.6.33 installed
* For pre-3.3 kernels, check out the pre-3.3 branch
* Install kernel header files and gcc
* Simple type "make"


DKMS integration
----------------
* Install the dkms package for your distro
* Check out the required branch from this repository
* Do a git archive --prefix=cantwi-1.0/ -o /usr/src/cantwi-1.0.tar HEAD
* Extract the archive in the /usr/src directory
* Add the module, e.g. with dkms add -m cantwi -v 1.0 --verbose
* Build the module, e.g. with dkms build -m cantwi -v 1.0 --verbose
* Install the module, e.g. with dkms install -m cantwi -v 1.0 --verbose
* You can also build packages for your distro, see man dkms


Set up interface
----------------
    modprobe can_raw
    modprobe can_dev
    insmod can_twi.ko
    ip link set can0 up type can bitrate 1000000 sample-point 0.875


Shut down interface
-------------------
    ip link set can0 down
    rmmod can_twi
