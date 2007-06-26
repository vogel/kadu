/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chat_message.h"
#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "kadu_parser.h"
#include "misc.h"
#include "syntax_editor.h"

#include "chat_messages_view.h"

ChatMessagesView::ChatMessagesView(QWidget *parent, char *name) : KaduTextBrowser(parent, name),
	Prune(0), CfgNoHeaderRepeat(config_file.readBoolEntry("Look", "NoHeaderRepeat")),
	CfgHeaderSeparatorHeight(0), CfgNoHeaderInterval(0),
	ParagraphSeparator(config_file.readUnsignedNumEntry("Look", "ParagraphSeparator")),
	ScrollLocked(false)
{
	if ((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EMOTS_ANIMATED)
		setStyleSheet(new AnimStyleSheet(this, emoticons->themePath()));
	else
		setStyleSheet(new StaticStyleSheet(this, emoticons->themePath()));

	setMargin(ParagraphSeparator);
	setMinimumSize(QSize(100,100));
	setFont(config_file.readFontEntry("Look","ChatFont"));

	// background color of chat
// 	QString bgImage = KaduParser::parse(config_file.readEntry("Look", "ChatBgImage"), usrs[0]);
	QBrush brush(config_file.readColorEntry("Look", "ChatBgColor"));
// 	if (!bgImage.isEmpty() && QFile::exists(bgImage))
// 		brush.setPixmap(QPixmap(bgImage));
	setPaper(brush);

	QString chatSyntax = SyntaxList::readSyntax("chat", config_file.readEntry("Look", "Style"));
	int beginOfHeader = chatSyntax.find("<kadu:header>");
	int endOfHeader = chatSyntax.find("</kadu:header>");
	ChatSyntaxWithHeader = chatSyntax;
	ChatSyntaxWithHeader.replace("<kadu:header>", "");
	ChatSyntaxWithHeader.replace("</kadu:header>", "");

	if (endOfHeader != -1)
		ChatSyntaxWithoutHeader = chatSyntax.mid(0, beginOfHeader) + chatSyntax.mid(endOfHeader + strlen("</kadu:header>"));
	else
		ChatSyntaxWithoutHeader = ChatSyntaxWithHeader;

	// headers removal stuff
	if (CfgNoHeaderRepeat)
	{
	    CfgHeaderSeparatorHeight = config_file.readUnsignedNumEntry("Look", "HeaderSeparatorHeight");
	    CfgNoHeaderInterval = config_file.readUnsignedNumEntry("Look", "NoHeaderInterval");
	}

	bodyformat = new QMimeSourceFactory();
	setMimeSourceFactory(bodyformat);
	setTextFormat(Qt::RichText);
	setFocusPolicy(QWidget::NoFocus);
}

ChatMessagesView::~ChatMessagesView()
{
	FOREACH(msg, Messages)
		delete *msg;
	Messages.clear();

	delete bodyformat;
	bodyformat = 0;
}

void ChatMessagesView::pageUp()
{
	scrollBy(0, (height() * -2) / 3);
}

void ChatMessagesView::pageDown()
{
	scrollBy(0, (height() * 2) / 3);
}

void ChatMessagesView::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32,const QString & /*path*/)
{
	FOREACH(message, Messages)
		(*message)->replaceLoadingImages(sender, size, crc32);
	repaintMessages();
}

QString ChatMessagesView::formatMessage(ChatMessage *message, ChatMessage *after)
{
	kdebugf();

	int separatorSize;
	QString format;
	bool includeHeader = (!CfgNoHeaderRepeat || !after);

	if (after && !includeHeader)
		includeHeader =
			(message->date().toTime_t() - after->date().toTime_t() > (CfgNoHeaderInterval * 60)) ||
			(message->sender() != after->sender());

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

	message->setSeparatorSize(separatorSize);
	return KaduParser::parse(format, message->sender(), message);
}

void ChatMessagesView::repaintMessages()
{
	viewport()->setUpdatesEnabled(false);

	QString text;

	QValueList<ChatMessage *>::const_iterator message = Messages.constBegin();
	QValueList<ChatMessage *>::const_iterator prevMessage;
	QValueList<ChatMessage *>::const_iterator end = Messages.constEnd();

	if (message == end)
		return;

	(*message)->setSeparatorSize(0);

	text += KaduParser::parse(ChatSyntaxWithHeader, (*message)->sender(), *message);

	prevMessage = message;
	while (++message != end)
	{
		text += formatMessage(*message, *prevMessage);
		prevMessage = message;
	}

	viewport()->setUpdatesEnabled(false);

	setText(text);
	updateBackgrounds();

	if (!ScrollLocked)
		scrollToBottom();

	viewport()->setUpdatesEnabled(true);
	viewport()->repaint();
}

void ChatMessagesView::updateBackgrounds()
{

	int i = 0;
	CONST_FOREACH(message, Messages)
		setParagraphBackgroundColor(i++, (*message)->backgroundColor);
}

void ChatMessagesView::appendMessage(ChatMessage *message)
{
	kdebugf();

	Messages.append(message);
	pruneMessages();
	repaintMessages();
}

void ChatMessagesView::appendMessages(QValueList<ChatMessage *> messages)
{
	kdebugf2();

	Messages += messages;
	pruneMessages();
	repaintMessages();
}

void ChatMessagesView::pruneMessages()
{
	kdebugf();

	printf("%d vs %d\n", Messages.size(), Prune);

	if (Prune == 0)
		return;

	if (Messages.count() < Prune)
		return;

	QValueList<ChatMessage *>::iterator start = Messages.begin();
	QValueList<ChatMessage *>::iterator stop = Messages.at(Messages.size() - Prune);
	for (QValueList<ChatMessage *>::iterator it = start; it != stop; ++it)
	{
		printf("one removed\n");
		delete *it;
	}

	Messages.erase(start, stop);
}

void ChatMessagesView::setPrune(unsigned int prune)
{
	Prune = prune;
}

void ChatMessagesView::clearMessages()
{
	FOREACH(message, Messages)
		delete *message;
	Messages.clear();

	clear();
	updateBackgrounds();
	viewport()->repaint();
}

void ChatMessagesView::setScrollLocked(bool scrollLocked)
{
	ScrollLocked = scrollLocked;
}
