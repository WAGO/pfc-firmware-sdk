<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
                xmlns:f="http://www.wago.com/security/firewall"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!--
    Templates for common xslt functions.
 -->

<xsl:variable name="newline" select="'&#x0A;'"/>

<!-- Search for 'limit' and 'burst' arguments and add both entries if present. -->
<xsl:template name="limit">
    <xsl:if test="@limit">
        <xsl:text> --limit </xsl:text><xsl:value-of select="@limit"/>
        <xsl:if test="@burst">
            <xsl:text> --limit-burst </xsl:text>
            <xsl:value-of select="@burst"/>
        </xsl:if>
    </xsl:if>
</xsl:template>

<xsl:template name="mlimit">
    <xsl:if test="@limit">
        <xsl:text> -m limit --limit </xsl:text><xsl:value-of select="@limit"/>
        <xsl:if test="@burst">
            <xsl:text> --limit-burst </xsl:text>
            <xsl:value-of select="@burst"/>
        </xsl:if>
    </xsl:if>
</xsl:template>

<!-- Lookups for interface filters - COMMON. -->
<xsl:variable name="parameters" select="document('./params_gen.xml')"/>

<!-- Get system interface name - use current element. -->
<xsl:template name="ifname-cur">
    <xsl:choose>
        <xsl:when test="current()/@rname">
            <xsl:value-of select="current()/@rname"/>
        </xsl:when>
        <xsl:when test="current()/@name">
            <xsl:value-of select="current()/@name"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="current()"/>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<!-- Get system interface name with IPsec policy flags - use current element. -->
<xsl:template name="ifname-ipsec-in-cur">
    <xsl:text>-i </xsl:text>
    <xsl:call-template name="ifname-cur"/>
    <xsl:if test="current()[@ipsec='yes']">
        <xsl:text> -m policy --dir in --pol ipsec --proto esp --mode tunnel</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template name="ifname-ipsec-out-cur">
    <xsl:text>-o </xsl:text>
    <xsl:call-template name="ifname-cur"/>
    <xsl:if test="current()[@ipsec='yes']">
        <xsl:text> -m policy --dir out --pol ipsec --proto esp --mode tunnel</xsl:text>
    </xsl:if>
</xsl:template>

</xsl:stylesheet>
