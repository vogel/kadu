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

KaduChatMessagesRenderer::KaduChatMessagesRenderer(KaduChatSyntax syntax) :
		m_syntax{std::move(syntax)}
{
	QFile file(KaduPaths::instance()->dataPath() + QLatin1String("scripts/chat-scripts.js"));
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		m_jsCode = file.readAll();
}

void KaduChatMessagesRenderer::clearMessages(QWebFrame &frame)
{
	frame.evaluateJavaScript("kadu_clearMessages()");
}

void KaduChatMessagesRenderer::removeFirstMessage(QWebFrame &frame)
{
	frame.evaluateJavaScript("kadu_removeFirstMessage()");
}

void KaduChatMessagesRenderer::appendChatMessage(QWebFrame &frame, const Message &message, const MessageRenderInfo &messageRenderInfo)
{
	QString html(replacedNewLine(formatMessage(message, messageRenderInfo), QLatin1String(" ")));
	html.replace('\\', QLatin1String("\\\\"));
	html.replace('\'', QLatin1String("\\'"));
	if (!message.id().isEmpty())
		html.prepend(QString("<span class=\"kadu_message\" id=\"message_%1\">").arg(message.id()));
	else
		html.prepend("<span class=\"kadu_message\">");
	html.append("</span>");

	frame.evaluateJavaScript("kadu_appendMessage('" + html + "')");
}

void KaduChatMessagesRenderer::refreshView(QWebFrame &frame, const Chat &chat, const QVector<Message> &allMessages, bool useTransparency)
{
	Q_UNUSED(useTransparency)

	paintMessages(frame, chat, allMessages);
}

void KaduChatMessagesRenderer::messageStatusChanged(QWebFrame &frame, Message message, MessageStatus status)
{
	frame.evaluateJavaScript(QString("kadu_messageStatusChanged(\"%1\", %2);").arg(message.id()).arg((int)status));
}

void KaduChatMessagesRenderer::contactActivityChanged(QWebFrame &frame, ChatStateService::State state, const QString &message, const QString &name)
{
	frame.evaluateJavaScript(QString("kadu_contactActivityChanged(%1, \"%2\", \"%3\");").arg((int)state).arg(message).arg(name));
}

void KaduChatMessagesRenderer::chatImageAvailable(QWebFrame &frame, const ChatImage &chatImage, const QString &fileName)
{
	frame.evaluateJavaScript(QString("kadu_chatImageAvailable(\"%1\", \"%2\");").arg(chatImage.key()).arg(fileName));
}

QString KaduChatMessagesRenderer::formatMessage(const Message &message, const MessageRenderInfo &messageRenderInfo)
{
	auto sender = message.messageSender();
	auto format = messageRenderInfo.includeHeader()
			? m_syntax.withHeader()
			: m_syntax.withoutHeader();

	return Parser::parse(format, Talkable{sender}, &messageRenderInfo, true);
}

void KaduChatMessagesRenderer::paintMessages(QWebFrame &frame, const Chat &chat, const QVector<Message> &allMessages)
{
	QString text = QString(
		"<html>"
		"	<head>"
		"		<style type='text/css'>") +
		ChatStylesManager::instance()->mainStyle() +
		"		</style>"
		"	</head>"
		"	<body>";

	text += QString("<script>%1</script>").arg(m_jsCode);

	auto contact = chat.contacts().count() == 1
			? *(chat.contacts().constBegin())
			: Contact{};
	text += Parser::parse(m_syntax.top(), Talkable(contact), true);

	auto messageRenderInfoFactory = Core::instance()->messageRenderInfoFactory();
	auto prevMessage = Message::null;
	for (auto const &message : allMessages)
	{
		auto info = messageRenderInfoFactory->messageRenderInfo(prevMessage, message);
		auto messageText = message.id().isEmpty()
				? QString("<span class=\"kadu_message\">%1</span>").arg(formatMessage(message, info))
				: QString("<span class=\"kadu_message\" id=\"message_%1\">%2</span>").arg(message.id()).arg(formatMessage(message, info));
		messageText = scriptsAtEnd(messageText);
		text += messageText;
		prevMessage = message;
	}

	text += "</body></html>";

	frame.setHtml(text);
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
