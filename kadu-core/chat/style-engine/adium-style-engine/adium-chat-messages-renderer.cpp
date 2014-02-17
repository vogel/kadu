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

#include "adium-chat-messages-renderer.h"

#include "avatars/avatar.h"
#include "chat/chat-styles-manager.h"
#include "chat/html-messages-renderer.h"
#include "chat/style-engine/adium-style-engine/adium-style-engine.h"
#include "chat/style-engine/adium-style-engine/adium-time-formatter.h"
#include "configuration/chat-configuration-holder.h"
#include "contacts/contact-set.h"
#include "core/core.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "message/message-html-renderer-service.h"
#include "message/message-render-info.h"
#include "message/message-render-info-factory.h"
#include "misc/date-time.h"
#include "misc/kadu-paths.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/services/chat-image.h"

#include <QtCore/QFile>
#include <QtWebKit/QWebPage>
#include <QtWebKit/QWebFrame>

AdiumChatMessagesRenderer::AdiumChatMessagesRenderer(AdiumStyle style) :
		m_style{style}
{
	// Load required javascript functions
	QFile file(KaduPaths::instance()->dataPath() + QLatin1String("scripts/chat-scripts.js"));
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		m_jsCode = file.readAll();
}

void AdiumChatMessagesRenderer::setMessageHtmlRendererService(MessageHtmlRendererService *messageHtmlRendererService)
{
	m_messageHtmlRendererService = messageHtmlRendererService;
}

void AdiumChatMessagesRenderer::removeFirstMessage(QWebFrame &frame)
{
	frame.evaluateJavaScript("adium_removeFirstMessage()");
}

void AdiumChatMessagesRenderer::clearMessages(QWebFrame &frame)
{
	frame.evaluateJavaScript("adium_clearMessages()");
}

void AdiumChatMessagesRenderer::paintMessages(QWebFrame &frame, const Chat &chat, const QVector<Message> &messages)
{
	Q_UNUSED(chat);

	clearMessages(frame);
	auto lastMessage = Message::null;
	for (auto const &oldMessage : messages)
	{
		appendChatMessage(frame, oldMessage, lastMessage);
		lastMessage = oldMessage;
	}
}

void AdiumChatMessagesRenderer::appendChatMessage(QWebFrame &frame, const Message &newMessage, const Message &lastMessage)
{
	auto messageRenderInfoFactory = Core::instance()->messageRenderInfoFactory();
	auto info = messageRenderInfoFactory->messageRenderInfo(lastMessage, newMessage);

	QString formattedMessageHtml;

	switch (newMessage.type())
	{
		case MessageTypeReceived:
		{
			if (info.includeHeader())
				formattedMessageHtml = m_style.incomingHtml();
			else
				formattedMessageHtml = m_style.nextIncomingHtml();
			break;
		}
		case MessageTypeSent:
		{
			if (info.includeHeader())
				formattedMessageHtml = m_style.outgoingHtml();
			else
				formattedMessageHtml = m_style.nextOutgoingHtml();
			break;
		}
		case MessageTypeSystem:
		{
			formattedMessageHtml = m_style.statusHtml();
			break;
		}

		default:
			break;
	}

	formattedMessageHtml = replacedNewLine(replaceKeywords(m_style.baseHref(), formattedMessageHtml, newMessage, info.nickColor()), QLatin1String(" "));
	formattedMessageHtml.replace('\\', QLatin1String("\\\\"));
	formattedMessageHtml.replace('\'', QLatin1String("\\'"));
	if (!newMessage.id().isEmpty())
		formattedMessageHtml.prepend(QString("<span id=\"message_%1\">").arg(newMessage.id()));
	else
		formattedMessageHtml.prepend("<span>");
	formattedMessageHtml.append("</span>");

	if (info.includeHeader())
		frame.evaluateJavaScript("appendMessage('"+ formattedMessageHtml +"')");
	else
		frame.evaluateJavaScript("appendNextMessage('"+ formattedMessageHtml +"')");
}

QString AdiumChatMessagesRenderer::preprocessStyleBaseHtml(AdiumStyle &style, const Chat &chat)
{
	QString styleBaseHtml = style.templateHtml();
	styleBaseHtml.replace(styleBaseHtml.indexOf("%@"), 2, KaduPaths::webKitPath(style.baseHref()));
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(chat, style.baseHref(), style.footerHtml()));
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(chat, style.baseHref(), style.headerHtml()));

	if (style.usesCustomTemplateHtml() && style.styleViewVersion() < 3)
	{
		if (style.currentVariant() != style.defaultVariant())
			styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, "Variants/" + style.currentVariant());
		else
			styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, style.mainHref());
	}
	else
	{
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, (style.styleViewVersion() < 3 && style.defaultVariant() == style.currentVariant()) ? style.mainHref() : "Variants/" + style.currentVariant());
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, (style.styleViewVersion() < 3) ? QString() : QString("@import url( \"" + style.mainHref() + "\" );"));
	}

	return styleBaseHtml;
}

