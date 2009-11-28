/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtWebKit/QWebFrame>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/avatar.h"
#include "buddies/buddy-set.h"
#include "chat/chat-styles-manager.h"
#include "chat/html-messages-renderer.h"
#include "chat/message/message-render-info.h"
#include "contacts/contact.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/preview.h"
#include "parser/parser.h"
#include "protocols/protocol-factory.h"
#include "misc/misc.h"
#include "adium-time-formatter.h"

#include "chat-engine-adium.h"

const char *AdiumChatStyleEngine::xhtmlBase =
	"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\"\n"
		"\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n"
		"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
    	"<head>\n"
        "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\n\" />\n"
        "<base href=\"file://%1\">\n"
		"<style id=\"KaduStyle\" type=\"text/css\" >\n"
		"	%5\n"
		"</style>\n"
		"<style id=\"baseStyle\" type=\"text/css\" >\n"
		"	@import url(\"main.css\");\n"
		"	*{ word-wrap:break-word; }\n"
		"</style>\n"
		"<style id=\"mainStyle\" type=\"text/css\" >\n"
		"	@import url(\"%4\");\n"
        "</style>\n"
		"</head>\n"
		"<body>\n"
		"%2\n"
		"<div id=\"Chat\"></div>\n"
		"%3\n"
		"</body>"
		"</html>\n";

AdiumChatStyleEngine::AdiumChatStyleEngine()
{
	timeFormatter = new AdiumTimeFormatter;
	// Load required javascript funtions
	QFile file(dataPath("kadu") + "/scripts/adium-style-scripts.js");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		jsCode = file.readAll();

}

AdiumChatStyleEngine::~AdiumChatStyleEngine()
{
	delete timeFormatter;
}

void AdiumChatStyleEngine::pruneMessage(HtmlMessagesRenderer *renderer)
{
	if (!ChatStylesManager::instance()->cfgNoHeaderRepeat())
		renderer->webPage()->mainFrame()->evaluateJavaScript("kadu_removeFirstMessage()");
}

void AdiumChatStyleEngine::clearMessages(HtmlMessagesRenderer *renderer)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript("kadu_clearMessages()");
}

QString AdiumChatStyleEngine::isThemeValid(QString stylePath)
{
	// Minimal Adium style layout
	QDir dir(stylePath);
	if (!dir.cd("Contents/Resources/"))
		return QString::null;

	QFileInfo fi(dir, "Incoming/Content.html");	
	if (!fi.isReadable())
		return QString::null;
	
	fi.setFile(dir, "main.css");
	if (!fi.isReadable())
		return QString::null;

	fi.setFile(dir, "Status.html");
	if (!fi.isReadable())
		return QString::null;
	
	dir.setPath(stylePath);
	return dir.dirName();
}

QStringList AdiumChatStyleEngine::styleVariants(QString styleName)
{
	QDir dir;
	QString styleBaseHref = ggPath() + "/syntax/chat/" + styleName + "/Contents/Resources/Variants/";
	if (!dir.exists(styleBaseHref))
		styleBaseHref = dataPath("kadu") + "/syntax/chat/" + styleName + "/Contents/Resources/Variants/";
	dir.setPath(styleBaseHref);
	dir.setNameFilters(QStringList("*.css"));
	return dir.entryList(); 
}

void AdiumChatStyleEngine::appendMessages(HtmlMessagesRenderer *renderer, QList<MessageRenderInfo *> messages)
{
	foreach (MessageRenderInfo *message, messages)
		appendMessage(renderer, message);
}

