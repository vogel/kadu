/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QFileInfo>
#include <QtWebKit/QWebFrame>

#include "accounts/account-manager.h"
#include "chat/chat-message.h"
#include "chat/chat-styles-manager.h"
#include "chat/html-messages-renderer.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/preview.h"
#include "gui/windows/syntax-editor-window.h"
#include "misc/syntax-list.h"
#include "parser/parser.h"

#include "chat-engine-kadu.h"

KaduChatStyleEngine::KaduChatStyleEngine()
{
	EngineName = "Kadu";
	syntaxList = new SyntaxList("chat");
}

KaduChatStyleEngine::~KaduChatStyleEngine()
{
	if (syntaxList)
	{
		delete syntaxList;
		syntaxList = 0;
	}
}

void KaduChatStyleEngine::clearMessages(HtmlMessagesRenderer *renderer)
{
	renderer->webPage()->mainFrame()->setHtml("<body bgcolor=\"" + config_file.readColorEntry("Look", "ChatBgColor").name() + "\"></body>");
}

void KaduChatStyleEngine::appendMessages(HtmlMessagesRenderer *renderer, QList<ChatMessage *> messages)
{
	repaintMessages(renderer);
}

void KaduChatStyleEngine::appendMessage(HtmlMessagesRenderer *renderer, ChatMessage *message)
{
	repaintMessages(renderer);
}

void KaduChatStyleEngine::refreshView(HtmlMessagesRenderer *renderer)
{
	repaintMessages(renderer);
}

QString KaduChatStyleEngine::isThemeValid(QString stylePath)
{
	QFileInfo fi;
	fi.setFile(stylePath);
	return fi.suffix() == "syntax" ? fi.completeBaseName() : QString::null;
}

void KaduChatStyleEngine::loadTheme(const QString &styleName, const QString &variantName)
{
	QString chatSyntax = SyntaxList::readSyntax("chat", styleName,
		"<p style=\"background-color: #{backgroundColor};\">#{separator}"
		  "<font color=\"#{fontColor}\"><kadu:header><b><font color=\"#{nickColor}\">%a</font> :: "
			"#{receivedDate}[ / S #{sentDate}]</b><br /></kadu:header>"
		"#{message}</font></p>"
	);
	int beginOfHeader = chatSyntax.indexOf("<kadu:header>");
	int endOfHeader = chatSyntax.indexOf("</kadu:header>");
	ChatSyntaxWithHeader = chatSyntax;
	ChatSyntaxWithHeader.replace("<kadu:header>", "");
	ChatSyntaxWithHeader.replace("</kadu:header>", "");

	if (endOfHeader != -1)
		ChatSyntaxWithoutHeader = chatSyntax.mid(0, beginOfHeader) + chatSyntax.mid(endOfHeader + strlen("</kadu:header>"));
	else
		ChatSyntaxWithoutHeader = ChatSyntaxWithHeader;
	CurrentStyleName = styleName;
}

QString KaduChatStyleEngine::formatMessage(ChatMessage *message, ChatMessage *after)
{
	int separatorSize;
	QString format;
	bool includeHeader;

	Contact contact = message->sender();
	Account *account = message->chat()->account();

	if (message->type() == TypeSystem)
	{
		separatorSize = ChatStylesManager::instance()->paragraphSeparator();
		format = ChatSyntaxWithoutHeader;

		message->setSeparatorSize(separatorSize);
		return Parser::parse(format, account, contact, message, true);
	}
	else
	{
		includeHeader = (!ChatStylesManager::instance()->cfgNoHeaderRepeat() || !after);

		if (after && !includeHeader)
			includeHeader =
				(after->type() != TypeSystem) &&
				((message->date().toTime_t() - after->date().toTime_t() > (ChatStylesManager::instance()->cfgNoHeaderInterval() * 60)) ||
				 (message->sender() != after->sender()));

		if (includeHeader)
		{
			separatorSize = ChatStylesManager::instance()->cfgHeaderSeparatorHeight();
			format = ChatSyntaxWithHeader;
		}
		else
		{
			separatorSize = ChatStylesManager::instance()->paragraphSeparator();
			format = ChatSyntaxWithoutHeader;
		}

		message->setShowServerTime(ChatStylesManager::instance()->noServerTime(), ChatStylesManager::instance()->noServerTimeDiff());
		message->setSeparatorSize(separatorSize);
		
		return Parser::parse(format, account, contact, message, true);
	}
}

