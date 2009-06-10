/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>

#include "icons-manager.h"

#include "formatted-message-part.h"

QString FormattedMessagePart::loadingImageHtml(const QString &imageId)
{
	return QString("<img src=\"file:///%1\" id=\"%2\" />")
			.arg(IconsManager::instance()->iconPath("LoadingImage"))
			.arg(imageId);
}

QString FormattedMessagePart::replaceLoadingImages(QString message, const QString &imageId, const QString &imagePath)
{
	QString img = QString("<img src=\"file:///%1\" />").arg(imagePath);
	return message.replace(loadingImageHtml(imageId), img);
}

FormattedMessagePart::FormattedMessagePart(const QString &content, bool bold, bool italic, bool underline, QColor color)
	: Image(false), ImageDelayed(false), Content(content), Bold(bold), Italic(italic), Underline(underline), Color(color)
{
}

FormattedMessagePart::FormattedMessagePart(const QString &image, bool imageDelayed) :
		Image(true), ImageDelayed(imageDelayed), Content("\n"),
		ImagePath(imageDelayed ? QString::null : image),
		ImageId(imageDelayed ? image : QString::null)
{
}

FormattedMessagePart::~FormattedMessagePart()
{
}

QString FormattedMessagePart::toHtml() const
{
	if (Image && ImageDelayed)
	{
		return FormattedMessagePart::loadingImageHtml(ImageId);;
	}
	else if (Image)
	{
		return QString("<img src=\"file:///%1\" />").arg(ImagePath);
	}
	else
	{
		QString result = Qt::escape(Content);
		result.replace("\r\n", "<br/>");
		result.replace("\n",   "<br/>");
		result.replace("\r",   "<br/>");
		result.replace(QChar::LineSeparator, "<br />");

		if (!Bold && !Italic && !Underline && !Color.isValid())
			return result;

		QString span = "<span style='";
		if (Bold)
			span += "font-weight:600;";
		if (Italic)
			span += "font-style:italic;";
		if (Underline)
			span += "text-decoration:underline;";
		if (Color.isValid())
			span += QString("color:%1;").arg(Color.name());
		span += "'>";

		return span + result + "</span>";
	}
}
