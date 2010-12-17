/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtWebKit/QWebFrame>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "avatars/avatar.h"
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

AdiumChatStyleEngine::AdiumChatStyleEngine(QObject *parent) :
		QObject(parent)
{
	// Load required javascript functions
	QFile file(dataPath("kadu") + "/scripts/chat-scripts.js");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		jsCode = file.readAll();
}

AdiumChatStyleEngine::~AdiumChatStyleEngine()
{
}

void AdiumChatStyleEngine::pruneMessage(HtmlMessagesRenderer *renderer)
{
	if (!ChatStylesManager::instance()->cfgNoHeaderRepeat())
		renderer->webPage()->mainFrame()->evaluateJavaScript("adium_removeFirstMessage()");
}

void AdiumChatStyleEngine::clearMessages(HtmlMessagesRenderer *renderer)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript("adium_clearMessages()");
}

QString AdiumChatStyleEngine::isStyleValid(QString stylePath)
{
	return AdiumStyle::isStyleValid(stylePath) ? QDir(stylePath).dirName() : QString();
}

bool AdiumChatStyleEngine::styleUsesTransparencyByDefault(QString styleName)
{
	AdiumStyle style(styleName);
	return style.defaultBackgroundIsTransparent();
}

QString AdiumChatStyleEngine::defaultVariant(const QString &styleName)
{
	AdiumStyle style(styleName);
	return style.defaultVariant();
}

QString AdiumChatStyleEngine::currentStyleVariant()
{
	return CurrentStyle.currentVariant();
}

QStringList AdiumChatStyleEngine::styleVariants(QString styleName)
{
	QDir dir;
	QString styleBaseHref = profilePath() + "/syntax/chat/" + styleName + "/Contents/Resources/Variants/";
	if (!dir.exists(styleBaseHref))
		styleBaseHref = dataPath("kadu") + "/syntax/chat/" + styleName + "/Contents/Resources/Variants/";
	dir.setPath(styleBaseHref);
	dir.setNameFilters(QStringList("*.css"));
	return dir.entryList();
}

void AdiumChatStyleEngine::appendMessages(HtmlMessagesRenderer *renderer, const QList<MessageRenderInfo *> &messages)
{
	if (ChatStylesManager::instance()->cfgNoHeaderRepeat() && renderer->pruneEnabled())
	{
		clearMessages(renderer);

		foreach (MessageRenderInfo *message, renderer->messages())
			appendChatMessage(renderer, message);
		return;
	}

	foreach (MessageRenderInfo *message, messages)
		appendChatMessage(renderer, message);
}

void AdiumChatStyleEngine::appendMessage(HtmlMessagesRenderer *renderer, MessageRenderInfo *message)
{
	if (ChatStylesManager::instance()->cfgNoHeaderRepeat() && renderer->pruneEnabled())
	{
		clearMessages(renderer);

		foreach (MessageRenderInfo *message, renderer->messages())
			appendChatMessage(renderer, message);
		return;
	}

	appendChatMessage(renderer, message);
}

void AdiumChatStyleEngine::appendChatMessage(HtmlMessagesRenderer *renderer, MessageRenderInfo *message)
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
			msg.type() == Message::TypeSystem ||
			last.type() == Message::TypeSystem ||
			msg.messageSender() != last.messageSender() ||
			msg.receiveDate().toTime_t() - last.receiveDate().toTime_t() > (ChatStylesManager::instance()->cfgNoHeaderInterval() * 60);
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
		}
		case Message::TypeSystem:
		{
			formattedMessageHtml = CurrentStyle.statusHtml();
			break;
		}

		default:
			break;
	}

	formattedMessageHtml = replaceKeywords(renderer->chat(), CurrentStyle.baseHref(), formattedMessageHtml, message);
	formattedMessageHtml.replace('\n', ' ');
	formattedMessageHtml.replace('\'', QLatin1String("\\'"));
	formattedMessageHtml.replace('\\', QLatin1String("\\\\"));
	formattedMessageHtml.prepend("<span>");
	formattedMessageHtml.append("</span>");

	if (includeHeader)
		renderer->webPage()->mainFrame()->evaluateJavaScript("appendMessage(\'"+ formattedMessageHtml +"\')");
	else
		renderer->webPage()->mainFrame()->evaluateJavaScript("appendNextMessage(\'"+ formattedMessageHtml +"\')");

	renderer->setLastMessage(message);
}

