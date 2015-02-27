/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "kadu-style-renderer.h"

#include "chat-style/chat-style-manager.h"
#include "chat-style/engine/kadu/kadu-chat-syntax.h"
#include "contacts/contact-set.h"
#include "message/message-render-info.h"
#include "misc/misc.h"
#include "parser/parser.h"
#include "protocols/services/chat-image.h"

#include <QtGui/QTextDocument>
#include <QtWebKitWidgets/QWebFrame>

KaduStyleRenderer::KaduStyleRenderer(ChatStyleRendererConfiguration configuration, std::shared_ptr<KaduChatSyntax> style, QObject *parent) :
		ChatStyleRenderer{std::move(configuration), parent},
		m_style{std::move(style)}
{
	auto top = Parser::parse(m_style->top(), Talkable(this->configuration().chat().contacts().toContact()), ParserEscape::HtmlEscape);
	auto html = QString{
		"<html>"
		"	<head>"
		"		<style type='text/css'>"
		"			%1"
		"		</style>"
		"	</head>"
		"	<body>"
		"		<script>"
		"			%2"
		"		</script>"
		"		%3"
		"	</body>"
		"</html>"
	};

	this->configuration().webFrame().setHtml(html
		.arg(Qt::escape(ChatStyleManager::instance()->mainStyle()))
		.arg(this->configuration().javaScript())
		.arg(top)
	);

	connect(&this->configuration().webFrame(), SIGNAL(loadFinished(bool)), this, SLOT(setReady()));
}

KaduStyleRenderer::~KaduStyleRenderer()
{
}

void KaduStyleRenderer::clearMessages()
{
	configuration().webFrame().evaluateJavaScript("kadu_clearMessages()");
}

void KaduStyleRenderer::removeFirstMessage()
{
	configuration().webFrame().evaluateJavaScript("kadu_removeFirstMessage()");
}

void KaduStyleRenderer::appendChatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo)
{
	QString html(replacedNewLine(formatMessage(message, messageRenderInfo), QLatin1String(" ")));
	html.replace('\\', QLatin1String("\\\\"));
	html.replace('\'', QLatin1String("\\'"));
	if (!message.id().isEmpty())
		html.prepend(QString("<span class=\"kadu_message\" id=\"message_%1\">").arg(Qt::escape(message.id())));
	else
		html.prepend("<span class=\"kadu_message\">");
	html.append("</span>");

	configuration().webFrame().evaluateJavaScript("kadu_appendMessage('" + html + "')");
}

void KaduStyleRenderer::displayMessageStatus(const QString &id, MessageStatus status)
{
	configuration().webFrame().evaluateJavaScript(QString("kadu_messageStatusChanged(\"%1\", %2);").arg(Qt::escape(id)).arg(static_cast<int>(status)));
}

void KaduStyleRenderer::displayChatState(ChatState state, const QString &message, const QString &name)
{
	configuration().webFrame().evaluateJavaScript(QString("kadu_contactActivityChanged(%1, \"%2\", \"%3\");").arg(static_cast<int>(state)).arg(Qt::escape(message)).arg(Qt::escape(name)));
}

void KaduStyleRenderer::displayChatImage(const ChatImage &chatImage, const QString &fileName)
{
	configuration().webFrame().evaluateJavaScript(QString("kadu_chatImageAvailable(\"%1\", \"%2\");").arg(Qt::escape(chatImage.key())).arg(Qt::escape(fileName)));
}

QString KaduStyleRenderer::formatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo)
{
	auto sender = message.messageSender();
	auto format = messageRenderInfo.includeHeader()
			? m_style->withHeader()
			: m_style->withoutHeader();

	return Parser::parse(format, Talkable{sender}, &messageRenderInfo, ParserEscape::HtmlEscape);
}

#include "moc_kadu-style-renderer.cpp"
