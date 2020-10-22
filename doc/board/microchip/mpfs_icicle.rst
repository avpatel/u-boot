.. SPDX-License-Identifier: GPL-2.0+

Microchip PolarFire SoC Icicle Kit
==================================

RISC-V PolarFire SoC
---------------------
The PolarFire SoC is the 4+1 64-bit RISC-V SoC from Microchip.

The Icicle Kit development platform is based on PolarFire SoC and capable
of running Linux.

Mainline support
----------------
The support for following drivers are already enabled:

1. NS16550 UART Driver.
2. Microchip Clock Driver.
3. Cadence MACB ethernet driver for networking support.
4. Cadence MMC Driver for eMMC/SD support.

Booting from eMMC using HSS
---------------------------

Building
--------

1. Add the RISC-V toolchain to your PATH.
2. Setup ARCH & cross compilation environment variable:

.. code-block:: none

   export CROSS_COMPILE=<riscv64 toolchain prefix>

3. make microchip_mpfs_icicle_defconfig
4. make

Flashing
--------

The current U-Boot port is supported in S-mode only and loaded from DRAM.

A prior stage M-mode firmware/bootloader (e.g HSS with OpenSBI) is required to
boot the u-boot.bin in S-mode.

Currently, the u-boot.bin is used as a payload of the HSS firmware.

You will be creating a payload from `u-boot-dtb.bin`.
Copy this file to the toplevel HSS (Hart Software Services) directory.

Creating the HSS payload
------------------------

Please refer to HSS documenation to build the HSS firmware.
(Note: HSS git repo is at
https://github.com/polarfire-soc/hart-software-services/blob/master
/tools/hss-payload-generator/README.md)

Once the payload binary is generated, it should be copied to the eMMC.

FPGA design with HSS programming file
-------------------------------------
https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/boards
/mpfs-icicle-kit-es/updating-icicle-kit/updating-icicle-kit-design-and-linux.md

The HSS firmware runs from the PolarFire SoC eNVM on reset.

eMMC
----
Program eMMC with payload binary is explained in the PolarFire SoC documentation.