void AdiumChatMessagesRenderer::refreshView(QWebFrame &frame, const Chat &chat, const QVector<Message> &allMessages, bool useTransparency)
{
	Q_UNUSED(allMessages);

	QString styleBaseHtml = preprocessStyleBaseHtml(m_style, chat);

	if (useTransparency && !m_style.defaultBackgroundIsTransparent())
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("==bodyBackground=="), static_cast<int>(qstrlen("==bodyBackground==")), "background-image: none; background: none; background-color: rgba(0, 0, 0, 0)");

	frame.setHtml(styleBaseHtml);
	frame.evaluateJavaScript(m_jsCode);
	//I don't know why, sometimes 'initStyle' was performed after 'appendMessage'
	frame.evaluateJavaScript("initStyle()");
}

// Some parts of the code below are borrowed from Kopete project (http://kopete.kde.org/)
QString AdiumChatMessagesRenderer::replaceKeywords(const Chat &chat, const QString &styleHref, const QString &style)
{
	if (!chat)
		return {};

	QString result = style;

	//TODO: get Chat name (contacts' nicks?)
	//Replace %chatName% //TODO. Find way to dynamic update this tag (add id ?)
	int contactsCount = chat.contacts().count();

	QString chatName;
	if (!chat.display().isEmpty())
		chatName = chat.display();
	else if (contactsCount > 1)
		chatName = tr("Conference [%1]").arg(contactsCount);
	else
		chatName = chat.name();

	result.replace(QString("%chatName%"), chatName);

	// Replace %sourceName%
	result.replace(QString("%sourceName%"), chat.chatAccount().accountIdentity().name());
	// Replace %destinationName%
	result.replace(QString("%destinationName%"), chat.name());
	// For %timeOpened%, display the date and time. TODO: get real time
	result.replace(QString("%timeOpened%"), printDateTime(QDateTime::currentDateTime()));

	//TODO 0.10.0: get real time!!!
	QRegExp timeRegExp("%timeOpened\\{([^}]*)\\}%");
	int pos = 0;
	while ((pos=timeRegExp.indexIn(result, pos)) != -1)
		result.replace(pos, timeRegExp.cap(0).length(), AdiumTimeFormatter::convertTimeDate(timeRegExp.cap(1), QDateTime::currentDateTime()));

	QString photoIncoming;
	QString photoOutgoing;

	int contactsSize = chat.contacts().size();
	if (contactsSize == 1)
	{
		const Avatar &avatar = chat.contacts().toContact().avatar(true);
		if (!avatar.isEmpty())
			photoIncoming = KaduPaths::webKitPath(avatar.smallFilePath());
		else
			photoIncoming = KaduPaths::webKitPath(styleHref + QLatin1String("Incoming/buddy_icon.png"));
	}
	else
		photoIncoming = KaduPaths::webKitPath(styleHref + QLatin1String("Incoming/buddy_icon.png"));

	const Avatar &avatar = chat.chatAccount().accountContact().avatar(true);
	if (!avatar.isEmpty())
		photoOutgoing = KaduPaths::webKitPath(avatar.smallFilePath());
	else
		photoOutgoing = KaduPaths::webKitPath(styleHref + QLatin1String("Outgoing/buddy_icon.png"));

	result.replace(QString("%incomingIconPath%"), photoIncoming);
	result.replace(QString("%outgoingIconPath%"), photoOutgoing);

	return result;
}

