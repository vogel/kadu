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

#ifndef FORMATTED_STRING_IMAGE_KEY_RECEIVED_VISITOR_H
#define FORMATTED_STRING_IMAGE_KEY_RECEIVED_VISITOR_H

#include <QtCore/QObject>

#include "protocols/services/chat-image-key.h"

#include "formatted-string/formatted-string-visitor.h"

class FormattedStringImageKeyReceivedVisitor : public QObject, public FormattedStringVisitor
{
	Q_OBJECT
	Q_DISABLE_COPY(FormattedStringImageKeyReceivedVisitor);

	QString Id;

public:
	explicit FormattedStringImageKeyReceivedVisitor(const QString &id, QObject *parent = 0);
	virtual ~FormattedStringImageKeyReceivedVisitor();

	virtual void visit(const CompositeFormattedString * const compositeFormattedString);
	virtual void visit(const FormattedStringImageBlock * const formattedStringImageBlock);
	virtual void visit(const FormattedStringTextBlock * const FormattedStringTextBlock);

signals:
	void chatImageKeyReceived(const QString &id, const ChatImageKey &imageKey);

};

#endif // FORMATTED_STRING_IMAGE_KEY_RECEIVED_VISITOR_H
