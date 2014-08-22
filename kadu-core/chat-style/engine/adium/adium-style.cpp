/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "adium-style.h"

#include "misc/kadu-paths.h"

#include <QtCore/QDir>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtXml/QDomNode>

//Functions to parse plist format are borrowed from pluginsystem.cpp from qutIM instant messenger (see: http://www.qutim.org/)
QSettings::SettingsMap parseDict(const QDomNode &rootElement)
{
	QSettings::SettingsMap styleHash;

	if (rootElement.isNull())
		return styleHash;

	QDomNode subelement = rootElement;

	QString key;

	for (QDomNode node = subelement.firstChild(); !node.isNull(); node = node.nextSibling())
	{
		QDomElement element = node.toElement();
		if (element.nodeName() == "key")
			key = element.text();
		else
		{
			QVariant value;
			if (element.nodeName() == "true")
				value = QVariant(true);
			else if (element.nodeName() == "false")
				value = QVariant(false);
			else if (element.nodeName() == "real")
				value = QVariant(element.text().toDouble());
			else if (element.nodeName() == "string")
				value=QVariant(element.text());
			else if (element.nodeName() == "integer")
				value = QVariant(element.text().toInt());
			else if (element.nodeName() == "dict")
				value = parseDict(node);
			styleHash.insert(key, value);
		}
	}
	return styleHash;
}

bool plistWriteFunction(QIODevice &device, const QSettings::SettingsMap &map)
{
	Q_UNUSED(device)
	Q_UNUSED(map)

	return true;
}

bool plistReadFunction(QIODevice &device, QSettings::SettingsMap &map)
{
	QDomDocument documentElement;

	if (documentElement.setContent(&device))
	{
		QDomElement rootElement = documentElement.documentElement();
		if (rootElement.isNull())
			return false;

		map = parseDict(rootElement.firstChild());
	}
	return true;
}

QSettings::Format AdiumStyle::plistFormat = QSettings::registerFormat("plist", plistReadFunction, plistWriteFunction);

AdiumStyle::AdiumStyle() :
		StyleViewVersion{0}, DefaultBackgroundIsTransparent{false}, UsesCustomTemplateHtml{false}
{
}

AdiumStyle::AdiumStyle(const QString &styleName) :
		Name{styleName}, StyleViewVersion{0}, DefaultBackgroundIsTransparent{false}, UsesCustomTemplateHtml{false}
{
	QDir dir;
	BaseHref = KaduPaths::instance()->profilePath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/");
	if (!dir.exists(BaseHref))
		BaseHref = KaduPaths::instance()->dataPath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/");

	readConfiugrationFile();
	loadHtmlFiles();
	loadVariants();
}

void AdiumStyle::readConfiugrationFile()
{
	QSettings styleSettings(BaseHref + "../Info.plist", plistFormat);

	bool ok = false;
	QRgb color = styleSettings.value("DefaultBackgroundColor", "ffffff").toString().toUInt(&ok, 16);
	DefaultBackgroundColor = QColor(ok ? color : 0xffffff);

	DefaultBackgroundIsTransparent = styleSettings.value("DefaultBackgroundIsTransparent", false).toBool();

	StyleViewVersion = styleSettings.value("MessageViewVersion", 1).toInt();

	DefaultVariant = styleSettings.value("DefaultVariant", QString()).toString() + ".css";
	if (DefaultVariant == ".css")
		DefaultVariant = styleSettings.value("DisplayNameForNoVariant", "Default").toString() + ".css";
}

void AdiumStyle::loadHtmlFiles()
{
	IncomingHtml = readStylePart(BaseHref + "Incoming/Content.html");

	// Why the hell themes creators ignore fact that paths are case sensitive? :/
	UsesCustomTemplateHtml = true;
	if (QFile::exists(BaseHref + "Template.html"))
		TemplateHref = BaseHref + "Template.html";
	else if (QFile::exists(BaseHref + "template.html"))
		TemplateHref = BaseHref + "template.html";
	else
	{
		TemplateHref = KaduPaths::instance()->dataPath() + QLatin1String("syntax/chat/Default/Template.html");
		UsesCustomTemplateHtml = false;
	}

	if (QFile::exists(BaseHref + "main.css"))
		MainHref = BaseHref + "main.css";
	else if (QFile::exists(BaseHref + "Main.css"))
		MainHref = BaseHref + "Main.css";

	if (QFile::exists(BaseHref + "Incoming/NextContent.html"))
		NextIncomingHtml = readStylePart(BaseHref + "Incoming/NextContent.html");
	else
		NextIncomingHtml = IncomingHtml;

	if (QFile::exists(BaseHref + "Outgoing/Content.html"))
		OutgoingHtml = readStylePart(BaseHref + "Outgoing/Content.html");
	else
		OutgoingHtml = IncomingHtml;

	if (QFile::exists(BaseHref + "Outgoing/NextContent.html"))
		NextOutgoingHtml = readStylePart(BaseHref + "Outgoing/NextContent.html");
	else
		NextOutgoingHtml = OutgoingHtml;

	HeaderHtml = readStylePart(BaseHref + "Header.html");
	FooterHtml = readStylePart(BaseHref + "Footer.html");

	StatusHtml = readStylePart(BaseHref + "Status.html");
}

void AdiumStyle::loadVariants()
{
	QDir dir(BaseHref + "Variants/");
	dir.setNameFilters(QStringList("*.css"));
	StyleVariants = dir.entryList();
}

bool AdiumStyle::isStyleValid(const QString &stylePath)
{
	// Minimal Adium style layout
	QDir dir(stylePath);

	QFileInfo fi(dir, "Contents/Info.plist");
	if (!fi.isReadable())
		return false;

	if (!dir.cd("Contents/Resources/"))
		return false;

	fi.setFile(dir, "Incoming/Content.html");
	if (!fi.isReadable())
		return false;

	fi.setFile(dir, "Status.html");
	if (!fi.isReadable())
		return false;

	return true;
}

QString AdiumStyle::readStylePart(const QString &part)
{
	QFile fileAccess;
	QString resultHtml;
	if (QFile::exists(part))
	{
		fileAccess.setFileName(part);
		if (fileAccess.open(QIODevice::ReadOnly))
		{
			QTextStream stream(&fileAccess);
			stream.setCodec(QTextCodec::codecForName("UTF-8"));
			resultHtml = stream.readAll();
			fileAccess.close();
		}
	}
	return resultHtml;
}

QString AdiumStyle::templateHtml()
{
	return performTemplateHtmlWorkarounds(readStylePart(TemplateHref));
}

QString AdiumStyle::performTemplateHtmlWorkarounds(QString html)
{
	if (Name.contains("renkoo") || Name.contains("Renkoo"))
	{
		//renkoo styles always scroll to bottom on new messages
		int index = html.indexOf("alignChat(true);") + 1;
		html.replace(html.indexOf("alignChat(true);", index), 16, "alignChat(shouldScroll);");
		html.replace(html.indexOf("alignChat(true);", index), 16, "alignChat(shouldScroll);");
	}

	return html;
}
