<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
                xmlns:f="http://www.wago.com/security/firewall"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="../transform.xsl"/>

<xsl:output method="text" indent="no" encoding="utf-8" media-type="text/plain" />
<xsl:strip-space elements="*" />

<xsl:template match="/f:firewall/f:ethernet">
<xsl:text>firewall-ebtables-mode=</xsl:text>
<xsl:value-of select="@mode"/>
<xsl:value-of select="$newline"/>
<xsl:apply-templates select="f:log"/>
<xsl:apply-templates select="f:interfaces/f:interface"/>
<xsl:apply-templates select="f:protocols"/>
<xsl:apply-templates select="f:whitelist/f:host"/>
</xsl:template>

<xsl:template match="f:log">
<xsl:text>firewall-ebtables-log=</xsl:text>
<xsl:value-of select="@state"/><xsl:text>|</xsl:text>
<xsl:value-of select="@tag"/><xsl:text>|</xsl:text>
<xsl:value-of select="@limit"/><xsl:text>|</xsl:text>
<xsl:value-of select="@burst"/><xsl:text>|</xsl:text>
<xsl:value-of select="@level"/>
<xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template match="f:interfaces/f:interface">
<xsl:text>firewall-ebtables-interface=</xsl:text>
<xsl:value-of select="@state"/><xsl:text>|</xsl:text>
<xsl:value-of select="@if"/>
<xsl:value-of select="$newline"/>
<xsl:if test="@if='br0'">
<xsl:text>firewall-ebtables-interface=</xsl:text>
<xsl:value-of select="@state"/><xsl:text>|</xsl:text>
<xsl:text>X1</xsl:text>
<xsl:value-of select="$newline"/>
</xsl:if>
<xsl:if test="@if='br1'">
<xsl:text>firewall-ebtables-interface=</xsl:text>
<xsl:value-of select="@state"/><xsl:text>|</xsl:text>
<xsl:text>X2</xsl:text>
<xsl:value-of select="$newline"/>
</xsl:if>
</xsl:template>

<xsl:template match="f:protocols">
<xsl:text>firewall-ebtables-protocols-state=</xsl:text>
<xsl:value-of select="@state"/>
<xsl:value-of select="$newline"/>
<xsl:apply-templates select="f:protocol"/>
</xsl:template>

<xsl:template match="f:protocol">
<xsl:text>firewall-ebtables-protocol=</xsl:text>
<xsl:value-of select="@eproto"/>
<xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template match="f:whitelist/f:host">
<xsl:text>firewall-ebtables-wlhost=</xsl:text>
<xsl:value-of select="@state"/><xsl:text>|</xsl:text>
<xsl:value-of select="@mac"/><xsl:text>|</xsl:text>
<xsl:value-of select="@mask"/>
<xsl:value-of select="$newline"/>
</xsl:template>

</xsl:stylesheet>
