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
        <xsl:value-of select="@total"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@limit"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@burst"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@tcp"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@udp"/>
        <xsl:value-of select="$newline"/>
    </xsl:template>

    <xsl:template match="f:echo">
        <xsl:text>firewall-iptables-echo=</xsl:text>
        <xsl:value-of select="@policy"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@limit"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@burst"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@broadcast_protection"/>
        <xsl:value-of select="$newline"/>
        <xsl:apply-templates select="f:request"/>
    </xsl:template>

    <xsl:template match="f:request">
        <xsl:choose>
            <xsl:when test="@if = 'br0'">
                <xsl:call-template name="print_request">
                    <xsl:with-param name = "policy" select="@policy"/>
                    <xsl:with-param name= "ifname" select="'X1'" />
                    <xsl:with-param name = "limit" select="@limit"/>
                    <xsl:with-param name = "burst" select="@burst"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:when test="@if = 'br1'">
                <xsl:call-template name="print_request">
                    <xsl:with-param name = "policy" select="@policy"/>
                    <xsl:with-param name= "ifname" select="'X2'" />
                    <xsl:with-param name = "limit" select="@limit"/>
                    <xsl:with-param name = "burst" select="@burst"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:call-template name="print_request">
                    <xsl:with-param name = "policy" select="@policy"/>
                    <xsl:with-param name = "ifname" select="@if"/>
                    <xsl:with-param name = "limit" select="@limit"/>
                    <xsl:with-param name = "burst" select="@burst"/>
                </xsl:call-template>
            </xsl:otherwise>
        </xsl:choose>
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
        <xsl:choose>
            <xsl:when test="@if1 = 'br0'">
                <xsl:choose>
                    <xsl:when test="@if2 = 'br1'">
                        <xsl:call-template name="print_forward_link">
                            <xsl:with-param name = "state" select="@state"/>
                            <xsl:with-param name = "ifname1" select="'X1'"/>
                            <xsl:with-param name = "ifname2" select="'X2'"/>
                        </xsl:call-template>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:call-template name="print_forward_link">
                            <xsl:with-param name = "state" select="@state"/>
                            <xsl:with-param name = "ifname1" select="'X1'"/>
                            <xsl:with-param name = "ifname2" select="@if2"/>
                        </xsl:call-template>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:when test="@if1 = 'br1'">
                <xsl:choose>
                    <xsl:when test="@if2 = 'br0'">
                        <xsl:call-template name="print_forward_link">
                            <xsl:with-param name = "state" select="@state"/>
                            <xsl:with-param name = "ifname1" select="'X2'"/>
                            <xsl:with-param name = "ifname2" select="'X1'"/>
                        </xsl:call-template>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:call-template name="print_forward_link">
                            <xsl:with-param name = "state" select="@state"/>
                            <xsl:with-param name = "ifname1" select="'X2'"/>
                            <xsl:with-param name = "ifname2" select="@if2"/>
                        </xsl:call-template>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
                <xsl:call-template name="print_forward_link">
                    <xsl:with-param name = "state" select="@state"/>
                    <xsl:with-param name = "ifname1" select="@if1"/>
                    <xsl:with-param name = "ifname2" select="@if2"/>
                </xsl:call-template>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="f:snat/f:masquerade">
        <xsl:text>firewall-iptables-snatmasq=</xsl:text>
        <xsl:value-of select="@if"/>
        <xsl:value-of select="$newline"/>
    </xsl:template>

    <xsl:template match="f:dnat/f:fw_port">
        <xsl:text>firewall-iptables-dnatfwp=</xsl:text>
        <xsl:value-of select="@state"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@if"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@proto"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@dst_ip"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@dst_port"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@fw_ip"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="@fw_port"/>
        <xsl:value-of select="$newline"/>
    </xsl:template>

    <xsl:template match="f:input">
        <xsl:apply-templates select="f:open/f:interface"/>
        <xsl:apply-templates select="f:filter/f:rule"/>
    </xsl:template>

    <xsl:template match="f:open/f:interface">
        <xsl:choose>
            <xsl:when test="@if = 'br0'">
                <xsl:call-template name="print_open_interface">
                    <xsl:with-param name = "state" select="@state"/>
                    <xsl:with-param name = "ifname" select="'X1'"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:when test="@if = 'br1'">
                <xsl:call-template name="print_open_interface">
                    <xsl:with-param name = "state" select="@state"/>
                    <xsl:with-param name = "ifname" select="'X2'"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:call-template name="print_open_interface">
                    <xsl:with-param name = "state" select="@state"/>
                    <xsl:with-param name = "ifname" select="@if"/>
                </xsl:call-template>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="f:filter/f:rule">
        <xsl:choose>
            <xsl:when test="@if = 'br0'">
                <xsl:call-template name="print_filter_rule">
                    <xsl:with-param name = "state" select="@state"/>
                    <xsl:with-param name = "ifname" select="'X1'"/>
                    <xsl:with-param name = "proto" select="@proto"/>
                    <xsl:with-param name = "src_ip" select="@src_ip"/>
                    <xsl:with-param name = "src_mask" select="@src_mask"/>
                    <xsl:with-param name = "src_port" select="@src_port"/>
                    <xsl:with-param name = "dst_ip" select="@dst_ip"/>
                    <xsl:with-param name = "dst_mask" select="@dst_mask"/>
                    <xsl:with-param name = "dst_port" select="@dst_port"/>
                    <xsl:with-param name = "policy" select="@policy"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:when test="@if = 'br1'">
                <xsl:call-template name="print_filter_rule">
                    <xsl:with-param name = "state" select="@state"/>
                    <xsl:with-param name = "ifname" select="'X2'"/>
                    <xsl:with-param name = "proto" select="@proto"/>
                    <xsl:with-param name = "src_ip" select="@src_ip"/>
                    <xsl:with-param name = "src_mask" select="@src_mask"/>
                    <xsl:with-param name = "src_port" select="@src_port"/>
                    <xsl:with-param name = "dst_ip" select="@dst_ip"/>
                    <xsl:with-param name = "dst_mask" select="@dst_mask"/>
                    <xsl:with-param name = "dst_port" select="@dst_port"/>
                    <xsl:with-param name = "policy" select="@policy"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:call-template name="print_filter_rule">
                    <xsl:with-param name = "state" select="@state"/>
                    <xsl:with-param name = "ifname" select="@if"/>
                    <xsl:with-param name = "proto" select="@proto"/>
                    <xsl:with-param name = "src_ip" select="@src_ip"/>
                    <xsl:with-param name = "src_mask" select="@src_mask"/>
                    <xsl:with-param name = "src_port" select="@src_port"/>
                    <xsl:with-param name = "dst_ip" select="@dst_ip"/>
                    <xsl:with-param name = "dst_mask" select="@dst_mask"/>
                    <xsl:with-param name = "dst_port" select="@dst_port"/>
                    <xsl:with-param name = "policy" select="@policy"/>
                </xsl:call-template>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template name = "print_request">
        <xsl:param name = "policy"/>
        <xsl:param name = "ifname"/>
        <xsl:param name = "limit"/>
        <xsl:param name = "burst"/>
        <xsl:text>firewall-iptables-echo-request=</xsl:text>
        <xsl:value-of select="$policy"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$ifname"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$limit"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$burst"/>
        <xsl:value-of select="$newline"/>
    </xsl:template>

    <xsl:template name = "print_forward_link">
        <xsl:param name = "state"/>
        <xsl:param name = "ifname1"/>
        <xsl:param name = "ifname2"/>
        <xsl:text>firewall-iptables-fwlink=</xsl:text>
        <xsl:value-of select="$state"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$ifname1"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$ifname2"/>
        <xsl:value-of select="$newline"/>
    </xsl:template>

    <xsl:template name = "print_open_interface">
        <xsl:param name = "state"/>
        <xsl:param name = "ifname"/>
        <xsl:text>firewall-iptables-input-openif=</xsl:text>
        <xsl:value-of select="$state"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$ifname"/>
        <xsl:value-of select="$newline"/>
    </xsl:template>

    <xsl:template name = "print_filter_rule">
        <xsl:param name = "state"/>
        <xsl:param name = "ifname"/>
        <xsl:param name = "proto"/>
        <xsl:param name = "src_ip"/>
        <xsl:param name = "src_mask"/>
        <xsl:param name = "src_port"/>
        <xsl:param name = "dst_ip"/>
        <xsl:param name = "dst_mask"/>
        <xsl:param name = "dst_port"/>
        <xsl:param name = "policy"/>
        <xsl:text>firewall-iptables-input-filter=</xsl:text>
        <xsl:value-of select="$state"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$ifname"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$proto"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$src_ip"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$src_mask"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$src_port"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$dst_ip"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$dst_mask"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$dst_port"/>
        <xsl:text>|</xsl:text>
        <xsl:value-of select="$policy"/>
        <xsl:value-of select="$newline"/>
    </xsl:template>
</xsl:stylesheet>
