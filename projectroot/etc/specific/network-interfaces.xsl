<xsl:stylesheet version='1.0'
  xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>

<xsl:output method="text" encoding="UTF-8" omit-xml-declaration="yes" indent="no"/>
<xsl:strip-space elements="*"/>

<xsl:variable name='newline'><xsl:text>
</xsl:text></xsl:variable>

<xsl:variable name='all_ifaces'>
    <xsl:for-each select="/interfaces/iface/device_name">
        <xsl:value-of select="."/>
        <xsl:if test="position() != last()"><xsl:text> </xsl:text> </xsl:if>
    </xsl:for-each>
    <xsl:value-of select="$newline"/>
</xsl:variable>

<!-- root template -->
<xsl:template match="/">
#   Auto-generated /etc/network/interfaces
#   Don't edit manually!
#

# dsa_mode=<xsl:value-of select="/interfaces/dsa_mode"/>

auto lo <xsl:value-of select="$all_ifaces"/>

iface lo inet loopback

<xsl:apply-templates select="/interfaces/iface"/>
</xsl:template>

<!-- generic template: do nothing if nothing else matches -->
<xsl:template match="*|text()">
</xsl:template>

<!-- main template: node iface -->
<xsl:template match="iface">
iface <xsl:value-of select="device_name"/> <xsl:text> inet manual</xsl:text>
<xsl:if test="(/interfaces/dsa_mode = '1') or ((/interfaces/dsa_mode = '0') and (./no_dsa_disable != 'yes'))">
    <xsl:if test="./state='enabled'">
        <xsl:apply-templates select="*|@*"/>
 pre-up    sudo ip link set dev <xsl:value-of select="device_name"/> <xsl:text> up </xsl:text>
 post-down sudo ip link set dev <xsl:value-of select="device_name"/> <xsl:text> down || true</xsl:text>
    </xsl:if>
    </xsl:if>
    <xsl:value-of select="$newline"/>
</xsl:template>

<!-- bridge template -->
<xsl:template match="/interfaces/iface/bridge">
     <xsl:choose>
         <xsl:when test="(/interfaces/dsa_mode='1') and (./dsa_slave != '')">
 pre-up    sudo brctl addif <xsl:value-of select="../device_name"/> <xsl:text> </xsl:text> <xsl:value-of select="./dsa_slave"/>
 post-down sudo brctl delif <xsl:value-of select="../device_name"/> <xsl:text> </xsl:text> <xsl:value-of select="./dsa_slave"/><xsl:text> || true</xsl:text>
        </xsl:when>
        <xsl:when test="(/interfaces/dsa_mode='0') and (./no_dsa_slave != '')">
 pre-up    sudo brctl addif <xsl:value-of select="../device_name"/> <xsl:text> </xsl:text> <xsl:value-of select="./no_dsa_slave"/>
 post-down sudo brctl delif <xsl:value-of select="../device_name"/> <xsl:text> </xsl:text> <xsl:value-of select="./no_dsa_slave"/><xsl:text> || true</xsl:text>
        </xsl:when>
    </xsl:choose>
	post-down sudo /etc/init.d/net-snmpd restart || true <xsl:text></xsl:text>
</xsl:template>

<!-- template promisc mode (must be enabled on master to set different MAC addresses on slave eth devices) -->
<xsl:template match="/interfaces/iface/promisc">
 up sudo ip link set dev <xsl:value-of select="../device_name"/> <xsl:text> promisc </xsl:text> <xsl:value-of select="."/>
</xsl:template>


<!-- mode template: static/dhcp/bootp. -->
<xsl:template match="/interfaces/iface/ip_settings">
    <xsl:choose>
        <xsl:when test="type='static'">
 pre-up     sudo ip addr add   <xsl:value-of select="static_ipaddr"/>/<xsl:value-of select="static_netmask"/><xsl:if test="static_broadcast != ''">&#x20;broadcast <xsl:value-of select="static_broadcast"/></xsl:if>&#x20;dev <xsl:value-of select="../device_name"/>
 down   sudo ip addr flush dev <xsl:value-of select="../device_name"/> <xsl:text> || true</xsl:text>
        </xsl:when>

        <xsl:when test="type='dhcp'">
 post-down rm -rf /var/run/udhcpc_<xsl:value-of select="../device_name"/>.pid
 up /sbin/udhcpc -i <xsl:value-of select="../device_name"/> -h `/bin/hostname` -V `/etc/config-tools/get_typelabel_value ORDER` -p /var/run/udhcpc_<xsl:value-of select="../device_name"/>.pid > /dev/null 2> /dev/null &amp;
 down kill -9 $(cat /var/run/udhcpc_<xsl:value-of select="../device_name"/>.pid) || true
 down   sudo ip addr flush dev <xsl:value-of select="../device_name"/> <xsl:text> || true</xsl:text>
        </xsl:when>

        <xsl:when test="type='bootp'">
 up daemonize /sbin/bootpc-startup <xsl:value-of select="../device_name"/>
 down /usr/bin/killall bootpc-startup || true
 down   sudo ip addr flush dev <xsl:value-of select="../device_name"/> <xsl:text> || true</xsl:text>
        </xsl:when>

    </xsl:choose>
 <xsl:apply-templates select="gw"/>
</xsl:template>

<!-- port template: ethernet settings -->
<xsl:template match="/interfaces/iface/ethernet_settings">
 <xsl:variable name="ethtoolParams">
     <xsl:value-of select="../device_name"/>
     <xsl:choose>
        <xsl:when test=" autoneg = 'enabled'">
            autoneg on
        </xsl:when>
        <xsl:otherwise>
            autoneg off speed <xsl:value-of select="substring-before(./speed, 'M')"/> duplex <xsl:value-of select="duplex"/>
        </xsl:otherwise>
     </xsl:choose>
 </xsl:variable>
 <xsl:value-of select="concat($newline, ' pre-up sudo ethtool -s ', normalize-space($ethtoolParams))"/>
 <xsl:apply-templates select="*|@*"/>
</xsl:template>

<!-- default mac address template
     NOTE: for MAC address to work for ethX1/2, eth0 has to be configured in promiscuitive mode -->
<xsl:template match="ethernet_settings/mac">
    <xsl:if test=". != ''">
    <xsl:text>
</xsl:text>
    <xsl:value-of select="concat(' up sudo ip link set dev ', ../../device_name, ' address ', .)"/>
    </xsl:if>
</xsl:template>

</xsl:stylesheet>
