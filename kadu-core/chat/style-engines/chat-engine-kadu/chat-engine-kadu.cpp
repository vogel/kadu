/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "chat/message/message-render-info.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/preview.h"
#include "gui/windows/syntax-editor-window.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "misc/syntax-list.h"
#include "parser/parser.h"

#include "chat-engine-kadu.h"

KaduChatStyleEngine::KaduChatStyleEngine(QObject *parent) :
	QObject(parent)
{
	EngineName = "Kadu";
	syntaxList = QSharedPointer<SyntaxList>(new SyntaxList("chat"));
	QFile file(dataPath("kadu") + "/scripts/chat-scripts.js");
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

void KaduChatStyleEngine::appendMessages(HtmlMessagesRenderer *renderer, const QList<MessageRenderInfo *> &messages)
{
	if (ChatStylesManager::instance()->cfgNoHeaderRepeat() && renderer->pruneEnabled())
	{
		repaintMessages(renderer);
		return;
	}

	foreach (MessageRenderInfo *message, messages)
		appendMessage(renderer, message);
}

void KaduChatStyleEngine::appendMessage(HtmlMessagesRenderer *renderer, MessageRenderInfo *message)
{
	if (ChatStylesManager::instance()->cfgNoHeaderRepeat() && renderer->pruneEnabled())
	{
		repaintMessages(renderer);
		return;
	}

	QString html(replacedNewLine(formatMessage(message, renderer->lastMessage()), QLatin1String(" ")));
	html.replace('\\', QLatin1String("\\\\"));
	html.replace('\'', QLatin1String("\\'"));
	if (!message->message().id().isEmpty())
		html.prepend(QString("<span id=\"message_%1\">").arg(message->message().id()));
	else
		html.prepend("<span>");
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

void KaduChatStyleEngine::contactActivityChanged(HtmlMessagesRenderer *renderer, ChatStateService::ContactActivity state, const QString &message, const QString &name)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript(QString("kadu_contactActivityChanged(%1, \"%2\", \"%3\");").arg((int)state).arg(message).arg(name));
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

QString KaduChatStyleEngine::formatMessage(MessageRenderInfo *message, MessageRenderInfo *after)
{
	int separatorSize;
	QString format;
	bool includeHeader;

	Message msg = message->message();

	if (msg.type() == MessageTypeSystem)
	{
		separatorSize = ChatStylesManager::instance()->paragraphSeparator();
		format = CurrentChatSyntax.withHeader();

		message->setSeparatorSize(separatorSize);

		Contact sender = msg.messageSender();
		return Parser::parse(format, BuddyOrContact(sender), message, true);
	}
	else
	{
		includeHeader = (!ChatStylesManager::instance()->cfgNoHeaderRepeat() || !after);

		if (after && !includeHeader)
		{
			Message aft = after->message();
			includeHeader =
				(aft.type() != MessageTypeSystem) &&
				((msg.receiveDate().toTime_t() - aft.receiveDate().toTime_t() > (ChatStylesManager::instance()->cfgNoHeaderInterval() * 60)) ||
				 (msg.messageSender() != aft.messageSender()));
		}

		if (includeHeader)
		{
			separatorSize = ChatStylesManager::instance()->cfgHeaderSeparatorHeight();
			format = CurrentChatSyntax.withHeader();
		}
		else
		{
			separatorSize = ChatStylesManager::instance()->paragraphSeparator();
			format = CurrentChatSyntax.withoutHeader();
		}

		message->setShowServerTime(ChatStylesManager::instance()->noServerTime(), ChatStylesManager::instance()->noServerTimeDiff());
		message->setSeparatorSize(separatorSize);

		Contact sender = msg.messageSender();
		return Parser::parse(format, BuddyOrContact(sender), message, true);
	}
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
	text += Parser::parse(CurrentChatSyntax.top(), BuddyOrContact(contact), true);

	MessageRenderInfo *prevMessage = 0;
	foreach (MessageRenderInfo *message, renderer->messages())
	{
		QString messageText;
		if (!message->message().id().isEmpty())
			messageText = QString("<span id=\"message_%1\">%2</span>").arg(message->message().id()).arg(formatMessage(message, prevMessage));
		else
			messageText = QString("<span>%1</span>").arg(formatMessage(message, prevMessage));
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

	Contact contact = preview->getContactList().count() == 1 ? *(preview->getContactList().constBegin()) : Contact();
	QString text = Parser::parse(syntax.top(), BuddyOrContact(contact), true);

	int count = preview->getObjectsToParse().count();
	if (count)
	{
		MessageRenderInfo *message;
		for (int i = 0; i < count; i++)
		{
			message = qobject_cast<MessageRenderInfo *>(preview->getObjectsToParse().at(i));
			Contact sender = message->message().messageSender();
			text += Parser::parse(syntax.withHeader(), BuddyOrContact(sender), message);
		}
	}
	preview->setHtml(QString("<html><head><style type='text/css'>%1</style></head><body>%2</body>").arg(ChatStylesManager::instance()->mainStyle(), text));
}

void KaduChatStyleEngine::styleEditionRequested(QString styleName)
{
	QString syntaxHint = qApp->translate(
		"@default", "Syntax:\n"
		"%s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name,"
		" %r - surname, %m - mobile, %u - uin, %g - group,"
		" %h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size, %z - gender (0/1/2),\n"
		"#{message} - message content, #{messageId} - message id, #{messageStatus} - message status value, #{backgroundColor} - background color of message,"
		" #{fontColor} - font color of message, #{nickColor} - font color of nick, #{sentDate} - when message was sent,"
		" #{receivedDate} - when message was received, #{separator} - separator between messages,\n"
		"<kadu:header>...</kadu:header> - content will not be displayed in 'Remove repeated headers' mode,\n"
		"<kadu:top>...</kadu:top> - content will be included once at the begining of the document"
	);

	SyntaxEditorWindow *editor = new SyntaxEditorWindow(syntaxList, styleName, "Chat", syntaxHint);
	connect(editor, SIGNAL(updated(const QString &)), ChatStylesManager::instance(), SLOT(syntaxUpdated(const QString &)));
	connect(editor, SIGNAL(syntaxAdded(const QString &)), this, SLOT(syntaxAdded(const QString &)));
	connect(editor, SIGNAL(isNameValid(const QString &, bool &)), this, SLOT(validateStyleName(const QString &, bool &)));
	ChatStylesManager::instance()->preparePreview(editor->preview());
	connect(editor->preview(), SIGNAL(needSyntaxFixup(QString &)), this, SLOT(chatSyntaxFixup(QString &)));
	connect(editor->preview(), SIGNAL(needFixup(QString &)), this, SLOT(chatFixup(QString &)));
	editor->refreshPreview();
	editor->show();
}

void KaduChatStyleEngine::chatSyntaxFixup(QString &syntax)
{
	syntax.remove("<kadu:header>");
	syntax.remove("</kadu:header>");
}

void KaduChatStyleEngine::chatFixup(QString &syntax)
{
	syntax = QString("<html><head><style type='text/css'>%1</style></head><body>%2</body>").arg(ChatStylesManager::instance()->mainStyle(), syntax);
}

void KaduChatStyleEngine::validateStyleName(const QString &name, bool &valid)
{
	valid = !ChatStylesManager::instance()->hasChatStyle(name) || (ChatStylesManager::instance()->hasChatStyle(name) && syntaxList->contains(name));
}

bool KaduChatStyleEngine::removeStyle(const QString &styleName)
{
	return syntaxList->deleteSyntax(styleName);
}

void KaduChatStyleEngine::syntaxAdded(const QString &syntaxName)
{
	ChatStylesManager::instance()->addStyle(syntaxName, this);
}