void AdiumChatStyleEngine::refreshView(HtmlMessagesRenderer *renderer, bool useTransparency)
{
	QString styleBaseHtml = CurrentStyle.templateHtml();
	styleBaseHtml.replace(styleBaseHtml.indexOf("%@"), 2, webKitPath(CurrentStyle.baseHref()));
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(renderer->chat(), CurrentStyle.baseHref(), CurrentStyle.footerHtml()));
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(renderer->chat(), CurrentStyle.baseHref(), CurrentStyle.headerHtml()));

	if (CurrentStyle.usesCustomTemplateHtml() && CurrentStyle.styleViewVersion() < 3)
	{
		if (CurrentStyle.currentVariant() != CurrentStyle.defaultVariant())
			styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, "Variants/" + CurrentStyle.currentVariant());
		else
			styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, CurrentStyle.mainHref());
	}
	else
	{
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, (CurrentStyle.styleViewVersion() < 3 && CurrentStyle.defaultVariant() == CurrentStyle.currentVariant()) ? CurrentStyle.mainHref() : "Variants/" + CurrentStyle.currentVariant());
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, (CurrentStyle.styleViewVersion() < 3) ? "s" : "@import url( \"" + CurrentStyle.mainHref() + "\" );");
	}


	if (useTransparency && !CurrentStyle.defaultBackgroundIsTransparent())
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("==bodyBackground=="), 18, "background-image: none; background: none; background-color: rgba(0, 0, 0, 0)");

	renderer->webPage()->mainFrame()->setHtml(styleBaseHtml);
	renderer->webPage()->mainFrame()->evaluateJavaScript(jsCode);
	//I don't know why, sometimes 'initStyle' was performed after 'appendMessage'
	renderer->webPage()->mainFrame()->evaluateJavaScript("initStyle()");

	renderer->setLastMessage(0);

	foreach (MessageRenderInfo *message, renderer->messages())
		appendChatMessage(renderer, message);
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
			if (!clearDirectory(QString(directory + '/' + fileName)) || !dir.rmdir(fileName))
				return false;

		}
	}
	return dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden).isEmpty();
}

bool AdiumChatStyleEngine::removeStyle(const QString &styleName)
{
	QDir dir(QString(profilePath() + "/syntax/chat/" + styleName));
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
	styleBaseHtml.replace(styleBaseHtml.indexOf("%@"), 2, webKitPath(style.baseHref()));
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(msg.messageChat(), style.baseHref(), style.footerHtml()));
	styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, replaceKeywords(msg.messageChat(), style.baseHref(), style.headerHtml()));

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
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, (style.styleViewVersion() < 3) ? "s" : "@import url( \"" + style.mainHref() + "\" );");
	}

	preview->page()->mainFrame()->setHtml(styleBaseHtml);
	preview->page()->mainFrame()->evaluateJavaScript(jsCode);
	//I don't know why, sometimes 'initStyle' was performed after 'appendMessage'
	preview->page()->mainFrame()->evaluateJavaScript("initStyle()");

	QString outgoingHtml = replaceKeywords(msg.messageChat(), style.baseHref(), style.outgoingHtml(), message);
	outgoingHtml.replace('\n', ' ');
	outgoingHtml.replace('\'', QLatin1String("\\'"));
	outgoingHtml.prepend("<span>");
	outgoingHtml.append("</span>");
	preview->page()->mainFrame()->evaluateJavaScript("appendMessage(\'" + outgoingHtml + "\')");

	message = dynamic_cast<MessageRenderInfo *>(preview->getObjectsToParse().at(1));
	QString incomingHtml = replaceKeywords(msg.messageChat(), style.baseHref(), style.incomingHtml(), message);
	incomingHtml.replace('\n', ' ');
	incomingHtml.replace('\'', QLatin1String("\\'"));
	incomingHtml.prepend("<span>");
	incomingHtml.append("</span>");
	preview->page()->mainFrame()->evaluateJavaScript("appendMessage(\'" + incomingHtml + "\')");

#if (QT_VERSION >= 0x040600)
	/* in Qt 4.6.3 / WebKit there is a bug making the following call not working */
	/* according to: https://bugs.webkit.org/show_bug.cgi?id=35633 */
	/* the proper refreshing behaviour should occur once the bug is fixed */
	/* possible temporary solution: use QWebElements API to randomly change */
	/* URLs in the HTML/CSS content. */
	preview->page()->triggerAction(QWebPage::ReloadAndBypassCache, false);
#endif
}

