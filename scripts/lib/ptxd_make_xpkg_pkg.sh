# In 2015, Pengutronix introduced the function ptxd_install_fixup_timestamps(), which sets the time stamp of
# all files within an ipk file to the first day of PTXdist's release month. The aim was to get more reproducible
# artifacts. As they admit, this is only a small step towards that target.
# Cf. https://www.mail-archive.com/ptxdist@pengutronix.de/msg10352.html
# As it is considered beneficial to be able to distinguish files that have been installed into the root image during
# PTXdist's build process from those that have been install via opkg in a later step, the time stamp is kept to
# build time by overwriting this shell function.
# Cf. also https://svreq01002.wago.local/polarion/#/project/kvp/workitem?id=SE-4999

ptxd_install_fixup_timestamps() {
  :
}
