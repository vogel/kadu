/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#ifndef CHAT_STYLE_ENGINE_H
#define CHAT_STYLE_ENGINE_H

#include <QtCore/QString>
#include <QtCore/QStringList>

#include "message/message.h"
#include "protocols/services/chat-state-service.h"

class HtmlMessagesRenderer;
class MessageRenderInfo;
class Preview;

class ChatStyleEngine
{
protected:
	QString EngineName;
	QString CurrentStyleName;

public:
	virtual ~ChatStyleEngine() {}
	virtual const QString & engineName() const { return EngineName; }
	virtual const QString & currentStyleName() const { return CurrentStyleName; }
	virtual QString currentStyleVariant() { return "Default"; }
	virtual QString defaultVariant(const QString &) { return "Default"; }
	virtual QStringList styleVariants(QString) { return QStringList(); }

	virtual bool supportVariants() = 0;
	virtual bool supportEditing() = 0;
	virtual QString isStyleValid(QString) = 0; //return QString(), if style is not valid
	virtual bool styleUsesTransparencyByDefault(QString) = 0;

	virtual void clearMessages(HtmlMessagesRenderer *) = 0;
	virtual void appendMessages(HtmlMessagesRenderer *, const QList<MessageRenderInfo *> &) = 0;
	virtual void appendMessage(HtmlMessagesRenderer *, MessageRenderInfo *) = 0;
	virtual void pruneMessage(HtmlMessagesRenderer *) = 0;
	virtual void refreshView(HtmlMessagesRenderer *, bool useTransparency = false) = 0;
	virtual void messageStatusChanged(HtmlMessagesRenderer *, Message, MessageStatus) = 0;
	virtual void contactActivityChanged(HtmlMessagesRenderer *, ChatStateService::State, const QString &, const QString &) = 0;

	virtual void prepareStylePreview(Preview *, QString, QString) = 0;

	virtual void configurationUpdated() = 0;

	virtual void loadStyle(const QString &, const QString &) = 0;

	virtual void styleEditionRequested(QString) = 0;

	virtual bool removeStyle(const QString &) = 0;
};

#endif // CHAT_STYLE_ENGINE_H
