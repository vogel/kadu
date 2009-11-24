/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_ENGINE_KADU_H
#define CHAT_ENGINE_KADU_H

#include <QtCore/QObject>

#include "../chat-style-engine.h"

class Preview;
class SyntaxList;

class KaduChatStyleEngine : public QObject, public ChatStyleEngine
{
	Q_OBJECT

	SyntaxList *syntaxList;

	QString ChatSyntaxWithHeader; /*!< Chat syntax with header */
	QString ChatSyntaxWithoutHeader; /*!< Chat syntax without header */

	QString formatMessage(MessageRenderInfo *message, MessageRenderInfo *after);
	void repaintMessages(HtmlMessagesRenderer *page);

private slots:
	void chatSyntaxFixup(QString &syntax);
	void chatFixup(QString &syntax);
	void validateStyleName(const QString &name, bool &valid);
	void syntaxAdded(const QString &syntaxName);

public:
	KaduChatStyleEngine();
	~KaduChatStyleEngine();
	virtual bool supportVariants() { return false; }
	virtual bool supportEditing() { return true; }
	virtual QString isThemeValid(QString styleName);
	
	virtual void clearMessages(HtmlMessagesRenderer *renderer);
	virtual void appendMessages(HtmlMessagesRenderer *renderer, QList<MessageRenderInfo *> messages);
	virtual void appendMessage(HtmlMessagesRenderer *renderer, MessageRenderInfo *message);
	virtual void pruneMessage(HtmlMessagesRenderer *renderer) {}
	virtual void refreshView(HtmlMessagesRenderer *renderer);
	virtual void messageStatusChanged(HtmlMessagesRenderer *renderer, Message message, Message::Status status);

	virtual void prepareStylePreview(Preview *preview, QString styleName, QString variantName);

	virtual void configurationUpdated();

	virtual void loadTheme(const QString &styleName, const QString &variantName);

	virtual void styleEditionRequested(QString styleName);

	virtual bool removeStyle(const QString &styleName);
};

#endif // CHAT_ENGINE_KADU_H
