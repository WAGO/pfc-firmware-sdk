<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
                xmlns:f="http://www.wago.com/security/firewall"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="../transform.xsl"/>

<xsl:output method="text" indent="no" encoding="utf-8" media-type="text/plain"/>
<xsl:strip-space elements="*" />

<xsl:template match="/f:firewall/f:ipv4">
<xsl:text>
*filter
-F in_generic
-F in_climits
-F in_echo
-F in_rules
-F fw_common
-A in_generic -p 2 -j ACCEPT
</xsl:text>
<xsl:call-template name="ipsec-srv"/>
<xsl:apply-templates select="f:climits[@total or @limit or @tcp or @udp]"/>
<xsl:apply-templates select="f:echo[@policy='accept']"/>
<xsl:apply-templates select="f:echo[@policy='drop']"/>
<xsl:apply-templates select="f:forward[@state='on']"/>
<xsl:apply-templates select="f:forward[not(@state) or @state='off']/f:fw"/>
<xsl:apply-templates select="f:input/f:open/f:interface[@state='on' and @if]"/>
<xsl:apply-templates select="f:input/f:filter/f:rule[@state='on' and @proto and (@if or @src_ip or @src_port or @dst_ip or @dst_port)]"/>
<xsl:text>COMMIT</xsl:text>
<xsl:value-of select="$newline"/>
</xsl:template>

<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
<!-- SECTION START : CLIMITS -->
<xsl:template match="f:climits[@total or @limit or @tcp or @udp]">
    <xsl:if test="@total">
        <xsl:text>-A in_climits -m connlimit --connlimit-above </xsl:text>
        <xsl:value-of select="@total"/>
        <xsl:text> -j DROP</xsl:text>
        <xsl:value-of select="$newline"/>
    </xsl:if>
    <xsl:if test="@tcp">
        <xsl:text>-A in_climits -p tcp --syn -m conntrack --ctstate NEW -m limit --limit </xsl:text>
        <xsl:value-of select="@tcp"/>
        <xsl:text> -j RETURN</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:text>-A in_climits -p tcp --syn -m conntrack --ctstate NEW -j DROP</xsl:text>
        <xsl:value-of select="$newline"/>
    </xsl:if>
    <xsl:if test="@udp">
        <xsl:text>-A in_climits -p udp -m conntrack --ctstate NEW -m limit --limit </xsl:text>
        <xsl:value-of select="@udp"/>
        <xsl:text> -j RETURN</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:text>-A in_climits -p udp -m conntrack --ctstate NEW -j DROP</xsl:text>
        <xsl:value-of select="$newline"/>
    </xsl:if>
    <xsl:if test="@limit">
        <xsl:text>-A in_climits</xsl:text>
        <xsl:call-template name="mlimit"/>
        <xsl:text> -m conntrack --ctstate NEW -j RETURN</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:text>-A in_climits -j DROP</xsl:text>
        <xsl:value-of select="$newline"/>
    </xsl:if>
</xsl:template>
<!-- SECTION END   : CLIMITS -->

<!-- xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx -->
<!-- SECTION START : ECHO (WAT10525) -->
<xsl:template match="f:echo[@policy='accept']">
    <xsl:text>-A in_echo</xsl:text>
    <xsl:call-template name="mlimit"/>
    <xsl:text> -j ACCEPT</xsl:text>
    <xsl:value-of select="$newline"/>
    <xsl:if test="@limit">
        <xsl:text>-A in_echo -j DROP</xsl:text>
        <xsl:value-of select="$newline"/>
    </xsl:if>
</xsl:template>
<xsl:template match="f:echo[@policy='drop']">
    <xsl:for-each select="f:request[@policy='accept']">
        <xsl:variable name="el" select="current()"/>
        <xsl:variable name="li"><xsl:call-template name="mlimit"/></xsl:variable>
        <xsl:for-each select="$parameters/f:firewall/f:parameters/f:interfaces/f:interface[@name=$el/@if]">
            <xsl:text>-A in_echo </xsl:text>
            <xsl:call-template name="ifname-ipsec-in-cur"/>
            <xsl:value-of select="$li"/>
            <xsl:text> -j ACCEPT</xsl:text>
            <xsl:value-of select="$newline"/>
        </xsl:for-each>
    </xsl:for-each>
    <xsl:text>-A in_echo -j DROP</xsl:text>
    <xsl:value-of select="$newline"/>
</xsl:template>
<!-- SECTION END   : ECHO -->

<!-- SECTION START : INPUT (WAT10528 - filter) -->
<xsl:template match="f:input/f:open/f:interface[@state='on' and @if]">
    <xsl:variable name="el" select="current()"/>
    <xsl:for-each select="$parameters/f:firewall/f:parameters/f:interfaces/f:interface[@name=$el/@if]">
        <xsl:text>-A in_rules </xsl:text>
        <xsl:call-template name="ifname-ipsec-in-cur"/>
        <xsl:text> -j ACCEPT</xsl:text>
        <xsl:value-of select="$newline"/>
    </xsl:for-each>
