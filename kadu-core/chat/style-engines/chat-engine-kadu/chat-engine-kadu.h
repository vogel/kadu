/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_ENGINE_KADU_H
#define CHAT_ENGINE_KADU_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>

#include "../chat-style-engine.h"

class Preview;
class SyntaxList;

class KaduChatStyleEngine : public QObject, public ChatStyleEngine
{
	Q_OBJECT

	QSharedPointer<SyntaxList> syntaxList;

	QString jsCode;

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
	explicit KaduChatStyleEngine(QObject *parent = 0);
	virtual ~KaduChatStyleEngine();
	virtual bool supportVariants() { return false; }
	virtual bool supportEditing() { return true; }
	virtual QString isStyleValid(QString styleName);
	virtual bool styleUsesTransparencyByDefault(QString styleName)
	{
		Q_UNUSED(styleName)
		return false;
	}

	virtual void clearMessages(HtmlMessagesRenderer *renderer);
	virtual void appendMessages(HtmlMessagesRenderer *renderer, const QList<MessageRenderInfo *> &messages);
	virtual void appendMessage(HtmlMessagesRenderer *renderer, MessageRenderInfo *message);
	virtual void pruneMessage(HtmlMessagesRenderer *renderer);
	virtual void refreshView(HtmlMessagesRenderer *renderer, bool useTransparency = false);
	virtual void messageStatusChanged(HtmlMessagesRenderer *renderer, Message message, Message::Status status);

	virtual void prepareStylePreview(Preview *preview, QString styleName, QString variantName);

	virtual void configurationUpdated();

	virtual void loadStyle(const QString &styleName, const QString &variantName);

	virtual void styleEditionRequested(QString styleName);

	virtual bool removeStyle(const QString &styleName);
};

#endif // CHAT_ENGINE_KADU_H
