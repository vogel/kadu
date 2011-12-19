/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QHBoxLayout>

#include "chat/style-engines/chat-engine-kadu/kadu-chat-syntax.h"
#include "configuration/chat-configuration-holder.h"
#include "gui/widgets/kadu-web-view.h"
#include "message/message-render-info.h"
#include "parser/parser.h"

#include "preview.h"

Preview::Preview(QWidget *parent) :
		QFrame(parent)
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	setFixedHeight(PREVIEW_DEFAULT_HEIGHT);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(5, 5, 5, 5);

	WebView = new KaduWebView(this);
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

void Preview::addMessage(MessageRenderInfo *messageRenderInfo)
{
	 Messages.append(messageRenderInfo);
}

const QList<MessageRenderInfo *> & Preview::messages() const
{
	return Messages;
}

KaduWebView * Preview::webView() const
{
	return WebView;
}

void Preview::syntaxChanged(const QString &content)
{
	// this method is used only with Kadu styles

	QString syntax = content;
	emit needSyntaxFixup(syntax);

	QString text;

	if (!Messages.isEmpty())
	{
		KaduChatSyntax syntax(content);

		text = Parser::parse(syntax.top(), Talkable(), true);

		foreach (MessageRenderInfo *message, Messages)
			text += Parser::parse(syntax.withHeader(), message->message().messageSender(), message);
	}
	else
		text = Parser::parse(syntax, Talkable(Buddy::dummy()));

	emit needFixup(text);

	WebView->setHtml(text);
}

void Preview::configurationUpdated()
{
	WebView->setUserFont(ChatConfigurationHolder::instance()->chatFont().toString(), ChatConfigurationHolder::instance()->forceCustomChatFont());
}
