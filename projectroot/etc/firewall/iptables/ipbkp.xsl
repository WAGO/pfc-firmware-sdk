<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
                xmlns:f="http://www.wago.com/security/firewall"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="../transform.xsl"/>

<xsl:output method="text" indent="no" encoding="utf-8" media-type="text/plain"/>
<xsl:strip-space elements="*" />

<xsl:template match="/f:firewall/f:ipv4">
<xsl:apply-templates select="f:climits"/>
<xsl:apply-templates select="f:echo"/>
<xsl:apply-templates select="f:forward"/>
<xsl:apply-templates select="f:input"/>
</xsl:template>

<xsl:template match="f:climits">
<xsl:text>firewall-iptables-climits=</xsl:text>
<xsl:value-of select="@total"/><xsl:text>|</xsl:text>
<xsl:value-of select="@limit"/><xsl:text>|</xsl:text>
<xsl:value-of select="@burst"/><xsl:text>|</xsl:text>
<xsl:value-of select="@tcp"/><xsl:text>|</xsl:text>
<xsl:value-of select="@udp"/>
<xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template match="f:echo">
<xsl:text>firewall-iptables-echo=</xsl:text>
<xsl:value-of select="@policy"/><xsl:text>|</xsl:text>
<xsl:value-of select="@limit"/><xsl:text>|</xsl:text>
<xsl:value-of select="@burst"/><xsl:text>|</xsl:text>
<xsl:value-of select="@broadcast_protection"/>
<xsl:value-of select="$newline"/>
<xsl:apply-templates select="f:request"/>
</xsl:template>

<xsl:template match="f:request">
<xsl:text>firewall-iptables-echo-request=</xsl:text>
<xsl:value-of select="@policy"/><xsl:text>|</xsl:text>
<xsl:value-of select="@if"/><xsl:text>|</xsl:text>
<xsl:value-of select="@limit"/><xsl:text>|</xsl:text>
<xsl:value-of select="@burst"/>
<xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template match="f:forward">
<xsl:text>firewall-iptables-forward=</xsl:text>
<xsl:value-of select="@state"/>
<xsl:value-of select="$newline"/>
<xsl:apply-templates select="f:fw/f:link"/>
<xsl:apply-templates select="f:snat/f:masquerade"/>
<xsl:apply-templates select="f:dnat/f:fw_port"/>
</xsl:template>

<xsl:template match="f:fw/f:link">
<xsl:text>firewall-iptables-fwlink=</xsl:text>
<xsl:value-of select="@state"/><xsl:text>|</xsl:text>
<xsl:value-of select="@if1"/><xsl:text>|</xsl:text>
<xsl:value-of select="@if2"/>
<xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template match="f:snat/f:masquerade">
<xsl:text>firewall-iptables-snatmasq=</xsl:text>
<xsl:value-of select="@if"/>
<xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template match="f:dnat/f:fw_port">
<xsl:text>firewall-iptables-dnatfwp=</xsl:text>
<xsl:value-of select="@state"/><xsl:text>|</xsl:text>
<xsl:value-of select="@if"/><xsl:text>|</xsl:text>
<xsl:value-of select="@proto"/><xsl:text>|</xsl:text>
<xsl:value-of select="@dst_ip"/><xsl:text>|</xsl:text>
<xsl:value-of select="@dst_port"/><xsl:text>|</xsl:text>
<xsl:value-of select="@fw_ip"/><xsl:text>|</xsl:text>
<xsl:value-of select="@fw_port"/>
<xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template match="f:input">
<xsl:apply-templates select="f:open/f:interface"/>
<xsl:apply-templates select="f:filter/f:rule"/>
</xsl:template>

<xsl:template match="f:open/f:interface">
<xsl:text>firewall-iptables-input-openif=</xsl:text>
<xsl:value-of select="@state"/><xsl:text>|</xsl:text>
<xsl:value-of select="@if"/>
<xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template match="f:filter/f:rule">
<xsl:text>firewall-iptables-input-filter=</xsl:text>
<xsl:value-of select="@state"/><xsl:text>|</xsl:text>
<xsl:value-of select="@if"/><xsl:text>|</xsl:text>
<xsl:value-of select="@proto"/><xsl:text>|</xsl:text>
<xsl:value-of select="@src_ip"/><xsl:text>|</xsl:text>
<xsl:value-of select="@src_mask"/><xsl:text>|</xsl:text>
<xsl:value-of select="@src_port"/><xsl:text>|</xsl:text>
<xsl:value-of select="@dst_ip"/><xsl:text>|</xsl:text>
<xsl:value-of select="@dst_mask"/><xsl:text>|</xsl:text>
<xsl:value-of select="@dst_port"/><xsl:text>|</xsl:text>
<xsl:value-of select="@policy"/>
<xsl:value-of select="$newline"/>
</xsl:template>

</xsl:stylesheet>
