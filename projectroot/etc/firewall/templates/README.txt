The template files show a basic configuration which for each part of the firewall
setup. In case of ebwlist.xml and ipcmn.xml this is reflected one-to-one and
the files may be directly copied. In case of service.xml file it is an example
which needs some adjustemnts:
- the name of the file shall be changed to reflect the service it relates to,
- the "service" placeholder from /firwall/ipv4/service[name] attribute shall be
  changed to a name reated to the service. Please not that this name is directly
  used by the firewall framework and thus shall be short and contain no whitespaces,
- proper rules shall be added in the place where a remarked example resides.

A basic use-case for the templates is the restore process. Please, in case a change
in the templates is required, synchronize it with the restore code.
