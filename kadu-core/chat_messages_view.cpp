/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QScrollBar>

#include "chat_message.h"
#include "config_file.h"
#include "debug.h"
#include "gadu.h"
#include "kadu_parser.h"
#include "misc.h"
#include "syntax_editor.h"

#include "chat_messages_view.h"

ChatMessagesView::ChatMessagesView(QWidget *parent) : KaduTextBrowser(parent),
	Prune(0)
{
	setMargin(ParagraphSeparator);
	setMinimumSize(QSize(100,100));

	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		this, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));

	configurationUpdated();

//	bodyformat = new Q3MimeSourceFactory();
//	setMimeSourceFactory(bodyformat);
	setTextFormat(Qt::RichText);
	setFocusPolicy(Qt::NoFocus);

//	scrollToBottom();
}

ChatMessagesView::~ChatMessagesView()
{
	qDeleteAll(Messages);
	Messages.clear();

//	delete bodyformat;
//	bodyformat = 0;
}

void ChatMessagesView::pageUp()
{
	scrollContentsBy(0, (height() * -2) / 3);
}

void ChatMessagesView::pageDown()
{
	scrollContentsBy(0, (height() * 2) / 3);
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
			separatorSize = ParagraphSeparator;
			format = ChatSyntaxWithHeader;
		}
		else
		{
			separatorSize = CfgHeaderSeparatorHeight;
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

	QString text;

	QList<ChatMessage *>::const_iterator message = Messages.constBegin();
	QList<ChatMessage *>::const_iterator prevMessage;
	QList<ChatMessage *>::const_iterator end = Messages.constEnd();

	if (message == end)
		return;

	(*message)->setSeparatorSize(0);

	if ((*message)->type() == TypeSystem)
		text = KaduParser::parse(ChatSyntaxWithoutHeader, (*message)->sender(), *message);
	else
	{
		(*message)->setShowServerTime(NoServerTime, NoServerTimeDiff);
		text = KaduParser::parse(ChatSyntaxWithHeader, (*message)->sender(), *message);
	}

	prevMessage = message;
	while (++message != end)
	{
		text += formatMessage(*message, *prevMessage);
		prevMessage = message;
	}

	int lastScrollValue = verticalScrollBar()->value();
	bool lastLine = (lastScrollValue == verticalScrollBar()->maxValue());

	HtmlDocument htmlDocument;
	htmlDocument.parseHtml(text);
	htmlDocument.convertUrlsToHtml();
	htmlDocument.convertMailToHtml();
	emoticons->expandEmoticons(htmlDocument, "black", (EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));

	setText(htmlDocument.generateHtml());
	updateBackgrounds();

	if (lastLine)
		verticalScrollBar()->setValue(verticalScrollBar()->maxValue());
	else
		verticalScrollBar()->setValue(lastScrollValue);

	kdebugf2();
}

void ChatMessagesView::updateBackgrounds()
{

//	int i = 0;
//	CONST_FOREACH(message, Messages)
//		setParagraphBackgroundColor(i++, (*message)->backgroundColor);
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

	clear();
	updateBackgrounds();
	viewport()->repaint();
}

unsigned int ChatMessagesView::countMessages()
{
	return Messages.count();
}

void ChatMessagesView::configurationUpdated()
{
// 	if ((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EMOTS_ANIMATED)
// 		setStyleSheet(new AnimStyleSheet(this, emoticons->themePath()));
// 	else
// 		setStyleSheet(new StaticStyleSheet(this, emoticons->themePath()));

//	if ((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") != EMOTS_NONE)
//		mimeSourceFactory()->addFilePath(emoticons->themePath());

	setFont(config_file.readFontEntry("Look","ChatFont"));

	// background color of chat
// 	QString bgImage = KaduParser::parse(config_file.readEntry("Look", "ChatBgImage"), usrs[0]);
 	setStyleSheet(QString("QTextBrowser {background-color:%1}").arg(config_file.readColorEntry("Look", "ChatBgColor").name()));
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
	ParagraphSeparator = config_file.readUnsignedNumEntry("Look", "ParagraphSeparator");

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

	setMargin(ParagraphSeparator);

	repaintMessages();
}

void ChatMessagesView::resizeEvent(QResizeEvent *e)
{

	int lastScrollValue = verticalScrollBar()->value();
	bool lastLine = (lastScrollValue == verticalScrollBar()->maxValue());

	KaduTextBrowser::resizeEvent(e);

	if (lastLine)
		verticalScrollBar()->setValue(verticalScrollBar()->maxValue());
	else
		verticalScrollBar()->setValue(lastScrollValue);
}
