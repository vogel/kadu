/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012 Marcel Zięba (marseel@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "avatars/avatar.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "chat/chat-styles-manager.h"
#include "chat/html-messages-renderer.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "gui/widgets/chat-messages-view.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/preview.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "message/message-render-info.h"
#include "misc/misc.h"
#include "parser/parser.h"
#include "protocols/protocol-factory.h"

#include "adium-style.h"
#include "adium-time-formatter.h"

#include "chat-engine-adium.h"

RefreshViewHack::RefreshViewHack(AdiumChatStyleEngine *engine, HtmlMessagesRenderer *renderer, QObject *parent) :
		QObject(parent), Engine(engine), Renderer(renderer)
{
	connect(Engine, SIGNAL(destroyed()), this, SLOT(cancel()));
	connect(Renderer, SIGNAL(destroyed()), this, SLOT(cancel()));
}

RefreshViewHack::~RefreshViewHack()
{
}

void RefreshViewHack::cancel()
{
	Engine = 0;
	Renderer = 0;

	deleteLater();
}

void RefreshViewHack::loadFinished()
{
	if (!Engine || !Renderer)
	{
		deleteLater();
		return;
	}

	emit finished(Renderer);

	Renderer->setLastMessage(0);

	foreach (MessageRenderInfo *message, Renderer->messages())
		Engine->appendChatMessage(Renderer, message);

	deleteLater();
}

PreviewHack::PreviewHack(AdiumChatStyleEngine *engine, Preview *preview, const QString &baseHref, const QString &outgoingHtml,
                         const QString &incomingHtml, QObject *parent) :
		QObject(parent), Engine(engine), CurrentPreview(preview), BaseHref(baseHref), OutgoingHtml(outgoingHtml), IncomingHtml(incomingHtml)
{
	connect(Engine, SIGNAL(destroyed()), this, SLOT(cancel()));
	connect(CurrentPreview, SIGNAL(destroyed()), this, SLOT(cancel()));
}

PreviewHack::~PreviewHack()
{
}

void PreviewHack::cancel()
{
	Engine = 0;
	CurrentPreview = 0;

	deleteLater();
}

void PreviewHack::loadFinished()
{
	if (!Engine || !CurrentPreview)
		return;

	MessageRenderInfo *message = CurrentPreview->messages().at(0);

	QString outgoingHtml(replacedNewLine(Engine->replaceKeywords(BaseHref, OutgoingHtml, message), QLatin1String(" ")));
	outgoingHtml.replace('\'', QLatin1String("\\'"));
	if (!message->message().id().isEmpty())
		outgoingHtml.prepend(QString("<span id=\"message_%1\">").arg(message->message().id()));
	else
		outgoingHtml.prepend("<span>");
	outgoingHtml.append("</span>");
	CurrentPreview->webView()->page()->mainFrame()->evaluateJavaScript("appendMessage(\'" + outgoingHtml + "\')");

	message = CurrentPreview->messages().at(1);
	QString incomingHtml(replacedNewLine(Engine->replaceKeywords(BaseHref, IncomingHtml, message), QLatin1String(" ")));
	incomingHtml.replace('\'', QLatin1String("\\'"));
	if (!message->message().id().isEmpty())
		incomingHtml.prepend(QString("<span id=\"message_%1\">").arg(message->message().id()));
	else
		incomingHtml.prepend("<span>");
	incomingHtml.append("</span>");
	CurrentPreview->webView()->page()->mainFrame()->evaluateJavaScript("appendMessage(\'" + incomingHtml + "\')");

	/* in Qt 4.6.3 / WebKit there is a bug making the following call not working */
	/* according to: https://bugs.webkit.org/show_bug.cgi?id=35633 */
	/* the proper refreshing behaviour should occur once the bug is fixed */
	/* possible temporary solution: use QWebElements API to randomly change */
	/* URLs in the HTML/CSS content. */
	CurrentPreview->webView()->page()->triggerAction(QWebPage::ReloadAndBypassCache, false);

	deleteLater();
}


AdiumChatStyleEngine::AdiumChatStyleEngine(QObject *parent) :
		QObject(parent)
{
	// Load required javascript functions
	QFile file(KaduPaths::instance()->dataPath() + QLatin1String("scripts/chat-scripts.js"));
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
	QString styleBaseHref = KaduPaths::instance()->profilePath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/Variants/");
	if (!dir.exists(styleBaseHref))
		styleBaseHref = KaduPaths::instance()->dataPath() + QLatin1String("syntax/chat/") + styleName + QLatin1String("/Contents/Resources/Variants/");
	dir.setPath(styleBaseHref);
	dir.setNameFilters(QStringList("*.css"));
	return dir.entryList();
}

void AdiumChatStyleEngine::appendMessages(HtmlMessagesRenderer *renderer, const QList<MessageRenderInfo *> &messages)
{
	if (CurrentRefreshHacks.contains(renderer))
		return;

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
	if (CurrentRefreshHacks.contains(renderer))
		return;

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
	if (CurrentRefreshHacks.contains(renderer))
		return;

	QString formattedMessageHtml;
	bool includeHeader = true;

	MessageRenderInfo *lastMessage = ChatStylesManager::instance()->cfgNoHeaderRepeat()
			? renderer->lastMessage()
			: 0;

	Message msg = message->message();

	if (lastMessage)
	{
		Message last = lastMessage->message();
		if (msg.receiveDate().toTime_t() < last.receiveDate().toTime_t())
			qWarning("New message has earlier date than last message");

		includeHeader =
			msg.type() == MessageTypeSystem ||
			last.type() == MessageTypeSystem ||
			msg.messageSender() != last.messageSender() ||
			msg.receiveDate().toTime_t() - last.receiveDate().toTime_t() > (ChatStylesManager::instance()->cfgNoHeaderInterval() * 60);
	}
	switch (msg.type())
	{
		case MessageTypeReceived:
		{
			if (includeHeader)
				formattedMessageHtml = CurrentStyle.incomingHtml();
			else
				formattedMessageHtml = CurrentStyle.nextIncomingHtml();
			break;
		}
		case MessageTypeSent:
		{
			if (includeHeader)
				formattedMessageHtml = CurrentStyle.outgoingHtml();
			else
				formattedMessageHtml = CurrentStyle.nextOutgoingHtml();
			break;
		}
		case MessageTypeSystem:
		{
			formattedMessageHtml = CurrentStyle.statusHtml();
			break;
		}

		default:
			break;
	}

	formattedMessageHtml = replacedNewLine(replaceKeywords(CurrentStyle.baseHref(), formattedMessageHtml, message), QLatin1String(" "));
	formattedMessageHtml.replace('\\', QLatin1String("\\\\"));
	formattedMessageHtml.replace('\'', QLatin1String("\\'"));
	if (!message->message().id().isEmpty())
		formattedMessageHtml.prepend(QString("<span id=\"message_%1\">").arg(message->message().id()));
	else
		formattedMessageHtml.prepend("<span>");
	formattedMessageHtml.append("</span>");

	if (includeHeader)
		renderer->webPage()->mainFrame()->evaluateJavaScript("appendMessage('"+ formattedMessageHtml +"')");
	else
		renderer->webPage()->mainFrame()->evaluateJavaScript("appendNextMessage('"+ formattedMessageHtml +"')");

	renderer->setLastMessage(message);
}

void AdiumChatStyleEngine::refreshHackFinished(HtmlMessagesRenderer *renderer)
{
	CurrentRefreshHacks.remove(renderer);
}

void AdiumChatStyleEngine::refreshView(HtmlMessagesRenderer *renderer, bool useTransparency)
{
	QString styleBaseHtml = CurrentStyle.templateHtml();
	styleBaseHtml.replace(styleBaseHtml.indexOf("%@"), 2, KaduPaths::webKitPath(CurrentStyle.baseHref()));
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
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, (CurrentStyle.styleViewVersion() < 3) ? "" : QString("@import url( \"" + CurrentStyle.mainHref() + "\" );"));
	}


	if (useTransparency && !CurrentStyle.defaultBackgroundIsTransparent())
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("==bodyBackground=="), 18, "background-image: none; background: none; background-color: rgba(0, 0, 0, 0)");

	if (CurrentRefreshHacks.contains(renderer))
		CurrentRefreshHacks.value(renderer)->cancel();

	RefreshViewHack *currentHack = new RefreshViewHack(this, renderer, this);
	CurrentRefreshHacks.insert(renderer, currentHack);
	connect(currentHack, SIGNAL(finished(HtmlMessagesRenderer*)),
			this, SLOT(refreshHackFinished(HtmlMessagesRenderer *)));

	// lets wait a while for all javascript to resolve and execute
	// we dont want to get to the party too early
	connect(renderer->webPage()->mainFrame(), SIGNAL(loadFinished(bool)),
			currentHack, SLOT(loadFinished()), Qt::QueuedConnection);

	renderer->webPage()->mainFrame()->setHtml(styleBaseHtml);
	renderer->webPage()->mainFrame()->evaluateJavaScript(jsCode);
	//I don't know why, sometimes 'initStyle' was performed after 'appendMessage'
	renderer->webPage()->mainFrame()->evaluateJavaScript("initStyle()");
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
	QDir dir(QString(KaduPaths::instance()->profilePath() + QLatin1String("syntax/chat/") + styleName));
	return clearDirectory(dir.absolutePath()) && dir.cdUp() && dir.rmdir(styleName);
}

