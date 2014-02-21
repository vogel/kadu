/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "kadu-chat-messages-renderer.h"

#include "chat/chat-styles-manager.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "message/message-render-info.h"
#include "message/message-render-info-factory.h"
#include "misc/kadu-paths.h"
#include "misc/misc.h"
#include "parser/parser.h"
#include "protocols/services/chat-image.h"

#include <QtCore/QFile>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebPage>

KaduChatMessagesRenderer::KaduChatMessagesRenderer(ChatMessagesRendererConfiguration configuration, std::shared_ptr<KaduChatSyntax> style, QObject *parent) :
		ChatMessagesRenderer{std::move(configuration), parent},
		m_style{std::move(style)}
{
	auto top = Parser::parse(m_style->top(), Talkable(this->configuration().chat().contacts().toContact()), true);

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
		.arg(ChatStylesManager::instance()->mainStyle())
		.arg(this->configuration().javaScript())
		.arg(top)
	);
}

KaduChatMessagesRenderer::~KaduChatMessagesRenderer()
{
}

void KaduChatMessagesRenderer::clearMessages()
{
	configuration().webFrame().evaluateJavaScript("kadu_clearMessages()");
}

void KaduChatMessagesRenderer::removeFirstMessage()
{
	configuration().webFrame().evaluateJavaScript("kadu_removeFirstMessage()");
}

void KaduChatMessagesRenderer::appendChatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo)
{
	QString html(replacedNewLine(formatMessage(message, messageRenderInfo), QLatin1String(" ")));
	html.replace('\\', QLatin1String("\\\\"));
	html.replace('\'', QLatin1String("\\'"));
	if (!message.id().isEmpty())
		html.prepend(QString("<span class=\"kadu_message\" id=\"message_%1\">").arg(message.id()));
	else
		html.prepend("<span class=\"kadu_message\">");
	html.append("</span>");

	configuration().webFrame().evaluateJavaScript("kadu_appendMessage('" + html + "')");
}

void KaduChatMessagesRenderer::messageStatusChanged(const QString &id, MessageStatus status)
{
	configuration().webFrame().evaluateJavaScript(QString("kadu_messageStatusChanged(\"%1\", %2);").arg(id).arg(static_cast<int>(status)));
}

void KaduChatMessagesRenderer::contactActivityChanged(ChatStateService::State state, const QString &message, const QString &name)
{
	configuration().webFrame().evaluateJavaScript(QString("kadu_contactActivityChanged(%1, \"%2\", \"%3\");").arg((int)state).arg(message).arg(name));
}

void KaduChatMessagesRenderer::chatImageAvailable(const ChatImage &chatImage, const QString &fileName)
{
	configuration().webFrame().evaluateJavaScript(QString("kadu_chatImageAvailable(\"%1\", \"%2\");").arg(chatImage.key()).arg(fileName));
}

QString KaduChatMessagesRenderer::formatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo)
{
	auto sender = message.messageSender();
	auto format = messageRenderInfo.includeHeader()
			? m_style->withHeader()
			: m_style->withoutHeader();

	return Parser::parse(format, Talkable{sender}, &messageRenderInfo, true);
}

QString KaduChatMessagesRenderer::scriptsAtEnd(const QString &html)
{
	QString html2 = html;
	QString scripts;
	QRegExp scriptRegexp("<script[^>]*>.*</script>", Qt::CaseInsensitive);
	scriptRegexp.setMinimal(true);
	int k = 0;
	while (true)
	{
		k = html2.indexOf(scriptRegexp, k);
		if (k == -1)
			break;
		scripts += scriptRegexp.cap();
		k += scriptRegexp.matchedLength();
	}
	html2.remove(scriptRegexp);
	return html2 + scripts;
}

#include "moc_kadu-chat-messages-renderer.cpp"
