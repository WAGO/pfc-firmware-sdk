<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
<diagnostic>
<xsl:text>&#10;</xsl:text>
  <xsl:for-each select="diagnostic/eventclass"> 
  <xsl:sort select="@class_range"/>
    <eventclass>
    <xsl:attribute name="class_range"><xsl:value-of select="@class_range" /></xsl:attribute>
    <xsl:attribute name="name"><xsl:value-of select="@name" /></xsl:attribute>
    <xsl:text>&#10;</xsl:text>
    <xsl:for-each select="event">  
      <xsl:sort select="@id"/>
      <xsl:text>  </xsl:text>
      <event>
      <xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
      <xsl:attribute name="name"><xsl:value-of select="@name" /></xsl:attribute>
          <xsl:if test="boolean(@persistent)">
    <xsl:choose>
	     <xsl:when test="@persistent = 'yes'">
		   <xsl:attribute name="persistent">yes</xsl:attribute>
		 </xsl:when>
		 <xsl:when test="@persistent = 'none'">
		   <xsl:attribute name="persistent">none</xsl:attribute>
		 </xsl:when>
		 <xsl:when test="@persistent = 'no'"></xsl:when>
		<xsl:otherwise>
		  <xsl:message terminate="yes">class:0: "<xsl:value-of select="@name" />" error: invalid persistency: <xsl:value-of select="@persistent" />! Possible are yes and no</xsl:message>
	   </xsl:otherwise> 
    </xsl:choose>
    </xsl:if>
      <xsl:text>&#10;    </xsl:text>
        <xsl:if test="boolean(led)">
          <xsl:call-template name="create_led">
          <xsl:with-param name="newline" select="'&#10;      '"/>
          </xsl:call-template>
          <xsl:text>&#10;    </xsl:text>
        </xsl:if>
        <xsl:copy-of select="string" />
        <xsl:text>&#10;    </xsl:text>
        <xsl:copy-of select="rststr" />
        <xsl:text>&#10;  </xsl:text>
      </event>
      <xsl:text>&#10;</xsl:text>
    </xsl:for-each>
    </eventclass><xsl:text>&#10;</xsl:text>
  </xsl:for-each>
  <xsl:text>&#10;</xsl:text>
</diagnostic>
</xsl:template>

<xsl:variable name="smallcase" select="'abcdefghijklmnopqrstuvwxyz-'" />
<xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ_'" />

<xsl:template name="create_led">
  <xsl:param name="newline"/>

  <led>
  <xsl:attribute name="name"><xsl:value-of select="led/@name" /></xsl:attribute>
  
   <xsl:if test="boolean(led/@default)">
     <xsl:choose>
	   <xsl:when test="led/@default = 'yes'">
	   <xsl:attribute name="default">yes</xsl:attribute>
	   </xsl:when>
	   <xsl:when test="led/@default = 'only'">
	   <xsl:attribute name="default">only</xsl:attribute>
	   </xsl:when>
	   <xsl:when test="led/@default = 'no'"></xsl:when>
	   <xsl:otherwise></xsl:otherwise> 
     </xsl:choose>     
   </xsl:if>
  
  <xsl:attribute name="state"><xsl:value-of select="led/@state" /></xsl:attribute>
  <xsl:choose>
    <xsl:when test="led/@state = 'BLINK_SEQ_750'">
      <xsl:value-of select="$newline"/>
      <blinkcode><xsl:value-of select="led/blinkcode"/></blinkcode>
      <xsl:value-of select="$newline"/>
      <blinkarg><xsl:value-of select="led/blinkarg"/></blinkarg>
    </xsl:when>
    <xsl:when test="led/@state = 'SPECIAL_BLINK'">
      <xsl:value-of select="$newline"/>
      <blinkcode><xsl:value-of select="led/blinkcode"/></blinkcode>
      <xsl:value-of select="$newline"/>
      <blinkarg><xsl:value-of select="led/blinkarg"/></blinkarg>
    </xsl:when>
    <xsl:when test="led/@state = 'BLINK_SEQ_RESET'"></xsl:when>
    <xsl:when test="substring(led/@state,1,5) = 'BLINK'">
      <xsl:value-of select="$newline"/>
      <on><xsl:value-of select="led/on"/></on>
      <xsl:value-of select="$newline"/>
      <off><xsl:value-of select="led/off"/></off>
    </xsl:when>
    <xsl:when test="substring(led/@state,1,5) = 'FLASH'">
      <xsl:if test="not(boolean(led/on))">
            <xsl:message terminate="yes">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": error: element &lt;on&gt; must be defined for blinking!</xsl:message> 
      </xsl:if>
      <xsl:value-of select="$newline"/>
      <on><xsl:value-of select="led/on"/></on>
    </xsl:when>
  </xsl:choose>
  <xsl:value-of select="substring($newline, 1, string-length($newline) - 2)"/>
  </led>
  
</xsl:template>
</xsl:stylesheet>