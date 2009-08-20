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
#include "chat/chat-message.h"
#include "chat/chat-styles-manager.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/preview.h"
#include "parser/parser.h"
#include "protocols/protocol-factory.h"
#include "misc/misc.h"

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

void AdiumChatStyleEngine::pruneMessage(ChatMessagesView *view)
{
	if (!ChatStylesManager::instance()->cfgNoHeaderRepeat())
		view->page()->mainFrame()->evaluateJavaScript("kadu_removeFirstMessage()");	
}

void AdiumChatStyleEngine::clearMessages(ChatMessagesView *view)
{
	view->page()->mainFrame()->evaluateJavaScript("kadu_clearMessages()");
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

void AdiumChatStyleEngine::appendMessages(ChatMessagesView *view, QList<ChatMessage *> messages)
{
	foreach (ChatMessage *message, messages)
		appendMessage(view, message);
}

void AdiumChatStyleEngine::appendMessage(ChatMessagesView *view, ChatMessage *message)
{
	QString formattedMessageHtml;
	bool includeHeader = true;

	if (ChatStylesManager::instance()->cfgNoHeaderRepeat() && view->prevMessage())
	{
		includeHeader =
			(view->prevMessage()->type() != TypeSystem) &&
			((message->date().toTime_t() - view->prevMessage()->date().toTime_t() > (ChatStylesManager::instance()->cfgNoHeaderInterval() * 60)) ||
			 (message->sender() != view->prevMessage()->sender()));
	}
	switch (message->type())
	{
		case TypeReceived:
		{
			if (includeHeader)
				formattedMessageHtml = IncomingHtml;
			else
				formattedMessageHtml = NextIncomingHtml;
			break;
		}
		case TypeSent:
		{
			if (includeHeader)
				formattedMessageHtml = OutgoingHtml;
			else
				formattedMessageHtml = NextOutgoingHtml;
			break;
		}//TODO 0.6.6:
		case TypeSystem:
		{
			formattedMessageHtml = StatusHtml;
			break;
		}
	}
	
	formattedMessageHtml = replaceKeywords(view->chat(), BaseHref, formattedMessageHtml, message);
	formattedMessageHtml.replace("\n", " ");
	formattedMessageHtml.prepend("<span>");
	formattedMessageHtml.append("</span>");

	if (includeHeader)
		view->page()->mainFrame()->evaluateJavaScript("kadu_appendNextMessage(\'"+ formattedMessageHtml +"\')");
	else
		view->page()->mainFrame()->evaluateJavaScript("kadu_appendConsecutiveMessage(\'"+ formattedMessageHtml +"\')");

	view->prevMessage() = message;
}

void AdiumChatStyleEngine::refreshView(ChatMessagesView *view)
{
	QString styleBaseHtml = QString(xhtmlBase).arg(BaseHref)
		.arg(replaceKeywords(view->chat(), BaseHref, HeaderHtml))
		.arg(replaceKeywords(view->chat(), BaseHref, FooterHtml))
		.arg("Variants/" + StyleVariantName)
		.arg(ChatStylesManager::instance()->mainStyle());
	view->setHtml(styleBaseHtml);

	view->page()->mainFrame()->evaluateJavaScript(jsCode);

	foreach (ChatMessage *message, view->messages())
		appendMessage(view, message);


}

AdiumChatStyleEngine::AdiumChatStyleEngine()
{
	// Load required javascript funtions
	QFile file(dataPath("kadu") + "/scripts/adium-style-scripts.js");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		jsCode = file.readAll();

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

	ChatMessage *message = dynamic_cast<ChatMessage *>(preview->getObjectsToParse().at(0));

	QString styleBaseHtml = QString(xhtmlBase).arg(styleHref)
		.arg(replaceKeywords(message->chat(), styleHref, headerHtml))
		.arg(replaceKeywords(message->chat(), styleHref, footerHtml))
		.arg("Variants/" + variantName)
		.arg(ChatStylesManager::instance()->mainStyle());
	preview->setHtml(styleBaseHtml);

	preview->page()->mainFrame()->evaluateJavaScript(jsCode);

	incomingHtml = replaceKeywords(message->chat(), styleHref, incomingHtml, message);
	incomingHtml.replace("\n", " ");
	incomingHtml.prepend("<span>");
	incomingHtml.append("</span>");
	preview->page()->mainFrame()->evaluateJavaScript("kadu_appendNextMessage(\'"+ incomingHtml +"\')");

	message = dynamic_cast<ChatMessage *>(preview->getObjectsToParse().at(1));
	outgoingHtml = replaceKeywords(message->chat(), styleHref, outgoingHtml, message);
	outgoingHtml.replace("\n", " ");
	outgoingHtml.prepend("<span>");
	outgoingHtml.append("</span>");
	preview->page()->mainFrame()->evaluateJavaScript("kadu_appendNextMessage(\'"+ outgoingHtml +"\')");

}

// Some parts of the code below are borrowed from Kopete project (http://kopete.kde.org/)
QString AdiumChatStyleEngine::replaceKeywords(Chat *chat, QString &styleHref, QString &style)
{
	QString result = style;
	QString name;

//TODO: get Chat name (contacts' nicks?)
	if (chat)
	{
		// Replace %chatName% //TODO. Find way to dynamic update this tag (add id ?)
		int uinsSize = chat->contacts().count();
		int i = 0;

		foreach (const Contact &contact, chat->contacts())
		{
			name.append(contact.display());

			if (++i < uinsSize)
				name.append(", ");
		}
	}
	result.replace(QString("%chatName%"), name);
	// Replace %sourceName%
	result.replace(QString("%sourceName%"), chat->account()->name());
	// Replace %destinationName%
	result.replace(QString("%destinationName%"), name);
	// For %timeOpened%, display the date and time. TODO: get real time 
	result.replace(QString("%timeOpened%"), printDateTime(QDateTime::currentDateTime()));

	// Look for %timeOpened{X}% TODO: get real time
	QRegExp timeRegExp("%timeOpened\\{([^}]*)\\}%");
	int pos=0;
	while ((pos=timeRegExp.indexIn(result, pos)) != -1)
		result.replace(pos, timeRegExp.cap(0).length(), QDateTime::currentDateTime().toString(timeRegExp.cap(1)));
	// Get avatars. TODO: We don't support avatars at this time
	QString photoIncoming;
	QString photoOutgoing;

	photoIncoming = QString("file://") + styleHref + QString("Incoming/buddy_icon.png");
	photoOutgoing = QString("file://") + styleHref + QString("Outgoing/buddy_icon.png");

	result.replace(QString("%incomingIconPath%"), photoIncoming);
	result.replace(QString("%outgoingIconPath%"), photoOutgoing);

	return result;
}
QString AdiumChatStyleEngine::replaceKeywords(Chat *chat, QString &styleHref, QString &source, ChatMessage *message)
{
	QString result = source;
	QString nick, contactId, service, protocolIcon, nickLink;

	// Replace sender (contact nick)
	result.replace(QString("%sender%"), message->sender().display());
	// Replace %screenName% (contact ID)
	result.replace(QString("%senderScreenName%"), message->sender().id(chat->account()));
	// Replace service name (protocol name)
	result.replace(QString("%service%"), chat->account()->protocol()->protocolFactory()->displayName());
	// Replace protocolIcon (sender statusIcon). TODO:
	result.replace(QString("%senderStatusIcon%"), chat->account()->protocol()->protocolFactory()->iconName());

	// Replace time
	QDateTime time = message->sdate().isNull() ? message->date(): message->sdate();
	result.replace(QString("%time%"), printDateTime(time));
	// Look for %time{X}%
	QRegExp timeRegExp("%time\\{([^}]*)\\}%");
	int pos = 0;
	while ((pos = timeRegExp.indexIn(result , pos)) != -1)
		result.replace(pos, timeRegExp.cap(0).length(), time.toString(timeRegExp.cap(1)));

	// Look for %textbackgroundcolor{X}%
	// TODO: highlight background color: use the X value.
	QRegExp textBackgroundRegExp("%textbackgroundcolor\\{([^}]*)\\}%");
	int textPos = 0;
	while ((textPos=textBackgroundRegExp.indexIn(result, textPos)) != -1)
		result.replace(textPos, textBackgroundRegExp.cap(0).length(), "inherit");

	// Replace userIconPath TODO: add avatars support
	QString photoPath;
	if (message->type() == TypeReceived)
	{
   		result.replace(QString("%messageClasses%"), "message incoming");
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
	const QString colorName = message->nickColor;
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
	QString messageText = QString("<span>") + formatMessage(message->unformattedMessage, message->backgroundColor) + QString("</span>");
	result.replace(QString("%message%"), messageText);

	return result;
}