void AdiumChatStyleEngine::prepareStylePreview(Preview *preview, QString styleName, QString variantName)
{
	AdiumStyle style(styleName);

	style.setCurrentVariant(variantName);
	if (preview->messages().count() != 2)
		return;

	MessageRenderInfo *message = preview->messages().at(0);
	if (!message)
		return;
	Message msg = message->message();

	QString styleBaseHtml = style.templateHtml();
	styleBaseHtml.replace(styleBaseHtml.indexOf("%@"), 2, KaduPaths::webKitPath(style.baseHref()));
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
		styleBaseHtml.replace(styleBaseHtml.lastIndexOf("%@"), 2, (style.styleViewVersion() < 3) ? "s" : QString("@import url( \"" + style.mainHref() + "\" );"));
	}

	PreviewHack *previousHack = CurrentPreviewHack.data();
	CurrentPreviewHack = new PreviewHack(this, preview, style.baseHref(), style.outgoingHtml(), style.incomingHtml(), this);
	delete previousHack;

	// lets wait a while for all javascript to resolve and execute
	// we dont want to get to the party too early
	connect(preview->webView()->page()->mainFrame(), SIGNAL(loadFinished(bool)),
	        CurrentPreviewHack.data(), SLOT(loadFinished()),  Qt::QueuedConnection);

	preview->webView()->page()->mainFrame()->setHtml(styleBaseHtml);
	preview->webView()->page()->mainFrame()->evaluateJavaScript(jsCode);
	//I don't know why, sometimes 'initStyle' was performed after 'appendMessage'
	preview->webView()->page()->mainFrame()->evaluateJavaScript("initStyle()");
}

