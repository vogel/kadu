/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>

#include "misc/path-conversion.h"

#include "adium-style.h"

AdiumStyle::AdiumStyle(const QString &styleName) :
		Name(styleName)
{
	QDir dir;
	BaseHref = ggPath() + "/syntax/chat/" + styleName + "/Contents/Resources/";
	if (!dir.exists(BaseHref))
		BaseHref = dataPath("kadu") + "/syntax/chat/" + styleName + "/Contents/Resources/";

	loadHtmlFiles();
	loadVariants();
}

void AdiumStyle::loadHtmlFiles()
{
	IncomingHtml = readThemePart(BaseHref + "Incoming/Content.html");

	if (QFile::exists(BaseHref + "Template.html"))
		TemplateHref = BaseHref + "Template.html";
	else if (QFile::exists(BaseHref + "template.html"))
		TemplateHref = BaseHref + "template.html";
	else // TODO 0.6.6: move it to proper place
		TemplateHref = dataPath("kadu") + "/syntax/chat/Default/Template.html";

	if (QFile::exists(BaseHref + "Incoming/NextContent.html"))
		NextIncomingHtml = readThemePart(BaseHref + "Incoming/NextContent.html");
	else
		NextIncomingHtml = IncomingHtml;

	if (QFile::exists(BaseHref + "Outgoing/Content.html"))
		OutgoingHtml = readThemePart(BaseHref + "Outgoing/Content.html");
	else
		OutgoingHtml = IncomingHtml;

	if (QFile::exists(BaseHref + "Outgoing/NextContent.html"))
		NextOutgoingHtml = readThemePart(BaseHref + "Outgoing/NextContent.html");
	else
		NextOutgoingHtml = OutgoingHtml;

	HeaderHtml = readThemePart(BaseHref + "Header.html");
	FooterHtml = readThemePart(BaseHref + "Footer.html");

	StatusHtml = readThemePart(BaseHref + "Status.html");
}

void AdiumStyle::loadVariants()
{
	QDir dir(BaseHref);
	dir.setNameFilters(QStringList("*.css"));
	StyleVariants = dir.entryList();
}

bool AdiumStyle::isThemeValid(const QString &stylePath)
{
	// Minimal Adium style layout
	QDir dir(stylePath);
	if (!dir.cd("Contents/Resources/"))
		return false;

	QFileInfo fi(dir, "Incoming/Content.html");
	if (!fi.isReadable())
		return false;

	fi.setFile(dir, "main.css");
	if (!fi.isReadable())
		return false;

	fi.setFile(dir, "Status.html");
	if (!fi.isReadable())
		return false;

	return true;
}

QString AdiumStyle::readThemePart(const QString &part)
{
	QFile fileAccess;
	QString resultHtml = QString::null;
	if (QFile::exists(part))
	{

		fileAccess.setFileName(part);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream stream(&fileAccess);
		stream.setCodec(QTextCodec::codecForName("UTF-8"));
		resultHtml = stream.readAll();

		fileAccess.close();
	}
	return resultHtml;
}

QString AdiumStyle::templateHtml()
{
	return readThemePart(TemplateHref);
}

QStringList AdiumStyle::styleVariants()
{
}

