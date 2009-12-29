/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtWebKit/QWebFrame>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/avatar.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "chat/chat-styles-manager.h"
#include "chat/html-messages-renderer.h"
#include "chat/message/message-render-info.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/preview.h"
#include "parser/parser.h"
#include "protocols/protocol-factory.h"
#include "misc/misc.h"

#include "adium-style.h"
#include "adium-time-formatter.h"

#include "chat-engine-adium.h"

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

QString AdiumChatStyleEngine::isStyleValid(QString stylePath)
{
	return AdiumStyle::isStyleValid(stylePath) ? QDir(stylePath).dirName() : QString::null;
}

bool AdiumChatStyleEngine::styleUsesTransparencyByDefault(QString styleName)
{
	//TODO: optimize:
	AdiumStyle style(styleName);
	return style.defaultBackgroundIsTransparent();
}

QString AdiumChatStyleEngine::currentStyleVariant()
{
	return CurrentStyle.currentVariant();
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
			(msg.messageSender() != last.messageSender()));
	}
	switch (msg.type())
	{
		case Message::TypeReceived:
		{
			if (includeHeader)
				formattedMessageHtml = CurrentStyle.incomingHtml();
			else
				formattedMessageHtml = CurrentStyle.nextIncomingHtml();
			break;
		}
		case Message::TypeSent:
		{
			if (includeHeader)
				formattedMessageHtml = CurrentStyle.outgoingHtml();
			else
				formattedMessageHtml = CurrentStyle.nextOutgoingHtml();
			break;
		}//TODO 0.6.6:
		case Message::TypeSystem:
		{
			formattedMessageHtml = CurrentStyle.statusHtml();
			break;
		}
	}
	
	formattedMessageHtml = replaceKeywords(renderer->chat(), CurrentStyle.baseHref(), formattedMessageHtml, message);
	formattedMessageHtml.replace("\n", " ");
	formattedMessageHtml.replace("'", "\\'");
	formattedMessageHtml.prepend("<span>");
	formattedMessageHtml.append("</span>");

	if (includeHeader)
		renderer->webPage()->mainFrame()->evaluateJavaScript("appendMessage(\'"+ formattedMessageHtml +"\')");
	else
		renderer->webPage()->mainFrame()->evaluateJavaScript("appendNextMessage(\'"+ formattedMessageHtml +"\')");

	renderer->setLastMessage(message);
}

void AdiumChatStyleEngine::refreshView(HtmlMessagesRenderer *renderer)
{
	QString styleBaseHtml = CurrentStyle.templateHtml();
	styleBaseHtml.replace(styleBaseHtml.indexOf("%@"), 2, "file://" + CurrentStyle.baseHref());
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(renderer->chat(), CurrentStyle.baseHref(), CurrentStyle.footerHtml()));
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(renderer->chat(), CurrentStyle.baseHref(), CurrentStyle.headerHtml()));
	//TODO: implement style versions:
	if (CurrentStyle.currentVariant() != "Default")
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, "Variants/" + CurrentStyle.currentVariant());
	else
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, "main.css");

	if (styleBaseHtml.contains("%@"))
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, "@import url( \"main.css\" );");

	renderer->webPage()->mainFrame()->setHtml(styleBaseHtml);
	renderer->webPage()->mainFrame()->evaluateJavaScript(jsCode);
	//I don't know why, sometimes 'initStyle' was performed after 'appendMessage'
	renderer->webPage()->mainFrame()->evaluateJavaScript("initStyle()");

	foreach (MessageRenderInfo *message, renderer->messages())
		appendMessage(renderer, message);
}

void AdiumChatStyleEngine::loadStyle(const QString &styleName, const QString &variantName)
{
	CurrentStyle = AdiumStyle(styleName);
	CurrentStyle.setCurrentVariant(variantName);

	CurrentStyleName = styleName;
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
	AdiumStyle style(styleName);
	style.setCurrentVariant(variantName);
	if (preview->getObjectsToParse().count() != 2)
		return;

	MessageRenderInfo *message = dynamic_cast<MessageRenderInfo *>(preview->getObjectsToParse().at(0));
	if (!message)
		return;
	Message msg = message->message();

	QString styleBaseHtml = style.templateHtml();
	styleBaseHtml.replace(styleBaseHtml.indexOf("%@"), 2, "file://" + style.baseHref());
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(msg.messageChat(), style.baseHref(), style.footerHtml()));
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(msg.messageChat(), style.baseHref(), style.headerHtml()));
	//TODO: implement style versions:
	if (style.currentVariant() != "Default")
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, "Variants/" + style.currentVariant());
	else
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, "main.css");

	if (styleBaseHtml.contains("%@"))
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, "@import url( \"main.css\" );");

	preview->page()->mainFrame()->setHtml(styleBaseHtml);
	preview->page()->mainFrame()->evaluateJavaScript(jsCode);
	//I don't know why, sometimes 'initStyle' was performed after 'appendMessage'
	preview->page()->mainFrame()->evaluateJavaScript("initStyle()");

	QString incomingHtml = replaceKeywords(msg.messageChat(), style.baseHref(), style.incomingHtml(), message);
	incomingHtml.replace("\n", " ");
	incomingHtml.replace("'", "\\'");
	incomingHtml.prepend("<span>");
	incomingHtml.append("</span>");
	preview->page()->mainFrame()->evaluateJavaScript("appendMessage(\'" + incomingHtml + "\')");

	message = dynamic_cast<MessageRenderInfo *>(preview->getObjectsToParse().at(1));
	QString outgoingHtml = replaceKeywords(msg.messageChat(), style.baseHref(), style.outgoingHtml(), message);
	outgoingHtml.replace("\n", " ");
	outgoingHtml.replace("'", "\\'");
	outgoingHtml.prepend("<span>");
	outgoingHtml.append("</span>");
	preview->page()->mainFrame()->evaluateJavaScript("appendMessage(\'" + outgoingHtml + "\')");
}

