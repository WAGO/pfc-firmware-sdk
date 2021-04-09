***The open source software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.***

# ..:: Attention ::.. 
## For FW18 a new toolchain is required!
Follow the steps in chapter "Install "cross-toolchain" also
check the installation path.

# Install WAGO-PFC-SDK on Ubuntu 16.04.5 (64bit) LTS

This HowTo describes steps to install the Software-Development-Kit (SDK) for PFC's on Ubuntu16.04

The start into the embedded linux world requires substantial technical know-how to have success. 
Therefore WAGO recommends familiarity with the following topics for the future embedded linux developer:
- Linux-OS
- working on command line interface(CLI)
- writing Makefile, C or C++ code, shell-scripts.
- Cross-Platform-Development.

Helpful when heard about:
- Realtime operating systems
- PTXdist,
- Eclipse-CDT
- ARM architecture
- Bootloader

We will provide ’ready to use’ toolchains in a binary manner, which saves a lot of time and headaches.
This document describes the installation and usage of the binary toolchain.

The advantages of the ready to use toolchain have to be paid with some dependencies,
which make a uniquely defined host environment mandatory. Ubuntu 16.04 (64bit) fits these requirements,
others may or may not.

# PREREQUISITES
You need a internet connection on your development host. Because during image creation the source file
"nlohmann-json-v3.7.0.tar.gz" will be downloaded automatically.
In case you do not have a internet connetion on your development host. Please refer to section
[ 4.6) Download packages ].

# Installation and building STEP-BY-STEP:

## 0.) Download and setup a virtual or physical Ubuntu 16.04.5 LTS 64bit machine from scratch
You can download the Ubuntu-Iso directly form their website: http://releases.ubuntu.com/16.04/

```
Is Ubuntu 16.04 LTS still supported?

Yes, Ubuntu 16.04 LTS is supported until 2024 through
Canonical’s Extended Security Maintenance (ESM) product.
Xenial enters the ESM period in April 2021, with security
patches provided for an additional three years beyond the 
traditional five-year standard support.
https://ubuntu.com/16-04
```

**Make sure if you like to use the precompiled toolchain that you use the Ubuntu-64bit variant.**

A minimal disk space of 25GB is required. We recommend 50GB.
> For virtual machines on windows x64 hosts:
You may enable virtualization support(Intel VT-x or AMD-V), typical turned on in BIOS or UEFI.

## 1.) Before you clone the repository !!!

Due to the fact that the repository contains files over 50MB you need the **GIT large file support** extension for GIT before you clone the repository.

### 1.1) Download and install GIT

Make sure that you install GIT version >= 1.8.2

_The recommended Ubuntu version will meet the requirement!_

```
    >sudo apt install git
```

### 1.2) Install git-lfs (large file support)

Please refer to this link to get detailed information about the installation on other systems:
https://github.com/git-lfs/git-lfs/wiki/Installation

These steps will only concentrate on the recommented Ubuntu version:
```
    >sudo apt install software-properties-common curl
    >sudo add-apt-repository ppa:git-core/ppa
    >curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
      Detected operating system as Ubuntu/xenial.
      Checking for curl...
      Detected curl...
      Checking for gpg...
      Detected gpg...
      Running apt update... done.
      Installing apt-transport-https... done.
      Installing /etc/apt/sources.list.d/github_git-lfs.list...done.
      Importing packagecloud gpg key... done.
      Running apt update... done.

      The repository is setup! You can now install packages.
    >sudo apt install git-lfs
    >git lfs install
```


### 1.3) Now we are able to clone the repository.
```
    >cd ~
    >mkdir -p wago/ptxproj/
    >cd wago/ptxproj/
    >git clone https://github.com/WAGO/pfc-firmware-sdk.git .
```

## 2.) Install "cross toolchain"
We will provide a ’ready to use’ toolchain in a binary manner.
To install the binary toolchain act as follows:

