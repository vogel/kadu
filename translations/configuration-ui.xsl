<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output method="text" indent="yes" />

	<xsl:template match="*">
		<xsl:if test="@name != ''">
			QT_TRANSLATE_NOOP("@default", "<xsl:value-of select="@name" />");
		</xsl:if>
		<xsl:if test="@caption != ''">
			QT_TRANSLATE_NOOP("@default", "<xsl:value-of select="@caption" />");
		</xsl:if>
		<xsl:if test="@tool-tip != ''">
			QT_TRANSLATE_NOOP("@default", "<xsl:value-of select="@tool-tip" />");
		</xsl:if>
		<xsl:apply-templates />
	</xsl:template>

</xsl:stylesheet>
