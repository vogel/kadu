/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "formatted-string/composite-formatted-string.h"
#include "formatted-string/formatted-string-image-block.h"
#include "formatted-string/formatted-string-text-block.h"
#include "services/image-storage-service.h"

#include "formatted-string-factory.h"

void FormattedStringFactory::setImageStorageService(ImageStorageService *imageStorageService)
{
	CurrentImageStorageService = imageStorageService;
}

FormattedString * FormattedStringFactory::fromPlainText(const QString& plainText)
{
	return new FormattedStringTextBlock(plainText, false, false, false, QColor());
}

FormattedString * FormattedStringFactory::partFromQTextCharFormat(const QTextCharFormat &textCharFormat, const QString &text)
{
	if (text.isEmpty())
		return 0;

	QString replacedNewLine = text;
	replacedNewLine.replace(QChar::LineSeparator, '\n');
	return new FormattedStringTextBlock(replacedNewLine, textCharFormat.font().bold(), textCharFormat.font().italic(), textCharFormat.font().underline(), textCharFormat.foreground().color());
}

FormattedString * FormattedStringFactory::partFromQTextImageFormat(const QTextImageFormat& textImageFormat)
{
	QString filePath = textImageFormat.name();
	QFileInfo fileInfo(filePath);

	if (!filePath.startsWith("kaduimg:///") && (!fileInfo.isAbsolute() || !fileInfo.exists() || !fileInfo.isFile()))
		return 0;

	if (CurrentImageStorageService)
		filePath = CurrentImageStorageService.data()->storeImage(filePath);

	return new FormattedStringImageBlock(filePath);
}

FormattedString * FormattedStringFactory::partFromQTextFragment(const QTextFragment &textFragment, bool prependNewLine)
{
	if (!textFragment.isValid())
		return 0;

	QTextCharFormat format = textFragment.charFormat();
	if (!format.isImageFormat())
		return partFromQTextCharFormat(format, prependNewLine ? '\n' + textFragment.text() : textFragment.text());
	else
		return partFromQTextImageFormat(format.toImageFormat());
}

QList<FormattedString *> FormattedStringFactory::partsFromQTextBlock(const QTextBlock &textBlock, bool firstBlock)
{
	QList<FormattedString *> result;

	bool firstFragment = true;
	for (QTextBlock::iterator it = textBlock.begin(); !it.atEnd(); ++it)
	{
		FormattedString *part = partFromQTextFragment(it.fragment(), !firstBlock && firstFragment);
		if (part && !part->isEmpty())
		{
			result.append(part);
			firstFragment = false;
		}
	}

	return result;
}

FormattedString * FormattedStringFactory::fromHtml(const QString &html)
{
	QTextDocument document;
	document.setHtml(html);

	return fromTextDocument(&document);
}

FormattedString * FormattedStringFactory::fromTextDocument(QTextDocument *textDocument)
{
	bool firstBlock = true;

	QVector<FormattedString *> items;

	QTextBlock block = textDocument->firstBlock();
	while (block.isValid())
	{
		QList<FormattedString *> parts = partsFromQTextBlock(block, firstBlock);
		foreach (FormattedString *part, parts)
			items.append(part);

		block = block.next();
		firstBlock = false;
	}

	return new CompositeFormattedString(items);
}

FormattedString * FormattedStringFactory::fromText(const QString &text)
{
	QScopedPointer<QTextDocument> document(new QTextDocument());
	if (isHtml(text))
		document->setHtml(text);
	else
		document->setPlainText(text);

	return fromTextDocument(document.data());
}

bool FormattedStringFactory::isHtml(const QString &text) const
{
	return text.contains('<');
}

#include "moc_formatted-string-factory.cpp"
