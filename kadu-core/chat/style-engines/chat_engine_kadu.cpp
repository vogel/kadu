/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QFileInfo>

#include "accounts/account_manager.h"

#include "chat/chat_message.h"
#include "chat/chat_styles_manager.h"

#include "gui/widgets/chat_messages_view.h"
#include "gui/widgets/preview.h"

#include "config_file.h"
#include "kadu_parser.h"
#include "syntax_editor.h"

#include "chat_engine_kadu.h"

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

void KaduChatStyleEngine::clearMessages(ChatMessagesView *view)
{
	view->setHtml("<body bgcolor=\"" + config_file.readColorEntry("Look", "ChatBgColor").name() + "\"></body>");
}

void KaduChatStyleEngine::appendMessages(ChatMessagesView *view, QList<ChatMessage *> messages)
{
	repaintMessages(view);
}
void KaduChatStyleEngine::appendMessage(ChatMessagesView *view, ChatMessage *message)
{
	repaintMessages(view);
}

void KaduChatStyleEngine::refreshView(ChatMessagesView *view)
{
	repaintMessages(view);
}

bool KaduChatStyleEngine::isThemeValid(QString stylePath)
{
	QFileInfo fi;
	fi.setFile(stylePath);
	return fi.suffix() == "syntax";
}

void KaduChatStyleEngine::loadTheme(const QString &styleName)
{
	QString chatSyntax = SyntaxList::readSyntax("chat", styleName,
		"<p style=\"background-color: #{backgroundColor};\">#{separator}"
		  "<font color=\"#{fontColor}\"><kadu:header><b><font color=\"#{nickColor}\">%a</font> :: "
			"#{receivedDate}[ / S #{sentDate}]</b><br /></kadu:header>"
		"#{message}</font></p>"
	);
	int beginOfHeader = chatSyntax.find("<kadu:header>");
	int endOfHeader = chatSyntax.find("</kadu:header>");
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
	Account *account = message->account();

	if (message->type() == TypeSystem)
	{
		separatorSize = ChatStylesManager::instance()->paragraphSeparator();
		format = ChatSyntaxWithoutHeader;

		message->setSeparatorSize(separatorSize);
		return KaduParser::parse(format, account, contact, message, true);
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
		
		return KaduParser::parse(format, account, contact, message, true);
	}
}

void KaduChatStyleEngine::repaintMessages(ChatMessagesView *view)
{

	QString text = QString(
		"<html>"
		"	<head>"
		"		<style type='text/css'>") +
		ChatStylesManager::instance()->mainStyle() +
		"		</style>"
		"	</head>"
		"	<body>";

	QList<ChatMessage *>::const_iterator message = view->messages().constBegin();
	QList<ChatMessage *>::const_iterator prevMessage;
	QList<ChatMessage *>::const_iterator end = view->messages().constEnd();

	if (message != end)
	{
		(*message)->setSeparatorSize(0);
		Contact contact = (*message)->sender();
		Account *account = (*message)->account();

		if ((*message)->type() == TypeSystem)
			text += KaduParser::parse(ChatSyntaxWithoutHeader, account, contact, *message);
		else
		{
			(*message)->setShowServerTime(ChatStylesManager::instance()->noServerTime(), ChatStylesManager::instance()->noServerTimeDiff());
			text += KaduParser::parse(ChatSyntaxWithHeader, account, contact, *message);
		}

		prevMessage = message;
		while (++message != end)
		{
			text += formatMessage(*message, *prevMessage);
			prevMessage = message;
		}
	}

	view->rememberScrollBarPosition();

	text += "</body></html>";

	view->setHtml(text);
}

void KaduChatStyleEngine::configurationUpdated()
{
	QString chatSyntax = SyntaxList::readSyntax("chat", CurrentStyleName, "");
	if (ChatSyntaxWithHeader != chatSyntax)
		loadTheme(CurrentStyleName);
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
		for (int i = 0; i < count; i++)
			text += KaduParser::parse(content, AccountManager::instance()->defaultAccount(),
				preview->getContactList().at(i), preview->getObjectsToParse().at(i));
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
	ChatStylesManager::instance()->preparaPreview(editor->preview());
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
