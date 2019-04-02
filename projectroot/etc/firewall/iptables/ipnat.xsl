<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
                xmlns:f="http://www.wago.com/security/firewall"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="../transform.xsl"/>

<xsl:output method="text" indent="no" encoding="utf-8" media-type="text/plain"/>
<xsl:strip-space elements="*" />

<xsl:variable name="is_forwarding_on">
    <xsl:choose>
        <xsl:when test="/f:firewall/f:ipv4/f:forward[@state='on'] or /f:firewall/f:ipv4/f:forward/f:fw/f:link[@state='on']">
            <xsl:text>on</xsl:text>
        </xsl:when>
        <xsl:otherwise>
            <xsl:text>off</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
</xsl:variable>

<xsl:template match="/f:firewall/f:ipv4">
<xsl:text>*nat
-F dnat
-F snat
</xsl:text>
<xsl:if test="$is_forwarding_on='on'">
    <xsl:apply-templates select="f:forward/f:dnat"/>
    <xsl:apply-templates select="f:forward/f:snat"/>
</xsl:if>
<xsl:text>COMMIT
*filter
-F fw_common
</xsl:text>
<xsl:apply-templates select="f:forward[@state='on']"/>
<xsl:apply-templates select="f:forward[not(@state) or @state='off']/f:fw"/>
<xsl:text>COMMIT</xsl:text>
<xsl:value-of select="$newline"/>
</xsl:template>

<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
<!-- SECTION START : FORWARD (WAT10531 - router, WAT10530 - masquerade, WAT10529 - port forwarding) -->
<xsl:template name="port-forward">
    <xsl:param name="proto"/>
    <xsl:variable name="el" select="current()"/>
    <xsl:variable name="rl">
        <xsl:text> -p </xsl:text>
        <xsl:value-of select="$proto"/>
        <xsl:if test="@dst_ip">
            <xsl:text> -d </xsl:text>
            <xsl:value-of select="@dst_ip"/>
        </xsl:if>
        <xsl:if test="@dst_port">
            <xsl:text> --dport </xsl:text>
            <xsl:value-of select="@dst_port"/>
        </xsl:if>
        <xsl:call-template name="mlimit"/>
        <xsl:text> -j DNAT --to-destination </xsl:text>
        <xsl:value-of select="@fw_ip"/>
        <xsl:if test="@fw_port">
            <xsl:text>:</xsl:text>
            <xsl:value-of select="@fw_port"/>
        </xsl:if>
    </xsl:variable>
    <xsl:if test="@if">
        <xsl:for-each select="$parameters/f:firewall/f:parameters/f:interfaces/f:interface[@name=$el/@if]">
            <xsl:text>-A dnat </xsl:text>
            <xsl:call-template name="ifname-ipsec-in-cur"/>
            <xsl:value-of select="$rl"/>
            <xsl:value-of select="$newline"/>
        </xsl:for-each>
    </xsl:if>
    <xsl:if test="not(@if)">
        <xsl:text>-A dnat </xsl:text>
        <xsl:value-of select="$rl"/>
        <xsl:value-of select="$newline"/>
    </xsl:if>
</xsl:template>
<xsl:template match="f:forward/f:dnat">
    <xsl:for-each select="f:fw_port[@state='on' and (@dst_ip or @dst_port) and (@fw_ip or @fw_port)]">
        <xsl:if test="@proto='tcp' or @proto='udp'">
            <xsl:call-template name="port-forward">
                <xsl:with-param name="proto" select="@proto"/>
            </xsl:call-template>
        </xsl:if>
        <xsl:if test="@proto='tcpudp'">
            <xsl:call-template name="port-forward">
                <xsl:with-param name="proto">tcp</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="port-forward">
                <xsl:with-param name="proto">udp</xsl:with-param>
            </xsl:call-template>
        </xsl:if>
    </xsl:for-each>
</xsl:template>
<xsl:template match="f:forward/f:snat">
    <xsl:for-each select="f:masquerade[@if]">
        <xsl:variable name="el" select="current()"/>
        <xsl:for-each select="$parameters/f:firewall/f:parameters/f:interfaces/f:interface[@name=$el/@if]">
            <xsl:text>-A snat </xsl:text>
            <xsl:call-template name="ifname-ipsec-out-cur"/>
            <xsl:text> -j MASQUERADE</xsl:text>
            <xsl:value-of select="$newline"/>
        </xsl:for-each>
    </xsl:for-each>
</xsl:template>
<xsl:template match="f:forward[@state='on']">
    <xsl:text>-A fw_common -m conntrack --ctstate NEW -j ACCEPT</xsl:text>
    <xsl:value-of select="$newline"/>
</xsl:template>
<xsl:template name="forward-ifs">
    <xsl:param name="if1"/>
    <xsl:param name="if2"/>
    <xsl:for-each select="$parameters/f:firewall/f:parameters/f:interfaces/f:interface[@name=$if1]">
        <xsl:variable name="in"><xsl:call-template name="ifname-ipsec-in-cur"/></xsl:variable>
        <xsl:for-each select="$parameters/f:firewall/f:parameters/f:interfaces/f:interface[@name=$if2]">
            <xsl:variable name="ou"><xsl:call-template name="ifname-ipsec-out-cur"/></xsl:variable>
            <xsl:text>-A fw_common </xsl:text>
            <xsl:value-of select="$in"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="$ou"/>
            <xsl:text> -j ACCEPT</xsl:text>
            <xsl:value-of select="$newline"/>
        </xsl:for-each>
    </xsl:for-each>
</xsl:template>
<xsl:template match="f:forward[not(@state) or @state='off']/f:fw">
    <xsl:for-each select="f:link[@state='on']">
        <xsl:call-template name="forward-ifs">
            <xsl:with-param name="if1" select="@if1" />
            <xsl:with-param name="if2" select="@if2" />
        </xsl:call-template>
        <xsl:call-template name="forward-ifs">
            <xsl:with-param name="if1" select="@if2" />
            <xsl:with-param name="if2" select="@if1" />
        </xsl:call-template>
    </xsl:for-each>
</xsl:template>
<!-- SECTION END   : FORWARD -->

</xsl:stylesheet>
