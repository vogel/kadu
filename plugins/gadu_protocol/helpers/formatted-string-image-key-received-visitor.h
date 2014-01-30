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

#ifndef FORMATTED_STRING_IMAGE_KEY_RECEIVED_VISITOR_H
#define FORMATTED_STRING_IMAGE_KEY_RECEIVED_VISITOR_H

#include <QtCore/QObject>

#include "protocols/services/chat-image.h"

#include "formatted-string/formatted-string-visitor.h"

/**
 * @addtogroup Gadu
 * @{
 */

/**
 * @class FormattedStringImageKeyReceivedVisitor
 * @short Visitor that emits chatImageKeyReceived() signal each time it visits FormattedStringImageBlock instance.
 * @author Rafał 'Vogel' Malinowski
 *
 * This visitor emits chatImageKeyReceived() signal each time it visits FormattedStringImageBlock instance.
 * Use this visitor to inform ChatImageService instances about received image keys.
 */
class FormattedStringImageKeyReceivedVisitor : public QObject, public FormattedStringVisitor
{
	Q_OBJECT
	Q_DISABLE_COPY(FormattedStringImageKeyReceivedVisitor);

	QString Id;

public:
	/**
	 * @short Create new instance of FormattedStringImageKeyReceivedVisitor.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id id that will be emitted in chatImageKeyReceived() signal
	 * @param parent QObject parent
	 */
	explicit FormattedStringImageKeyReceivedVisitor(const QString &id, QObject *parent = 0);
	virtual ~FormattedStringImageKeyReceivedVisitor();

	virtual void beginVisit(const CompositeFormattedString * const compositeFormattedString);
	virtual void endVisit(const CompositeFormattedString * const compositeFormattedString);
	virtual void visit(const FormattedStringImageBlock * const formattedStringImageBlock);
	virtual void visit(const FormattedStringTextBlock * const formattedStringTextBlock);

signals:
	/**
	 * @short Signal emitted each time this object visits FormattedStringImageBlock instance.
	 * @author Rafał 'Vogel' Malinowski
	 * @param id id passed in constructor
	 * @param chatImage chatImage from visited FormattedStringImageBlock
	 */
	void chatImageKeyReceived(const QString &id, const ChatImage &chatImage);

};

/**
 * @}
 */

#endif // FORMATTED_STRING_IMAGE_KEY_RECEIVED_VISITOR_H
