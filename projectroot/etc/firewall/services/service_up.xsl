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
<xsl:variable name="srv_name" select="@name"/>
<xsl:text>*filter</xsl:text>
<xsl:value-of select="$newline"/>
<xsl:text>:in_</xsl:text><xsl:value-of select="@name"/><xsl:text> - [0:0]</xsl:text>
<xsl:value-of select="$newline"/>
<xsl:apply-templates select="f:interfaces/f:interface[@state='on']">
    <xsl:with-param name="srv_name" select="$srv_name"/>
</xsl:apply-templates>
<xsl:text>-A in_services -j in_</xsl:text><xsl:value-of select="@name"/>
<xsl:value-of select="$newline"/>
<xsl:text>COMMIT</xsl:text>
</xsl:template>

<xsl:template match="f:interfaces/f:interface[@state='on']">
    <xsl:param name="srv_name"/>

    <xsl:variable name="el" select="current()"/>
    <xsl:for-each select="$parameters/f:firewall/f:parameters/f:interfaces/f:interface[@name=$el/@if]">
        <xsl:variable name="if">
            <xsl:call-template name="ifname-ipsec-in-cur"/>
        </xsl:variable>
        <xsl:apply-templates select="$el/../../f:rules/f:rule[@state='on' and @proto and @dst_port]">
            <xsl:with-param name="srv_name" select="$srv_name"/>
            <xsl:with-param name="if" select="$if"/>
        </xsl:apply-templates>
    </xsl:for-each>
</xsl:template>

<xsl:template name="service-input-filter">
    <xsl:param name="srv_name"/>
    <xsl:param name="if"/>
    <xsl:param name="proto"/>

    <xsl:text>-A in_</xsl:text>
    <xsl:value-of select="$srv_name"/>

    <xsl:text> </xsl:text>
    <xsl:value-of select="$if"/>

    <xsl:text> -p </xsl:text>
    <xsl:value-of select="$proto"/>

    <xsl:if test="@src_port">
        <xsl:text> --sport </xsl:text>
        <xsl:value-of select="@src_port"/>
    </xsl:if>

    <xsl:text> --dport </xsl:text>
    <xsl:value-of select="@dst_port"/>

    <xsl:text> -j ACCEPT</xsl:text>
    <xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template match="f:rules/f:rule[@state='on' and @proto and @dst_port]">
    <xsl:param name="srv_name"/>
    <xsl:param name="if"/>

    <xsl:if test="@proto='tcp' or @proto='udp'">
        <xsl:call-template name="service-input-filter">
            <xsl:with-param name="srv_name" select="$srv_name"/>
            <xsl:with-param name="if" select="$if"/>
            <xsl:with-param name="proto" select="@proto"/>
        </xsl:call-template>
    </xsl:if>
    <xsl:if test="@proto='tcpudp'">
        <xsl:call-template name="service-input-filter">
            <xsl:with-param name="srv_name" select="$srv_name"/>
            <xsl:with-param name="if" select="$if"/>
            <xsl:with-param name="proto">tcp</xsl:with-param>
        </xsl:call-template>
        <xsl:call-template name="service-input-filter">
            <xsl:with-param name="srv_name" select="$srv_name"/>
            <xsl:with-param name="if" select="$if"/>
            <xsl:with-param name="proto">udp</xsl:with-param>
        </xsl:call-template>
    </xsl:if>
</xsl:template>

</xsl:stylesheet>