### 2.1) Clone toolchain to /opt directory
We need to clone the pre-compiled toolchain archive to the storage directory. We expect **/opt**
```
    >sudo mkdir -p /opt/gcc-Toolchain-2019.12/
    >sudo git clone https://github.com/WAGO/gcc-toolchain-2019.12-precompiled.git /opt/gcc-Toolchain-2019.12/
```
>After that the cross toolchain is located into folder: /opt/gcc-Toolchain-2019.12/arm-linux-gnueabihf/bin/

If you are interested in the sources of the toolchain take a look at:
```
http://www.github.com/wago/gcc-toolchain-2019.12
```

## 3.) Get, configure, build and install the build tool "ptxdist"(as normal user)

### 3.1) Install additional required packages for "Ubuntu16.04":
```
    >sudo apt install libncurses5-dev
    >sudo apt install gawk
    >sudo apt install flex
    >sudo apt install bison
    >sudo apt install texinfo
    >sudo apt install python-dev
    >sudo apt install g++
    >sudo apt install dialog
    >sudo apt install libc6-dev
    >sudo apt install lzop       #used to build kernel image, ./configure did not check if installed
    >sudo apt install autoconf   #used to build kernel image, ./configure did not check if installed
    >sudo apt install libtool    #used to build kernel image, ./configure did not check if installed
    >sudo apt install xmlstarlet #used to build led_server package, ./configure did not check if installed
    >sudo apt install xsltproc   #used to build led_server package, ./configure did not check if installed
    >sudo apt install doxygen    #used to build modular-config-tools package, ./configure did not check if installed
    >sudo apt install autopoint  #used to build libmodbus_tglx package
```

or in one-shot:

```
    >sudo apt install libncurses5-dev gawk flex bison texinfo python-dev g++ dialog libc6-dev lzop autoconf libtool xmlstarlet xsltproc doxygen autopoint
```

### 3.2) Clone the build tool "ptxdist" somewhere, we expect ~/ptxdist
```
    >git clone http://github.com/wago/ptxdist.git ~/ptxdist
```
### 3.3) Configure build enviroment tool "ptxdist"
```
    >cd ~/ptxdist
    >./configure
```
The output looks like this:

    checking build system type... x86_64-unknown-linux-gnu
    checking host system type... x86_64-unknown-linux-gnu
    checking for ptxdist patches... yes
    checking for gcc... gcc
    checking whether the C compiler works... yes
    checking for C compiler default output file name... a.out
    checking for suffix of executables... 
    checking whether we are cross compiling... no

    ....

    checking for gunzip... /bin/gunzip
    checking for mktemp... /bin/mktemp
    checking for wget... /usr/bin/wget
    checking find version... 4.4.2
    checking for gmake... no
    checking for gnumake... no
    checking for make... /usr/bin/make
    checking for file... /usr/bin/file
    checking for msgfmt... /usr/bin/msgfmt
    checking for gcc... /usr/bin/gcc
    checking for python2.7... /usr/bin/python2.7
    checking whether /usr/bin/python2.7 finds distutils... yes
    checking whether python development files are present... yes
    checking for patch... /usr/bin/patch
    checking whether /usr/bin/patch will work... yes

    configure: creating ./config.status
    config.status: creating Makefile

    ptxdist version 2017.11.1 configured.
    Using '/usr/local' for installation prefix.

    Report bugs to ptxdist@pengutronix.de

>If "configure" fails, install missing packages and run configure again.

### 3.4) Build the build environment tool "ptxdist"
```
    >cd ~/ptxdist
    >make
```

The output looks like this:

    building conf and mconf ...
    make[1]: Betrete Verzeichnis '/home/wago/Downloads/ptxdist-2013.03.0/scripts/kconfig'
    ...
    gcc -g -O2  -DCURSES_LOC="<curses.h>" -DKBUILD_NO_NLS -DPACKAGE='"ptxdist"' -DCONFIG_='"PTXCONF_"' -c nconf.gui.c -o nconf.gui.o
    gcc nconf.o zconf.tab.o nconf.gui.o  -o nconf -lncurses -lmenu -lpanel
    make[1]: Verlasse Verzeichnis '/home/wago/Downloads/ptxdist-2013.03.0/scripts/kconfig'
    done.
    preparing PTXdist environment ... done