void AdiumChatStyleEngine::appendMessage(HtmlMessagesRenderer *renderer, MessageRenderInfo *message)
{
	QString formattedMessageHtml;
	bool includeHeader = true;

	MessageRenderInfo *lastMessage = ChatStylesManager::instance()->cfgNoHeaderRepeat()
			? renderer->lastMessage()
			: 0;
			
	Message msg = message->message();

	if (lastMessage)
	{
		Message last = lastMessage->message();

		includeHeader =
			(last.type() != Message::TypeSystem) &&
			((msg.receiveDate().toTime_t() - last.receiveDate().toTime_t() > (ChatStylesManager::instance()->cfgNoHeaderInterval() * 60)) ||
			(msg.sender() != last.sender()));
	}
	switch (msg.type())
	{
		case Message::TypeReceived:
		{
			if (includeHeader)
				formattedMessageHtml = IncomingHtml;
			else
				formattedMessageHtml = NextIncomingHtml;
			break;
		}
		case Message::TypeSent:
		{
			if (includeHeader)
				formattedMessageHtml = OutgoingHtml;
			else
				formattedMessageHtml = NextOutgoingHtml;
			break;
		}//TODO 0.6.6:
		case Message::TypeSystem:
		{
			formattedMessageHtml = StatusHtml;
			break;
		}
	}
	
	formattedMessageHtml = replaceKeywords(renderer->chat(), BaseHref, formattedMessageHtml, message);
	formattedMessageHtml.replace("\n", " ");
	formattedMessageHtml.replace("'", "\\'");
	formattedMessageHtml.prepend("<span>");
	formattedMessageHtml.append("</span>");

	if (includeHeader)
		renderer->webPage()->mainFrame()->evaluateJavaScript("kadu_appendNextMessage(\'"+ formattedMessageHtml +"\')");
	else
		renderer->webPage()->mainFrame()->evaluateJavaScript("kadu_appendConsecutiveMessage(\'"+ formattedMessageHtml +"\')");

	renderer->setLastMessage(message);
}

void AdiumChatStyleEngine::refreshView(HtmlMessagesRenderer *renderer)
{
	QString styleBaseHtml = QString(xhtmlBase).arg(BaseHref)
			.arg(replaceKeywords(renderer->chat(), BaseHref, HeaderHtml))
			.arg(replaceKeywords(renderer->chat(), BaseHref, FooterHtml))
			.arg("Variants/" + StyleVariantName)
			.arg(ChatStylesManager::instance()->mainStyle());
	renderer->webPage()->mainFrame()->setHtml(styleBaseHtml);
	renderer->webPage()->mainFrame()->evaluateJavaScript(jsCode);

	foreach (MessageRenderInfo *message, renderer->messages())
		appendMessage(renderer, message);
}

QString AdiumChatStyleEngine::readThemePart(QString part)
{
	QFile fileAccess;
	QString resultHtml = QString::null;
	if (QFile::exists(part))
	{

		fileAccess.setFileName(part);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream stream(&fileAccess);
		stream.setCodec(QTextCodec::codecForName("UTF-8"));
		resultHtml = stream.readAll();

		fileAccess.close();
	}
	return resultHtml;
}

void AdiumChatStyleEngine::loadTheme(const QString &styleName, const QString &variantName)
{
	QDir dir;
	CurrentStyleName = styleName;
	StyleVariantName = variantName;
	BaseHref = ggPath() + "/syntax/chat/" + styleName + "/Contents/Resources/";
	if (!dir.exists(BaseHref))
		BaseHref = dataPath("kadu") + "/syntax/chat/" + styleName + "/Contents/Resources/";

	IncomingHtml = readThemePart(BaseHref + "Incoming/Content.html");

	if (QFile::exists(BaseHref + "Incoming/NextContent.html"))
		NextIncomingHtml = readThemePart(BaseHref + "Incoming/NextContent.html");
	else
		NextIncomingHtml = IncomingHtml;

	if (QFile::exists(BaseHref + "Outgoing/Content.html"))
		OutgoingHtml = readThemePart(BaseHref + "Outgoing/Content.html");
	else
		OutgoingHtml = IncomingHtml;

	if (QFile::exists(BaseHref + "Outgoing/NextContent.html"))
		NextOutgoingHtml = readThemePart(BaseHref + "Outgoing/NextContent.html");
	else
		NextOutgoingHtml = OutgoingHtml;

	HeaderHtml = readThemePart(BaseHref + "Header.html");
	FooterHtml = readThemePart(BaseHref + "Footer.html");

	StatusHtml = readThemePart(BaseHref + "Status.html");
}

bool AdiumChatStyleEngine::clearDirectory(const QString &directory)
{
	QDir dir(directory);
	QFileInfo fi;

	foreach (const QString &fileName, dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden))
	{
		fi.setFile(dir, fileName);
		if (!fi.isDir())
		{
			if (!dir.remove(fileName))
				return false;
		}
		else
		{
			if (!clearDirectory(QString(directory + "/" + fileName)) || !dir.rmdir(fileName))
				return false;
				
		}	
	}
	return dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden).isEmpty();
}