</xsl:template>
<xsl:template name="input-filter">
    <xsl:param name="proto"/>
    <xsl:variable name="rl">
        <xsl:text> -p </xsl:text>
        <xsl:value-of select="$proto"/>
        <xsl:if test="@src_ip">
            <xsl:text> -s </xsl:text>
            <xsl:value-of select="@src_ip"/>
            <xsl:if test="@src_mask">
                <xsl:text>/</xsl:text>
                <xsl:value-of select="@src_mask"/>
            </xsl:if>
        </xsl:if>
        <xsl:if test="@src_port">
            <xsl:text> --sport </xsl:text>
            <xsl:value-of select="@src_port"/>
        </xsl:if>
        <xsl:if test="@dst_ip">
            <xsl:text> -d </xsl:text>
            <xsl:value-of select="@dst_ip"/>
            <xsl:if test="@dst_mask">
                <xsl:text>/</xsl:text>
                <xsl:value-of select="@dst_mask"/>
            </xsl:if>
        </xsl:if>
        <xsl:if test="@dst_port">
            <xsl:text> --dport </xsl:text>
            <xsl:value-of select="@dst_port"/>
        </xsl:if>
        <xsl:if test="@policy='accept'">
            <xsl:text> -j ACCEPT</xsl:text>
        </xsl:if>
        <xsl:if test="@policy='drop'">
            <xsl:text> -j DROP</xsl:text>
        </xsl:if>
    </xsl:variable>
    <xsl:if test="@if">
        <xsl:variable name="el" select="current()"/>
        <xsl:for-each select="$parameters/f:firewall/f:parameters/f:interfaces/f:interface[@name=$el/@if]">
            <xsl:text>-A in_rules </xsl:text>
            <xsl:call-template name="ifname-ipsec-in-cur"/>
            <xsl:value-of select="$rl"/>
            <xsl:value-of select="$newline"/>
        </xsl:for-each>
    </xsl:if>
    <xsl:if test="not(@if)">
        <xsl:text>-A in_rules </xsl:text>
        <xsl:value-of select="$rl"/>
        <xsl:value-of select="$newline"/>
    </xsl:if>
</xsl:template>
<xsl:template match="f:input/f:filter/f:rule[@state='on' and @proto and (@if or @src_ip or @src_port or @dst_ip or @dst_port)]">
    <xsl:if test="@proto='tcp' or @proto='udp'">
        <xsl:call-template name="input-filter">
            <xsl:with-param name="proto" select="@proto"/>
        </xsl:call-template>
    </xsl:if>
    <xsl:if test="@proto='tcpudp'">
        <xsl:call-template name="input-filter">
            <xsl:with-param name="proto">tcp</xsl:with-param>
        </xsl:call-template>
        <xsl:call-template name="input-filter">
            <xsl:with-param name="proto">udp</xsl:with-param>
        </xsl:call-template>
    </xsl:if>
</xsl:template>
<!-- SECTION END   : INPUT -->

<!-- SECTION START : IPsec -->
<xsl:template name="ipsec-rules">
    <xsl:param name="ifname"/>
    <xsl:param name="table"/>
    <xsl:param name="dir"/>

    <xsl:text>-A </xsl:text>
    <xsl:value-of select="$table"/>
    <xsl:text> -</xsl:text>
    <xsl:value-of select="$dir"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$ifname"/>
    <xsl:text> -p udp -m udp --dport 500 -j ACCEPT</xsl:text>
    <xsl:value-of select="$newline"/>

    <xsl:text>-A </xsl:text>
    <xsl:value-of select="$table"/>
    <xsl:text> -</xsl:text>
    <xsl:value-of select="$dir"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$ifname"/>
    <xsl:text> -p udp -m udp --dport 4500 -j ACCEPT</xsl:text>
    <xsl:value-of select="$newline"/>

    <xsl:text>-A </xsl:text>
    <xsl:value-of select="$table"/>
    <xsl:text> -</xsl:text>
    <xsl:value-of select="$dir"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$ifname"/>
    <xsl:text> -p esp -j ACCEPT</xsl:text>
    <xsl:value-of select="$newline"/>
</xsl:template>

<xsl:template name="ipsec-rname">
    <xsl:choose>
        <xsl:when test="@rname">
            <xsl:value-of select="@rname" />
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="@name" />
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="ipsec-srv">
    <xsl:for-each select="$parameters/f:firewall/f:parameters/f:interfaces/f:interface[@ipsec_srv='yes']">
        <xsl:variable name="ifname">
            <xsl:call-template name="ipsec-rname"/>
        </xsl:variable>
        <xsl:call-template name="ipsec-rules">
            <xsl:with-param name="ifname" select="$ifname" />
            <xsl:with-param name="table">in_generic</xsl:with-param>
            <xsl:with-param name="dir">i</xsl:with-param>
        </xsl:call-template>
    </xsl:for-each>
</xsl:template>
<!-- SECTION END   : IPsec -->

</xsl:stylesheet>
