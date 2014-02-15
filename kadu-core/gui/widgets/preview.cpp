/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QHBoxLayout>

#include "chat/chat-styles-manager.h"
#include "chat/html-messages-renderer.h"
#include "chat/style-engine/kadu-style-engine/kadu-chat-syntax.h"
#include "chat/type/chat-type-contact.h"
#include "configuration/chat-configuration-holder.h"
#include "core/core.h"
#include "gui/widgets/kadu-web-view.h"
#include "parser/parser.h"

#include "preview.h"

#define PREVIEW_DEFAULT_HEIGHT 250

Preview::Preview(QWidget *parent) :
		QFrame(parent)
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setFixedHeight(PREVIEW_DEFAULT_HEIGHT);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	HtmlRenderer = new HtmlMessagesRenderer(Chat::null, this);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	WebView = new KaduWebView(this);
	WebView->setPage(HtmlRenderer->webPage());
	WebView->setImageStorageService(Core::instance()->imageStorageService());
	layout->addWidget(WebView);

	QPalette p = palette();
	p.setBrush(QPalette::Base, Qt::transparent);
	WebView->page()->setPalette(p);
	WebView->setAttribute(Qt::WA_OpaquePaintEvent, false);

	configurationUpdated();
}

Preview::~Preview()
{
	qDeleteAll(Messages);
}

void Preview::setRenderer(std::unique_ptr<ChatMessagesRenderer> renderer)
{
	Renderer = std::move(renderer);
	Renderer.get()->clearMessages(HtmlRenderer);
	Renderer.get()->refreshView(HtmlRenderer);
}

void Preview::addMessage(const Message &message)
{
	if (!HtmlRenderer->chat())
		HtmlRenderer->setChat(ChatTypeContact::findChat(message.messageSender(), ActionCreate));
	HtmlRenderer->appendMessage(message);
	Messages.append(message);
}

const QVector<Message> & Preview::messages() const
{
	return Messages;
}

KaduWebView * Preview::webView() const
{
	return WebView;
}

void Preview::syntaxChanged(const QString &content)
{
	if (!Messages.isEmpty())
		return;

	// this method is used only with hints syntax

	QString syntax = content;
	QString text = Parser::parse(syntax, Talkable(Buddy::dummy()));
	emit needFixup(text);

	WebView->setHtml(text);
}

void Preview::configurationUpdated()
{
	WebView->setUserFont(ChatConfigurationHolder::instance()->chatFont().toString(), ChatConfigurationHolder::instance()->forceCustomChatFont());
}

#include "moc_preview.cpp"