(Note: PolarFire SoC Documentation git repo is at
https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/boards
/mpfs-icicle-kit-es/updating-icicle-kit/updating-icicle-kit-design-and-linux.md#eMMC

Once the payload image is copied to the eMMC, press CTRL+C in the HSS command
line interface, then type 'boot' and enter to boot the newly copied image.

.. code-block:: none

    sudo dd if=<payload_binary> of=/dev/sdX bs=512

Booting
-------
you should see the U-Boot prompt on UART0.

Sample boot log from MPFS Icicle Kit
-------------------------------------------

.. code-block:: none

   U-Boot 2020.10-00822-gb561436cc0-dirty (Oct 22 2020 - 11:21:24 +0530)

   CPU:   rv64imafdc
   Model: Microchip PolarFire-SoC
   DRAM:  1 GiB
   MMC:   sdhc@20008000: 0
   In:    serial@20100000
   Out:   serial@20100000
   Err:   serial@20100000
   Net:   eth0: ethernet@20112000
   Hit any key to stop autoboot:  0

Now you can configure your networking, tftp server and use tftp boot method to
load uImage(with initramfs).

.. code-block:: none

   RISC-V # setenv kernel_addr_r 0x80200000
   RISC-V # setenv fdt_addr_r 0x82200000

   RISC-V # setenv ipaddr 192.168.1.5
   RISC-V # setenv netmask 255.255.255.0
   RISC-V # setenv serverip 192.168.1.3
   RISC-V # setenv gateway 192.168.1.1

   RISC-V # tftpboot ${kernel_addr_r} uImage
   ethernet@20112000: PHY present at 9
   ethernet@20112000: Starting autonegotiation...
   ethernet@20112000: Autonegotiation complete
   ethernet@20112000: link up, 1000Mbps full-duplex (lpa: 0x7800)
   Using ethernet@20112000 device
   TFTP from server 192.168.1.3; our IP address is 192.168.1.5
   Filename 'uImage'.
   Load address: 0x80200000
   Loading: #################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			#################################################################
			############
			6.4 MiB/s
   done
   Bytes transferred = 14482480 (dcfc30 hex)

   RISC-V # tftpboot ${fdt_addr_r} microchip-icicle-kit.dtb
   ethernet@20112000: PHY present at 9
   ethernet@20112000: Starting autonegotiation...
   ethernet@20112000: Autonegotiation complete
   ethernet@20112000: link up, 1000Mbps full-duplex (lpa: 0x7800)
   Using ethernet@20112000 device
   TFTP from server 192.168.1.3; our IP address is 192.168.1.5
   Filename 'riscvpc.dtb'.
   Load address: 0x82200000
   Loading: #
			2.5 MiB/s
   done
   Bytes transferred = 10282 (282a hex)

   RISC-V # bootm ${kernel_addr_r} - ${fdt_addr_r}
   ## Booting kernel from Legacy Image at 80200000 ...
		Image Name:   Linux
		Image Type:   RISC-V Linux Kernel Image (uncompressed)
		Data Size:    14482416 Bytes = 13.8 MiB
		Load Address: 80200000
		Entry Point:  80200000
		Verifying Checksum ... OK
   ## Flattened Device Tree blob at 82200000
		Booting using the fdt blob at 0x82200000
		Loading Kernel Image
		Using Device Tree in place at 0000000082200000, end 0000000082205829

   Starting kernel ...

   [    0.000000] OF: fdt: Ignoring memory range 0x80000000 - 0x80200000
   [    0.000000] Linux version 5.6.17 (padmarao@padmarao-VirtualBox) (gcc version 7.2.0 (GCC)) #2 SMP Tue Jun 16 21:27:50 IST 2020
   [    0.000000] initrd not found or empty - disabling initrd
   [    0.000000] Zone ranges:
   [    0.000000]   DMA32    [mem 0x0000000080200000-0x00000000bfffffff]
   [    0.000000]   Normal   empty
   [    0.000000] Movable zone start for each node
   [    0.000000] Early memory node ranges
   [    0.000000]   node   0: [mem 0x0000000080200000-0x00000000bfffffff]
   [    0.000000] Initmem setup node 0 [mem 0x0000000080200000-0x00000000bfffffff]
   [    0.000000] software IO TLB: mapped [mem 0xbb1f5000-0xbf1f5000] (64MB)
   [    0.000000] elf_hwcap is 0x112d
   [    0.000000] percpu: Embedded 14 pages/cpu s24856 r0 d32488 u57344
   [    0.000000] Built 1 zonelists, mobility grouping on.  Total pages: 258055
   [    0.000000] Kernel command line: console=ttyS0,115200n8
   [    0.000000] Dentry cache hash table entries: 131072 (order: 8, 1048576 bytes, linear)
   [    0.000000] Inode-cache hash table entries: 65536 (order: 7, 524288 bytes, linear)
   [    0.000000] Sorting __ex_table...
   [    0.000000] mem auto-init: stack:off, heap alloc:off, heap free:off
   [    0.000000] Memory: 950308K/1046528K available (3289K kernel code, 212K rwdata, 900K rodata, 9476K init, 250K bss, 96220K reserved, 0K cma-reserved)
   [    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=4, Nodes=1
   [    0.000000] rcu: Hierarchical RCU implementation.
   [    0.000000] rcu: 	RCU event tracing is enabled.
   [    0.000000] rcu: 	RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=4.
   [    0.000000] rcu: RCU calculated value of scheduler-enlistment delay is 10 jiffies.
   [    0.000000] rcu: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=4
   [    0.000000] NR_IRQS: 0, nr_irqs: 0, preallocated irqs: 0
   [    0.000000] plic: mapped 186 interrupts with 4 handlers for 9 contexts.
   [    0.000000] riscv_timer_init_dt: Registering clocksource cpuid [0] hartid [1]
   [    0.000000] clocksource: riscv_clocksource: mask: 0xffffffffffffffff max_cycles: 0x1d854df40, max_idle_ns: 3526361616960 ns
   [    0.000015] sched_clock: 64 bits at 1000kHz, resolution 1000ns, wraps every 2199023255500ns
   [    0.000311] Calibrating delay loop (skipped), value calculated using timer frequency.. 2.00 BogoMIPS (lpj=10000)
   [    0.000349] pid_max: default: 32768 minimum: 301
   [    0.000846] Mount-cache hash table entries: 2048 (order: 2, 16384 bytes, linear)
   [    0.000964] Mountpoint-cache hash table entries: 2048 (order: 2, 16384 bytes, linear)
   [    0.005630] rcu: Hierarchical SRCU implementation.
   [    0.006901] smp: Bringing up secondary CPUs ...
   [    0.012545] smp: Brought up 1 node, 4 CPUs
   [    0.014431] devtmpfs: initialized
   [    0.020526] random: get_random_bytes called from setup_net+0x36/0x192 with crng_init=0
   [    0.020928] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 19112604462750000 ns
   [    0.020999] futex hash table entries: 1024 (order: 4, 65536 bytes, linear)
   [    0.022768] NET: Registered protocol family 16
   [    0.035478] microchip-pfsoc-clkcfg 20002000.clkcfg: Registered PFSOC core clocks
   [    0.048429] SCSI subsystem initialized
   [    0.049694] pps_core: LinuxPPS API ver. 1 registered
   [    0.049719] pps_core: Software ver. 5.3.6 - Copyright 2005-2007 Rodolfo Giometti <giometti@linux.it>
   [    0.049780] PTP clock support registered
   [    0.051781] clocksource: Switched to clocksource riscv_clocksource
   [    0.055326] NET: Registered protocol family 2
   [    0.056922] tcp_listen_portaddr_hash hash table entries: 512 (order: 1, 8192 bytes, linear)
   [    0.057053] TCP established hash table entries: 8192 (order: 4, 65536 bytes, linear)
   [    0.057648] TCP bind hash table entries: 8192 (order: 5, 131072 bytes, linear)
   [    0.058579] TCP: Hash tables configured (established 8192 bind 8192)
   [    0.059648] UDP hash table entries: 512 (order: 2, 16384 bytes, linear)
   [    0.059837] UDP-Lite hash table entries: 512 (order: 2, 16384 bytes, linear)
   [    0.060707] NET: Registered protocol family 1
   [    0.266229] workingset: timestamp_bits=62 max_order=18 bucket_order=0
   [    0.287107] io scheduler mq-deadline registered
   [    0.287140] io scheduler kyber registered
   [    0.429601] Serial: 8250/16550 driver, 4 ports, IRQ sharing disabled
   [    0.433979] printk: console [ttyS0] disabled
   [    0.434154] 20000000.serial: ttyS0 at MMIO 0x20000000 (irq = 18, base_baud = 9375000) is a 16550A
   [    0.928039] printk: console [ttyS0] enabled
   [    0.939804] libphy: Fixed MDIO Bus: probed
   [    0.948702] libphy: MACB_mii_bus: probed
   [    0.993698] macb 20112000.ethernet eth0: Cadence GEM rev 0x0107010c at 0x20112000 irq 21 (56:34:12:00:fc:00)
   [    1.006751] mousedev: PS/2 mouse device common for all mice
   [    1.013803] i2c /dev entries driver
   [    1.019451] sdhci: Secure Digital Host Controller Interface driver
   [    1.027242] sdhci: Copyright(c) Pierre Ossman
   [    1.032731] sdhci-pltfm: SDHCI platform and OF driver helper
   [    1.091826] mmc0: SDHCI controller on 20008000.sdhc [20008000.sdhc] using ADMA 64-bit
   [    1.102738] NET: Registered protocol family 17
   [    1.170326] Freeing unused kernel memory: 9476K
   [    1.176067] This architecture does not have kernel memory protection.
   [    1.184157] Run /init as init process
   Starting logging: OK
   Starting mdev...
   /etc/init.d/S10mdev: line 21: can't create /proc/sys/kernel/hotplug: nonexiste[    1.331981] mmc0: mmc_select_hs200 failed, error -74
   nt directory
   [    1.355011] mmc0: new MMC card at address 0001
   [    1.363981] mmcblk0: mmc0:0001 DG4008 7.28 GiB
   [    1.372248] mmcblk0boot0: mmc0:0001 DG4008 partition 1 4.00 MiB
   [    1.382292] mmcblk0boot1: mmc0:0001 DG4008 partition 2 4.00 MiB
   [    1.390265] mmcblk0rpmb: mmc0:0001 DG4008 partition 3 4.00 MiB, chardev (251:0)
   [    1.425234] GPT:Primary header thinks Alt. header is not at the end of the disk.
   [    1.434656] GPT:2255809 != 15273599
   [    1.439038] GPT:Alternate GPT header not at the end of the disk.
   [    1.446671] GPT:2255809 != 15273599
   [    1.451048] GPT: Use GNU Parted to correct GPT errors.
   [    1.457755]  mmcblk0: p1 p2 p3
   sort: /sys/devices/platform/Fixed: No such file or directory
   modprobe: can't change directory to '/lib/modules': No such file or directory
   Initializing random number generator... [    2.830198] random: dd: uninitialized urandom read (512 bytes read)
   done.
   Starting network...
   [    3.061867] macb 20112000.ethernet eth0: PHY [20112000.ethernet-ffffffff:09] driver [Vitesse VSC8662] (irq=POLL)
   [    3.074674] macb 20112000.ethernet eth0: configuring for phy/sgmii link mode
   [    3.084263] pps pps0: new PPS source ptp0
   [    3.089710] macb 20112000.ethernet: gem-ptp-timer ptp clock registered.
   udhcpc (v1.24.2) started
   Sending discover...
   Sending discover...
   [    6.380169] macb 20112000.ethernet eth0: Link is Up - 1Gbps/Full - flow control tx
   Sending discover...
   Sending select for 192.168.1.2...
   Lease of 192.168.1.2 obtained, lease time 86400
   deleting routers
   adding dns 192.168.1.1
   Starting dropbear sshd: [   11.385619] random: dropbear: uninitialized urandom read (32 bytes read)
   OK

   Welcome to Buildroot
   buildroot login: root
   Password:
   #

Booting U-Boot and Linux from eMMC
----------------------------------

FPGA design with HSS programming file and Linux Image
-----------------------------------------------------
https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/boards
/mpfs-icicle-kit-es/updating-icicle-kit/updating-icicle-kit-design-and-linux.md

The HSS firmware runs from the PolarFire SoC eNVM on reset.

eMMC
----
Program eMMC with payload binary and Linux image is explained in the
PolarFire SoC documentation.
The payload binary should copied to partition 2 of the eMMC.

(Note: PolarFire SoC Documentation git repo is at
https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/boards
/mpfs-icicle-kit-es/updating-icicle-kit/updating-icicle-kit-design-and-linux.md#eMMC)

once the Linux image and payload binary is copied to the eMMC, press CTRL+C
in the HSS command line interface, then type 'boot' and enter to boot the newly
copied payload and Linux image.

.. code-block:: none

    zcat <linux-image>.wic.gz | sudo dd of=/dev/sdX bs=4096 iflag=fullblock oflag=direct conv=fsync status=progress

    sudo dd if=<payload_binary> of=/dev/sdX2 bs=512

You should see the U-Boot prompt on UART0.

Sample boot log from MPFS Icicle Kit
-------------------------------------------

.. code-block:: none

   U-Boot 2020.10-00822-gb561436cc0-dirty (Oct 22 2020 - 11:21:24 +0530)

   CPU:   rv64imafdc
   Model: Microchip PolarFire-SoC
   DRAM:  1 GiB
   MMC:   sdhc@20008000: 0
   In:    serial@20100000
   Out:   serial@20100000
   Err:   serial@20100000
   Net:   eth0: ethernet@20112000
   Hit any key to stop autoboot:  0

   RISC-V # mmc info
   Device: sdhc@20008000
   Manufacturer ID: 45
   OEM: 100
   Name: DG400
   Bus Speed: 52000000
   Mode: MMC High Speed (52MHz)
   Rd Block Len: 512
   MMC version 5.1
   High Capacity: Yes
   Capacity: 7.3 GiB
   Bus Width: 4-bit
   Erase Group Size: 512 KiB
   HC WP Group Size: 8 MiB
   User Capacity: 7.3 GiB WRREL
   Boot Capacity: 4 MiB ENH
   RPMB Capacity: 4 MiB ENH

   RISC-V # mmc part
   Partition Map for MMC device 0  --   Partition Type: EFI

   Part	Start LBA	End LBA		Name
		Attributes
		Type GUID
		Partition GUID
	1	0x00002000	0x0000b031	"boot"
		attrs:	0x0000000000000004
		type:	ebd0a0a2-b9e5-4433-87c0-68b6b72699c7
		guid:	99ff6a94-f2e7-44dd-a7df-f3a2da106ef9
	2	0x0000b032	0x0000f031	"primary"
		attrs:	0x0000000000000000
		type:	21686148-6449-6e6f-744e-656564454649
		guid:	12006052-e64b-4423-beb0-b956ea00f1ba
	3	0x00010000	0x00226b9f	"root"
		attrs:	0x0000000000000000
		type:	0fc63daf-8483-4772-8e79-3d69d8477de4
		guid:	dd2c5619-2272-4c3c-8dc2-e21942e17ce6

   RISC-V # fatload mmc 0 ${ramdisk_addr_r} fitimage
   RISC-V # bootm ${ramdisk_addr_r}
   ## Loading kernel from FIT Image at 88300000 ...
   Using 'conf@microchip_icicle-kit-es-a000-microchip.dtb' configuration
   Trying 'kernel@1' kernel subimage
     Description:  Linux kernel
     Type:         Kernel Image
     Compression:  gzip compressed
     Data Start:   0x883000fc
     Data Size:    3574555 Bytes = 3.4 MiB
     Architecture: RISC-V
     OS:           Linux
     Load Address: 0x80200000
     Entry Point:  0x80200000
     Hash algo:    sha256
     Hash value:   21f18d72cf2f0a7192220abb577ad25c77c26960052d779aa02bf55dbf0a6403
   Verifying Hash Integrity ... sha256+ OK
   ## Loading fdt from FIT Image at 88300000 ...
   Using 'conf@microchip_icicle-kit-es-a000-microchip.dtb' configuration
   Trying 'fdt@microchip_icicle-kit-es-a000-microchip.dtb' fdt subimage
     Description:  Flattened Device Tree blob
     Type:         Flat Device Tree
     Compression:  uncompressed
     Data Start:   0x88668d44
     Data Size:    9760 Bytes = 9.5 KiB
     Architecture: RISC-V
     Load Address: 0x82200000
     Hash algo:    sha256
     Hash value:   5c3a9f30d41b6b8e53b47916e1f339b3a4d454006554d1f7e1f552ed62409f4b
   Verifying Hash Integrity ... sha256+ OK
   Loading fdt from 0x88668d44 to 0x82200000
   Booting using the fdt blob at 0x82200000
   Uncompressing Kernel Image
   Using Device Tree in place at 0000000082200000, end 000000008220561f

   Starting kernel ...

   [    0.568114] printk: console [ttyS0] enabled
   [    0.578504] printk: bootconsole [sbi0] disabled
   [    0.592089] 20102000.serial: ttyS1 at MMIO 0x20102000 (irq = 13, base_baud = 9375000) is a 16550A
   [    0.605351] 20104000.serial: ttyS2 at MMIO 0x20104000 (irq = 14, base_baud = 9375000) is a 16550A
   [    0.643484] loop: module loaded
   [    0.697876] Rounding down aligned max_sectors from 4294967295 to 4294967288
   [    0.707427] db_root: cannot open: /etc/target
   [    0.714543] libphy: Fixed MDIO Bus: probed
   [    0.722533] libphy: MACB_mii_bus: probed
   [    0.731024] macb 20112000.ethernet eth0: Cadence GEM rev 0x0107010c at 0x20112000 irq 17 (56:34:12:00:fc:00)
   [    0.744081] ehci_hcd: USB 2.0 'Enhanced' Host Controller (EHCI) Driver
   [    0.752318] ehci-platform: EHCI generic platform driver
   [    0.759092] ohci_hcd: USB 1.1 'Open' Host Controller (OHCI) Driver
   [    0.766849] ohci-platform: OHCI generic platform driver
   [    0.774100] usbcore: registered new interface driver cdc_acm
   [    0.781164] cdc_acm: USB Abstract Control Model driver for USB modems and ISDN adapters
   [    0.791986] i2c /dev entries driver
   [    0.798057] microsemi-mss-i2c 2010b000.i2c: Microsemi I2C Probe Complete
   [    0.807319] sdhci: Secure Digital Host Controller Interface driver
   [    0.815094] sdhci: Copyright(c) Pierre Ossman
   [    0.820527] sdhci-pltfm: SDHCI platform and OF driver helper
   [    0.860631] mmc0: SDHCI controller on 20008000.sdhc [20008000.sdhc] using ADMA 64-bit
   [    0.871064] usbcore: registered new interface driver usbhid
   [    0.878085] usbhid: USB HID core driver
   [    0.980158] mmc0: mmc_select_hs200 failed, error -74
   [    0.989240] mmc0: new MMC card at address 0001
   [    0.997930] mmcblk0: mmc0:0001 DG4008 7.28 GiB
   [    1.005847] mmcblk0boot0: mmc0:0001 DG4008 partition 1 4.00 MiB
   [    1.015369] mmcblk0boot1: mmc0:0001 DG4008 partition 2 4.00 MiB
   [    1.023364] mmcblk0rpmb: mmc0:0001 DG4008 partition 3 4.00 MiB, chardev (247:0)
   [    1.051870] GPT:Primary header thinks Alt. header is not at the end of the disk.
   [    1.061102] GPT:2255809 != 15273599
   [    1.065561] GPT:Alternate GPT header not at the end of the disk.
   [    1.073088] GPT:2255809 != 15273599
   [    1.077439] GPT: Use GNU Parted to correct GPT errors.
   [    1.084003]  mmcblk0: p1 p2 p3
   [    1.891482] pac193x 0-0010: failed reading data from register 0xFD
   [    1.899195] pac193x 0-0010: cannot read PAC193x revision
   [    1.905955] pac193x: probe of 0-0010 failed with error -22
   [    1.915372] NET: Registered protocol family 10
   [    1.924358] Segment Routing with IPv6
   [    1.929125] sit: IPv6, IPv4 and MPLS over IPv4 tunneling driver
   [    1.938379] NET: Registered protocol family 17
   [    1.944944] hctosys: unable to open rtc device (rtc0)
   [    1.962991] EXT4-fs (mmcblk0p3): INFO: recovery required on readonly filesystem
   [    1.972196] EXT4-fs (mmcblk0p3): write access will be enabled during recovery
   [    2.054832] EXT4-fs (mmcblk0p3): recovery complete
   [    2.064742] EXT4-fs (mmcblk0p3): mounted filesystem with ordered data mode. Opts: (null)
   [    2.075057] VFS: Mounted root (ext4 filesystem) readonly on device 179:3.
   [    2.084573] Freeing unused kernel memory: 168K
   [    2.090122] This architecture does not have kernel memory protection.
   [    2.098235] Run /sbin/init as init process
   [    2.612563] random: fast init done
   [    2.809794] systemd[1]: System time before build time, advancing clock.
   [    2.868818] systemd[1]: systemd 244.3+ running in system mode. (+PAM -AUDIT -SELINUX +IMA -APPARMOR -SMACK +SYSVINIT +UTMP -LIBCRYPTSETUP -GCRYPT -GNUTLS +ACL +XZ -LZ4 -SECCOMP +BLKID -ELFUTILS +KMOD -IDN2 -IDN -PCRE2 default-hierarchy=hybrid)
   [    2.896774] systemd[1]: Detected architecture riscv64.

   Welcome to OpenEmbedded nodistro.0!

   [    2.953510] systemd[1]: Set hostname to <icicle-kit-es>.
   [    4.269288] random: systemd: uninitialized urandom read (16 bytes read)
   [    4.281772] systemd[1]: Created slice system-getty.slice.
   [  OK  ] Created slice system-getty.slice.
   [    4.321956] random: systemd: uninitialized urandom read (16 bytes read)
   [    4.332378] systemd[1]: Created slice system-serial\x2dgetty.slice.
   [  OK  ] Created slice system-serial\x2dgetty.slice.
   [    4.371978] random: systemd: uninitialized urandom read (16 bytes read)
   [    4.382106] systemd[1]: Created slice User and Session Slice.
   [  OK  ] Created slice User and Session Slice.
   [    4.422798] systemd[1]: Started Dispatch Password Requests to Console Directory Watch.
   [  OK  ] Started Dispatch Password …ts to Console Directory Watch.
   [    4.472693] systemd[1]: Started Forward Password Requests to Wall Directory Watch.
   [  OK  ] Started Forward Password R…uests to Wall Directory Watch.
   [    4.522586] systemd[1]: Reached target Paths.
   [  OK  ] Reached target Paths.
   [    4.562253] systemd[1]: Reached target Remote File Systems.
   [  OK  ] Reached target Remote File Systems.
   [    4.602154] systemd[1]: Reached target Slices.
   [  OK  ] Reached target Slices.
   [    4.642301] systemd[1]: Reached target Swap.
   [  OK  ] Reached target Swap.
   [    4.683183] systemd[1]: Listening on initctl Compatibility Named Pipe.
   [  OK  ] Listening on initctl Compatibility Named Pipe.
   [    4.750416] systemd[1]: Condition check resulted in Journal Audit Socket being skipped.
   [    4.762960] systemd[1]: Listening on Journal Socket (/dev/log).
   [  OK  ] Listening on Journal Socket (/dev/log).
   [    4.803839] systemd[1]: Listening on Journal Socket.
   [  OK  ] Listening on Journal Socket.
   [    4.844307] systemd[1]: Listening on Network Service Netlink Socket.
   [  OK  ] Listening on Network Service Netlink Socket.
   [    4.883652] systemd[1]: Listening on udev Control Socket.
   [  OK  ] Listening on udev Control Socket.
   [    4.923218] systemd[1]: Listening on udev Kernel Socket.
   [  OK  ] Listening on udev Kernel Socket.
   [    4.963758] systemd[1]: Condition check resulted in Huge Pages File System being skipped.
   [    4.975387] systemd[1]: Condition check resulted in POSIX Message Queue File System being skipped.
   [    4.988039] systemd[1]: Condition check resulted in Kernel Debug File System being skipped.
   [    5.008120] systemd[1]: Mounting Temporary Directory (/tmp)...
            Mounting Temporary Directory (/tmp)...
   [    5.052616] systemd[1]: Condition check resulted in Create list of static device nodes for the current kernel being skipped.
   [    5.075536] systemd[1]: Starting File System Check on Root Device...
            Starting File System Check on Root Device...
   [    5.136915] systemd[1]: Starting Journal Service...
            Starting Journal Service...
   [    5.160116] systemd[1]: Condition check resulted in Load Kernel Modules being skipped.
   [    5.172867] systemd[1]: Condition check resulted in FUSE Control File System being skipped.
   [    5.197446] systemd[1]: Mounting Kernel Configuration File System...
            Mounting Kernel Configuration File System...
   [    5.222039] systemd[1]: Starting Apply Kernel Variables...
            Starting Apply Kernel Variables...
   [    5.242677] systemd[1]: Starting udev Coldplug all Devices...
            Starting udev Coldplug all Devices...
   [    5.277117] systemd[1]: Mounted Temporary Directory (/tmp).
   [  OK  ] Mounted Temporary Di[    5.288434] systemd[1]: Mounted Kernel Configuration File System.
   rectory (/tmp).
   [  OK  ] Mounted Kernel Configuration File System.
   [    5.347121] systemd[1]: Started Journal Service.
   [  OK  ] Started Journal Service.
   [  OK  ] Started Apply Kernel Variables.
   [  OK  ] Started File System Check on Root Device.
            Starting Remount Root and Kernel File Systems...
   [    5.700818] EXT4-fs (mmcblk0p3): re-mounted. Opts: (null)
   [  OK  ] Started Remount Root and Kernel File Systems.
            Starting Flush Journal to Persistent Storage...
            Starting Create Static Device Nodes in /dev...
   [    5.857779] systemd-journald[75]: Received client request to flush runtime journal.
   [  OK  ] Started Flush Journal to Persistent Storage.
   [  OK  ] Started Create Static Device Nodes in /dev.
   [  OK  ] Reached target Local File Systems (Pre).
            Mounting /var/volatile...
            Starting udev Kernel Device Manager...
   [  OK  ] Mounted /var/volatile.
            Starting Load/Save Random Seed...
   [  OK  ] Reached target Local File Systems.
            Starting Create Volatile Files and Directories...
   [  OK  ] Started udev Kernel Device Manager.
   [  OK  ] Started Create Volatile Files and Directories.
            Starting Network Time Synchronization...
            Starting Update UTMP about System Boot/Shutdown...
   [  OK  ] Started Update UTMP about System Boot/Shutdown.
   [  OK  ] Started Network Time Synchronization.
   [  OK  ] Reached target System Time Set.
   [  OK  ] Reached target System Time Synchronized.
   [  OK  ] Started udev Coldplug all Devices.
   [  OK  ] Reached target System Initialization.
   [  OK  ] Started Daily Cleanup of Temporary Directories.
   [  OK  ] Reached target Timers.
   [  OK  ] Listening on D-Bus System Message Bus Socket.
   [  OK  ] Listening on dropbear.socket.
   [  OK  ] Reached target Sockets.
   [  OK  ] Reached target Basic System.
   [  OK  ] Started D-Bus System Message Bus.
            Starting IPv6 Packet Filtering Framework...
            Starting IPv4 Packet Filtering Framework...
            Starting Login Service...
   [  OK  ] Started IPv6 Packet Filtering Framework.
   [  OK  ] Started IPv4 Packet Filtering Framework.
   [   11.341568] random: crng init done
   [   11.345841] random: 7 urandom warning(s) missed due to ratelimiting
   [  OK  ] Started Load/Save Random Seed.
   [  OK  ] Started Login Service.
   [  OK  ] Reached target Network (Pre).
            Starting Network Service...
   [  OK  ] Started Network Service.
   [   13.673774] macb 20112000.ethernet eth0: PHY [20112000.ethernet-ffffffff:09] driver [Vitesse VSC8662] (irq=POLL)
   [   13.686635] macb 20112000.ethernet eth0: configuring for phy/sgmii link mode
   [   13.702061] pps pps0: new PPS source ptp0
   [   13.713053] macb 20112000.ethernet: gem-ptp-timer ptp clock registered.
            Starting Network Name Resolution...
   [  OK  ] Started Network Name Resolution.
   [  OK  ] Reached target Network.
   [  OK  ] Reached target Host and Network Name Lookups.
   [  OK  ] Started Collectd.
            Starting Permit User Sessions...
   [  OK  ] Started Permit User Sessions.
   [  OK  ] Started Getty on tty1.
   [  OK  ] Started Serial Getty on ttyS0.
   [  OK  ] Reached target Login Prompts.
   [  OK  ] Reached target Multi-User System.
            Starting Update UTMP about System Runlevel Changes...
   [  OK  ] Started Update UTMP about System Runlevel Changes.

   OpenEmbedded nodistro.0 icicle-kit-es ttyS0

   icicle-kit-es login: [   17.900317] macb 20112000.ethernet eth0: Link is Up - 1Gbps/Full - flow control tx
   [   17.909943] IPv6: ADDRCONF(NETDEV_CHANGE): eth0: link becomes ready

   icicle-kit-es login: root
   root@icicle-kit-es:~#
