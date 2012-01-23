/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_ENGINE_ADIUM_H
#define CHAT_ENGINE_ADIUM_H

#include "../chat-style-engine.h"
#include "adium-style.h"

class AdiumChatStyleEngine;
class Chat;
class Preview;

// What a ugly hack!
// TODO: remove
class RefreshViewHack : public QObject
{
	Q_OBJECT

	AdiumChatStyleEngine *Engine;
	HtmlMessagesRenderer *Renderer;

private slots:
	void cancel();

public:
	explicit RefreshViewHack(AdiumChatStyleEngine *engine, HtmlMessagesRenderer *renderer, QObject *parent = 0);
	virtual ~RefreshViewHack();

public slots:
	void loadFinished();

};

class PreviewHack : public QObject
{
	Q_OBJECT

	AdiumChatStyleEngine *Engine;
	Preview *CurrentPreview;
	QString BaseHref;
	QString OutgoingHtml;
	QString IncomingHtml;

private slots:
	void cancel();

public:
	explicit PreviewHack(AdiumChatStyleEngine *engine, Preview *preview, const QString &baseHref, const QString &outgoingHtml,
	                     const QString &incomingHtml, QObject *parent = 0);
	virtual ~PreviewHack();

public slots:
	void loadFinished();

};

class AdiumChatStyleEngine : public QObject, public ChatStyleEngine
{
	Q_OBJECT

	friend class RefreshViewHack;
	friend class PreviewHack;

	AdiumStyle CurrentStyle;
	RefreshViewHack *CurrentRefreshHack;
	PreviewHack *CurrentPreviewHack;

	QString jsCode;

	QString replaceKeywords(const Chat &chat, const QString &styleHref, const QString &style);
	QString replaceKeywords(const QString &styleHref, const QString &source, MessageRenderInfo *message);

	bool clearDirectory(const QString &directory);

	void appendChatMessage(HtmlMessagesRenderer *renderer, MessageRenderInfo *message);

private slots:
	void currentRefreshHackDestroyed();
	void currentPreviewHackDestroyed();

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
	virtual void messageStatusChanged(HtmlMessagesRenderer *renderer, Message message, MessageStatus status);
	virtual void contactActivityChanged(HtmlMessagesRenderer *renderer, ChatStateService::ContactActivity state, const QString &message, const QString &name);

	virtual void prepareStylePreview(Preview *preview, QString styleName, QString variantName);

	virtual void configurationUpdated() {}

	virtual void loadStyle(const QString &styleName, const QString &variantName);

	virtual void styleEditionRequested(QString ) {} //do nothing. Adium styles don't support editing

	virtual bool removeStyle(const QString &styleName);

};

#endif // CHAT_ENGINE_ADIUM_H
