<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output method="text" indent="yes" />

	<xsl:template match="*">

		<xsl:if test="@name != ''">
			<xsl:if test="not(contains(@name,'%n'))">
				QT_TRANSLATE_NOOP("@default", "<xsl:value-of select="@name" />");
			</xsl:if>
			<xsl:if test="contains(@name,'%n')">
				QApplication::translate("@default", "<xsl:value-of select="@name" />", 0, QCoreApplication::CodecForTr, 1234);
			</xsl:if>
		</xsl:if>

		<xsl:if test="@caption != ''">
			<xsl:if test="not(contains(@caption,'%n'))">
				QT_TRANSLATE_NOOP("@default", "<xsl:value-of select="@caption" />");
			</xsl:if>
			<xsl:if test="contains(@caption,'%n')">
				QApplication::translate("@default", "<xsl:value-of select="@caption" />", 0, QCoreApplication::CodecForTr, 1234);
			</xsl:if>
		</xsl:if>

		<xsl:if test="@tool-tip != ''">
			<xsl:if test="not(contains(@tool-tip,'%n'))">
				QT_TRANSLATE_NOOP("@default", "<xsl:value-of select="@tool-tip" />");
			</xsl:if>
			<xsl:if test="contains(@tool-tip,'%n')">
				QApplication::translate("@default", "<xsl:value-of select="@tool-tip" />", 0, QCoreApplication::CodecForTr, 1234);
			</xsl:if>
		</xsl:if>

		<xsl:if test="@syntax-hint != ''">
			<xsl:if test="not(contains(@syntax-hint,'%n'))">
				QT_TRANSLATE_NOOP("@default", "<xsl:value-of select="@syntax-hint" />");
			</xsl:if>
			<xsl:if test="contains(@syntax-hint,'%n')">
				QApplication::translate("@default", "<xsl:value-of select="@syntax-hint" />", 0, QCoreApplication::CodecForTr, 1234);
			</xsl:if>
		</xsl:if>

		<xsl:if test="@special-value != ''">
			<xsl:if test="not(contains(@special-value,'%n'))">
				QT_TRANSLATE_NOOP("@default", "<xsl:value-of select="@special-value" />");
			</xsl:if>
			<xsl:if test="contains(@special-value,'%n')">
				QApplication::translate("@default", "<xsl:value-of select="@special-value" />", 0, QCoreApplication::CodecForTr, 1234);
			</xsl:if>
		</xsl:if>

		<xsl:if test="@suffix != ''">
			<xsl:if test="not(contains(@suffix,'%n'))">
				QT_TRANSLATE_NOOP("@default", "<xsl:value-of select="@suffix" />");
			</xsl:if>
			<xsl:if test="contains(@suffix,'%n')">
				QApplication::translate("@default", "<xsl:value-of select="@suffix" />", 0, QCoreApplication::CodecForTr, 1234);
			</xsl:if>
		</xsl:if>

		<xsl:apply-templates />

	</xsl:template>

</xsl:stylesheet>