### 3.5) Install the build environment tool "ptxdist"
```
    >cd ~/ptxdist
    >sudo make install
```

>Default setup installs build environment to folder: /usr/local/lib/ptxdist-2017.11.1/bin/ptxdist
>
>Default setup also creates a sym-link to it in: /usr/local/bin/ptxdist

---

## 4) Configure the "project environment"

### 4.1) Select "software config" we want to build
```
    >cd ~/wago/ptxproj
    >ptxdist select configs/wago-pfcXXX/ptxconfig_generic
    info: selected ptxconfig:
        'configs/wago-pfcXXX/ptxconfig_generic'
```

### 4.2) Select "hardware platform" to dial with
```
    >cd ~/wago/ptxproj
    >ptxdist platform configs/wago-pfcXXX/platformconfig
    info: selected platformconfig:
        'configs/wago-pfcXXX/platformconfig'

    info: insufficient information in your platformconfig file
        please use 'ptxdist toolchain </path/to/toolchain>' to select your toolchain
```

### 4.3) Select "toolchain" to use
```
    >cd ~/wago/ptxproj
    >ptxdist toolchain /opt/gcc-Toolchain-2019.12/arm-linux-gnueabihf/bin/
    found and using toolchain:
    'ptxdist toolchain /opt/gcc-Toolchain-2019.12/arm-linux-gnueabihf/bin/'
```

### 4.4) Enter the main menu dialog once
```
    >cd ~/wago/ptxproj
    >ptxdist menu
```
```
    Leave dialog with [Exit]
```

### 4.5) Open the menuconfig dialog once
```
    >cd ~/wago/ptxproj
    >ptxdist menuconfig
```
```
    Leave dialog with [Exit]
```

### 4.6) Download packages
In case you have a internet connection skip this section.
Otherwise follow along.

The source file can be found at:

https://github.com/nlohmann/json/archive/v3.7.0.tar.gz

Download it. Store it to an exchangable storage device of your choise.
After you have downloaded the source file and copied it for example to a USB thumb drive.

On development host:
 * On Ubuntu 16.04 USB drives are mounted automatically.
 * copy the file: nlohmann-json-v3.7.0.tar.gz to the following location
 ```
    ~/<pfc-firmware-sdk-directory/src
 ```
Now you are able to build the firmware image.

## 5) Build the firmware image file "sd.hdimg"

### 5.1) Compile all packages of PFC firmware
```
        >cd ~/wago/ptxproj
        >ptxdist go -q
```
```
    started : ethtool.get
    finished: ethtool.get
    started : ethtool.extract
    finished: ethtool.extract
    started : base.get
    finished: base.get
    started : base.extract
    finished: base.extract
    started : kernel-header.get
    finished: kernel-header.get
    started : kernel-header.extract
    ...
```
>__Now it's probably time to go home for the day.
>Even on reasonably fast machines the time to build the PFC 
>firmware takes from 30 minutes up to a few hours.
>Another possibility is to read "How to become a PTXdist Guru"
>http://public.pengutronix.de/software/ptxdist/appnotes/
>in the meantime.__

### 5.2) Build PFC firmware image "sd.hdimg"
```
    >cd ~/wago/ptxproj
    >ptxdist images
```
Afterwards you should find the firmware image "sd.hdimg" in folder
   **~/wago/ptxproj/platform-wago-pfcXXX/images/ .**


## 6.) Write the binary image file "sd.hdimg" to SD-Card
ATTENTION: Size of generated 'sd.hdimg' has been changed to 204MB.

### 6.1) Virtual machine on a windows host

