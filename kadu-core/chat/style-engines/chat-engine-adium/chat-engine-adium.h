/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef CHAT_ENGINE_ADIUM_H
#define CHAT_ENGINE_ADIUM_H

#include "../chat-style-engine.h"
#include "adium-style.h"

class Chat;
class Preview;

class AdiumChatStyleEngine : public QObject, public ChatStyleEngine
{
	Q_OBJECT

	AdiumStyle CurrentStyle;

	QString jsCode;

	QString replaceKeywords(Chat chat, const QString &styleHref, const QString &style);
	QString replaceKeywords(Chat chat, const QString &styleHref, const QString &source, MessageRenderInfo *message);

	bool clearDirectory(const QString &directory);

	void appendChatMessage(HtmlMessagesRenderer *renderer, MessageRenderInfo *message);

public:
	explicit AdiumChatStyleEngine(QObject *parent = 0);
	virtual ~AdiumChatStyleEngine();

	virtual bool supportVariants() { return true; }
	virtual bool supportEditing() { return false; }
	virtual QString isStyleValid(QString styleName);
	virtual QString currentStyleVariant();
	virtual QString defaultVariant(const QString &styleName);

	virtual QStringList styleVariants(QString styleName);
	virtual bool styleUsesTransparencyByDefault(QString styleName);

	virtual void clearMessages(HtmlMessagesRenderer *renderer);
	virtual void appendMessages(HtmlMessagesRenderer *renderer, const QList<MessageRenderInfo *> &messages);
	virtual void appendMessage(HtmlMessagesRenderer *renderer, MessageRenderInfo *message);
	virtual void pruneMessage(HtmlMessagesRenderer *renderer);
	virtual void refreshView(HtmlMessagesRenderer *renderer, bool useTransparency = false);
	virtual void messageStatusChanged(HtmlMessagesRenderer *renderer, Message message, Message::Status status)
	{
		Q_UNUSED(renderer)
		Q_UNUSED(message)
		Q_UNUSED(status)
	}

	virtual void prepareStylePreview(Preview *preview, QString styleName, QString variantName);

	virtual void configurationUpdated() {}

	virtual void loadStyle(const QString &styleName, const QString &variantName);

	virtual void styleEditionRequested(QString ) {} //do nothing. Adium styles don't support editing

	virtual bool removeStyle(const QString &styleName);
};

#endif // CHAT_ENGINE_ADIUM_H
