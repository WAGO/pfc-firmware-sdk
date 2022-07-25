<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text"/>
<xsl:template match="/">
<xsl:text  disable-output-escaping="yes">
<![CDATA[ //------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     diagnostic header file
///
///  \version  $Revision: 1 $
///
///  \brief    Automatically generated Header for Error-Codes in PAC
///
///  \author   Hans Florian Scholz : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
///defines
//------------------------------------------------------------------------------]]>
</xsl:text>
  <xsl:for-each select="diagnostic/eventclass"> 
  <xsl:sort select="@class_range"/>
  <xsl:variable name="class_name" select="@name"/>
  <xsl:variable name="class_id_range" select="@class_range"/>
  
  <xsl:variable name="class_event_id">
	  <xsl:call-template name="create_id">
	    <xsl:with-param name="class_name" select="'CLASS'" />
	    <xsl:with-param name="element_name" select="$class_name" />
	    <xsl:with-param name="class_id_range" select="$class_id_range" />
	    <xsl:with-param name="act_id" select="'x0000'" />
	    <xsl:with-param name="persistent" select="'no'" />
	  </xsl:call-template>
  </xsl:variable>
  <xsl:call-template name="define_output">
    <xsl:with-param name="define" select="concat('DIAG_CLASS_', $class_name)"/>
    <xsl:with-param name="id" select="$class_event_id"/>
    <xsl:with-param name="comment" select="concat('ClassID', ' not for use as LogId')"/>
    <xsl:with-param name="anzValue" select="40"/>
    <xsl:with-param name="anzComment" select="12"/>
  </xsl:call-template>
  <xsl:text>&#10;</xsl:text>
  
    <xsl:for-each select="event">  
    <xsl:sort select="@id"/>
    <xsl:variable name="event_id">
      <xsl:call-template name="create_id">
        <xsl:with-param name="class_name" select="$class_name" />
        <xsl:with-param name="element_name" select="@name" />
        <xsl:with-param name="class_id_range" select="$class_id_range" />
        <xsl:with-param name="act_id" select="@id" />
        <xsl:with-param name="persistent" select="@persistent" />
      </xsl:call-template>
    </xsl:variable>
      <xsl:call-template name="define_output">
        <xsl:with-param name="define" select="concat('DIAG_', $class_name, '_', @name)"/>
        <xsl:with-param name="id" select="$event_id"/>
        <xsl:with-param name="comment" select="string/en"/>
        <xsl:with-param name="anzValue" select="40"/>
        <xsl:with-param name="anzComment" select="12"/>
      </xsl:call-template>
      <xsl:text>&#10;</xsl:text>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>

<xsl:variable name="smallcase" select="'abcdefghijklmnopqrstuvwxyz-'" />
<xsl:variable name="uppercase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ_'" />

<xsl:template name="create_id">
  <xsl:param name="class_id_range"/>
  <xsl:param name="act_id" type="xs:hexBinary?"/>
  <xsl:param name="class_name"/>
  <xsl:param name="element_name"/>
  <xsl:param name="persistent"/>
  
  <xsl:variable name="id_range_error">
    <xsl:choose>
      <xsl:when test="not(string-length($class_id_range) = 5)">1</xsl:when>
      <xsl:when test="not(substring($class_id_range, 1, 1) = 'x')">1</xsl:when>
      <xsl:when test="not(translate(substring($class_id_range, 2,4),'0123456789abcdefABCDEF','') = '')">1</xsl:when>
      <xsl:when test="translate(substring($class_id_range, 2,1),'456789abcdefCDEF','') = ''">1</xsl:when>
      <xsl:otherwise>0</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  
  <xsl:if test="$id_range_error = 1">
    <xsl:message terminate="yes">class:0: "<xsl:value-of select="$class_name" />"at element "<xsl:value-of select="$element_name" />": error: invalid class_range: <xsl:value-of select="$class_id_range" /></xsl:message> 
  </xsl:if>

  <xsl:variable name="act_id_error">
    <xsl:choose>
      <xsl:when test="not(string-length($act_id) = 5)">1</xsl:when>
      <xsl:when test="not(substring($act_id, 1, 1) = 'x')">1</xsl:when>
      <xsl:when test="not(translate(substring($act_id, 2,4),'0123456789abcdefABCDEF','') = '')">1</xsl:when>
      <xsl:otherwise>0</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:if test="$act_id_error = 1">
    <xsl:message terminate="yes">class:0: "<xsl:value-of select="$class_name" />" at element "<xsl:value-of select="$element_name" />": error: invalid id: <xsl:value-of select="$act_id" /></xsl:message> 
  </xsl:if>
  
  <xsl:text> 0x</xsl:text>
  <xsl:choose>
    <xsl:when test="$persistent = 'yes'">
      <xsl:value-of select="translate(substring($class_id_range, 2, 1), '0123', '89AB')" />
    </xsl:when>
    <xsl:when test="$persistent = 'none'">
      <xsl:value-of select="translate(substring($class_id_range, 2, 1), '0123', '4567')" />
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="substring($class_id_range, 2, 1)" />
    </xsl:otherwise>
  </xsl:choose>
  
  <xsl:value-of select="substring($class_id_range, 3)" />
  <xsl:value-of select="substring($act_id, 2)" />
  
</xsl:template>

<xsl:template name="define_output">

  <xsl:param name="define"/>
  <xsl:param name="id"/>
  <xsl:param name="comment"/>
  <xsl:param name="anzValue"/>
  <xsl:param name="anzComment"/>

  
  <xsl:call-template name="string_output">
    <xsl:with-param name="str">
      <xsl:call-template name="string_output">
        <xsl:with-param name="str" select="concat('#define ', translate($define, $smallcase, $uppercase))"/>
        <xsl:with-param name="anz" select="$anzValue"/>
        <xsl:with-param name="str2" select="$id"/>
      </xsl:call-template>
    </xsl:with-param>
    <xsl:with-param name="anz" select="$anzValue + $anzComment"/>
    <xsl:with-param name="str2" select="concat('/* ', $comment, ' */')"/>
  </xsl:call-template>
  
</xsl:template>

<xsl:template name="string_output">

  <xsl:param name="str"/>
  <xsl:param name="str2"/>
  <xsl:param name="anz"/>

  <xsl:choose>
    <xsl:when test="string-length($str) &lt; $anz">
      <xsl:call-template name="string_output">
        <xsl:with-param name="str" select="concat($str,'&#x20;')"/>
        <xsl:with-param name="anz" select="$anz"/>
        <xsl:with-param name="str2" select="$str2"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="concat($str,$str2)"/>
    </xsl:otherwise>    
  </xsl:choose>
</xsl:template>


</xsl:stylesheet>