When working in a virtual machine on a windows host and you don't feel
familar or save using dd on the virtual machine. Just follow the follwing steps:
- Copy firmware image "sd.hdimg" to your windows host
- Download, install and use "Win32 Disk imager.exe"...

### 6.2) For physical Ubuntu16 hosts follow the next steps.

- Disable desktops "automount-open" feature
```
    >gsettings set org.gnome.desktop.media-handling automount true
    >gsettings set org.gnome.desktop.media-handling automount-open false
```

- Identify SD-Card \
In this example we will identify **/dev/sde** as out sd card device.
```
    >mount
    /dev/sda1 on / type ext4 (rw,errors=remount-ro)
    proc on /proc type proc (rw,noexec,nosuid,nodev)
    sysfs on /sys type sysfs (rw,noexec,nosuid,nodev)
    ....
    /dev/sde1 on /media/BOOT type vfat (rw,nosuid,nodev,uid=1000,gid=1000,shortname=mixed,dmask=0077,utf8=1,showexec,flush,uhelper=udisks)
    /dev/sde2 on /media/disk type ext3 (rw,nosuid,nodev,uhelper=udisks)

    >umount /dev/sde1
    >umount /dev/sde2
```
- Copy "sd.hdimg" to SD-Card
```
    >cd <workspace>/ptxproj/platform-wago-pfcXXX/images
```
Identify your sd card device, this will be needed as parameter for dd. 'of=[/dev/[sd card device].`\
We will use /dev/sde for example.
```
    >sudo dd if=sd.hdimg of=/dev/sde 
    [sudo] password for <user>:
    399360+0 Datasets in
    399360+0 Datasets out
    204427320 Bytes (204 MB) copied, 73.5553 s, 2.8 MB/s
```

## 7.) Boot PFC with custom image "sd.hdimg" on SD-Card
PFC boot device order checks SD-Card slot before internal NAND.\
By default, PFC tries to find a DHCP-Server to retrieve an IP-Address.

1. PowerOff PFC
2. Insert SD-Card with custom-image
3. PowerOn PFC

## 8.) Default settings for PFC

### 8.1) User accounts (usr/pwd):

| User  | Password |
|-------|----------|
| root  | wago |
| admin | wago |
| user  | user |

### 8.2) Hostname 
__"PFCx00-uvwxyz"__ \
Where 'uvwxyz' is the last part of eth0's MAC-ID in capital letters.

### 8.3) Network Interfaces X1, X2: "br0"(Default)
- Operate in "switched-mode"
- DHCP-Client request dynamic Ip-Address from DHCP Server.
- SSH-Daemon is enabled.
- https-Server is enabled.

### 8.4) Service Interface
Prepared to dial with:
- "WAGO IO-Check.exe" version 5.3.1.17 or higher 
- "WAGO EthernetSettings.exe" version 5.4.1.3 or higher
REQUIRE: WAGO-Service-Cable 750-920 or 750-923

### 8.5) PFC Onboard Serial Interface X3 "/dev/ttyO0"(9p SubD):
Is no more owned by operating system Linux, so it has to be configured to be the console output.\
This can be easily be done via the web-confuguration.

### 8.6) Get in touch with your PFC the first time

- Variant A: Use tool WAGO-IO-Check to set ip address or obtain actual\
A.1) Connect PFC-Service-Interface and PC with Service-Cable 750-920 or 750-923.\
A.2) Start windows tool "WAGO-IO-Check.exe".\
A.3) Configure local serial interface to use\
A.4) Press button [Identify]\
A.5) Move to register card "Network"\

- Variant B: Web-Based-Management(WBM) via https\
B.0) Obtain actual IP address\
B.1) Open WBM in browser(https://<ip or hostname>)\
B.2) Ignore x509 certificate warning and proceed.\
B.3) Now you should be able to view and change settings.\

