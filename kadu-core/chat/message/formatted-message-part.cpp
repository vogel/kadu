/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>

#include "icons-manager.h"

#include "formatted-message-part.h"

QString FormattedMessagePart::loadingImageHtml(const QString &imageId)
{
	return QString("<img src=\"file:///%1\" id=\"%2\" />")
			.arg(IconsManager::instance()->iconPath("kadu_icons/16x16/please-wait.gif"))
			.arg(imageId);
}

QString FormattedMessagePart::replaceLoadingImages(QString message, const QString &imageId, const QString &imageFileName)
{
	QString img = QString("<img src=\"kaduimg:///%1\" />").arg(imageFileName);
	return message.replace(loadingImageHtml(imageId), img);
}

FormattedMessagePart::FormattedMessagePart(const QString &content, bool bold, bool italic, bool underline, QColor color) :
		Content(content), Bold(bold), Italic(italic), Underline(underline), Color(color), Image(false), ImageDelayed(false)
{
}

FormattedMessagePart::FormattedMessagePart(const QString &imageFileName, bool delayed, const QString &imageId) :
		Content("\n"), Image(true), ImageDelayed(delayed),
		ImageFileName(imageFileName), ImageId(imageId)
{
}

FormattedMessagePart::~FormattedMessagePart()
{
}

QString FormattedMessagePart::toHtml() const
{
	if (Image)
		return ImageDelayed
				? loadingImageHtml(ImageId)
				: QString("<img src=\"kaduimg:///%1\" />").arg(ImageFileName);

	QString result = Qt::escape(Content);
	result.replace("\r\n", "<br/>");
	result.replace("\n",   "<br/>");
	result.replace("\r",   "<br/>");
	result.replace(QChar::LineSeparator, "<br/>");

	if (!Bold && !Italic && !Underline && !Color.isValid())
		return result;

	QString span = "<span style=\"";
	if (Bold)
		span += "font-weight:600;";
	if (Italic)
		span += "font-style:italic;";
	if (Underline)
		span += "text-decoration:underline;";
//  TODO: Ignore colors settings for now. Many clients send black as default color.
//	This breaks all dark chat themes. We have to find better solution for post 0.6.6 versions
//	if (Color.isValid())
//		span += QString("color:%1;").arg(Color.name());
	span += "\">";

	return span + result + "</span>";
}
