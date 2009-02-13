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

#include "../modules/gadu_protocol/gadu_images_manager.h"

#include "icons_manager.h"

#include "message-part.h"

MessagePart::MessagePart(const QString &content, bool bold, bool italic, bool underline, QColor color)
	: Image(false), Content(content), Bold(bold), Italic(italic), Underline(underline), Color(color)
{
}

MessagePart::MessagePart(const QString &imagePath)
	: Image(true), ImagePath(imagePath), ImageSender(0), ImageSize(0), ImageCrc32(0)
{
}

MessagePart::MessagePart(UinType imageSender, int imageSize, int imageCrc32)
	: Image(true), ImagePath(icons_manager->iconPath("LoadingImage")),
		ImageSender(imageSender), ImageSize(imageSize), ImageCrc32(imageCrc32)
{
}

MessagePart::~MessagePart()
{
}

QString MessagePart::toHtml() const
{
	if (Image && ImageSender)
	{
		return GaduImagesManager::loadingImageHtml(ImageSender, ImageSize,ImageCrc32);;
	}
	else if (Image)
	{
		return QString("<img src=\"file:///%1\" />").arg(ImagePath);
	}
	else
	{
		QString result = Qt::escape(Content);
		result.replace("\n", "<br />");

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