bool AdiumChatStyleEngine::removeStyle(const QString &styleName)
{
	QDir dir(QString(ggPath() + "/syntax/chat/" + styleName));
	return clearDirectory(dir.absolutePath()) && dir.cdUp() && dir.rmdir(styleName);
}

void AdiumChatStyleEngine::prepareStylePreview(Preview *preview, QString styleName, QString variantName)
{
	QDir dir;
	QString styleHref = ggPath() + "/syntax/chat/" + styleName + "/Contents/Resources/";
	if (!dir.exists(styleHref))
		styleHref = dataPath("kadu") + "/syntax/chat/" + styleName + "/Contents/Resources/";

	QString incomingHtml = readThemePart(styleHref + "Incoming/Content.html");
	QString outgoingHtml;
	if (QFile::exists(styleHref + "Outgoing/Content.html"))
		outgoingHtml = readThemePart(styleHref + "Outgoing/Content.html");
	else
		outgoingHtml = incomingHtml;

	QString headerHtml = readThemePart(styleHref + "Header.html");
	QString footerHtml = readThemePart(styleHref + "Footer.html");
	if (preview->getObjectsToParse().count() != 2)
		return;

	MessageRenderInfo *message = dynamic_cast<MessageRenderInfo *>(preview->getObjectsToParse().at(0));
	if (!message)
		return;
	Message msg = message->message();

	QString styleBaseHtml = QString(xhtmlBase).arg(styleHref)
		.arg(replaceKeywords(msg.chat(), styleHref, headerHtml))
		.arg(replaceKeywords(msg.chat(), styleHref, footerHtml))
		.arg("Variants/" + variantName)
		.arg(ChatStylesManager::instance()->mainStyle());
	preview->setHtml(styleBaseHtml);

	preview->page()->mainFrame()->evaluateJavaScript(jsCode);

	incomingHtml = replaceKeywords(msg.chat(), styleHref, incomingHtml, message);
	incomingHtml.replace("\n", " ");
	incomingHtml.prepend("<span>");
	incomingHtml.append("</span>");
	preview->page()->mainFrame()->evaluateJavaScript("kadu_appendNextMessage(\'"+ incomingHtml +"\')");

	message = dynamic_cast<MessageRenderInfo *>(preview->getObjectsToParse().at(1));
	outgoingHtml = replaceKeywords(msg.chat(), styleHref, outgoingHtml, message);
	outgoingHtml.replace("\n", " ");
	outgoingHtml.prepend("<span>");
	outgoingHtml.append("</span>");
	preview->page()->mainFrame()->evaluateJavaScript("kadu_appendNextMessage(\'"+ outgoingHtml +"\')");

}

// Some parts of the code below are borrowed from Kopete project (http://kopete.kde.org/)
QString AdiumChatStyleEngine::replaceKeywords(Chat chat, QString &styleHref, QString &style)
{
    	if (!chat)
		return QString("");

	QString result = style;
	QString name;

//TODO: get Chat name (contacts' nicks?)
	if (chat)
	{
		// Replace %chatName% //TODO. Find way to dynamic update this tag (add id ?)
		int uinsSize = chat.buddies().count();
		int i = 0;

		foreach (const Buddy &buddy, chat.buddies())
		{
			name.append(buddy.display());

			if (++i < uinsSize)
				name.append(", ");
		}
	}
	result.replace(QString("%chatName%"), name);
	// Replace %sourceName%
	result.replace(QString("%sourceName%"), chat.chatAccount().name());
	// Replace %destinationName%
	result.replace(QString("%destinationName%"), name);
	// For %timeOpened%, display the date and time. TODO: get real time 
	result.replace(QString("%timeOpened%"), printDateTime(QDateTime::currentDateTime()));

	//TODO 0.6.6: get real time!!!
	QRegExp timeRegExp("%timeOpened\\{([^}]*)\\}%");
	int pos=0;
	while ((pos=timeRegExp.indexIn(result, pos)) != -1)
		result.replace(pos, timeRegExp.cap(0).length(), timeFormatter->convertTimeDate(timeRegExp.cap(1), QDateTime::currentDateTime()));

	QString photoIncoming;
	QString photoOutgoing;

	if (chat.buddies().count() > 1)
		photoIncoming = QString("file://") + styleHref + QString("Incoming/buddy_icon.png");
	else if (chat.buddies().count() == 1)
	{
		Contact contact = chat.buddies().toContactList(chat.chatAccount()).first();
		if (!contact.isNull() && !contact.contactAvatar().pixmap().isNull())
			photoIncoming = QString("file://") + contact.contactAvatar().filePath();
		else
			photoIncoming = QString("file://") + styleHref + QString("Incoming/buddy_icon.png");
	}

	photoOutgoing = QString("file://") + styleHref + QString("Outgoing/buddy_icon.png");

	result.replace(QString("%incomingIconPath%"), photoIncoming);
	result.replace(QString("%outgoingIconPath%"), photoOutgoing);

	return result;
}