void KaduChatStyleEngine::repaintMessages(HtmlMessagesRenderer *renderer)
{
	QString text = QString(
		"<html>"
		"	<head>"
		"		<style type='text/css'>") +
		ChatStylesManager::instance()->mainStyle() +
		"		</style>"
		"	</head>"
		"	<body>";

	QList<ChatMessage *>::const_iterator message = renderer->messages().constBegin();
	QList<ChatMessage *>::const_iterator prevMessage;
	QList<ChatMessage *>::const_iterator end = renderer->messages().constEnd();

	if (message != end)
	{
		(*message)->setSeparatorSize(0);
		Contact contact = (*message)->sender();
		Account *account = (*message)->chat()->account();

		if ((*message)->type() == TypeSystem)
			text += Parser::parse(ChatSyntaxWithoutHeader, account, contact, *message);
		else
		{
			(*message)->setShowServerTime(ChatStylesManager::instance()->noServerTime(), ChatStylesManager::instance()->noServerTimeDiff());
			text += Parser::parse(ChatSyntaxWithHeader, account, contact, *message);
		}

		prevMessage = message;
		while (++message != end)
		{
			text += QString("[%1]" ).arg((*message)->message().status()) + formatMessage(*message, *prevMessage);
			prevMessage = message;
		}
	}

	text += "</body></html>";

	renderer->webPage()->mainFrame()->setHtml(text);
}

void KaduChatStyleEngine::configurationUpdated()
{
	QString chatSyntax = SyntaxList::readSyntax("chat", CurrentStyleName, "");
	if (ChatSyntaxWithHeader != chatSyntax)
		loadTheme(CurrentStyleName, "");
}

void KaduChatStyleEngine::prepareStylePreview(Preview *preview, QString styleName, QString variantName)
{
	QString content = SyntaxList::readSyntax("chat", styleName, "");

	content.replace(QRegExp("%o"),  " ");
	content.replace("<kadu:header>", "");
	content.replace("</kadu:header>", "");

	int count = preview->getObjectsToParse().count();
	QString text;
	if (count)
	{
		ChatMessage *message;
		for (int i = 0; i < count; i++)
		{
			message = dynamic_cast<ChatMessage *>(preview->getObjectsToParse().at(i));
			text += Parser::parse(content, message->chat()->account(),
				message->sender(), message);
		}
	}
	preview->setHtml(QString("<html><head><style type='text/css'>%1</style></head><body>%2</body>").arg(ChatStylesManager::instance()->mainStyle(), text));
}

void KaduChatStyleEngine::styleEditionRequested(QString styleName)
{
	QString syntaxHint = QT_TRANSLATE_NOOP
	(
		"@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - first name\n"
		"%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n"
		"%h - gg version, %v - revDNS, %p - port, %e - email, %x - max image size,\n"
		"#{message} - message content,\n"
		"#{backgroundColor} - background color of message,\n"
		"#{fontColor} - font color of message,\n"
		"#{nickColor} - font color of nick,\n"
		"#{sentDate} - when message was sent,\n"
		"#{receivedDate} - when message was received,\n"
		"#{separator} - separator between messages,\n"
		"<kadu:header>text</kadu:header> - text will not be displayed in 'Remove repeated headers' mode\n"
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
	syntax.replace("<kadu:header>", "");
	syntax.replace("</kadu:header>", "");
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
	return syntaxList && syntaxList->deleteSyntax(styleName);
}

void KaduChatStyleEngine::syntaxAdded(const QString &syntaxName)
{
	ChatStylesManager::instance()->addStyle(syntaxName, this);
}
