<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:import href="template_schema.xslt" />
<xsl:output method="xml"/>
<xsl:template match="/">
<xsl:apply-imports />
</xsl:template>
</xsl:stylesheet>