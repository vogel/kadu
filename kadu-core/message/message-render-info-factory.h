/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>

class ChatStyleManager;
class Message;
class MessageRenderInfo;

enum class MessageRenderHeaderBehavior;

class KADUAPI MessageRenderInfoFactory : public QObject
{
	Q_OBJECT

public:
	explicit MessageRenderInfoFactory(QObject *parent = nullptr);
	virtual ~MessageRenderInfoFactory();

	void setChatStyleManager(ChatStyleManager *chatStylesManager);

	MessageRenderInfo messageRenderInfo(const Message &previous, const Message &message, MessageRenderHeaderBehavior renderHeaderBehavior);

private:
	QPointer<ChatStyleManager> m_chatStylesManager;

	QString backgroundColor(const Message &message) const;
	QString nickColor(const Message &message) const;
	QString fontColor(const Message &message) const;
	bool includeHeader(const Message &previous, const Message &message, MessageRenderHeaderBehavior renderHeaderBehavior) const;
	int separatorSize(bool includeHeader) const;
	bool showServerTime(const Message &message) const;

};
