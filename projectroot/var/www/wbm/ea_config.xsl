<?xml version='1.0' encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" encoding="ISO-8859-1"/>
<xsl:strip-space elements="WAGO" />
<xsl:template match="/">
  <html>
    <head>
      <meta http-equiv="content-type" content="text/html; charset=iso-8859-1"></meta>
      <link rel="stylesheet" type="text/css" href="styles.css"></link>
      <title>EA-config file</title>
    </head>
    <body>
      <!-#cat headline.html >

      <div id="lower_area" class="clearfix">

        <!-##print_navigation I/O-Configuration >

        <div id="content_area">

          <h2>I/O-Configuration</h2>

          <div class="liste">

            <table class="pi">
              <caption>I/O configuration and values</caption>
              <colgroup>
                <col width="20px"></col>
                <col width="74px"></col>
                <col width="70px"></col>
                <col></col>
              </colgroup>
              <tr>
                <th>Pos</th>
                <th>Module</th>
                <th>Type</th>
                <th class="values"><span class="channel">Channel</span>Values</th>
              </tr>
              <xsl:apply-templates />
            </table>
          </div>
        </div>
      </div>
    </body>
  </html>
</xsl:template>

<xsl:template match="Module">
  <tr>
    <td class="position"> <xsl:value-of select="@POSITION" /></td>
    <td><xsl:value-of select="@ARTIKELNR" /></td>
    <td><xsl:value-of select="@CHANNELCOUNT" /><xsl:value-of select="@MODULETYPE" /></td>
    <td class="channels" colspan="2">
      <table class="channels">
        <colgroup>
          <col width="63px"></col>
          <col></col>
        </colgroup>
        <xsl:apply-templates select="Kanal"/>
      </table>
    </td>
  </tr>
</xsl:template>


<xsl:template match="Kanal">
        <tr>
          <td><xsl:value-of select="@CHANNELNAME" /></td>
          <td class="values">
            <table class="values">
              <colgroup>
                <col width="60px"></col>
                <col></col>
              </colgroup>
              <xsl:apply-templates select="Value" />
            </table>
          </td>
        </tr>
</xsl:template>

<xsl:template match="Value">
            <tr>
              <td class="direction"><xsl:value-of select="@IEC_ADR" /></td>
              <td><xsl:value-of select="." /></td>
            </tr>
</xsl:template>

</xsl:stylesheet>