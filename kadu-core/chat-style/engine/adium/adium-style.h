/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QtCore/QSettings>
#include <QtGui/QColor>

#undef PROPERTY
#define PROPERTY(type, name, capitalized_name) \
	const type & name() const { return capitalized_name; } \
	void set##capitalized_name(const type &name) { capitalized_name = name; }

class AdiumStyle
{
	static QSettings::Format plistFormat;

	QString Name;

	int StyleViewVersion;
	QColor DefaultBackgroundColor;
	bool DefaultBackgroundIsTransparent;

	bool UsesCustomTemplateHtml;

	QString BaseHref;
	QString TemplateHref;
	QString MainHref;
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

	QString readStylePart(const QString &part);

	void readConfiugrationFile();
	void loadHtmlFiles();
	void loadVariants();

	QString performTemplateHtmlWorkarounds(QString html);

public:
	AdiumStyle();
	explicit AdiumStyle(const QString &styleName);

	static bool isStyleValid(const QString &stylePath);

	PROPERTY(QString, name, Name)
	PROPERTY(QString, baseHref, BaseHref)
	PROPERTY(QString, mainHref, MainHref)
	PROPERTY(QString, headerHtml, HeaderHtml)
	PROPERTY(QString, footerHtml, FooterHtml)
	PROPERTY(QString, incomingHtml, IncomingHtml)
	PROPERTY(QString, nextIncomingHtml, NextIncomingHtml)
	PROPERTY(QString, outgoingHtml, OutgoingHtml)
	PROPERTY(QString, nextOutgoingHtml, NextOutgoingHtml)
	PROPERTY(QString, statusHtml, StatusHtml)
	PROPERTY(QString, currentVariant, CurrentVariant)

	PROPERTY(QString, defaultVariant, DefaultVariant)
	PROPERTY(bool, defaultBackgroundIsTransparent, DefaultBackgroundIsTransparent)
	PROPERTY(bool, usesCustomTemplateHtml, UsesCustomTemplateHtml)
	PROPERTY(int, styleViewVersion, StyleViewVersion)

	QString templateHtml();

	const QStringList & styleVariants() const { return StyleVariants; }

};
