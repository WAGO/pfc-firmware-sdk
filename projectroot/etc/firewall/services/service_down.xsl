<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
                xmlns:f="http://www.wago.com/security/firewall"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="../transform.xsl"/>

<xsl:output method="text" indent="no" encoding="utf-8" media-type="text/plain"/>
<xsl:strip-space elements="*" />

<xsl:template match="/f:firewall">
<xsl:apply-templates select="f:ipv4/f:service[@name]"/>
<xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template match="f:ipv4/f:service[@name]">
    <xsl:text>*filter</xsl:text>
    <xsl:value-of select="$newline"/>
    <xsl:text>-F in_</xsl:text><xsl:value-of select="@name"/>
    <xsl:value-of select="$newline"/>
    <xsl:text>-D in_services -j in_</xsl:text><xsl:value-of select="@name"/>
    <xsl:value-of select="$newline"/>
    <xsl:text>-X in_</xsl:text><xsl:value-of select="@name"/>
    <xsl:value-of select="$newline"/>
    <xsl:text>COMMIT</xsl:text>
</xsl:template>

</xsl:stylesheet>
