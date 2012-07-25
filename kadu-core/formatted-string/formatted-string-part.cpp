/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFileInfo>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>

#include "formatted-string/formatted-string-visitor.h"
#include "misc/misc.h"
#include "protocols/services/chat-image-service.h"

#include "formatted-string-part.h"

FormattedStringPart::FormattedStringPart(const QString &content, bool bold, bool italic, bool underline, QColor color) :
		Content(content), Bold(bold), Italic(italic), Underline(underline), Color(color), IsImage(false), ImageKey(0, 0)
{
}

FormattedStringPart::FormattedStringPart(const QString &imagePath) :
		IsImage(false), ImagePath(imagePath), ImageKey(0, 0)
{
	if (!ImagePath.isEmpty())
	{
		Content = QChar(QChar::Nbsp);
		IsImage = true;
	}
}

FormattedStringPart::FormattedStringPart(const ChatImageKey &chatImageKey) :
		IsImage(false), ImageKey(chatImageKey)
{
	if (!ImageKey.isNull())
	{
		ImagePath = ImageKey.toString();
		Content = QChar(QChar::Nbsp);
		IsImage = true;
	}
}

FormattedStringPart::FormattedStringPart(const FormattedStringPart &copyMe) :
		FormattedString(), ImageKey(0, 0)
{
	Content = copyMe.Content;
	Bold = copyMe.Bold;
	Italic = copyMe.Italic;
	Underline = copyMe.Underline;
	Color = copyMe.Color;
	IsImage = copyMe.IsImage;
	ImagePath = copyMe.ImagePath;
	ImageKey = copyMe.ImageKey;
}

FormattedStringPart::~FormattedStringPart()
{
}

FormattedStringPart & FormattedStringPart::operator = (const FormattedStringPart &copyMe)
{
	Content = copyMe.Content;
	Bold = copyMe.Bold;
	Italic = copyMe.Italic;
	Underline = copyMe.Underline;
	Color = copyMe.Color;
	IsImage = copyMe.IsImage;
	ImagePath = copyMe.ImagePath;
	ImageKey = copyMe.ImageKey;

	return *this;
}

void FormattedStringPart::accept(FormattedStringVisitor *visitor) const
{
	visitor->visit(this);
}

QString FormattedStringPart::imagePath() const
{
	return ImagePath;
}

ChatImageKey FormattedStringPart::imageKey() const
{
	return ImageKey;
}

QString FormattedStringPart::toHtml() const
{
	if (IsImage)
		return QFileInfo(ImagePath).isAbsolute()
				? QString("<img src=\"file://%1\" id=\"%2\" />").arg(ImagePath).arg(ImageKey.toString())
				: QString("<img src=\"kaduimg:///%1\" id=\"%2\" />").arg(ImagePath).arg(ImageKey.toString());

	QString result(replacedNewLine(Qt::escape(Content), QLatin1String("<br/>")));
	result.replace(QChar::LineSeparator, QLatin1String("<br/>"));

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
//	This breaks all dark chat themes. We have to find better solution for post 0.9.0 versions
//	if (Color.isValid())
//		span += QString("color:%1;").arg(Color.name());
	span += "\">";

	return span + result + "</span>";
}

QString FormattedStringPart::toPlain() const
{
	return Content;
}