// Some parts of the code below are borrowed from Kopete project (http://kopete.kde.org/)
QString AdiumChatStyleEngine::replaceKeywords(Chat chat, const QString &styleHref, const QString &style)
{
	if (!chat)
		return QString();

	QString result = style;

	//TODO: get Chat name (contacts' nicks?)
	//Replace %chatName% //TODO. Find way to dynamic update this tag (add id ?)
	result.replace(QString("%chatName%"), chat.name());
	// Replace %sourceName%
	result.replace(QString("%sourceName%"), chat.chatAccount().accountIdentity().name());
	// Replace %destinationName%
	result.replace(QString("%destinationName%"), chat.name());
	// For %timeOpened%, display the date and time. TODO: get real time
	result.replace(QString("%timeOpened%"), printDateTime(QDateTime::currentDateTime()));

	//TODO 0.6.6: get real time!!!
	QRegExp timeRegExp("%timeOpened\\{([^}]*)\\}%");
	int pos = 0;
	while ((pos=timeRegExp.indexIn(result, pos)) != -1)
		result.replace(pos, timeRegExp.cap(0).length(), AdiumTimeFormatter::convertTimeDate(timeRegExp.cap(1), QDateTime::currentDateTime()));

	QString photoIncoming;
	QString photoOutgoing;

	int contactsSize = chat.contacts().size();
	if (contactsSize > 1)
		photoIncoming = webKitPath(styleHref + QLatin1String("Incoming/buddy_icon.png"));
	else if (contactsSize == 1)
	{
		Contact contact = chat.contacts().toContact();
		if (!contact.isNull() && !contact.ownerBuddy().buddyAvatar().pixmap().isNull())
			photoIncoming = webKitPath(contact.ownerBuddy().buddyAvatar().filePath());
		else if (!contact.isNull() && !contact.contactAvatar().pixmap().isNull())
			photoIncoming = webKitPath(contact.contactAvatar().filePath());
		else
			photoIncoming = webKitPath(styleHref + QLatin1String("Incoming/buddy_icon.png"));
	}

	Avatar avatar = chat.chatAccount().accountContact().contactAvatar();
	if (!avatar.isEmpty())
		photoOutgoing = webKitPath(avatar.filePath());
	else
		photoOutgoing = webKitPath(styleHref + QLatin1String("Outgoing/buddy_icon.png"));

	result.replace(QString("%incomingIconPath%"), photoIncoming);
	result.replace(QString("%outgoingIconPath%"), photoOutgoing);

	return result;
}

QString AdiumChatStyleEngine::replaceKeywords(Chat chat, const QString &styleHref, const QString &source, MessageRenderInfo *message)
{
	QString result = source;

	Message msg = message->message();

	// Replace sender (contact nick)
	result.replace(QString("%sender%"), BuddyManager::instance()->byContact(msg.messageSender(), ActionCreateAndAdd).display());
	// Replace %screenName% (contact ID)
	result.replace(QString("%senderScreenName%"), msg.messageSender().id());
	// Replace service name (protocol name)
	if (chat && chat.chatAccount().protocolHandler() && chat.chatAccount().protocolHandler()->protocolFactory())
	{
		result.replace(QString("%service%"), chat.chatAccount().protocolHandler()->protocolFactory()->displayName());
		// Replace protocolIcon (sender statusIcon). TODO:
		result.replace(QString("%senderStatusIcon%"), chat.chatAccount().protocolHandler()->protocolFactory()->iconPath());
	}
	else
	{
		result.remove("%service%");
		result.remove("%senderStatusIcon%");
	}

	// Replace time
	QDateTime time = msg.sendDate().isNull() ? msg.receiveDate(): msg.sendDate();
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
	if (msg.type() == Message::TypeReceived)
	{
		result.replace(QString("%messageClasses%"), "message incoming");

		if (!msg.messageSender().ownerBuddy().buddyAvatar().pixmap().isNull())
			photoPath = webKitPath(msg.messageSender().ownerBuddy().buddyAvatar().filePath());
		else if (!msg.messageSender().contactAvatar().pixmap().isNull())
			photoPath = webKitPath(msg.messageSender().contactAvatar().filePath());
		else
			photoPath = webKitPath(styleHref + QLatin1String("Incoming/buddy_icon.png"));
	}
	else if (msg.type() == Message::TypeSent)
	{
   		result.replace(QString("%messageClasses%"), "message outgoing");
		Avatar avatar = chat.chatAccount().accountContact().contactAvatar();
		if (!avatar.isEmpty())
			photoPath = webKitPath(avatar.filePath());
		else
			photoPath = webKitPath(styleHref + QLatin1String("Outgoing/buddy_icon.png"));
	}
	else
		result.remove(QString("%messageClasses%"));

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
	QString messageText = QString("<span>") + formatMessage(message->htmlMessageContent()) + QString("</span>");
	result.replace(QString("%message%"), messageText);

	return result;
}