QString AdiumChatMessagesRenderer::replaceKeywords(const QString &styleHref, const QString &source, const Message &message, const QString &nickColor)
{
	QString result = source;

	// Replace sender (contact nick)
	result.replace(QString("%sender%"), message.messageSender().display(true));
	// Replace %screenName% (contact ID)
	result.replace(QString("%senderScreenName%"), message.messageSender().id());
	// Replace service name (protocol name)
	if (message.messageChat().chatAccount().protocolHandler() && message.messageChat().chatAccount().protocolHandler()->protocolFactory())
	{
		result.replace(QString("%service%"), message.messageChat().chatAccount().protocolHandler()->protocolFactory()->displayName());
		// Replace protocolIcon (sender statusIcon). TODO:
		result.replace(QString("%senderStatusIcon%"), message.messageChat().chatAccount().protocolHandler()->protocolFactory()->icon().fullPath());
	}
	else
	{
		result.replace(QString("%service%"), message.messageChat().chatAccount().accountIdentity().name());
		result.remove("%senderStatusIcon%");
	}

	// Replace time
	QDateTime time = message.sendDate().isNull() ? message.receiveDate(): message.sendDate();
	result.replace(QString("%time%"), printDateTime(time));
	// Look for %time{X}%
	QRegExp timeRegExp("%time\\{([^}]*)\\}%");
	int pos = 0;
	while ((pos = timeRegExp.indexIn(result , pos)) != -1)
		result.replace(pos, timeRegExp.cap(0).length(), AdiumTimeFormatter::convertTimeDate(timeRegExp.cap(1), time));

	result.replace("%shortTime%", printDateTime(time));

	// Look for %textbackgroundcolor{X}%
	// TODO: highlight background color: use the X value.
	QRegExp textBackgroundRegExp("%textbackgroundcolor\\{([^}]*)\\}%");
	int textPos = 0;
	while ((textPos=textBackgroundRegExp.indexIn(result, textPos)) != -1)
		result.replace(textPos, textBackgroundRegExp.cap(0).length(), "inherit");

	// Replace userIconPath
	QString photoPath;
	if (message.type() == MessageTypeReceived)
	{
		result.replace(QString("%messageClasses%"), "message incoming");

		const Avatar &avatar = message.messageSender().avatar(true);
		if (!avatar.isEmpty())
			photoPath = KaduPaths::webKitPath(avatar.smallFilePath());
		else
			photoPath = KaduPaths::webKitPath(styleHref + QLatin1String("Incoming/buddy_icon.png"));
	}
	else if (message.type() == MessageTypeSent)
	{
		result.replace(QString("%messageClasses%"), "message outgoing");
		const Avatar &avatar = message.messageChat().chatAccount().accountContact().avatar(true);
		if (!avatar.isEmpty())
			photoPath = KaduPaths::webKitPath(avatar.smallFilePath());
		else
			photoPath = KaduPaths::webKitPath(styleHref + QLatin1String("Outgoing/buddy_icon.png"));
	}
	else
		result.remove(QString("%messageClasses%"));

	result.replace(QString("%userIconPath%"), photoPath);

	//Message direction ("rtl"(Right-To-Left) or "ltr"(Left-to-right))
	result.replace(QString("%messageDirection%"), "ltr");

	// Replace contact's color
	QString lightColorName;
	QRegExp senderColorRegExp("%senderColor(?:\\{([^}]*)\\})?%");
	textPos = 0;
	while ((textPos = senderColorRegExp.indexIn(result, textPos)) != -1)
	{
		int light = 100;
		bool doLight = false;
		if (senderColorRegExp.captureCount() >= 1)
			light = senderColorRegExp.cap(1).toInt(&doLight);

		if (doLight && lightColorName.isNull())
			lightColorName = QColor(nickColor).light(light).name();

		result.replace(textPos, senderColorRegExp.cap(0).length(), doLight ? lightColorName : nickColor);
	}

	QString messageText = m_messageHtmlRendererService
			? m_messageHtmlRendererService.data()->renderMessage(message)
			: message.htmlContent();

	if (!message.id().isEmpty())
		messageText.prepend(QString("<span id=\"message_%1\">").arg(message.id()));
	else
		messageText.prepend("<span>");
	messageText.append("</span>");

	result.replace(QString("%messageId%"), message.id());
	result.replace(QString("%messageStatus%"), QString::number(message.status()));

	result.replace(QString("%message%"), messageText);

	return result;
}

void AdiumChatMessagesRenderer::messageStatusChanged(QWebFrame &frame, Message message, MessageStatus status)
{
	frame.evaluateJavaScript(QString("adium_messageStatusChanged(\"%1\", %2);").arg(message.id()).arg((int)status));
}

void AdiumChatMessagesRenderer::contactActivityChanged(QWebFrame &frame, ChatStateService::State state, const QString &message, const QString &name)
{
	frame.evaluateJavaScript(QString("adium_contactActivityChanged(%1, \"%2\", \"%3\");").arg((int)state).arg(message).arg(name));
}

void AdiumChatMessagesRenderer::chatImageAvailable(QWebFrame &frame, const ChatImage &chatImage, const QString &fileName)
{
	frame.evaluateJavaScript(QString("adium_chatImageAvailable(\"%1\", \"%2\");").arg(chatImage.key()).arg(fileName));
}

#include "moc_adium-chat-messages-renderer.cpp"
