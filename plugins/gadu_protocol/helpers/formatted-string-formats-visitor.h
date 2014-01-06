/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FORMATTED_STRING_FORMATS_VISITOR_H
#define FORMATTED_STRING_FORMATS_VISITOR_H

#include <QtCore/QPointer>

#include "formatted-string/formatted-string-visitor.h"

class ChatImageService;
class ImageStorageService;

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class FormattedStringFormatsVisitor
 * @short Visitor that creates formats libgadu structure to describe visited FormattedString.
 * @author Rafał 'Vogel' Malinowski
 *
 * This visitor creates formats libgadu structure to describe visited FormattedString.
 */
class FormattedStringFormatsVisitor : public FormattedStringVisitor
{
	Q_DISABLE_COPY(FormattedStringFormatsVisitor);

	QPointer<ChatImageService> CurrentChatImageService;
	QPointer<ImageStorageService> CurrentImageStorageService;

	bool First;
	unsigned short TextPosition;
	QByteArray Result;

public:
	/**
	 * @short Create new instance of FormattedStringFormatsVisitor.
	 * @author Rafał 'Vogel' Malinowski
	 */
	FormattedStringFormatsVisitor();
	virtual ~FormattedStringFormatsVisitor();

	/**
	 * @short Set ChatImageService to use by this visitor.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chatImageService ChatImageService to use by this visitor
	 *
	 * If no ChatImageService is provided then empty images will be inserted into resulting libgadu formats structure.
	 */
	void setChatImageService(ChatImageService *chatImageService);

	/**
	 * @short Set ImageStorageService to use by this visitor.
	 * @author Rafał 'Vogel' Malinowski
	 * @param imageStorageService ImageStorageService to use by this visitor
	 *
	 * If no ImageStorageService is provided then all images will use full path instead of relative one.
	 */
	void setImageStorageService(ImageStorageService *imageStorageService);

	virtual void beginVisit(const CompositeFormattedString * const compositeFormattedString);
	virtual void endVisit(const CompositeFormattedString * const compositeFormattedString);
	virtual void visit(const FormattedStringImageBlock * const formattedStringImageBlock);
	virtual void visit(const FormattedStringTextBlock * const formattedStringTextBlock);

	/**
	 * @short Return libgadu formats structure that matches visited FormattedString.
	 * @author Rafał 'Vogel' Malinowski
	 * @return libgadu formats structure
	 */
	QByteArray result();

};

/**
 * @}
 */

#endif // FORMATTED_STRING_FORMATS_VISITOR_H