// Some parts of the code below are borrowed from Kopete project (http://kopete.kde.org/)
QString AdiumChatStyleEngine::replaceKeywords(const Chat &chat, const QString &styleHref, const QString &style)
{
	if (!chat)
		return QString();

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

QString AdiumChatStyleEngine::replaceKeywords(const QString &styleHref, const QString &source, MessageRenderInfo *message)
{
	QString result = source;

	Message msg = message->message();

	// Replace sender (contact nick)
	result.replace(QString("%sender%"), msg.messageSender().display(true));
	// Replace %screenName% (contact ID)
	result.replace(QString("%senderScreenName%"), msg.messageSender().id());
	// Replace service name (protocol name)
	if (msg.messageChat().chatAccount().protocolHandler() && msg.messageChat().chatAccount().protocolHandler()->protocolFactory())
	{
		result.replace(QString("%service%"), msg.messageChat().chatAccount().protocolHandler()->protocolFactory()->displayName());
		// Replace protocolIcon (sender statusIcon). TODO:
		result.replace(QString("%senderStatusIcon%"), msg.messageChat().chatAccount().protocolHandler()->protocolFactory()->icon().fullPath());
	}
	else
	{
		result.replace(QString("%service%"), msg.messageChat().chatAccount().accountIdentity().name());
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
	if (msg.type() == MessageTypeReceived)
	{
		result.replace(QString("%messageClasses%"), "message incoming");

		const Avatar &avatar = msg.messageSender().avatar(true);
		if (!avatar.isEmpty())
			photoPath = KaduPaths::webKitPath(avatar.smallFilePath());
		else
			photoPath = KaduPaths::webKitPath(styleHref + QLatin1String("Incoming/buddy_icon.png"));
	}
	else if (msg.type() == MessageTypeSent)
	{
		result.replace(QString("%messageClasses%"), "message outgoing");
		const Avatar &avatar = msg.messageChat().chatAccount().accountContact().avatar(true);
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
	QString messageText = formatMessage(message->htmlMessageContent());

	if (!message->message().id().isEmpty())
		messageText.prepend(QString("<span id=\"message_%1\">").arg(message->message().id()));
	else
		messageText.prepend("<span>");
	messageText.append("</span>");

	result.replace(QString("%messageId%"), message->message().id());
	result.replace(QString("%messageStatus%"), QString::number(message->message().status()));

	result.replace(QString("%message%"), messageText);

	return result;
}

void AdiumChatStyleEngine::messageStatusChanged(HtmlMessagesRenderer *renderer, Message message, MessageStatus status)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript(QString("adium_messageStatusChanged(\"%1\", %2);").arg(message.id()).arg((int)status));
}

void AdiumChatStyleEngine::contactActivityChanged(HtmlMessagesRenderer *renderer, ChatStateService::State state, const QString &message, const QString &name)
{
	renderer->webPage()->mainFrame()->evaluateJavaScript(QString("adium_contactActivityChanged(%1, \"%2\", \"%3\");").arg((int)state).arg(message).arg(name));
}
