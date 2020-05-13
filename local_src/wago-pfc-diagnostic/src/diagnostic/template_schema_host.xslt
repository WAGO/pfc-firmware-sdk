<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
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
      <xsl:if test="boolean(comment)">
        <xsl:text>&#10;    </xsl:text>
          <comment>
            <xsl:text>&#10;</xsl:text>
            <xsl:value-of select="comment" />
            
            <xsl:text>&#10;   </xsl:text>
          </comment>
        <xsl:text>&#10;    </xsl:text>
      </xsl:if>
        <xsl:if test="boolean(led)">
          <xsl:text>&#10;    </xsl:text>
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
    </eventclass>
  </xsl:for-each>
</xsl:template>

<xsl:variable name="smallcase" select="'abcdefghijklmnopqrstuvwxyz-'" />
<xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ_'" />

<xsl:template name="create_led">
  <xsl:param name="newline"/>

  <led>
  <xsl:attribute name="name">
    <xsl:choose>
      <xsl:when test="led/@name = 'NONE'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'IO'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'USER'">U1<xsl:message terminate="no">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": warning: Obsolet LED name: <xsl:value-of select="led/@name" /> use U1 unstead</xsl:message></xsl:when>
      <xsl:when test="led/@name = 'U1'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'KBUS'">IO<xsl:message terminate="no">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": warning: Obsolet LED name: <xsl:value-of select="led/@name" /> use IO unstead</xsl:message></xsl:when>
      <xsl:when test="led/@name = 'CODESYS'">SYS<xsl:message terminate="no">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": warning: Obsolet LED name: <xsl:value-of select="led/@name" /> use SYS unstead</xsl:message></xsl:when>
      <xsl:when test="led/@name = 'SYS'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'ETH_IP_MS'">MS<xsl:message terminate="no">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": warning: Obsolet LED name: <xsl:value-of select="led/@name" /> use MS unstead</xsl:message></xsl:when>
      <xsl:when test="led/@name = 'ETH_IP_NS'">NS<xsl:message terminate="no">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": warning: Obsolet LED name: <xsl:value-of select="led/@name" /> use Ns unstead</xsl:message></xsl:when>
      <xsl:when test="led/@name = 'FB1'">MS<xsl:message terminate="no">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": warning: Obsolet LED name: <xsl:value-of select="led/@name" /> use MS unstead</xsl:message></xsl:when>
      <xsl:when test="led/@name = 'FB2'">NS<xsl:message terminate="no">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": warning: Obsolet LED name: <xsl:value-of select="led/@name" /> use NS unstead</xsl:message></xsl:when>
      <xsl:when test="led/@name = 'FB3'">U2<xsl:message terminate="no">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": warning: Obsolet LED name: <xsl:value-of select="led/@name" /> use U2 unstead</xsl:message></xsl:when>
      <xsl:when test="led/@name = 'FB3'">U3<xsl:message terminate="no">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": warning: Obsolet LED name: <xsl:value-of select="led/@name" /> use U3 unstead</xsl:message></xsl:when>
      <xsl:when test="led/@name = 'FB5'">U4<xsl:message terminate="no">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": warning: Obsolet LED name: <xsl:value-of select="led/@name" /> use U4 unstead</xsl:message></xsl:when>
      <xsl:when test="led/@name = 'U2'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'U3'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'U4'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'U5'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'U6'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'U7'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'U8'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'RUN'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'DIA'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'BF'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'CAN'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'MS'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'NS'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'S1'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'S2'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'S3'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'S4'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'S5'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'NET'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'USR'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'BT'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'WDS'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'EC'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'H11'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'H12'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:when test="led/@name = 'DS'"><xsl:value-of select="led/@name" /></xsl:when>
      <xsl:otherwise>
        <xsl:message terminate="yes">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": error: Invalid LED name: <xsl:value-of select="led/@name" /></xsl:message> 
      </xsl:otherwise>
    </xsl:choose>
   </xsl:attribute>

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
    <xsl:when test="led/@state = 'NONE'"></xsl:when>
    <xsl:when test="led/@state = 'STATIC_OFF'"></xsl:when>
    <xsl:when test="led/@state = 'STATIC_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'STATIC_RED'"></xsl:when>
    <xsl:when test="led/@state = 'STATIC_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'STATIC_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'STATIC_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'STATIC_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'STATIC_WHT'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_RED'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_WHT'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_GRN_RED'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_GRN_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_GRN_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_GRN_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_GRN_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_GRN_WHT'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_RED_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_RED_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_RED_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_RED_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_RED_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_RED_WHT'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_OFF_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_OFF_RED'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_OFF_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_OFF_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_OFF_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_OFF_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_OFF_WHT'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_YLW_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_YLW_RED'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_YLW_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_YLW_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_YLW_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_YLW_WHT'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_BLU_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_BLU_RED'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_BLU_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_BLU_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_BLU_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_BLU_WHT'"></xsl:when>
	<xsl:when test="led/@state = 'BLINK_CYN_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_CYN_RED'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_CYN_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_CYN_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_CYN_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_CYN_WHT'"></xsl:when>
	<xsl:when test="led/@state = 'BLINK_MAG_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_MAG_RED'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_MAG_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_MAG_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_MAG_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_MAG_WHT'"></xsl:when>
	<xsl:when test="led/@state = 'BLINK_WHT_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_WHT_RED'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_WHT_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_WHT_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_WHT_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_WHT_MAG'"></xsl:when>
	<xsl:when test="led/@state = 'BLINK_SEQ_750'"></xsl:when>
    <xsl:when test="led/@state = 'BLINK_SEQ_RESET'"></xsl:when>     
	<xsl:when test="led/@state = 'FLASH_GRN_OFF'"></xsl:when>
	<xsl:when test="led/@state = 'FLASH_GRN_RED'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_GRN_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_GRN_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_GRN_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_GRN_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_GRN_WHT'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_RED_OFF'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_RED_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_RED_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_RED_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_RED_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_RED_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_RED_WHT'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_OFF_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_OFF_RED'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_OFF_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_OFF_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_OFF_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_OFF_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_OFF_WHT'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_YLW_OFF'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_YLW_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_YLW_RED'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_YLW_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_YLW_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_YLW_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_YLW_WHT'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_BLU_OFF'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_BLU_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_BLU_RED'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_BLU_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_BLU_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_BLU_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_BLU_WHT'"></xsl:when>
	<xsl:when test="led/@state = 'FLASH_CYN_OFF'"></xsl:when>
	<xsl:when test="led/@state = 'FLASH_CYN_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_CYN_RED'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_CYN_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_CYN_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_CYN_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_CYN_WHT'"></xsl:when>
	<xsl:when test="led/@state = 'FLASH_MAG_OFF'"></xsl:when>
	<xsl:when test="led/@state = 'FLASH_MAG_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_MAG_RED'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_MAG_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_MAG_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_MAG_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_MAG_WHT'"></xsl:when>
	<xsl:when test="led/@state = 'FLASH_WHT_OFF'"></xsl:when>
	<xsl:when test="led/@state = 'FLASH_WHT_GRN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_WHT_RED'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_WHT_YLW'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_WHT_BLU'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_WHT_CYN'"></xsl:when>
    <xsl:when test="led/@state = 'FLASH_WHT_MAG'"></xsl:when>
    <xsl:when test="led/@state = 'SPECIAL_BLINK'"></xsl:when>
    <xsl:otherwise>
      <xsl:message terminate="yes">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": error: Invalid LED state: <xsl:value-of select="led/@state" /></xsl:message> 
    </xsl:otherwise>
  </xsl:choose>



  <xsl:choose>
    <xsl:when test="led/@state = 'BLINK_SEQ_750'">
      <xsl:if test="not(boolean(led/blinkcode))">
            <xsl:message terminate="yes">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": error: element &lt;blinkcode&gt; must be defined for blinking!</xsl:message> 
      </xsl:if>
      <xsl:if test="not(boolean(led/blinkarg))">
            <xsl:message terminate="yes">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": error: element &lt;blinkarg&gt; must be defined for blinking!</xsl:message> 
      </xsl:if>
      <xsl:value-of select="$newline"/>
      <blinkcode><xsl:value-of select="led/blinkcode"/></blinkcode>
      <xsl:value-of select="$newline"/>
      <blinkarg><xsl:value-of select="led/blinkarg"/></blinkarg>
    </xsl:when>
    <xsl:when test="led/@state = 'SPECIAL_BLINK'">
      <xsl:if test="not(boolean(led/blinkcode))">
            <xsl:message terminate="yes">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": error: element &lt;blinkcode&gt; must be defined for blinking!</xsl:message> 
      </xsl:if>
      <xsl:if test="not(boolean(led/blinkarg))">
            <xsl:message terminate="yes">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": error: element &lt;blinkarg&gt; must be defined for blinking!</xsl:message> 
      </xsl:if>
      <xsl:value-of select="$newline"/>
      <blinkcode><xsl:value-of select="led/blinkcode"/></blinkcode>
      <xsl:value-of select="$newline"/>
      <blinkarg><xsl:value-of select="led/blinkarg"/></blinkarg>
    </xsl:when>
    <xsl:when test="led/@state = 'BLINK_SEQ_RESET'"></xsl:when>
    <xsl:when test="substring(led/@state,1,5) = 'BLINK'">
      <xsl:if test="not(boolean(led/on))">
            <xsl:message terminate="yes">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": error: element &lt;on&gt; must be defined for blinking!</xsl:message> 
      </xsl:if>
      <xsl:if test="not(boolean(led/off))">
            <xsl:message terminate="yes">class:0: "<xsl:value-of select="../@name" />" at element "<xsl:value-of select="@name" />": error: element &lt;off&gt; must be defined for blinking!</xsl:message> 
      </xsl:if>
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
