/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef FORMATTED_STRING_FACTORY_H
#define FORMATTED_STRING_FACTORY_H

#include <memory>
#include <QtCore/QPointer>

#include "formatted-string/formatted-string.h"
#include "exports.h"

class QTextBlock;
class QTextCharFormat;
class QTextDocument;
class QTextFragment;
class QTextImageFormat;

class ImageStorageService;

/**
 * @addtogroup FormattedString
 * @{
 */

/**
 * @class FormattedStringFactory
 * @short Class for creating FormattedString instances from different sources.
 * @author Rafał 'Vogel' Malinowski
 */
class KADUAPI FormattedStringFactory : public QObject
{
	Q_OBJECT

	QPointer<ImageStorageService> CurrentImageStorageService;

	std::unique_ptr<FormattedString> partFromQTextCharFormat(const QTextCharFormat &textCharFormat, const QString &text);
	std::unique_ptr<FormattedString> partFromQTextImageFormat(const QTextImageFormat &textImageFormat);
	std::unique_ptr<FormattedString> partFromQTextFragment(const QTextFragment &textFragment, bool prependNewLine);
	std::vector<std::unique_ptr<FormattedString>> partsFromQTextBlock(const QTextBlock &textBlock, bool firstBlock);

public:
	/**
	 * @short Set ImageStorageService to use by this factory.
	 * @author Rafał 'Vogel' Malinowski
	 * @param imageStorageService ImageStorageService to use by this factory
	 *
	 * ImageStorageService will be used to store images and change path to them to make FormattedString working even when Kadu instance
	 * is moved to new location.
	 */
	void setImageStorageService(ImageStorageService *imageStorageService);

	/**
	 * @short Create FormattedString instance from plain text.
	 * @author Rafał 'Vogel' Malinowski
	 * @param plainText plain content of new FormattedString
	 *
	 * This method will create empty FormattedString if plainText is empty. If plainText is not empty then returned FormattedString
	 * will contain just this text.
	 *
	 * This method never returns null.
	 */
	std::unique_ptr<FormattedString> fromPlainText(const QString &plainText);

	/**
	 * @short Create FormattedString instance from HTML.
	 * @author Rafał 'Vogel' Malinowski
	 * @param html HTML content of new FormattedString
	 *
	 * Thie method will extract basic information about formatting from HTML content and create instance of FormattedString with
	 * this data.
	 *
	 * This method never returns null.
	 */
	std::unique_ptr<FormattedString> fromHtml(const QString &html);

	/**
	 * @short Create FormattedString instance from text document.
	 * @author Rafał 'Vogel' Malinowski
	 * @param textDocument text document to be converted to FormattedString
	 *
	 * This method will create empty FormattedString if text document is empty.
	 *
	 * This method never returns null.
	 */
	std::unique_ptr<FormattedString> fromTextDocument(QTextDocument *textDocument);

	/**
	 * @short Create FormattedString instance from HTML or plain text.
	 * @author Rafał 'Vogel' Malinowski
	 * @param text HTML or plain text to be converted to FormattedString
	 *
	 * This method will create empty FormattedString if text is empty. If text contains '<' character then it is assumed that
	 * it is HTML, else it is assumed to be plain text.
	 *
	 * This method never returns null.
	 */
	std::unique_ptr<FormattedString> fromText(const QString &text);

	/**
	 * @short Check if given text is in HTML format.
	 * @author Rafał 'Vogel' Malinowski
	 * @param text text to check
	 */
	bool isHtml(const QString &text) const;

};

/**
 * @}
 */

#endif // FORMATTED_STRING_FACTORY_H