QString AdiumChatStyleEngine::replaceKeywords(Chat chat, QString &styleHref, QString &source, MessageRenderInfo *message)
{
	if (!chat)
		return QString("");

	QString result = source;
	QString nick, contactId, service, protocolIcon, nickLink;

	Message msg = message->message();

	// Replace sender (contact nick)
	result.replace(QString("%sender%"), msg.sender().display());
	// Replace %screenName% (contact ID)
	result.replace(QString("%senderScreenName%"), msg.sender().id(chat.chatAccount()));
	// Replace service name (protocol name)
	result.replace(QString("%service%"), chat.chatAccount().protocolHandler()->protocolFactory()->displayName());
	// Replace protocolIcon (sender statusIcon). TODO:
	result.replace(QString("%senderStatusIcon%"), chat.chatAccount().protocolHandler()->protocolFactory()->iconName());

	// Replace time
	QDateTime time = msg.sendDate().isNull() ? msg.receiveDate(): msg.sendDate();
	result.replace(QString("%time%"), printDateTime(time));
	// Look for %time{X}%
	QRegExp timeRegExp("%time\\{([^}]*)\\}%");
	int pos = 0;
	while ((pos = timeRegExp.indexIn(result , pos)) != -1)
		result.replace(pos, timeRegExp.cap(0).length(), timeFormatter->convertTimeDate(timeRegExp.cap(1), time));

	// Look for %textbackgroundcolor{X}%
	// TODO: highlight background color: use the X value.
	QRegExp textBackgroundRegExp("%textbackgroundcolor\\{([^}]*)\\}%");
	int textPos = 0;
	while ((textPos=textBackgroundRegExp.indexIn(result, textPos)) != -1)
		result.replace(textPos, textBackgroundRegExp.cap(0).length(), "inherit");

	// Replace userIconPath
	QString photoPath;
	if (msg.type() == Message::TypeReceived)
	{
		result.replace(QString("%messageClasses%"), "message incoming");

		Contact contact = msg.sender().contact(chat.chatAccount());
		if (!contact.isNull() && !contact.contactAvatar().pixmap().isNull())
			photoPath = QString("file://") + contact.contactAvatar().filePath();
		else
			photoPath = QString("file://") + styleHref + QString("Incoming/buddy_icon.png");
	}
	else
	{
   		result.replace(QString("%messageClasses%"), "message outgoing");
		photoPath = QString("file://") + styleHref + QString("Outgoing/buddy_icon.png");
	}
	result.replace(QString("%userIconPath%"), photoPath);

	//Message direction ("rtl"(Right-To-Left) or "ltr"(Left-to-right))
	result.replace(QString("%messageDirection%"), "ltr");

	// Replace contact's color
	const QString colorName = message->nickColor();
	QString lightColorName;
	QRegExp senderColorRegExp("%senderColor(?:\\{([^}]*)\\})?%");
	textPos = 0;
	while((textPos = senderColorRegExp.indexIn(result, textPos)) != -1)
	{
		int light = 100;
		bool doLight = false;
		if(senderColorRegExp.numCaptures()>=1)
			light = senderColorRegExp.cap(1).toUInt(&doLight);

		if (doLight && lightColorName.isNull())
			lightColorName = QColor(colorName).light(light).name();

		result.replace(textPos ,senderColorRegExp.cap(0).length(), doLight ? lightColorName : colorName);
	}

	// Replace message TODO: do sth with formatMessage
	QString messageText = QString("<span>") + formatMessage(message->htmlMessageContent(), message->backgroundColor()) + QString("</span>");
	result.replace(QString("%message%"), messageText);

	return result;
}

