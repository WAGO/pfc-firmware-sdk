#
# Temporary solution for WAT27299:
#  Removes 60-persistent-storage-serial.rules from the list
# of the default rules to be installed by the udev package.
# 
#  When kept, this rule creates a /dev/serial/ directory
# for usb-serial devices and breaks WAGO's /dev/serial link
# that points to the primary serial interface on 750-8207
# devices.
#
# NOTE:
# 
#  /dev/serial link is not standards compliant and shall be
# renamed (see WAT28205 for details)
#
#

UDEV_LEGACY_RULES-y := \
    50-udev-default.rules \
    60-persistent-alsa.rules \
    60-persistent-input.rules \
    60-persistent-storage-tape.rules \
    60-persistent-storage.rules \
    75-net-description.rules \
    75-tty-description.rules \
    78-sound-card.rules \
    95-udev-late.rules

