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

#ifndef FORMATTED_STRING_FACTORY_H
#define FORMATTED_STRING_FACTORY_H

#include <QtCore/QWeakPointer>

class QTextBlock;
class QTextCharFormat;
class QTextFragment;
class QTextImageFormat;

class FormattedString;
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
class FormattedStringFactory : public QObject
{
	Q_OBJECT

	QWeakPointer<ImageStorageService> CurrentImageStorageService;

	FormattedString * partFromQTextCharFormat(const QTextCharFormat &textCharFormat, const QString &text);
	FormattedString * partFromQTextImageFormat(const QTextImageFormat &textImageFormat);
	FormattedString * partFromQTextFragment(const QTextFragment &textFragment, bool prependNewLine);
	QList<FormattedString *> partsFromQTextBlock(const QTextBlock &textBlock, bool firstBlock);

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
	 * will contain one text part.
	 *
	 * This method never returns null.
	 */
	FormattedString * fromPlainText(const QString &plainText);

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
	FormattedString * fromHTML(const QString &html);

};

/**
 * @}
 */

#endif // FORMATTED_STRING_FACTORY_H
