/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ADIUM_STYLE_H
#define ADIUM_STYLE_H

#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtGui/QColor>

#undef PROPERTY
#define PROPERTY(type, name, capitalized_name) \
	type name() const { return capitalized_name; } \
	void set##capitalized_name(const type &name) { capitalized_name = name; }

class AdiumStyle
{
	static QSettings::Format plistFormat;

	QString Name;

	bool StyleViewVersion;
	QString DisplayNameForNoVariant;
	QColor DefaultBackgroundColor;
	bool DefaultBackgroundIsTransparent;

	QString BaseHref;
	QString TemplateHref;
	QString HeaderHtml;
	QString FooterHtml;
	QString IncomingHtml;
	QString NextIncomingHtml;
	QString OutgoingHtml;
	QString NextOutgoingHtml;
	QString StatusHtml;

	QString CurrentVariant;
	QString DefaultVariant;
	QStringList StyleVariants;

	QString readThemePart(const QString &part);

	void readConfiugrationFile();
	void loadHtmlFiles();
	void loadVariants();

public:
	AdiumStyle() { }
	AdiumStyle(const QString &styleName);

	static bool isThemeValid(const QString &stylePath);

	PROPERTY(QString, name, Name)
	PROPERTY(QString, baseHref, BaseHref)
	PROPERTY(QString, headerHtml, HeaderHtml)
	PROPERTY(QString, footerHtml, FooterHtml)
	PROPERTY(QString, incomingHtml, IncomingHtml)
	PROPERTY(QString, nextIncomingHtml, NextIncomingHtml)
	PROPERTY(QString, outgoingHtml, OutgoingHtml)
	PROPERTY(QString, nextOutgoingHtml, NextOutgoingHtml)
	PROPERTY(QString, statusHtml, StatusHtml)
	PROPERTY(QString, currentVariant, CurrentVariant)

	QString	templateHtml();

	QStringList styleVariants() { return StyleVariants; }

};

#endif // ADIUM_STYLE_H
