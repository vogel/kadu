/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QScrollBar>
#include <QtWebKit/QWebFrame>

#include "chat_message.h"
#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "kadu_parser.h"
#include "misc.h"
#include "syntax_editor.h"

#include "chat_messages_view.h"

ChatMessagesView::ChatMessagesView(QWidget *parent) : KaduTextBrowser(parent),
	Prune(0), lastScrollValue(0), lastLine(false)
{
	setMinimumSize(QSize(100,100));

	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));

	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(scrollToLine()));

	configurationUpdated();

	setFocusPolicy(Qt::NoFocus);
}

ChatMessagesView::~ChatMessagesView()
{
	qDeleteAll(Messages);
	Messages.clear();
}

void ChatMessagesView::pageUp()
{
// 	TODO: 0.6.5 scrollContentsBy(0, (height() * -2) / 3);
}

void ChatMessagesView::pageDown()
{
// 	TODO: 0.6.5 scrollContentsBy(0, (height() * 2) / 3);
}

void ChatMessagesView::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32,const QString & /*path*/)
{
	foreach(ChatMessage *message, Messages)
		message->replaceLoadingImages(sender, size, crc32);
	repaintMessages();
}

QString ChatMessagesView::formatMessage(ChatMessage *message, ChatMessage *after)
{
	kdebugf();

	int separatorSize;
	QString format;
	bool includeHeader;

	if (message->type() == TypeSystem)
	{
		separatorSize = ParagraphSeparator;
		format = ChatSyntaxWithoutHeader;

		message->setSeparatorSize(separatorSize);
		return KaduParser::parse(format, message->sender(), message, true);
	}
	else
	{
		includeHeader = (!CfgNoHeaderRepeat || !after);

		if (after && !includeHeader)
			includeHeader =
				(after->type() != TypeSystem) &&
				((message->date().toTime_t() - after->date().toTime_t() > (CfgNoHeaderInterval * 60)) ||
				 (message->sender() != after->sender()));

		if (includeHeader)
		{
			separatorSize = CfgHeaderSeparatorHeight;
			format = ChatSyntaxWithHeader;
		}
		else
		{
			separatorSize = ParagraphSeparator;
			format = ChatSyntaxWithoutHeader;
		}

		message->setShowServerTime(NoServerTime, NoServerTimeDiff);
		message->setSeparatorSize(separatorSize);

		return KaduParser::parse(format, message->sender(), message, true);
	}
}

void ChatMessagesView::repaintMessages()
{
	kdebugf();

	QString text = QString(
		"<html>"
		"	<head>"
		"		<style type='text/css'>") +
		style +
		"		</style>"
		"	</head>"
		"	<body>";

	QList<ChatMessage *>::const_iterator message = Messages.constBegin();
	QList<ChatMessage *>::const_iterator prevMessage;
	QList<ChatMessage *>::const_iterator end = Messages.constEnd();

	if (message != end)
	{
		(*message)->setSeparatorSize(0);

		if ((*message)->type() == TypeSystem)
			text += KaduParser::parse(ChatSyntaxWithoutHeader, (*message)->sender(), *message);
		else
		{
			(*message)->setShowServerTime(NoServerTime, NoServerTimeDiff);
			text += KaduParser::parse(ChatSyntaxWithHeader, (*message)->sender(), *message);
		}

		prevMessage = message;
		while (++message != end)
		{
			text += formatMessage(*message, *prevMessage);
			prevMessage = message;
		}
	}

	lastScrollValue = page()->currentFrame()->scrollBarValue(Qt::Vertical);
	lastLine = (lastScrollValue == page()->currentFrame()->scrollBarMaximum(Qt::Vertical));

	text += "</body></html>";

	setHtml(text);

	kdebugf2();
}

void ChatMessagesView::updateBackgroundsAndColors()
{
	QString myBackgroundColor = config_file.readEntry("Look", "ChatMyBgColor");
	QString myFontColor = config_file.readEntry("Look", "ChatMyFontColor");
	QString myNickColor = config_file.readEntry("Look", "ChatMyNickColor");
	QString usrBackgroundColor = config_file.readEntry("Look", "ChatUsrBgColor");
	QString usrFontColor = config_file.readEntry("Look", "ChatUsrFontColor");
	QString usrNickColor = config_file.readEntry("Look", "ChatUsrNickColor");

	foreach(ChatMessage *message, Messages)
	{
		switch (message->type())
		{
			case TypeSent:
				message->setColorsAndBackground(myBackgroundColor, myNickColor, myFontColor);
				break;

			case TypeReceived:
				message->setColorsAndBackground(usrBackgroundColor, usrNickColor, usrFontColor);
				break;

			case TypeSystem:
				break;
		}

	}
}

