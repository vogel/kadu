/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include <QtCore/QFileInfo>
#include <QtWebKit/QWebFrame>

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "chat/chat-styles-manager.h"
#include "chat/html-messages-renderer.h"
#include "core/core.h"
#include "gui/widgets/preview.h"
#include "gui/widgets/webkit-messages-view.h"
#include "icons/kadu-icon.h"
#include "message/message-render-info.h"
#include "message/message-render-info-factory.h"
#include "misc/kadu-paths.h"
#include "misc/misc.h"
#include "misc/syntax-list.h"
#include "parser/parser.h"

#include "chat-engine-kadu.h"

KaduChatStyleEngine::KaduChatStyleEngine(QObject *parent) :
		QObject(parent)
{
	EngineName = "Kadu";
	syntaxList = QSharedPointer<SyntaxList>(new SyntaxList("chat"));
	QFile file(KaduPaths::instance()->dataPath() + QLatin1String("scripts/chat-scripts.js"));
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		jsCode = file.readAll();
}

KaduChatStyleEngine::~KaduChatStyleEngine()
{
}

void KaduChatStyleEngine::clearMessages(HtmlMessagesRenderer *renderer)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript("kadu_clearMessages()");
}

void KaduChatStyleEngine::pruneMessage(HtmlMessagesRenderer *renderer)
{
	if (!ChatStylesManager::instance()->cfgNoHeaderRepeat())
		renderer->webPage()->mainFrame()->evaluateJavaScript("kadu_removeFirstMessage()");
}

void KaduChatStyleEngine::appendMessages(HtmlMessagesRenderer *renderer, const QVector<Message> &messages)
{
	if (ChatStylesManager::instance()->cfgNoHeaderRepeat() && renderer->pruneEnabled())
	{
		repaintMessages(renderer);
		return;
	}

	for (auto const &message : messages)
		appendMessage(renderer, message);
}

void KaduChatStyleEngine::appendMessage(HtmlMessagesRenderer *renderer, const Message &message)
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

void KaduChatStyleEngine::refreshView(HtmlMessagesRenderer *renderer, bool useTransparency)
{
	Q_UNUSED(useTransparency)

	repaintMessages(renderer);
}

void KaduChatStyleEngine::messageStatusChanged(HtmlMessagesRenderer *renderer, Message message, MessageStatus status)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript(QString("kadu_messageStatusChanged(\"%1\", %2);").arg(message.id()).arg((int)status));
}

void KaduChatStyleEngine::contactActivityChanged(HtmlMessagesRenderer *renderer, ChatStateService::State state, const QString &message, const QString &name)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript(QString("kadu_contactActivityChanged(%1, \"%2\", \"%3\");").arg((int)state).arg(message).arg(name));
}

void KaduChatStyleEngine::chatImageAvailable(HtmlMessagesRenderer *renderer, const ChatImage &chatImage, const QString &fileName)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript(QString("kadu_chatImageAvailable(\"%1\", \"%2\");").arg(chatImage.key()).arg(fileName));
}

QString KaduChatStyleEngine::isStyleValid(QString stylePath)
{
	QFileInfo fi;
	fi.setFile(stylePath);
	return fi.suffix() == "syntax" ? fi.completeBaseName() : QString();
}

void KaduChatStyleEngine::loadStyle(const QString &styleName, const QString &variantName)
{
	Q_UNUSED(variantName)

	QString chatSyntax = SyntaxList::readSyntax("chat", styleName,
		"<p style=\"background-color: #{backgroundColor};\">#{separator}"
		  "<font color=\"#{fontColor}\"><kadu:header><b><font color=\"#{nickColor}\">%a</font> :: "
			"#{receivedDate}[ / S #{sentDate}]</b><br /></kadu:header>"
		"#{message}</font></p>"
	);
	CurrentChatSyntax.setSyntax(chatSyntax);

	CurrentStyleName = styleName;
}

QString KaduChatStyleEngine::formatMessage(const Message &message, const Message &after)
{
	auto messageRenderInfoFactory = Core::instance()->messageRenderInfoFactory();
	auto info = messageRenderInfoFactory->messageRenderInfo(after, message);
	auto sender = message.messageSender();
	auto format = info.includeHeader()
			? CurrentChatSyntax.withHeader()
			: CurrentChatSyntax.withoutHeader();

	return Parser::parse(format, Talkable{sender}, &info, true);
}

void KaduChatStyleEngine::repaintMessages(HtmlMessagesRenderer *renderer)
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

	text += QString("<script>%1</script>").arg(jsCode);

	Contact contact = renderer->chat().contacts().count() == 1 ? *(renderer->chat().contacts().constBegin()) : Contact();
	text += Parser::parse(CurrentChatSyntax.top(), Talkable(contact), true);

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

QString KaduChatStyleEngine::scriptsAtEnd(const QString &html)
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

void KaduChatStyleEngine::configurationUpdated()
{
	QString chatSyntax = SyntaxList::readSyntax("chat", CurrentStyleName, QString());
	if (CurrentChatSyntax.syntax() != chatSyntax)
		loadStyle(CurrentStyleName, QString());
}

void KaduChatStyleEngine::prepareStylePreview(Preview *preview, QString styleName, QString variantName)
{
	Q_UNUSED(variantName)

	KaduChatSyntax syntax(SyntaxList::readSyntax("chat", styleName, QString()));

	QString text = Parser::parse(syntax.top(), Talkable(), true);

	int count = preview->messages().count();
	if (count)
	{
		auto previous = Message::null;
		for (int i = 0; i < count; i++)
		{
			auto message = preview->messages().at(i);
			auto info = Core::instance()->messageRenderInfoFactory()->messageRenderInfo(previous, message);
			previous = message;

			text += Parser::parse(syntax.withHeader(), Talkable(message.messageSender()), &info);
		}
	}
	preview->webView()->setHtml(QString("<html><head><style type='text/css'>%1</style></head><body>%2</body>").arg(ChatStylesManager::instance()->mainStyle(), text));
}

#include "moc_chat-engine-kadu.cpp"
