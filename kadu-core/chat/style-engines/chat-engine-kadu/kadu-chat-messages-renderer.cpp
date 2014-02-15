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
#include "chat/html-messages-renderer.h"
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

void KaduChatMessagesRenderer::clearMessages(HtmlMessagesRenderer *renderer)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript("kadu_clearMessages()");
}

void KaduChatMessagesRenderer::pruneMessage(HtmlMessagesRenderer *renderer)
{
	if (!ChatStylesManager::instance()->cfgNoHeaderRepeat())
		renderer->webPage()->mainFrame()->evaluateJavaScript("kadu_removeFirstMessage()");
}

void KaduChatMessagesRenderer::appendMessages(HtmlMessagesRenderer *renderer, const QVector<Message> &messages)
{
	if (ChatStylesManager::instance()->cfgNoHeaderRepeat() && renderer->pruneEnabled())
	{
		repaintMessages(renderer);
		return;
	}

	for (auto const &message : messages)
		appendMessage(renderer, message);
}

void KaduChatMessagesRenderer::appendMessage(HtmlMessagesRenderer *renderer, const Message &message)
{
	if (ChatStylesManager::instance()->cfgNoHeaderRepeat() && renderer->pruneEnabled())
	{
		repaintMessages(renderer);
		return;
	}

	QString html(replacedNewLine(formatMessage(message, renderer->lastMessage()), QLatin1String(" ")));
	html.replace('\\', QLatin1String("\\\\"));
	html.replace('\'', QLatin1String("\\'"));
	if (!message.id().isEmpty())
		html.prepend(QString("<span class=\"kadu_message\" id=\"message_%1\">").arg(message.id()));
	else
		html.prepend("<span class=\"kadu_message\">");
	html.append("</span>");

	renderer->webPage()->mainFrame()->evaluateJavaScript("kadu_appendMessage('" + html + "')");

	renderer->setLastMessage(message);
}

void KaduChatMessagesRenderer::refreshView(HtmlMessagesRenderer *renderer, bool useTransparency)
{
	Q_UNUSED(useTransparency)

	repaintMessages(renderer);
}

void KaduChatMessagesRenderer::messageStatusChanged(HtmlMessagesRenderer *renderer, Message message, MessageStatus status)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript(QString("kadu_messageStatusChanged(\"%1\", %2);").arg(message.id()).arg((int)status));
}

void KaduChatMessagesRenderer::contactActivityChanged(HtmlMessagesRenderer *renderer, ChatStateService::State state, const QString &message, const QString &name)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript(QString("kadu_contactActivityChanged(%1, \"%2\", \"%3\");").arg((int)state).arg(message).arg(name));
}

void KaduChatMessagesRenderer::chatImageAvailable(HtmlMessagesRenderer *renderer, const ChatImage &chatImage, const QString &fileName)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript(QString("kadu_chatImageAvailable(\"%1\", \"%2\");").arg(chatImage.key()).arg(fileName));
}

QString KaduChatMessagesRenderer::formatMessage(const Message &message, const Message &after)
{
	auto messageRenderInfoFactory = Core::instance()->messageRenderInfoFactory();
	auto info = messageRenderInfoFactory->messageRenderInfo(after, message);
	auto sender = message.messageSender();
	auto format = info.includeHeader()
			? m_syntax.withHeader()
			: m_syntax.withoutHeader();

	return Parser::parse(format, Talkable{sender}, &info, true);
}

void KaduChatMessagesRenderer::repaintMessages(HtmlMessagesRenderer *renderer)
{
	if (!renderer)
		return;

	QString text = QString(
		"<html>"
		"	<head>"
		"		<style type='text/css'>") +
		ChatStylesManager::instance()->mainStyle() +
		"		</style>"
		"	</head>"
		"	<body>";

	text += QString("<script>%1</script>").arg(m_jsCode);

	Contact contact = renderer->chat().contacts().count() == 1 ? *(renderer->chat().contacts().constBegin()) : Contact();
	text += Parser::parse(m_syntax.top(), Talkable(contact), true);

	auto prevMessage = Message::null;
	for (auto const &message : renderer->messages())
	{
		QString messageText;
		if (!message.id().isEmpty())
			messageText = QString("<span class=\"kadu_message\" id=\"message_%1\">%2</span>").arg(message.id()).arg(formatMessage(message, prevMessage));
		else
			messageText = QString("<span class=\"kadu_message\">%1</span>").arg(formatMessage(message, prevMessage));
		messageText = scriptsAtEnd(messageText);
		text += messageText;
		prevMessage = message;
	}
	renderer->setLastMessage(prevMessage);

	text += "</body></html>";

	renderer->webPage()->mainFrame()->setHtml(text);
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
