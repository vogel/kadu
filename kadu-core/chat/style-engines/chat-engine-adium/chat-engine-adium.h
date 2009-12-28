/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_ENGINE_ADIUM_H
#define CHAT_ENGINE_ADIUM_H

#include "../chat-style-engine.h"
#include "adium-style.h"

class AdiumTimeFormatter;
class Chat;
class Preview;

class AdiumChatStyleEngine : public ChatStyleEngine
{
	AdiumTimeFormatter *timeFormatter;

	AdiumStyle CurrentStyle;

	QString jsCode;

	QString replaceKeywords(Chat chat, const QString &styleHref, const QString &style);
	QString replaceKeywords(Chat chat, const QString &styleHref, const QString &source, MessageRenderInfo *message);

	bool clearDirectory(const QString &directory);

public:
	AdiumChatStyleEngine();
	~AdiumChatStyleEngine();

	virtual bool supportVariants() { return true; }
	virtual bool supportEditing() { return false; }
	virtual QString isThemeValid(QString styleName);
	virtual QString currentStyleVariant();

	virtual QStringList styleVariants(QString styleName);
	virtual bool styleUsesTransparencyByDefault(QString styleName);

	virtual void clearMessages(HtmlMessagesRenderer *renderer);
	virtual void appendMessages(HtmlMessagesRenderer *renderer, QList<MessageRenderInfo *> messages);
	virtual void appendMessage(HtmlMessagesRenderer *renderer, MessageRenderInfo *message);
	virtual void pruneMessage(HtmlMessagesRenderer *renderer);
	virtual void refreshView(HtmlMessagesRenderer *renderer);
	virtual void messageStatusChanged(HtmlMessagesRenderer *renderer, Message message, Message::Status status) {}

	virtual void prepareStylePreview(Preview *preview, QString styleName, QString variantName);

	virtual void configurationUpdated() {}

	virtual void loadTheme(const QString &styleName, const QString &variantName);

	virtual void styleEditionRequested(QString ) {} //do nothing. Adium styles don't support editing

	virtual bool removeStyle(const QString &styleName);
};

#endif // CHAT_ENGINE_ADIUM_H
