/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtGui/QTextBlock>
#include <QtGui/QTextDocument>

#include "protocols/services/chat-image-service.h"
#include "html_document.h"
#include "icons-manager.h"

#include "formatted-message.h"

QString FormattedMessage::saveInImagesPath(const QString &filePath)
{
	QFileInfo fileInfo(filePath);
	if (!fileInfo.exists())
		return filePath;

	QFileInfo imagesPathInfo(ChatImageService::imagesPath());
	if (!imagesPathInfo.isDir() && !QDir().mkdir(imagesPathInfo.absolutePath()))
		return filePath;

	// if already in imagesPath, it'd be a waste to copy it in the same dir
	if (fileInfo.absolutePath() == imagesPathInfo.absolutePath())
		return fileInfo.fileName();

	QString copyFileName = QUuid::createUuid().toString();
	// Make this file name less exotic. First, get rid of '{' and '}' on edges.
	if (copyFileName.length() > 2)
		copyFileName = copyFileName.mid(1, copyFileName.length() - 2);
	// Second, try to add extension.
	QString ext = fileInfo.completeSuffix();
	if (!ext.isEmpty())
		copyFileName += '.' + ext;

	if (QFile::copy(filePath, imagesPathInfo.absolutePath() + '/' + copyFileName))
		return copyFileName;

	return filePath;
}

void FormattedMessage::parseImages(FormattedMessage &message, const QString &messageString, bool b, bool i, bool u, QColor color)
{
	static QRegExp imageRegExp("\\[IMAGE ([^\\]]+)\\]");

	int lastPos = -1;
	int pos = 0;

	while ((pos = imageRegExp.indexIn(messageString, pos)) != -1)
	{
		if (lastPos != pos)
		{
			if (lastPos == -1)
				message << FormattedMessagePart(messageString.left(pos), b, i, u, color);
			else
				message << FormattedMessagePart(messageString.mid(lastPos, pos - lastPos), b, i, u, color);
		}

		QString filePath = imageRegExp.cap(1);
		QFileInfo fileInfo(filePath);
		if (fileInfo.isAbsolute() && fileInfo.exists() && fileInfo.isFile())
			message << FormattedMessagePart(saveInImagesPath(filePath));
		else
			message << FormattedMessagePart(messageString.mid(pos, imageRegExp.matchedLength()), b, i, u, color);

		pos += imageRegExp.matchedLength();
		lastPos = pos;
	}

	if (lastPos == -1)
		message << FormattedMessagePart(messageString, b, i, u, color);
	else if (lastPos != messageString.length())
		message << FormattedMessagePart(messageString.mid(lastPos, messageString.length() - lastPos), b, i, u, color);
}

FormattedMessage FormattedMessage::parse(const QTextDocument *document)
{
	FormattedMessage result;

	QString text;

	QTextBlock block = document->firstBlock();
	bool firstParagraph = true;
	while (block.isValid())
	{
		bool firstFragment = true;
		for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it)
		{
			QTextFragment fragment = it.fragment();
			if (!fragment.isValid())
				continue;

			if (!firstParagraph && firstFragment)
				text = '\n' + fragment.text();
			else
				text = fragment.text();

			QTextCharFormat format = fragment.charFormat();
			parseImages(result, text,
				format.font().bold(),
				format.font().italic(),
				format.font().underline(),
				format.foreground().color());

			firstFragment = false;
		}

		if (firstFragment)
			parseImages(result, "\n", false, false, false, QColor());

		block = block.next();
		firstParagraph = false;
	}

	return result;
}

FormattedMessage::FormattedMessage()
{
}

FormattedMessage::FormattedMessage(const QString &messageString)
{
	Parts.append(FormattedMessagePart(messageString, false, false, false, QColor()));
}

FormattedMessage::~FormattedMessage()
{
}

const QList<FormattedMessagePart> & FormattedMessage::parts() const
{
	return Parts;
}

void FormattedMessage::prepend(FormattedMessagePart part)
{
	Parts.prepend(part);
}

void FormattedMessage::append(FormattedMessagePart part)
{
	Parts.append(part);
}

FormattedMessage & FormattedMessage::operator << (FormattedMessagePart part)
{
	Parts << part;
	return *this;
}

bool FormattedMessage::isEmpty() const
{
	foreach (const FormattedMessagePart &part, Parts)
		if (!part.isEmpty())
			return false;

	return true;
}

QString FormattedMessage::toPlain() const
{
	QString result;
	foreach (const FormattedMessagePart &part, Parts)
		result += part.content();

	result.replace(QChar::LineSeparator, "\n");
	return result;
}

QString FormattedMessage::toHtml() const
{
	QString result;
	foreach (const FormattedMessagePart &part, Parts)
		result += part.toHtml();

	return result;
}
