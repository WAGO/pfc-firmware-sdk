<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
    xmlns:f="http://www.wago.com/security/firewall" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

    <xsl:include href="../transform.xsl"/>

    <xsl:output method="text" indent="no" encoding="utf-8" media-type="text/plain"/>
    <xsl:strip-space elements="*" />

    <xsl:template match="/f:firewall/f:ipv4/f:service">
        <xsl:variable name="srvname" select="@name"/>
        <xsl:variable name="prefix">firewall-service-<xsl:value-of select="$srvname"/>
        </xsl:variable>
        <xsl:text>firewall-service=</xsl:text>
        <xsl:value-of select="$srvname"/>
        <xsl:value-of select="$newline"/>
        <xsl:apply-templates select="f:interfaces/f:interface">
            <xsl:with-param name="prefix" select="$prefix"/>
        </xsl:apply-templates>
        <xsl:apply-templates select="f:rules/f:rule">
            <xsl:with-param name="prefix" select="$prefix"/>
        </xsl:apply-templates>
    </xsl:template>

    <xsl:template match="f:interfaces/f:interface">
        <xsl:param name="prefix"/>
        <xsl:choose>
            <xsl:when test="@if = 'br0'">
                <xsl:call-template name="print_interface">
                    <xsl:with-param name="prefix" select="$prefix"/>
                    <xsl:with-param name="state" select="@state" />
                    <xsl:with-param name="ifname" select="'X1'" />
                </xsl:call-template>
            </xsl:when>
            <xsl:when test="@if = 'br1'">
                <xsl:call-template name="print_interface">
                    <xsl:with-param name="prefix" select="$prefix"/>
                    <xsl:with-param name="state" select="@state" />
                    <xsl:with-param name="ifname" select="'X2'" />
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:call-template name="print_interface">
                    <xsl:with-param name="prefix" select="$prefix"/>
                    <xsl:with-param name="state" select="@state" />
                    <xsl:with-param name="ifname" select="@if" />
                </xsl:call-template>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="f:rules/f:rule">
        <xsl:param name="prefix"/>
        <xsl:value-of select="$prefix"/>
        <xsl:text>-rule=</xsl:text>
        <xsl:value-of select="@state"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@proto"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@src_port"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@dst_port"/>
        <xsl:value-of select="$newline"/>
    </xsl:template>

    <xsl:template name = "print_interface">
        <xsl:param name="prefix"/>
        <xsl:param name="state"/>
        <xsl:param name="ifname"/>
        <xsl:value-of select="$prefix"/>
        <xsl:text>-interface=</xsl:text>
        <xsl:value-of select="$state"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$ifname"/>
        <xsl:value-of select="$newline"/>
    </xsl:template>

</xsl:stylesheet>
