/*
 * %kadu copyright begin%
 * Copyright 2012 Marcel Zięba (marseel@gmail.com)
 * Copyright 2009, 2010, 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "adium-style-engine.h"

#include "chat/html-messages-renderer.h"
#include "chat/style-engine/adium-style-engine/adium-chat-messages-renderer.h"
#include "message/message-html-renderer-service.h"
#include "misc/kadu-paths.h"
#include "misc/memory.h"

#include <QtCore/QDir>

RefreshViewHack::RefreshViewHack(AdiumChatMessagesRenderer *engine, HtmlMessagesRenderer *renderer, QObject *parent) :
		QObject(parent), Engine(engine), Renderer(renderer)
{
	connect(Engine, SIGNAL(destroyed()), this, SLOT(cancel()));
	connect(Renderer, SIGNAL(destroyed()), this, SLOT(cancel()));
}

RefreshViewHack::~RefreshViewHack()
{
}

void RefreshViewHack::cancel()
{
	Engine = 0;
	Renderer = 0;

	deleteLater();
}

void RefreshViewHack::loadFinished()
{
	if (!Engine || !Renderer)
	{
		deleteLater();
		return;
	}

	emit finished(Renderer);

	Renderer->setLastMessage(Message::null);

	for (auto const &message : Renderer->messages())
		Engine->appendChatMessage(Renderer, message);

	deleteLater();
}

AdiumStyleEngine::AdiumStyleEngine()
{
}

AdiumStyleEngine::~AdiumStyleEngine()
{
}

void AdiumStyleEngine::setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService)
{
	CurrentMessageHtmlRendererService = messageHtmlRendererService;
}

QString AdiumStyleEngine::isStyleValid(QString stylePath)
{
	return AdiumStyle::isStyleValid(stylePath) ? QDir(stylePath).dirName() : QString();
}

bool AdiumStyleEngine::styleUsesTransparencyByDefault(QString styleName)
{
	AdiumStyle style(styleName);
	return style.defaultBackgroundIsTransparent();
}

QString AdiumStyleEngine::defaultVariant(const QString &styleName)
{
	AdiumStyle style(styleName);
	return style.defaultVariant();
}

QString AdiumStyleEngine::currentStyleVariant()
{
	return CurrentStyle.currentVariant();
}

QStringList AdiumStyleEngine::styleVariants(QString styleName)
{
	QDir dir;
	QString styleBaseHref = KaduPaths::instance()->profilePath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/Variants/");
	if (!dir.exists(styleBaseHref))
		styleBaseHref = KaduPaths::instance()->dataPath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/Variants/");
	dir.setPath(styleBaseHref);
	dir.setNameFilters(QStringList("*.css"));
	return dir.entryList();
}

std::unique_ptr<ChatMessagesRenderer> AdiumStyleEngine::createRenderer(const QString &styleName, const QString &variantName)
{
	CurrentStyle = AdiumStyle(styleName);
	CurrentStyle.setCurrentVariant(variantName);
	CurrentStyleName = styleName;

	auto result = make_unique<AdiumChatMessagesRenderer>(CurrentStyle);
	result.get()->setMessageHtmlRendererService(CurrentMessageHtmlRendererService);
	return std::move(result);
}

#include "moc_adium-style-engine.cpp"
