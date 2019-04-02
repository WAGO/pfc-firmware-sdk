<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
                xmlns:f="http://www.wago.com/security/firewall"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="../transform.xsl"/>

<xsl:output method="text" indent="no" encoding="utf-8" media-type="text/plain" />
<xsl:strip-space elements="*" />

<xsl:template match="/f:firewall">
<xsl:text>*broute
:BROUTING ACCEPT

*filter
:INPUT ACCEPT
:FORWARD ACCEPT
-A FORWARD --logical-in br2 -j DROP
:OUTPUT ACCEPT

*nat
:PREROUTING ACCEPT
:OUTPUT ACCEPT
:POSTROUTING ACCEPT
</xsl:text>
<xsl:apply-templates select="f:ethernet[@mode='all-allow']"/>
<xsl:apply-templates select="f:ethernet[@mode='whitelist']"/>
</xsl:template>

<xsl:template match="f:ethernet[@mode='all-allow']">
<xsl:if test="f:protocols[@state='on']">
<xsl:text>:protocols DROP
-A PREROUTING -j protocols
-A protocols -p ARP -j ACCEPT
-A protocols -p IPv4 -j ACCEPT
</xsl:text>
    <xsl:for-each select="f:protocols/f:protocol[@eproto]">
        <xsl:text>-A protocols -p </xsl:text>
        <xsl:value-of select="@eproto"/>
        <xsl:text> -j ACCEPT</xsl:text>
        <xsl:value-of select="$newline"/>
    </xsl:for-each>
</xsl:if>
</xsl:template>

<xsl:template match="f:ethernet[@mode='whitelist']">
<xsl:if test="f:protocols[@state='on']">
<xsl:text>:protocols DROP
-A PREROUTING -j protocols
-A protocols -p ARP -j ACCEPT
-A protocols -p IPv4 --ip-proto udp --ip-sport 67 --ip-dport 68 -j ACCEPT
-A protocols -p IPv4 -j RETURN
</xsl:text>
    <xsl:for-each select="f:protocols/f:protocol[@eproto]">
        <xsl:text>-A protocols -p </xsl:text>
        <xsl:value-of select="@eproto"/>
        <xsl:text> -j RETURN</xsl:text>
        <xsl:value-of select="$newline"/>
    </xsl:for-each>
</xsl:if>
<xsl:text>:mac_filter DROP
-A PREROUTING -j mac_filter
</xsl:text>
<xsl:for-each select="f:interfaces/f:interface[@state='open']">
    <xsl:variable name="el" select="current()"/>
    <xsl:for-each select="$parameters/f:firewall/f:parameters/f:interfaces/f:interface[@name=$el/@if and @ethernet='yes']">
        <xsl:text>-A mac_filter --logical-in </xsl:text>
        <xsl:call-template name="ifname-cur"/>
        <xsl:text> -j ACCEPT</xsl:text>
        <xsl:value-of select="$newline"/>
    </xsl:for-each>
</xsl:for-each>
<xsl:for-each select="f:whitelist/f:host[@state='on']">
    <xsl:text>-A mac_filter -s </xsl:text>
    <xsl:value-of select="@mac"/>
    <xsl:if test="@mask">
        <xsl:text>/</xsl:text>
        <xsl:value-of select="@mask"/>
    </xsl:if>
    <xsl:text> -j ACCEPT</xsl:text>
    <xsl:value-of select="$newline"/>
</xsl:for-each>
<xsl:for-each select="f:log[@state='on']">
    <xsl:if test="/f:firewall/f:ethernet/f:protocols[@state='on']">
        <xsl:text>-A protocols --log --log-prefix </xsl:text>
        <xsl:value-of select="@tag"/>
        <xsl:call-template name="limit"/>
        <xsl:text> --log-level </xsl:text>
        <xsl:value-of select="@level"/>
        <xsl:value-of select="$newline"/>
    </xsl:if>

    <xsl:text>-A mac_filter --log --log-prefix </xsl:text>
    <xsl:value-of select="@tag"/>
    <xsl:call-template name="limit"/>
    <xsl:text> --log-level </xsl:text>
    <xsl:value-of select="@level"/>
    <xsl:value-of select="$newline"/>
</xsl:for-each>
</xsl:template>

</xsl:stylesheet>
