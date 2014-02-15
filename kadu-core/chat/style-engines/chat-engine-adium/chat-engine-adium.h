/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "chat/style-engines/chat-style-engine.h"

#include "adium-style.h"

#include <QtCore/QPointer>

class AdiumChatMessagesRenderer;
class MessageHtmlRendererService;

// What a ugly hack!
// TODO: remove
class RefreshViewHack : public QObject
{
	Q_OBJECT

	AdiumChatMessagesRenderer *Engine;
	HtmlMessagesRenderer *Renderer;

public:
	explicit RefreshViewHack(AdiumChatMessagesRenderer *engine, HtmlMessagesRenderer *renderer, QObject *parent = 0);
	virtual ~RefreshViewHack();

public slots:
	void cancel();
	void loadFinished();

signals:
	void finished(HtmlMessagesRenderer *);
};

class AdiumChatStyleEngine : public QObject, public ChatStyleEngine
{
	Q_OBJECT

	QPointer<MessageHtmlRendererService> CurrentMessageHtmlRendererService;
	AdiumStyle CurrentStyle;

public:
	explicit AdiumChatStyleEngine(QObject *parent = 0);
	virtual ~AdiumChatStyleEngine();

	void setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService);

	virtual std::unique_ptr<ChatMessagesRenderer> createRenderer(const QString &styleName, const QString &variantName);

	virtual bool supportVariants() { return true; }
	virtual QString isStyleValid(QString styleName);
	virtual QString currentStyleVariant();
	virtual QString defaultVariant(const QString &styleName);

	virtual QStringList styleVariants(QString styleName);
	virtual bool styleUsesTransparencyByDefault(QString styleName);

};