void ChatMessagesView::appendMessage(ChatMessage *message)
{
	kdebugf();

	Messages.append(message);
	pruneMessages();
	repaintMessages();
}

void ChatMessagesView::appendMessages(QList<ChatMessage *> messages)
{
	kdebugf2();

	Messages += messages;
	pruneMessages();
	repaintMessages();
}

void ChatMessagesView::pruneMessages()
{
	kdebugf();

	if (Prune == 0)
		return;

	if (Messages.count() < Prune)
		return;

	QList<ChatMessage *>::iterator start = Messages.begin();
	QList<ChatMessage *>::iterator stop = Messages.find(Messages.at(Messages.size() - Prune));
	for (QList<ChatMessage *>::iterator it = start; it != stop; ++it)
		delete *it;

	Messages.erase(start, stop);
}

void ChatMessagesView::setPrune(unsigned int prune)
{
	Prune = prune;
}

void ChatMessagesView::clearMessages()
{
	qDeleteAll(Messages);
	Messages.clear();

	setHtml("");
}

unsigned int ChatMessagesView::countMessages()
{
	return Messages.count();
}

void ChatMessagesView::configurationUpdated()
{
	ParagraphSeparator = config_file.readUnsignedNumEntry("Look", "ParagraphSeparator");

	QFont font = config_file.readFontEntry("Look","ChatFont");

	QString fontFamily = font.family();
	QString fontSize;
	if (font.pointSize() > 0)
#ifdef Q_OS_MAC
		/*  Dorr: On MacOSX this font is being displayed 3pts larger than
		 *  it really is, so reduce it's size to be coherent in entire
		 *  application.
		 */
		fontSize = QString::number(font.pointSize()-3) + "pt";
#else
		fontSize = QString::number(font.pointSize()) + "pt";
#endif
	else
		fontSize = QString::number(font.pixelSize()) + "px";
	QString fontStyle = font.italic() ? "italic" : "normal";
	QString fontWeight = font.bold() ? "bold" : "normal";
	QString textDecoration = font.underline() ? "underline" : "none";
	QString backgroundColor = config_file.readColorEntry("Look", "ChatBgColor").name();

	style = QString(
		"html {"
		"	font: %1 %2 %3 %4;"
		"	text-decoration: %5;"
		"}"
		"body {"
		"	margin: %6;"
		"	padding: 0;"
		"	background-color: %7;"
		"}"
		"p {"
		"	margin: 0;"
		"	padding: 3px;"
		"}").arg(fontStyle, fontWeight, fontSize, fontFamily, textDecoration, QString::number(ParagraphSeparator), backgroundColor);

	// background color of chat
// 	QString bgImage = KaduParser::parse(config_file.readEntry("Look", "ChatBgImage"), usrs[0]);

	// TODO: for me with empty styleSheet if has artifacts on scrollbars...
	// maybe Qt bug?
  	setStyleSheet("QWidget { }");
// 	if (!bgImage.isEmpty() && QFile::exists(bgImage))
// 		brush.setPixmap(QPixmap(bgImage));

	QString chatSyntax = SyntaxList::readSyntax("chat", config_file.readEntry("Look", "Style"),
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

	CfgNoHeaderRepeat = config_file.readBoolEntry("Look", "NoHeaderRepeat");


	// headers removal stuff
	if (CfgNoHeaderRepeat)
	{
		CfgHeaderSeparatorHeight = config_file.readUnsignedNumEntry("Look", "HeaderSeparatorHeight");
		CfgNoHeaderInterval = config_file.readUnsignedNumEntry("Look", "NoHeaderInterval");
	}
	else
	{
		CfgHeaderSeparatorHeight = 0;
		CfgNoHeaderInterval = 0;
	}

	NoServerTime = config_file.readBoolEntry("Look", "NoServerTime");
	NoServerTimeDiff = config_file.readUnsignedNumEntry("Look", "NoServerTimeDiff");

	updateBackgroundsAndColors();

	repaintMessages();
}

void ChatMessagesView::resizeEvent(QResizeEvent *e)
{
 	lastScrollValue = page()->currentFrame()->scrollBarValue(Qt::Vertical);
 	lastLine = (lastScrollValue == page()->currentFrame()->scrollBarMaximum(Qt::Vertical));

 	KaduTextBrowser::resizeEvent(e);

	scrollToLine();

}

void ChatMessagesView::scrollToLine()
{
 	if (lastLine)
 		page()->currentFrame()->setScrollBarValue(Qt::Vertical, page()->currentFrame()->scrollBarMaximum(Qt::Vertical));
 	else
 		page()->currentFrame()->setScrollBarValue(Qt::Vertical, lastScrollValue);
}