// Some parts of the code below are borrowed from Kopete project (http://kopete.kde.org/)
QString AdiumChatStyleEngine::replaceKeywords(Chat chat, const QString &styleHref, const QString &style)
{
    	if (!chat)
		return QString("");

	QString result = style;

	//TODO: get Chat name (contacts' nicks?)
	//Replace %chatName% //TODO. Find way to dynamic update this tag (add id ?)
	result.replace(QString("%chatName%"), chat.name());
	// Replace %sourceName%
	result.replace(QString("%sourceName%"), chat.chatAccount().name());
	// Replace %destinationName%
	result.replace(QString("%destinationName%"), chat.name());
	// For %timeOpened%, display the date and time. TODO: get real time 
	result.replace(QString("%timeOpened%"), printDateTime(QDateTime::currentDateTime()));

	//TODO 0.6.6: get real time!!!
	QRegExp timeRegExp("%timeOpened\\{([^}]*)\\}%");
	int pos = 0;
	while ((pos=timeRegExp.indexIn(result, pos)) != -1)
		result.replace(pos, timeRegExp.cap(0).length(), timeFormatter->convertTimeDate(timeRegExp.cap(1), QDateTime::currentDateTime()));

	QString photoIncoming;
	QString photoOutgoing;

	int contactsSize = chat.contacts().size();
	if (contactsSize > 1)
		photoIncoming = QString("file://") + styleHref + QString("Incoming/buddy_icon.png");
	else if (contactsSize == 1)
	{
		Contact contact = chat.contacts().toContact();
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

QString AdiumChatStyleEngine::replaceKeywords(Chat chat, const QString &styleHref, const QString &source, MessageRenderInfo *message)
{
	if (!chat)
		return QString("");

	QString result = source;
	QString nick, contactId, service, protocolIcon, nickLink;

	Message msg = message->message();

	// Replace sender (contact nick)
	result.replace(QString("%sender%"), BuddyManager::instance()->byContact(msg.messageSender(), true).display());
	// Replace %screenName% (contact ID)
	result.replace(QString("%senderScreenName%"), msg.messageSender().id());
	// Replace service name (protocol name)
	if (chat.chatAccount().protocolHandler() && chat.chatAccount().protocolHandler()->protocolFactory())
	{
		result.replace(QString("%service%"), chat.chatAccount().protocolHandler()->protocolFactory()->displayName());
		// Replace protocolIcon (sender statusIcon). TODO:
		result.replace(QString("%senderStatusIcon%"), chat.chatAccount().protocolHandler()->protocolFactory()->iconName());
	}
	else
	{
		result.replace(QString("%service%"), "");
		result.replace(QString("%senderStatusIcon%"), "");
	}

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

		if (!msg.messageSender().contactAvatar().pixmap().isNull())
			photoPath = QString("file://") + msg.messageSender().contactAvatar().filePath();
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
	while ((textPos = senderColorRegExp.indexIn(result, textPos)) != -1)
	{
		int light = 100;
		bool doLight = false;
		if (senderColorRegExp.numCaptures() >= 1)
			light = senderColorRegExp.cap(1).toUInt(&doLight);

		if (doLight && lightColorName.isNull())
			lightColorName = QColor(colorName).light(light).name();

		result.replace(textPos, senderColorRegExp.cap(0).length(), doLight ? lightColorName : colorName);
	}

	// Replace message TODO: do sth with formatMessage
	QString messageText = QString("<span>") + formatMessage(message->htmlMessageContent(), message->backgroundColor()) + QString("</span>");
	result.replace(QString("%message%"), messageText);

	return result;
}
