# Drivers

KyronOS reserves a generic device/block-device boundary. Keyboard and VGA console support are the first hardware targets. USB EHCI (USB 2.0) and xHCI (USB 3.x) detection/enumeration, plus optical-device read support, are planned behind this boundary.

There are intentionally no SATA, AHCI, NVMe, IDE, or general internal-disk drivers. A future USB mass-storage driver can provide a `BlockDevice` without changing KSFS.