- Variant C: Command-Line-Interface(CLI) via ssh\
C.0) Obtain actual IP address\
C.1) Start ssh-client, such as putty\
C.2) Open ssh session to PFC <ip or hostname>)\
C.3) Ignore 'PuTTY Security Alert' and proceed.\
C.4) Login as user/password (Default: "root"/"wago")\
C.5) Start WAGO config tool "/usr/sbin/cbm"\

## 9.) Learn more about "ptxdist"
Read "How to become a PTXdist Guru" and other ressources at http://public.pengutronix.de/software/ptxdist/appnotes/


## 10.) Aspects of the daily work
Building a complete firmware image is necessary exactly one time, to extract
and build the whole system. For your daily work, you usually will work on package level.
As usual, you can:
- Rebuild a package
```
    >cd <workspace>/ptxproj/
    >ptxdist clean <pkg-name> && ptxdist targetinstall <pkg-name>
```
- Copy "binaries" somehow by hand into PFC's file system, and make them executable.
- Transfer install-package "<pkg-name>.ipk" somehow into PFC file system and call
```
    >opkg install <pkg-name>.ipk
```
- Utilize Web-Based-Management(WBM) feature "Software-Upload" to upload and apply "<pkg-name>.ipk".
- Use Eclipse-CDT for remote debugging

**CAUTION:\
Every binary or installation package(.ipk) is generated for a specific firmware version,
do not mix up different versions.**

### 10.1) For larger projects/packages we recommend using an Integrated-Development-Environment(IDE)
We recommend ECLIPSE-CDT, see "HowTo_UtilizeEclipseCDT".

### 10.2) Small projects/packages can be managed from command line interface as follows:
Open 4 shell sessions on your development host(Ubuntu16.04).
- First shell used to open source code files within an editor like vim or gedit
```
    >cd /wago/ptxproj/src/kbusdemo
    >gedit kbusdemo1.c
```
- Second shell used for “clean” and “build” the ptxdist package
```
    >cd /wago/ptxproj/
    >ptxdist clean kbusdemo

    >ptxdist targetinstall kbusdemo
```
- Third shell used to transfer executable to PFC.
```
    >scp kbusdemo1 root@<ip or hostname>:/usr/bin/kbusdemo1
```
- Fourth shell used to manage and control the PFC via ssh
```
    >ssh root@<ip or hostname>

    >cd /usr/bin
    >chmod +x kbusdemo1
    >./kbusdemo1
```
For the next round trip:
        1. Switch to the first shell and use the cursor-up-key to call the last command…
        2. Modify source code and save changes
        3. Switch to the second shell and use the cursor-up-key to call the last command…
        4. Rebuild executables
        5. Switch to the third shell and use the cursor-up-key to call the last command…
        6. Transfer executables to PFC 
        7. Switch to the fourth shell and use the cursor-up-key to call the last command…
        8. Start executable on PFC

### 10.3) Using Web-Based-Management(WBM) feature "Software-Upload" for upload and applying IPK packages
Tool "ptxdist" automatically generates an ".ipk" file for each package during build process.
#### 10.3.1) Start your local browser, and navigate to PFC200's default homepage(WBM)
```
https://ip.address.assigned.by.dhcp
```
Ignore Cert-Warning ...
#### 10.3.2) Select "Software-Upload" in left hand "Navigation" pane, You will be requested to authenticate!
```
Login as "admin" with password "wago" (default)
```
#### 10.3.3) Click on button [Browse...] and open the local file dialog.
Browse to folder "~/wago/ptxproj/platform-wago-pfcXXX/packages/"\
Select package to install or update, here "kbusdemo_0.3_arm.ipk".

#### 10.3.4) Click on button [Start Upload].
Transfers selected file into PFC file system and show button [Activate].

#### 10.3.5) In newly shown section "Activate new software", click on button [Activate] install package.
Internally WBM just calls:
```
    >cd /home/
    >opkg install kbusdemo_0.3_arm.ipk
```
Depending on type of package a restart of PFC may required.


**It may a good idea to setup a build server for a nightly build to check dependencies 
and consistency and do some unit testing in an automated way.**

