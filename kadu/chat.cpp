/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qpushbutton.h>
#include <qfontdatabase.h>
#include <qregexp.h>
#include <qlayout.h>
#include <qaccel.h>
#include <qtimer.h>
#include <qcursor.h>
#include <qprocess.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qsplitter.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qvaluelist.h>
#include <qfiledialog.h>

#include <math.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "misc.h"
#include "config_dialog.h"
#include "config_file.h"
#include "kadu.h"
#include "chat.h"
#include "search.h"
#include "history.h"
#include "emoticons.h"
#include "pending_msgs.h"
#include "debug.h"
#include "gadu.h"
#include "status.h"
#include "message_box.h"

ChatManager::ChatManager(QObject* parent, const char* name)
	: QObject(parent, name)
{
}

void ChatManager::closeAllWindows()
{
	kdebugf();
	while (!Chats.empty())
	{
		Chat *chat=Chats.first();
		delete chat;
	}
	kdebugf2();
}

ChatManager::~ChatManager()
{
	kdebugf();
	closeAllWindows();
}

const ChatList& ChatManager::chats()
{
	return Chats;
}

int ChatManager::registerChat(Chat* chat)
{
	Chats.append(chat);
	return Chats.count() - 1;
}

void ChatManager::unregisterChat(Chat* chat)
{
	kdebugf();
	for(unsigned int i=0; i<Chats.count(); ++i)
		if(Chats[i]==chat)
		{
			setChatProperty(chat->uins(), "Geometry", QRect(chat->pos().x(), chat->pos().y(), chat->size().width(), chat->size().height()));
			setChatProperty(chat->uins(), "VerticalSizes", toVariantList(chat->vertSplit->sizes()));
			if (chat->horizSplit)
				setChatProperty(chat->uins(), "HorizontalSizes", toVariantList(chat->horizSplit->sizes()));

			emit chatDestroying(chat->uins());
			Chats.remove(Chats.at(i));
			emit chatDestroyed(chat->uins());
			kdebugf2();
			return;
		}
	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "NOT found\n");
}

void ChatManager::refreshTitles()
{
	kdebugf();
	for (unsigned int i = 0; i < Chats.count(); ++i)
		Chats[i]->setTitle();
	kdebugf2();
}

void ChatManager::refreshTitlesForUin(UinType uin)
{
	kdebugf();
	for (unsigned int i = 0; i < Chats.count(); ++i)
		if (Chats[i]->uins().contains(uin))
			Chats[i]->setTitle();
	kdebugf2();
}

void ChatManager::changeAppearance()
{
	kdebugf();
	for (unsigned int i = 0; i < Chats.count(); ++i)
		Chats[i]->changeAppearance();
	kdebugf2();
}

Chat* ChatManager::findChatByUins(UinsList uins)
{
	for(unsigned int i=0; i<Chats.count(); ++i)
		if(Chats[i]->uins().equals(uins))
			return Chats[i];
	kdebugmf(KDEBUG_WARNING, "return NULL\n");
	return NULL;
}

int ChatManager::openChat(UinsList senders,time_t time)
{
	kdebugf();
	for (unsigned int i = 0; i < Chats.count(); ++i)
		if (Chats[i]->uins().equals(senders))
		{
#if QT_VERSION >= 0x030300
			Chats[i]->setWindowState(Chats[i]->windowState() & ~WindowMinimized);
#endif
			Chats[i]->raise();
			Chats[i]->setActiveWindow();
			return i;
		}
	QStringList uins=senders.toStringList();	uins.sort();
	Chat* chat = new Chat(senders, 0, QString("chat:%1").arg(uins.join(",")).local8Bit().data());
	chat->setTitle();

	QRect geometry=getChatProperty(senders, "Geometry").toRect();
	if (geometry.isEmpty())
	{
		QPoint pos = QCursor::pos();
		int x,y,width,height;
		QDesktopWidget *desk=qApp->desktop();
		x=pos.x()+50;
		y=pos.y()+50;
		height=400;

		if (senders.count()>1)
			width=550;
		else
			width=400;
		if (x+width>desk->width())
			x=desk->width()-width-50;
		if (y+height>desk->height())
			y=desk->height()-height-50;
		if (x<50) x=50;
		if (y<50) y=50;
		geometry.setX(x);
		geometry.setY(y);
		geometry.setWidth(width);
		geometry.setHeight(height);
	}
	chat->setGeometry(geometry);

	QValueList<int> vertSizes=toIntList(getChatProperty(senders, "VerticalSizes").toList());
	if (!vertSizes.empty())
		chat->vertSplit->setSizes(vertSizes);

	if (chat->horizSplit)
	{
		QValueList<int> horizSizes=toIntList(getChatProperty(senders, "HorizontalSizes").toList());
		if (!horizSizes.empty())
			chat->horizSplit->setSizes(horizSizes);
	}

	chat->show();
	chat->writeMessagesFromHistory(senders, time);
	emit chatCreated(senders);
	kdebugf2();
	return Chats.count()-1;
}

int ChatManager::openPendingMsg(int index, ChatMessage &msg)
{
	kdebugf();
	PendingMsgs::Element p = pending[index];
	// jesli ktoregos z nadawcow nie mamy na liscie to dodajemy
	// go tam jako anonymous
	for (unsigned int j = 0; j < p.uins.count(); ++j)
		if (!userlist.containsUin(p.uins[j]))
			userlist.addAnonymous(p.uins[j]);
	// otwieramy chat (jesli nie istnieje)
	int k = openChat(p.uins,p.time);
	// dopisujemy nowa wiadomosc do to_add

	QDateTime date;
	date.setTime_t(p.time);

	msg=ChatMessage(userlist.byUin(p.uins[0]).altNick(), p.msg, false, QDateTime::currentDateTime(), date);
	Chats[k]->formatMessage(msg);

	// kasujemy wiadomosc z pending
	pending.deleteMsg(index);
	// zwracamy indeks okna chat
	kdebugf2();
	return k;
}

void ChatManager::openPendingMsgs(UinsList uins)
{
	kdebugf();
	PendingMsgs::Element elem;
	int k;
	bool stop = false;

	QValueList<ChatMessage *> messages;
	for (int i = 0; i < pending.count(); ++i)
	{
		elem = pending[i];
		if (elem.uins.equals(uins))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| !elem.msgclass)
			{
				ChatMessage *msg=new ChatMessage("");
				k=openPendingMsg(i, *msg);
				messages.append(msg);

				--i;
				uins = elem.uins;
				stop = true;
			}
	}
	if (stop)
	{
		Chats[k]->scrollMessages(messages);
		UserBox::all_refresh();
	}
	else
		k = openChat(uins, 0);
	kdebugf2();
}

void ChatManager::openPendingMsgs()
{
	kdebugf();
	UinsList uins;
	int i, k = -1;
	PendingMsgs::Element elem;
	bool stop = false;
	QValueList<ChatMessage *> messages;

	for(i = 0; i<pending.count(); ++i)
	{
		elem = pending[i];
		if (!uins.count() || elem.uins.equals(uins))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| (!elem.msgclass))
			{
				if (!uins.count())
					uins = elem.uins;

				ChatMessage *msg=new ChatMessage("");
				k=openPendingMsg(i, *msg);
				messages.append(msg);

				--i;
				stop = true;
			}
	}
	if (stop)
	{
		kdebugmf(KDEBUG_INFO, "stopped\n");
		Chats[k]->scrollMessages(messages);
		UserBox::all_refresh();
	}
	kdebugf2();
}

void ChatManager::sendMessage(UinType uin,UinsList selected_uins)
{
	kdebugf();
	QString tmp;
	int i, k = -1;
	bool stop = false;
	PendingMsgs::Element elem;
	UinsList uins;
	QValueList<ChatMessage *> messages;

	for (i = 0; i < pending.count(); ++i)
	{
		elem = pending[i];
		if ((!uins.count() && elem.uins.contains(uin)) || (uins.count() && elem.uins.equals(uins)))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| !elem.msgclass)
			{
				if (!uins.count())
					uins = elem.uins;

				ChatMessage *msg=new ChatMessage("");
				k=openPendingMsg(i, *msg);
				messages.append(msg);

				--i;
				stop = true;
			}
	}
	if (stop)
	{
		Chats[k]->scrollMessages(messages);
		UserBox::all_refresh();
	}
	else
	{
		// zawsze otwieraja sie czaty
		uins = selected_uins;
		k = openChat(uins, 0);
	}
	kdebugf2();
}

void ChatManager::chatMsgReceived(UinsList senders, const QString& msg, time_t time, bool& grab)
{
	Chat* chat=findChatByUins(senders);
	if(chat!=NULL)
	{
		QValueList<ChatMessage *> messages;
		chat->checkPresence(senders, msg, time, messages);
		chat->alertNewMessage();
		grab=true;
	}
}

QVariant& ChatManager::getChatProperty(const UinsList &uins, const QString &name)
{
	kdebugf();
	for (QValueList<ChatInfo>::iterator it=addons.begin(); it!=addons.end(); it++)
		if ((*it).uins.equals(uins))
		{
			kdebugf2();
			return (*it).map[name];
		}
	ChatInfo info;
	info.uins=uins;
	info.map[name]=QVariant();
	addons.push_front(info);
	kdebugmf(KDEBUG_FUNCTION_END, "end: %s NOT found\n", name.local8Bit().data());
	return addons[0].map[name];
//	return addons[uins][name];
}

void ChatManager::setChatProperty(const UinsList &uins, const QString &name, const QVariant &value)
{
	kdebugf();
	for (QValueList<ChatInfo>::iterator it=addons.begin(); it!=addons.end(); it++)
		if ((*it).uins.equals(uins))
		{
			(*it).map[name]=value;
			kdebugf2();
			return;
		}
	ChatInfo info;
	info.uins=uins;
	info.map[name]=value;
	addons.push_front(info);
//	addons[uins][name]=value;
	kdebugf2();
}

ChatManager* chat_manager=NULL;

const char *colors[16] = {"#FF0000", "#A00000", "#00FF00", "#00A000", "#0000FF", "#0000A0", "#FFFF00",
	"#A0A000", "#FF00FF", "#A000A0", "#00FFFF", "#00A0A0", "#FFFFFF", "#A0A0A0", "#808080", "#000000"};

CustomInput::CustomInput(QWidget* parent, const char* name)
	: QMultiLineEdit(parent, name)
{
	kdebugf();
	QStyleSheet *style=styleSheet();
	style->item("p")->setMargin(QStyleSheetItem::MarginVertical, 0);
	setStyleSheet(style);
	kdebugf2();
}

void CustomInput::keyPressEvent(QKeyEvent* e)
{
//	kdebugf();
	emit keyPressed(e, this);
	if (autosend_enabled && ((HotKey::shortCut(e,"ShortCuts", "chat_newline")) || e->key()==Key_Enter)&& !(e->state() & ShiftButton))
	{
		kdebugmf(KDEBUG_INFO, "emit sendMessage()\n");
		emit sendMessage();
	}
	else
	{
		if (e->key() == Key_Minus)
		{
			insert("-");
			return;
		}
		if (e->text() == "*")
		{
			insert("*");
			return;
		}
		if (HotKey::shortCut(e,"ShortCuts", "chat_bold"))
		{
			emit specialKeyPressed(CustomInput::KEY_BOLD);
			return;
		}
		else if (HotKey::shortCut(e,"ShortCuts", "chat_italic"))
		{
			emit specialKeyPressed(CustomInput::KEY_ITALIC);
			return;
		}
		else if (HotKey::shortCut(e,"ShortCuts", "chat_underline"))
		{
			emit specialKeyPressed(CustomInput::KEY_UNDERLINE);
			return;
		}
		QMultiLineEdit::keyPressEvent(e);
	}
	// przekazanie event'a do qwidget
	// aby obsluzyc skroty klawiszowe (definiowane sa dla okna chat)
	QWidget::keyPressEvent(e);
//	kdebugf2();
}

void CustomInput::keyReleaseEvent(QKeyEvent* e)
{
	emit keyReleased(e, this);
	QWidget::keyReleaseEvent(e);
}

void CustomInput::setAutosend(bool on)
{
	autosend_enabled = on;
}

void CustomInput::paste()
{
	pasteSubType("plain");
}

QValueList<Chat::RegisteredButton> Chat::RegisteredButtons;

KaduSplitter::KaduSplitter(QWidget* parent, const char* name)
	: QSplitter (parent, name)
{
}

KaduSplitter::KaduSplitter(Orientation o, QWidget* parent, const char* name)
	: QSplitter(o,parent,name)
{
}

void KaduSplitter::drawContents(QPainter *p)
{
	QSplitter::drawContents(p);
	kdebugf();
	for (QValueList<KaduTextBrowser *>::iterator i=list.begin(); i!=list.end(); ++i)
		(*i)->viewport()->repaint();
//	kdebugf2();
}

void KaduSplitter::childEvent(QChildEvent *c)
{
	QSplitter::childEvent(c);
	kdebugf();
	QObject *o=c->child();
	if (o->inherits("KaduTextBrowser"))
	{
		if (c->inserted())
			list.append((KaduTextBrowser*)o);
		else
			list.remove((KaduTextBrowser*)o);
	}
//	kdebugm(KDEBUG_INFO, "%d %d %p %p %s %s\n", c->inserted(), c->removed(), this, o, o->className(), o->name());
}

ChatMessage::ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate)
{
	needsToBeFormatted=true;
	this->nick=nick;
	this->unformattedMessage=unformattedMessage;
	this->isMyMessage=myMessage;
	this->date=date;
	this->sdate=sdate;
}

ChatMessage::ChatMessage(const QString &formattedMessage, const QColor &bgColor, const QColor &txtColor)
{
	needsToBeFormatted=false;
	message=formattedMessage;
	backgroundColor=bgColor;
	textColor=txtColor;
}


Chat::Chat(UinsList uins, QWidget* parent, const char* name)
	: QWidget(parent, name, Qt::WDestructiveClose), Uins(uins)
{
	kdebugf();
	QValueList<int> sizes;

	emoticon_selector = NULL;
	color_selector = NULL;

	title_timer = new QTimer(this, "title_timer");
	connect(title_timer,SIGNAL(timeout()),this,SLOT(changeTitle()));

	ParagraphSeparator=config_file.readNumEntry("General", "ParagraphSeparator");

	/* register us in the chats registry... */
	index=chat_manager->registerChat(this);

	vertSplit = new KaduSplitter(Qt::Vertical, this, "vertSplit");

	if (uins.count() > 1)
	{
		horizSplit = new KaduSplitter(Qt::Horizontal, vertSplit, "horizSplit");
		body = new KaduTextBrowser(horizSplit, "body");
	}
	else
	{
		horizSplit=NULL;
		body = new KaduTextBrowser(vertSplit, "body");
	}

	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")==EMOTS_ANIMATED)
		body->setStyleSheet(new AnimStyleSheet(body,emoticons->themePath()));
	else
		body->setStyleSheet(new StaticStyleSheet(body,emoticons->themePath()));

	if (config_file.readBoolEntry("General", "UseParagraphs"))
		body->setMargin(ParagraphSeparator);
	body->setMinimumSize(QSize(100,100));
	body->setFont(config_file.readFontEntry("Look","ChatFont"));

	QPoint pos = QCursor::pos();

	if (uins.count() > 1)
	{
		userbox = new UserBox(horizSplit, "userbox");
		userbox->setMinimumSize(QSize(30,30));
		userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
		userbox->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
		userbox->QListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));

		for (unsigned i = 0; i < uins.count(); ++i)
			userbox->addUser(userlist.byUin(uins[i]).altNick());
		userbox->refresh();

		connect(userbox, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)),
		UserBox::userboxmenu, SLOT(show(QListBoxItem *)));

		sizes.append(3);
		sizes.append(1);
		horizSplit->setSizes(sizes);
	}
	else
		userbox = NULL;

	QVBox *downpart = new QVBox(vertSplit, "downpartBox");
	QHBox *edtbuttontray = new QHBox(downpart, "edtbuttontrayBox");

	QLabel *edt = new QLabel(tr("Edit window:"), edtbuttontray, "editLabel");
	QToolTip::add(edt, tr("This is where you type in the text to be sent"));

	buttontray = new QHBox(edtbuttontray, "buttontrayBox");

	autosend = new QPushButton(buttontray, "autoSendButton");
	autosend->setPixmap(icons_manager.loadIcon("AutoSendMessage"));
	autosend->setToggleButton(true);
	QToolTip::add(autosend, tr("Enter key sends message"));

	lockscroll = new QPushButton(buttontray, "lockScrollButton");
	lockscroll->setPixmap(icons_manager.loadIcon("ScrollLock"));
	lockscroll->setToggleButton(true);
	QToolTip::add(lockscroll, tr("Blocks scrolling"));

	for(unsigned int i=0; i<RegisteredButtons.size(); ++i)
	{
		RegisteredButton& b=RegisteredButtons[i];
		QPushButton* btn=new QPushButton(buttontray,b.name.local8Bit().data());
		connect(btn, SIGNAL(clicked()), b.receiver, b.slot.local8Bit().data());
		Buttons.insert(b.name,btn);
	}

	QPushButton *clearchat= new QPushButton(buttontray, "clearChatButton");
	clearchat->setPixmap(icons_manager.loadIcon("ClearChat"));
	QToolTip::add(clearchat, tr("Clear messages in chat window"));

	iconsel = new QPushButton(buttontray, "selectIconButton");
	iconsel->setPixmap(icons_manager.loadIcon("ChooseEmoticon"));
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")==EMOTS_NONE)
	{
		QToolTip::add(iconsel, tr("Insert emoticon - enable in configuration"));
		iconsel->setEnabled(false);
	}
	else
		QToolTip::add(iconsel, tr("Insert emoticon"));

	QPushButton *history = new QPushButton(buttontray, "showHistoryButton");
	history->setPixmap(icons_manager.loadIcon("History"));
	QToolTip::add(history, tr("Show history"));

	QPushButton *whois = new QPushButton(buttontray, "whoisButton");
	whois->setPixmap(icons_manager.loadIcon("LookupUserInfo"));
	QToolTip::add(whois, tr("Lookup user info"));

	QPushButton* insertimage = new QPushButton(buttontray, "insertImageButton");
	insertimage->setPixmap(icons_manager.loadIcon("ChooseImage"));
	QToolTip::add(insertimage, tr("Insert image"));

	edtbuttontray->setStretchFactor(edt, 50);
	edtbuttontray->setStretchFactor(buttontray, 1);

	edit = new CustomInput(downpart, "edit");
	edit->setMinimumHeight(1);
	edit->setWordWrap(QMultiLineEdit::WidgetWidth);
	edit->setFont(config_file.readFontEntry("Look","ChatFont"));
	connect(body, SIGNAL(mouseReleased(QMouseEvent *, KaduTextBrowser *)), edit, SLOT(setFocus()));

	if (config_file.readBoolEntry("Chat","AutoSend"))
		autosend->setOn(true);
	edit->setAutosend(config_file.readBoolEntry("Chat","AutoSend"));

	QHBox *btnpart = new QHBox(downpart, "buttonpartBox");

	QFont afont = QApplication::font();
	QSize s=QFontMetrics(afont).size(0, "B")*6;

	boldbtn = new QPushButton("B", btnpart, "boldButton");
	boldbtn->setToggleButton(true);
	afont.setBold(true);
	boldbtn->setFont(afont);
	boldbtn->setMaximumSize(s);

	italicbtn = new QPushButton("I", btnpart, "italicButton");
	italicbtn->setToggleButton(true);
	afont.setBold(false);
	afont.setItalic(true);
	italicbtn->setFont(afont);
	italicbtn->setMaximumSize(s);

	underlinebtn = new QPushButton("U", btnpart, "underlineButton");
	underlinebtn->setToggleButton(true);
	afont.setItalic(false);
	afont.setUnderline(true);
	underlinebtn->setFont(afont);
	underlinebtn->setMaximumSize(s);

	colorbtn = new QPushButton(btnpart, "colorButton");
//	colorbtn->setMinimumSize(boldbtn->width(), boldbtn->height());
	QPixmap p(16, 16);
	actcolor=edit->paletteForegroundColor();
	p.fill(actcolor);
	colorbtn->setPixmap(p);

	(new QWidget(btnpart, "spacer"))->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	sendbtn = new QPushButton(QIconSet(icons_manager.loadIcon("SendMessage")),tr("&Send"),btnpart, "sendButton");
	sendbtn->setFixedWidth(120);
	connect(sendbtn, SIGNAL(clicked()), this, SLOT(sendMessage()));
	QAccel *acc = new QAccel(this, "returnAccel");
	acc->connectItem(acc->insertItem(Key_Return + CTRL), this, SLOT(sendMessage()));

	acc = new QAccel(this, "pageUpAccel");
	acc->connectItem(acc->insertItem(Key_PageUp + SHIFT), this, SLOT(pageUp()));
	acc = new QAccel(this, "pageDownAccel");
	acc->connectItem(acc->insertItem(Key_PageDown + SHIFT), this, SLOT(pageDown()));

	sizes.clear();
	sizes.append(3);
	sizes.append(2);
	vertSplit->setSizes(sizes);

	QGridLayout *grid = new QGridLayout (this, 5, 4, 3, 3, "main_grid_layout");
	grid->addMultiCellWidget(vertSplit, 0, 4, 0, 3);
	grid->addRowSpacing(1, 5);
	grid->setRowStretch(0, 2);

	QMimeSourceFactory *bodyformat = new QMimeSourceFactory();

	body->setMimeSourceFactory(bodyformat);
	body->setTextFormat(Qt::RichText);
	edit->setMimeSourceFactory(bodyformat);
	edit->setTextFormat(Qt::RichText);

	connect(autosend, SIGNAL(toggled(bool)), edit, SLOT(setAutosend(bool)));
	connect(history, SIGNAL(clicked()), this, SLOT(HistoryBox()));
	connect(iconsel, SIGNAL(clicked()), this, SLOT(emoticonSelectorClicked()));
	connect(whois, SIGNAL(clicked()), this, SLOT(userWhois()));
	connect(insertimage, SIGNAL(clicked()), this, SLOT(insertImage()));
	connect(clearchat, SIGNAL(clicked()), this, SLOT(clearChatWindow()));
	connect(boldbtn, SIGNAL(toggled(bool)), this, SLOT(toggledBold(bool)));
	connect(italicbtn, SIGNAL(toggled(bool)), this, SLOT(toggledItalic(bool)));
	connect(underlinebtn, SIGNAL(toggled(bool)), this, SLOT(toggledUnderline(bool)));
	connect(colorbtn, SIGNAL(clicked()), this, SLOT(changeColor()));
	connect(edit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(curPosChanged(int, int)));
	connect(edit, SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(edit, SIGNAL(specialKeyPressed(int)), this, SLOT(specialKeyPressed(int)));

	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)),
		this, SLOT(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)));

	edit->setFocus();
	kdebugf2();
}

Chat::~Chat()
{
	kdebugf();
	chat_manager->unregisterChat(this);

	disconnectAcknowledgeSlots();
	disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)),
		this, SLOT(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)));

	for(QValueList<ChatMessage *>::iterator it=ChatMessages.begin(); it!=ChatMessages.end(); ++it)
		delete *it;
	ChatMessages.clear();

	if (userbox)
		delete userbox;

	kdebugmf(KDEBUG_FUNCTION_END, "chat destroyed: index %d\n", index);
}

void Chat::registerButton(const QString& name,QObject* receiver,const QString& slot)
{
	kdebugf();
	RegisteredButton b;
	b.name=name;
	b.receiver=receiver;
	b.slot=slot;
	RegisteredButtons.append(b);
	kdebugf2();
}

void Chat::unregisterButton(const QString& name)
{
	kdebugf();
	for(unsigned int i=0; i<RegisteredButtons.size(); ++i)
		if(RegisteredButtons[i].name==name)
		{
			RegisteredButtons.remove(RegisteredButtons.at(i));
			break;
		}
	for(unsigned int i=0; i<chat_manager->chats().size(); ++i)
	{
		Chat* chat=chat_manager->chats()[i];
		if(chat->Buttons.contains(name))
		{
			delete chat->Buttons[name];
			chat->Buttons.remove(name);
		}
	}
	kdebugf2();
}

QPushButton* Chat::button(const QString& name)
{
	if(Buttons.contains(name))
		return Buttons[name];

	kdebugmf(KDEBUG_WARNING, " '%s' - return NULL\n", name.local8Bit().data());
	return NULL;
}

void Chat::specialKeyPressed(int key)
{
	kdebugf();
	switch (key)
	{
		case CustomInput::KEY_BOLD:
			boldbtn->setOn(!boldbtn->isOn());
			edit->setBold(boldbtn->isOn());
			break;
		case CustomInput::KEY_ITALIC:
			italicbtn->setOn(!italicbtn->isOn());
			edit->setItalic(italicbtn->isOn());
			break;
		case CustomInput::KEY_UNDERLINE:
			underlinebtn->setOn(!underlinebtn->isOn());
			edit->setUnderline(underlinebtn->isOn());
			break;
	}
	kdebugf2();
}

void Chat::toggledBold(bool on)
{
	kdebugf();
	edit->setBold(on);
}

void Chat::toggledItalic(bool on)
{
	kdebugf();
	edit->setItalic(on);
}

void Chat::toggledUnderline(bool on)
{
	kdebugf();
	edit->setUnderline(on);
}

void Chat::curPosChanged(int, int)
{
	int i;

	kdebugf();
	if (edit->bold() != boldbtn->isOn())
		boldbtn->setOn(edit->bold());
	if (edit->italic() != italicbtn->isOn())
		italicbtn->setOn(edit->italic());
	if (edit->underline() != underlinebtn->isOn())
		underlinebtn->setOn(edit->underline());
	if (edit->color() != actcolor)
	{
		for (i = 0; i < 16; ++i)
			if (edit->color() == QColor(colors[i]))
				break;
		QPixmap p(16, 16);
		if (i >= 16)
			actcolor = edit->paletteForegroundColor();
		else
			actcolor = colors[i];
		p.fill(actcolor);
		colorbtn->setPixmap(p);
	}
	kdebugf2();
}

void Chat::pageUp()
{
	body->scrollBy(0, (body->height() * -2) / 3);
}

void Chat::pageDown()
{
	body->scrollBy(0, (body->height() * 2) / 3);
}

void Chat::insertImage()
{
	kdebugf();
	ImageDialog* id = new ImageDialog(this);
	id->setDir(config_file.readEntry("Chat", "LastImagePath", "~"));
	id->setCaption(tr("Insert image"));
	if(id->exec() == QDialog::Accepted)
	{
		edit->insert(QString("[IMAGE ")+id->selectedFile()+"]");
		config_file.writeEntry("Chat", "LastImagePath", id->dirPath());
	}
	delete id;
	edit->setFocus();
	kdebugf2();
}

void Chat::imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& /*path*/)
{
	kdebugf();
	for (QValueList<ChatMessage*>::const_iterator i = ChatMessages.begin(); i != ChatMessages.end(); i++)
		(*i)->message =
			gadu_images_manager.replaceLoadingImages(
				(*i)->message,sender,size,crc32);
	repaintMessages();
	kdebugf2();
}

void Chat::changeAppearance()
{
	kdebugf();
	if (Uins.count() > 1 && userbox)
	{
		userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
		userbox->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
		userbox->QListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));
	}
	body->setFont(config_file.readFontEntry("Look","ChatFont"));
	edit->setFont(config_file.readFontEntry("Look","ChatFont"));
	kdebugf2();
}

void Chat::setTitle()
{
	kdebugf();
	QString title;

	kdebugmf(KDEBUG_FUNCTION_START, "Uins.size() = %d\n", Uins.size());
	if (Uins.size() > 1)
	{
		if (config_file.readEntry("Look","ConferencePrefix").isEmpty())
			title = tr("Conference with ");
		else
			title = config_file.readEntry("Look","ConferencePrefix");
		title.append(parse(config_file.readEntry("Look","ConferenceContents"),userlist.byUinValue(Uins[0]),false));
		for (unsigned int k = 1; k < Uins.size(); ++k)
		{
			title.append(", ");
			title.append(parse(config_file.readEntry("Look","ConferenceContents"),userlist.byUinValue(Uins[k]),false));
		}
		setIcon(icons_manager.loadIcon("Online"));
	}
	else
	{
		if (config_file.readEntry("Look","ChatContents").isEmpty())
			title = parse(tr("Chat with ")+"%a (%s[: %d])",userlist.byUinValue(Uins[0]),false);
		else
			title = parse(config_file.readEntry("Look","ChatContents"),userlist.byUinValue(Uins[0]),false);
		setIcon(userlist.byUinValue(Uins[0]).status().pixmap());
	}

	title.replace(QRegExp("<br/>"), " ");
	title.replace(QRegExp("&nbsp;"), " ");

	setCaption(title);
	title_buffer = title;
	kdebugf2();
}

void Chat::changeTitle()
{
	if(!isActiveWindow())
	{
		if (caption() != title_buffer)
			setCaption(title_buffer);
		else
			setCaption(QString().fill(' ', title_buffer.length()));
		title_timer->start(500,TRUE);
	}
}

void Chat::windowActivationChange(bool)
{
	if (isActiveWindow() && title_timer->isActive())
	{
		title_timer->stop();
		setCaption(title_buffer);
	}
}

void Chat::keyPressEvent(QKeyEvent* e)
{
	if (HotKey::shortCut(e,"ShortCuts", "chat_clear"))
		clearChatWindow();
	else if (HotKey::shortCut(e,"ShortCuts", "chat_close"))
		close();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_viewhistory"))
		HistoryBox();
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
		userWhois();
	QWidget::keyPressEvent(e);
}

void Chat::mouseReleaseEvent(QMouseEvent *e)
{
	kdebugf();
	edit->setFocus();
	QWidget::mouseReleaseEvent(e);
}

/* convert special characters into emoticons, HTML into plain text and so forth */
QString Chat::convertCharacters(QString edit, bool me)
{
	// zmieniamy windowsowe \r\n na unixowe \n
	edit.replace( QRegExp("\r\n"), "<br/>" );
	edit.replace( QRegExp("\n"), "<br/>" );

	HtmlDocument doc;
	doc.parseHtml(edit);

	// detekcja adresow url
	doc.convertUrlsToHtml();

	QColor bgcolor;
	if (me)
		bgcolor=config_file.readColorEntry("Look","ChatMyBgColor");
	else
		bgcolor=config_file.readColorEntry("Look","ChatUsrBgColor");

	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")!=EMOTS_NONE)
	{
		body->mimeSourceFactory()->addFilePath(emoticons->themePath());
		emoticons->expandEmoticons(doc, bgcolor);
	}

	GaduImagesManager::setBackgroundsForAnimatedImages(doc, bgcolor);
	edit=doc.generateHtml();
	return edit;
}

/* unregister us */
void Chat::closeEvent(QCloseEvent* e)
{
	kdebugf();
	QWidget::closeEvent(e);
}

/* look up party's info */
void Chat::userWhois()
{
	kdebugf();
	UinType uin;

	if (!userbox)
		uin = Uins[0];
	else
		if (userbox->currentItem() == -1)
			uin = Uins[0];
		else
			uin = userlist.byAltNick(userbox->currentText()).uin();
	SearchDialog *sd = new SearchDialog(0, QString("SearchDialog:%1").arg(uin).local8Bit().data(), uin);
	sd->show();
	sd->firstSearch();
	kdebugf2();
}

void Chat::formatMessage(ChatMessage &msg)
{
	QString formatString;
	if (config_file.readBoolEntry("General", "UseParagraphs"))
		formatString="<p style=\"background-color: %1\"><img title=\"\" height=\"%6\" width=\"10000\" align=\"right\"><font color=\"%2\"><b>%3 :: %4</b><br/>%5</font></p>";
	else
		formatString="<table width=\"100%\"><tr><td bgcolor=\"%1\"><font color=\"%2\"><b>%3 :: %4</b><br/>%5</font></td></tr></table>";

	if (msg.isMyMessage)
		msg.backgroundColor=config_file.readColorEntry("Look","ChatMyBgColor");
	else
		msg.backgroundColor=config_file.readColorEntry("Look","ChatUsrBgColor");
	if (msg.isMyMessage)
		msg.textColor=config_file.readColorEntry("Look","ChatMyFontColor");
	else
		msg.textColor=config_file.readColorEntry("Look","ChatUsrFontColor");

	QString date=printDateTime(msg.date);
	if (!msg.sdate.isNull())
		date.append(" / S "+printDateTime(msg.sdate));

	QString nick = msg.nick;
	HtmlDocument::escapeText(nick);

	msg.message=formatString
			.arg(msg.backgroundColor.name())
			.arg(msg.textColor.name())
			.arg(nick)
			.arg(date)
			.arg(convertCharacters(msg.unformattedMessage, msg.isMyMessage));
	if (config_file.readBoolEntry("General", "UseParagraphs"))
		msg.message=msg.message.arg(ParagraphSeparator);

	msg.needsToBeFormatted=false;
}

void Chat::repaintMessages()
{
	kdebugf();
	body->viewport()->setUpdatesEnabled(false);

	QString text;
	int i;
	if (config_file.readBoolEntry("Chat","ScrollDown"))
	{
		QValueList<ChatMessage *>::const_iterator it=ChatMessages.begin();
		//z pierwszej wiadomo¶ci usuwamy obrazek separatora
		if (it!=ChatMessages.end())
		{
			QString msg=(*it)->message;
			msg.replace(QRegExp("<img title=\"\" height=\"[0-9]*\" width=\"10000\" align=\"right\">"), "");
			text+=msg;
			++it;
		}
		for(; it!=ChatMessages.end(); ++it)
			text+=(*it)->message;
		body->setText(text);

		if (config_file.readBoolEntry("General", "UseParagraphs"))
		{
			i=0;
			for(QValueList<ChatMessage *>::const_iterator it=ChatMessages.begin(); it!=ChatMessages.end(); ++it, ++i)
				body->setParagraphBackgroundColor(i, (*it)->backgroundColor);
		}

		if (!lockscroll->isOn())
			body->scrollToBottom();
	}
	else
	{
		QValueList<ChatMessage *>::const_iterator it=ChatMessages.begin();
		if (it!=ChatMessages.end())
		{
			QString msg=(*it)->message;
			msg.replace(QRegExp("<img title=\"\" height=\"[0-9]*\" width=\"10000\" align=\"right\">"), "");
			text+=msg;
			++it;
		}
		for(; it!=ChatMessages.end(); ++it)
			text=(*it)->message+text;
		body->setText(text);
		if (config_file.readBoolEntry("General", "UseParagraphs"))
		{
			i=ChatMessages.size()-1;
			for(QValueList<ChatMessage *>::const_iterator it=ChatMessages.begin(); it!=ChatMessages.end(); ++it, --i)
				body->setParagraphBackgroundColor(i, (*it)->backgroundColor);
		}
	}

	body->viewport()->setUpdatesEnabled(true);
	body->viewport()->repaint();
	kdebugf2();
}

void Chat::scrollMessages(const QValueList<ChatMessage *> &messages)
{
	kdebugf();
	if (config_file.readBoolEntry("Chat","ChatPrune"))
		pruneWindow();
	ChatMessages+=messages;
	repaintMessages();
	kdebugf2();
}

void Chat::writeMessagesFromHistory(UinsList senders, time_t time)
{
	kdebugf();
	QValueList<HistoryEntry> entries;
	QValueList<HistoryEntry> entriestmp;
	QDateTime date;
	unsigned int from, end, count;

	date.setTime_t(time);
	count = history.getHistoryEntriesCount(senders);
	end = count - 1;

	from = count;
	while (from >= 1 && entries.count() < config_file.readUnsignedNumEntry("History","ChatHistoryCitation"))
	{
		if (end < config_file.readUnsignedNumEntry("History", "ChatHistoryCitation"))
			from = 0;
		else
			from = end - config_file.readUnsignedNumEntry("History","ChatHistoryCitation") + 1;

		entriestmp = history.getHistoryEntries(senders, from, end - from + 1, HISTORYMANAGER_ENTRY_CHATSEND
			| HISTORYMANAGER_ENTRY_MSGSEND | HISTORYMANAGER_ENTRY_CHATRCV | HISTORYMANAGER_ENTRY_MSGRCV);
		kdebugmf(KDEBUG_INFO, "temp entries = %d\n", entriestmp.count());
		if (time)
		{
			QValueList<HistoryEntry>::iterator it = entriestmp.begin();
			while (it != entriestmp.end())
			{
				if ((*it).type == HISTORYMANAGER_ENTRY_CHATRCV
					|| (*it).type == HISTORYMANAGER_ENTRY_MSGRCV)
				{
					kdebugmf(KDEBUG_INFO, "%s %s\n",
						(const char *)date.toString("dd.MM.yyyy hh:mm:ss").local8Bit(),
						(const char *)(*it).sdate.toString("dd.MM.yyyy hh:mm:ss").local8Bit());
					if (date <= (*it).sdate)
						it = entriestmp.remove(it);
					else
						++it;
				}
				else
					++it;
			}
		}
		if (entriestmp.count())
			entries = entriestmp + entries;
		kdebugmf(KDEBUG_INFO, "entries = %d\n", entries.count());
		end = from - 1;
	}
	if (entries.count() < config_file.readUnsignedNumEntry("History","ChatHistoryCitation"))
		from = 0;
	else
		from = entries.count() - config_file.readUnsignedNumEntry("History","ChatHistoryCitation");

	QValueList<ChatMessage *> messages;

	for (unsigned int i = from; i < entries.count(); ++i)
		if (entries[i].date.secsTo(QDateTime::currentDateTime()) <= -config_file.readNumEntry("History","ChatHistoryQuotationTime") * 3600)
		{
			ChatMessage *msg;
			if (entries[i].type == HISTORYMANAGER_ENTRY_MSGSEND	|| entries[i].type == HISTORYMANAGER_ENTRY_CHATSEND)
				msg=new ChatMessage(config_file.readEntry("General","Nick"), entries[i].message, true, entries[i].date);
			else
				msg=new ChatMessage(entries[i].nick, entries[i].message, false, entries[i].date, entries[i].sdate);
			formatMessage(*msg);
			messages.append(msg);
		}
	if (!messages.empty())
		scrollMessages(messages);
	kdebugf2();
}

/* invoked from outside when new message arrives, this is the window to the world */
void Chat::checkPresence(UinsList senders, const QString &msg, time_t time, QValueList<ChatMessage *> &messages)
{
	QDateTime date;
	date.setTime_t(time);

	ChatMessage *message=new ChatMessage(userlist.byUin(senders[0]).altNick(), msg, false, QDateTime::currentDateTime(), date);
	formatMessage(*message);
	messages.append(message);

	scrollMessages(messages);
}

void Chat::alertNewMessage()
{
	if (config_file.readBoolEntry("Chat","BlinkChatTitle"))
		if (!isActiveWindow() && !title_timer->isActive())
			changeTitle();
}

void Chat::writeMyMessage()
{
	kdebugf();
	QValueList<ChatMessage *> messages;
	ChatMessage *msg=new ChatMessage(config_file.readEntry("General","Nick"), myLastMessage, true, QDateTime::currentDateTime());
	formatMessage(*msg);
	messages.append(msg);
	scrollMessages(messages);

	if (!edit->isEnabled())
		cancelMessage();
	edit->clear();
	if (boldbtn->isOn())
		edit->setBold(true);
	if (italicbtn->isOn())
		edit->setItalic(true);
	if (underlinebtn->isOn())
		edit->setUnderline(true);
	kdebugf2();
}

void Chat::addMyMessageToHistory()
{
	history.addMyMessage(Uins, myLastMessage);
}

void Chat::clearChatWindow()
{
	kdebugf();
	for(QValueList<ChatMessage *>::iterator it=ChatMessages.begin(); it!=ChatMessages.end(); ++it)
		delete *it;
	ChatMessages.clear();
	body->clear();
	kdebugf2();
}

void Chat::cancelMessage()
{
	kdebugf();
	seq = 0;
	disconnectAcknowledgeSlots();
	edit->setReadOnly(false);
	edit->setEnabled(true);
	edit->setFocus();
	disconnect(sendbtn, SIGNAL(clicked()), this, SLOT(cancelMessage()));
	connect(sendbtn, SIGNAL(clicked()), this, SLOT(sendMessage()));
	sendbtn->setIconSet(QIconSet(icons_manager.loadIcon("SendMessage")));
	sendbtn->setText(tr("&Send"));
	kdebugf2();
}

void Chat::messageBlockedSlot(int Seq, UinType uin)
{
	kdebugf();
	if (seq != Seq)
		return;
	kdebugmf(KDEBUG_INFO, "This is my ack.\n");
	MessageBox::wrn(tr("Your message has been blocked by server. Message has not been delivered."), true);
	cancelMessage();
	kdebugf2();
}

void Chat::messageBoxFullSlot(int Seq, UinType uin)
{
	kdebugf();
	if (seq != Seq)
		return;
	kdebugmf(KDEBUG_INFO, "This is my ack.\n");
	MessageBox::wrn(tr("User's message box is full. Message has not been delivered."), true);
	cancelMessage();
	kdebugf2();
}

void Chat::messageNotDeliveredSlot(int Seq, UinType uin)
{
	kdebugf();
	if (seq != Seq)
		return;
	kdebugmf(KDEBUG_INFO, "This is my ack.\n");
	MessageBox::wrn(tr("Message has not been delivered."), true);
	cancelMessage();
	kdebugf2();
}

void Chat::messageAcceptedSlot(int Seq, UinType uin)
{
	kdebugf();
	if (seq != Seq)
		return;
	kdebugmf(KDEBUG_INFO, "This is my ack.\n");
	writeMyMessage();
	addMyMessageToHistory();
	seq = 0;
	disconnectAcknowledgeSlots();
	kdebugf2();
}

void Chat::connectAcknowledgeSlots()
{
	kdebugf();
	connect(gadu, SIGNAL(messageBlocked(int, UinType)), this, SLOT(messageBlockedSlot(int, UinType)));
	connect(gadu, SIGNAL(messageBoxFull(int, UinType)), this, SLOT(messageBoxFullSlot(int, UinType)));
	connect(gadu, SIGNAL(messageNotDelivered(int, UinType)), this, SLOT(messageNotDeliveredSlot(int, UinType)));
	connect(gadu, SIGNAL(messageAccepted(int, UinType)), this, SLOT(messageAcceptedSlot(int, UinType)));
	kdebugf2();
}

void Chat::disconnectAcknowledgeSlots()
{
	kdebugf();
	disconnect(gadu, SIGNAL(messageBlocked(int, UinType)), this, SLOT(messageBlockedSlot(int, UinType)));
	disconnect(gadu, SIGNAL(messageBoxFull(int, UinType)), this, SLOT(messageBoxFullSlot(int, UinType)));
	disconnect(gadu, SIGNAL(messageNotDelivered(int, UinType)), this, SLOT(messageNotDeliveredSlot(int, UinType)));
	disconnect(gadu, SIGNAL(messageAccepted(int, UinType)), this, SLOT(messageAcceptedSlot(int, UinType)));
	kdebugf2();
}

/* sends the message typed */
void Chat::sendMessage()
{
	kdebugf();
	if (edit->text() == "")
	{
		kdebugf2();
		return;
	}

	emit messageSendRequested(this);

	QString mesg;

	if (gadu->status().isOffline()) {
		QMessageBox::critical(this, tr("Send message error"),
			tr("Application encountered network error."));
		kdebugmf(KDEBUG_FUNCTION_END, "not connected!\n");
		return;
	}

	myLastMessage = edit->text();

	mesg = myLastMessage;
	mesg.replace(QRegExp("\n"), "\r\n");
	mesg = unformatGGMessage(mesg, myLastFormatsLength, myLastFormats);
	myLastMessage = mesg;
	if (myLastFormatsLength)
		myLastMessage = formatGGMessage(myLastMessage, myLastFormatsLength - sizeof(struct gg_msg_richtext),
			(void *)((char *)(myLastFormats) + sizeof(struct gg_msg_richtext)),0);
	else
		HtmlDocument::escapeText(myLastMessage);
	kdebugmf(KDEBUG_INFO, "\n%s\n", (const char *)unicode2latin(myLastMessage));
	// zmieniamy unixowe \n na windowsowe \r\n
	myLastMessage.replace(QRegExp("\r\n"), "\n");

	if (mesg.length() >= 2000)
	{
		MessageBox::wrn(tr("Message too long (%1>=%2)").arg(mesg.length()).arg(2000));
		kdebugmf(KDEBUG_FUNCTION_END, "end: message too long\n");
		return;
	}

	QCString msg = unicode2cp(mesg);

	bool stop=false;
	emit messageFiltering(Uins,msg,stop);
	if(stop)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: filter stopped processing\n");
		return;
	}

	if (mesg.length() >= 2000)
	{
		MessageBox::wrn(tr("Filtered message too long (%1>=%2)").arg(mesg.length()).arg(2000));
		kdebugmf(KDEBUG_FUNCTION_END, "end: filtered message too long\n");
		return;
	}

	if (config_file.readBoolEntry("Chat","MessageAcks")) {
		edit->setReadOnly(true);
		edit->setEnabled(false);
		disconnect(sendbtn, SIGNAL(clicked()), this, SLOT(sendMessage()));
		connect(sendbtn, SIGNAL(clicked()), this, SLOT(cancelMessage()));
		sendbtn->setIconSet(QIconSet(icons_manager.loadIcon("CancelMessage")));
		sendbtn->setText(tr("&Cancel"));
	}

	if (myLastFormatsLength)
		seq = gadu->sendMessageRichText(Uins, msg, (unsigned char *)myLastFormats, myLastFormatsLength);
	else
		seq = gadu->sendMessage(Uins, msg);

	if (myLastFormats)
		delete [](char *)myLastFormats;

 	if (config_file.readBoolEntry("Chat","MessageAcks"))
		connectAcknowledgeSlots();
	else
	{
		writeMyMessage();
		addMyMessageToHistory();
	}

	emit messageSent(this);
	kdebugf2();
}

/* prunes messages */
void Chat::pruneWindow()
{
	kdebugf();
	unsigned int chatPruneLen=config_file.readUnsignedNumEntry("Chat","ChatPruneLen");

	if (ChatMessages.size()<chatPruneLen)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: nothing to do\n");
		return;
	}
	QValueList<ChatMessage *>::iterator start=ChatMessages.begin();
	QValueList<ChatMessage *>::iterator stop=ChatMessages.at(ChatMessages.size()-chatPruneLen+1);
	for(QValueList<ChatMessage *>::iterator it=start; it!=stop; ++it)
		delete *it;
	ChatMessages.erase(start, stop);

	kdebugf2();
}

/* opens messages history */
void Chat::HistoryBox()
{
	kdebugf();
	(new History(Uins))->show();
	kdebugf2();
}

void Chat::emoticonSelectorClicked()
{
	//emoticons_selector zawsze bêdzie NULLem gdy wchodzimy do tej funkcji
	//bo EmoticonSelector ma ustawione flagi Qt::WDestructiveClose i Qt::WType_Popup
	//akcj± na opuszczenie okna jest ustawienie zmiennej emoticons_selector w Chacie na NULL
	emoticon_selector = new EmoticonSelector(NULL, "emoticon_selector", this);
	emoticon_selector->alignTo(iconsel);
	emoticon_selector->show();
}

void Chat::changeColor()
{
	//sytuacja podobna jak w przypadku emoticon_selectora
	color_selector = new ColorSelector(edit->paletteForegroundColor(), this, "color_selector");
	color_selector->alignTo(colorbtn);
	color_selector->show();
	connect(color_selector, SIGNAL(colorSelect(const QColor&)), this, SLOT(colorChanged(const QColor&)));
	connect(color_selector, SIGNAL(aboutToClose()), this, SLOT(colorSelectorAboutToClose()));
}

void Chat::colorSelectorAboutToClose()
{
	kdebugf();
	color_selector = NULL;
	kdebugf2();
}

void Chat::colorChanged(const QColor& color)
{
	color_selector = NULL;
	QPixmap p(16, 16);
	p.fill(color);
	colorbtn->setPixmap(p);
	edit->setColor(color);
	actcolor = color;
}

/* adds an emoticon code to the edit window */
void Chat::addEmoticon(QString string)
{
	if (string.length())
	{
		string.replace(QRegExp("&lt;"), "<");
		string.replace(QRegExp("&gt;"), ">");
		edit->insert(string);
		edit->setFocus();
	}
	emoticon_selector = NULL;
}

void Chat::initModule()
{
	kdebugf();

	ConfigDialog::addTab("ShortCuts", "ShortCutsTab");
	ConfigDialog::addVGroupBox("ShortCuts", "ShortCuts", QT_TRANSLATE_NOOP("@default", "Define keys"));
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "New line / send message:"), "chat_newline", "Return");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Clear Chat:"), "chat_clear", "F9");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Close Chat:"), "chat_close", "Esc");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Bold text:"), "chat_bold", "Ctrl+B");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Italic text:"), "chat_italic", "Ctrl+I");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Underline text:"), "chat_underline", "Ctrl+U");

	ConfigDialog::addTab("Chat","ChatTab");
	ConfigDialog::addVGroupBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Emoticons"));
	ConfigDialog::addComboBox("Chat", "Emoticons", QT_TRANSLATE_NOOP("@default", "Emoticons:"),
			"EmoticonsStyle", toStringList(tr("None"), tr("Static"), tr("Animated")), toStringList("0", "1", "2"), "2");

	ConfigDialog::addComboBox("Chat", "Emoticons", QT_TRANSLATE_NOOP("@default", "Emoticons theme"));
	ConfigDialog::addVGroupBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "WWW options"));

	ConfigDialog::addComboBox("Chat", "WWW options", QT_TRANSLATE_NOOP("@default", "Choose your browser"));
	ConfigDialog::addComboBox("Chat", "WWW options", QT_TRANSLATE_NOOP("@default", "Browser options"));
	ConfigDialog::addLineEdit("Chat", "WWW options", QT_TRANSLATE_NOOP("@default", "Custom Web browser"), "WebBrowser", "", QT_TRANSLATE_NOOP("@default", "%1 - Url clicked in chat window"));
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Automatically prune chat messages"), "ChatPrune", true);
	ConfigDialog::addHGroupBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Message pruning"));
	ConfigDialog::addSpinBox("Chat", "Message pruning", QT_TRANSLATE_NOOP("@default", "Reduce the number of visible messages to"), "ChatPruneLen", 1,255,1,20);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Automatically fold links"), "FoldLink", false);
	ConfigDialog::addHGroupBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Link folding"));
	ConfigDialog::addSpinBox("Chat", "Link folding", QT_TRANSLATE_NOOP("@default", "Automatically fold links longer than"), "LinkFoldTreshold", 1,500,1,50);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Open chat window on new message"), "OpenChatOnMessage");
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Scroll chat window downward, not upward"), "ScrollDown", true);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "\"Enter\" key in chat sends message by default"), "AutoSend", true);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Message acknowledgements (wait for delivery)"), "MessageAcks", true);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Flash chat title on new message"), "BlinkChatTitle", true);
	ConfigDialog::addCheckBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Ignore messages from anonymous users"), "IgnoreAnonymousUsers", false);
	ConfigDialog::addSpinBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Max image size"),
			"MaxImageSize", 0, 255, 5, 20);


// pierwsze uruchomienie kadu
	config_file.addVariable("Look", "ChatMyBgColor", QColor("#E0E0E0"));
	config_file.addVariable("Look", "ChatUsrBgColor", QColor("#F0F0F0"));
	config_file.addVariable("Look", "ChatMyFontColor", QColor("#000000"));
	config_file.addVariable("Look", "ChatUsrFontColor", QColor("#000000"));

	QFontInfo info(qApp->font());
	QFont def_font(info.family(),info.pointSize());
	config_file.addVariable("Look", "ChatFont", &def_font);

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Look"), "LookTab");

	ConfigDialog::addGrid("Look", "Look", "varOpts", 2);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show status button"), "ShowStatusButton", true);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Multiline description in userbox"), "ShowMultilineDesc", true);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Display group tabs"), "DisplayGroupTabs", true);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show available users in bold"), "ShowBold", true, QT_TRANSLATE_NOOP("@default","Displays users that are not offline using a bold font"));
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show description in userbox"), "ShowDesc", true);
	ConfigDialog::addVBox("Look", "Look", "varOpts2");//potrzebne userboksowi

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Colors"));
		ConfigDialog::addVGroupBox("Look", "Colors", QT_TRANSLATE_NOOP("@default", "Chat window"));
			ConfigDialog::addColorButton("Look", "Chat window", QT_TRANSLATE_NOOP("@default", "Your background color"), "ChatMyBgColor", config_file.readColorEntry("Look","ChatMyBgColor"), "", "own_bg_color");
			ConfigDialog::addColorButton("Look", "Chat window", QT_TRANSLATE_NOOP("@default", "User background color"), "ChatUsrBgColor", config_file.readColorEntry("Look","ChatUsrBgColor"), "", "his_bg_color");
			ConfigDialog::addColorButton("Look", "Chat window", QT_TRANSLATE_NOOP("@default", "Your font color"), "ChatMyFontColor", config_file.readColorEntry("Look","ChatMyFontColor"), "", "own_font_color");
			ConfigDialog::addColorButton("Look", "Chat window", QT_TRANSLATE_NOOP("@default", "User font color"), "ChatUsrFontColor", config_file.readColorEntry("Look","ChatUsrFontColor"), "", "his_font_color");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Fonts"));
		ConfigDialog::addSelectFont("Look", "Fonts", QT_TRANSLATE_NOOP("@default", "Font in chat window"), "ChatFont", def_font.toString(), "", "chat_font_box");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Previews"));
		ConfigDialog::addVGroupBox("Look", "Previews", QT_TRANSLATE_NOOP("@default", "Chat preview"));
			ConfigDialog::addHBox("Look", "Chat preview", "chat_prvw");
				ConfigDialog::addLabel("Look", "chat_prvw", QT_TRANSLATE_NOOP("@default", "<b>Me</b> 00:00:00"), "chat_me");
				ConfigDialog::addLabel("Look", "chat_prvw", QT_TRANSLATE_NOOP("@default", "<b>Other party</b> 00:00:02"), "chat_other");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Other"));
		ConfigDialog::addLineEdit("Look", "Other", QT_TRANSLATE_NOOP("@default", "Chat window title syntax:"), "ChatContents", "", Kadu::SyntaxText);
		ConfigDialog::addHBox("Look", "Other", "conference");
			ConfigDialog::addLineEdit("Look", "conference", QT_TRANSLATE_NOOP("@default", "Conference window title prefix:"), "ConferencePrefix", "", QT_TRANSLATE_NOOP("@default", "This text will be before syntax.\nIf you leave blank, default settings will be used."));
			ConfigDialog::addLineEdit("Look", "conference", QT_TRANSLATE_NOOP("@default", "syntax:"), "ConferenceContents", "%a (%s[: %d])", Kadu::SyntaxText);

	config_file.addVariable("Chat", "EmoticonsStyle", EMOTS_ANIMATED);
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));

	ChatSlots *chatslots =new ChatSlots(kadu, "chat_slots");
	ConfigDialog::registerSlotOnCreate(chatslots,SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(chatslots,SLOT(onDestroyConfigDialog()));
	ConfigDialog::connectSlot("Chat", "Emoticons:", SIGNAL(activated(int)), chatslots, SLOT(chooseEmoticonsStyle(int)));
	ConfigDialog::connectSlot("Chat", "Automatically prune chat messages", SIGNAL(toggled(bool)), chatslots, SLOT(onPruneChat(bool)));
	ConfigDialog::connectSlot("Chat", "Automatically fold links", SIGNAL(toggled(bool)), chatslots, SLOT(onFoldLink(bool)));

	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), chatslots, SLOT(chooseColor(const char *, const QColor&)), "own_bg_color");
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), chatslots, SLOT(chooseColor(const char *, const QColor&)), "his_bg_color");
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), chatslots, SLOT(chooseColor(const char *, const QColor&)), "own_font_color");
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), chatslots, SLOT(chooseColor(const char *, const QColor&)), "his_font_color");

	ConfigDialog::connectSlot("Look", "Font in chat window", SIGNAL(changed(const char *, const QFont&)), chatslots, SLOT(chooseFont(const char *, const QFont&)), "chat_font_box");

	chat_manager=new ChatManager(kadu, "chat_manager");
	connect(gadu,SIGNAL(chatMsgReceived1(UinsList,const QString&,time_t,bool&)),
		chat_manager,SLOT(chatMsgReceived(UinsList,const QString&,time_t,bool&)));
	kdebugf2();
}

const UinsList& Chat::uins()
{
	return Uins;
}

QValueList<ChatMessage*>& Chat::chatMessages()
{
	return ChatMessages;
}

ColorSelectorButton::ColorSelectorButton(QWidget* parent, const QColor& qcolor, int width, const char *name) : QPushButton(parent, name)
{
#define WIDTH1 15
#define BORDER1 3
	QPixmap p(WIDTH1*width+(width-1)*(BORDER1*2), WIDTH1);
	p.fill(qcolor);
	color = qcolor;
	setPixmap(p);
//	setAutoRaise(true);
	setMouseTracking(true);
	setFixedSize(WIDTH1*width+(BORDER1*2)+(width-1)*(BORDER1*2), WIDTH1+(BORDER1*2));
	QToolTip::add(this,color.name());
	connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void ColorSelectorButton::buttonClicked()
{
	emit clicked(color);
}

ColorSelector::ColorSelector(const QColor &defColor, QWidget* parent, const char* name)
	: QWidget (parent, name,Qt::WType_Popup|Qt::WDestructiveClose)
{
	kdebugf();
	QValueList<QColor> qcolors;
	int i;

	for (i = 0; i < 16; ++i)
		qcolors.append(colors[i]);

	int selector_count=qcolors.count();
	int selector_width=(int)sqrt((double)selector_count);
	QGridLayout *grid = new QGridLayout(this, 0, selector_width, 0, 0);

	for(int i=0; i<selector_count; ++i)
	{
		ColorSelectorButton* btn = new ColorSelectorButton(this, qcolors[i], 1, QString("color_selector:%1").arg(qcolors[i].name()).local8Bit().data());
		grid->addWidget(btn, i/selector_width, i%selector_width);
		connect(btn,SIGNAL(clicked(const QColor&)),this,SLOT(iconClicked(const QColor&)));
	}
	if (qcolors.contains(defColor)==0)
	{
		ColorSelectorButton* btn = new ColorSelectorButton(this, defColor, 4, QString("color_selector:%1").arg(defColor.name()).local8Bit().data());
		grid->addMultiCellWidget(btn, 4, 4, 0, 3);
		connect(btn,SIGNAL(clicked(const QColor&)),this,SLOT(iconClicked(const QColor&)));
	}
	kdebugf2();
}

void ColorSelector::iconClicked(const QColor& color)
{
	emit colorSelect(color);
	close();
}

void ColorSelector::closeEvent(QCloseEvent* e)
{
	kdebugf();
	emit aboutToClose();
	QWidget::closeEvent(e);
	kdebugf2();
}

void ColorSelector::alignTo(QWidget* w)
{
	kdebugf();
	// oblicz pozycjê widgetu do którego równamy
	QPoint w_pos = w->mapToGlobal(QPoint(0,0));
	// oblicz rozmiar selektora
	QSize e_size = sizeHint();
	// oblicz rozmiar pulpitu
	QSize s_size = QApplication::desktop()->size();
	// oblicz dystanse od widgetu do lewego brzegu i do prawego
	int l_dist = w_pos.x();
	int r_dist = s_size.width() - (w_pos.x() + w->width());
	// oblicz pozycjê w zale¿no¶ci od tego czy po lewej stronie
	// jest wiêcej miejsca czy po prawej
	int x;
	if (l_dist >= r_dist)
		x = w_pos.x() - e_size.width();
	else
		x = w_pos.x() + w->width();
	// oblicz pozycjê y - centrujemy w pionie
	int y = w_pos.y() + w->height()/2 - e_size.height()/2;
	// je¶li wychodzi poza doln± krawêd¼ to równamy do niej
	if (y + e_size.height() > s_size.height())
		y = s_size.height() - e_size.height();
	// je¶li wychodzi poza górn± krawêd¼ to równamy do niej
	if (y < 0)
		y = 0;
	// ustawiamy selektor na wyliczonej pozycji
	move(x, y);
	kdebugf2();
}

ChatSlots::ChatSlots(QObject* parent, const char* name)
	: QObject(parent, name)
{
}

void ChatSlots::initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
{
	/*
		UWAGA: w tej funkcji NIE WOLNO korzystaæ z klasy ConfigDialog
		(joi)
	*/
	kdebugf();
	browserCombo->insertItem(tr("Specify path"));
	browserCombo->insertItem("Konqueror");
	browserCombo->insertItem("Opera");
	browserCombo->insertItem("Mozilla");
	browserCombo->insertItem("Mozilla Firefox");
	browserCombo->insertItem("Dillo");
	browserCombo->insertItem("Galeon");
	QString browserCommandLine=browserPath->text();
	browserOptionsCombo->setEnabled(false);

	int browserNumber=config_file.readNumEntry("Chat", "WebBrowserNo", 0);
	browserCombo->setCurrentItem(browserNumber);
	browserOptionsCombo->clear();
	switch (browserNumber)
	{
		case 1: 	//konqueror
		{
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			if (browserCommandLine.find("newTab", 0, true) != -1)
				browserOptionsCombo->setCurrentItem(1);
			else
				browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 2: 	//opera
		{
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			browserOptionsCombo->insertItem(tr("Open in background tab"));
			if (browserCommandLine.find("-newpage", 0, true) != -1)	//jak znajdzie ta opcje to podswietla odpowiedni wpis w combo
				browserOptionsCombo->setCurrentItem(1);
			else if (browserCommandLine.find("-backgroundpage", 0, true) != -1)
				browserOptionsCombo->setCurrentItem(2);
			else
				browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);	//uaktywniamy combo
			break;
		}
		case 3:		//mozilla
		case 4: 	//firefox
		{
			browserOptionsCombo->insertItem(tr("Open in new window"));		//dodajemy pozycje combo
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			if (browserCommandLine.find("new-tab", 0, true) != -1)	//i wyszukujemy ktora opcje zaznaczyc
				browserOptionsCombo->setCurrentItem(1);
			else
				browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
	}
	browserPath->setText(config_file.readEntry("Chat", "WebBrowser"));//potrzebne dla modu³u start_wizard
	browserPath->setReadOnly(browserCombo->currentItem()!=0);
	kdebugf2();
}

void ChatSlots::onCreateConfigDialog()
{
	kdebugf();
	QComboBox* cb_emoticons_theme= ConfigDialog::getComboBox("Chat", "Emoticons theme");
	cb_emoticons_theme->insertStringList(emoticons->themes());
	cb_emoticons_theme->setCurrentText(config_file.readEntry("Chat", "EmoticonsTheme"));

	if ((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") == EMOTS_NONE)
		(cb_emoticons_theme)->setEnabled(false);

	//ustawienie pól w combo wyboru przegladarki
	QComboBox *browserCombo= ConfigDialog::getComboBox("Chat", "Choose your browser");
	QComboBox *browserOptionsCombo=ConfigDialog::getComboBox("Chat", "Browser options");
	QLineEdit *browserPath= ConfigDialog::getLineEdit("Chat", "Custom Web browser");
	initBrowserOptions(browserCombo, browserOptionsCombo, browserPath);

	//podpiecie pod zmiane w combo
	connect(browserCombo, SIGNAL(activated (int)), this, SLOT(findAndSetWebBrowser(int)));
	connect(browserOptionsCombo, SIGNAL(activated (int)), this, SLOT(findAndSetBrowserOption(int)));

	QCheckBox *c_prunechat= ConfigDialog::getCheckBox("Chat", "Automatically prune chat messages");
	QHGroupBox *h_prune= ConfigDialog::getHGroupBox("Chat", "Message pruning");

	h_prune->setEnabled(c_prunechat->isChecked());

	QCheckBox *c_foldlink= ConfigDialog::getCheckBox("Chat", "Automatically fold links");
	QHGroupBox *h_fold= ConfigDialog::getHGroupBox("Chat", "Link folding");
	QToolTip::add(h_fold, tr("URLs longer than this value will be shown truncated to this length"));
	QToolTip::add(c_foldlink, tr("This will show a long URL as http://www.start...end.com/\nto protect the chat window from a mess"));
	ConfigDialog::getSpinBox("Chat", "Max image size")->setSuffix(" kB");

	h_fold->setEnabled(c_foldlink->isChecked());
	updatePreview();
	kdebugf2();
}

void ChatSlots::onPruneChat(bool toggled)
{
	ConfigDialog::getHGroupBox("Chat", "Message pruning")->setEnabled(toggled);
}

void ChatSlots::onFoldLink(bool toggled)
{
	ConfigDialog::getHGroupBox("Chat", "Link folding")->setEnabled(toggled);
}

void ChatSlots::onDestroyConfigDialog()
{
	kdebugf();

	QComboBox* cb_emoticons_theme= ConfigDialog::getComboBox("Chat", "Emoticons theme");
	config_file.writeEntry("Chat", "EmoticonsTheme",cb_emoticons_theme->currentText());
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));

	config_file.writeEntry("Chat", "WebBrowserNo", ConfigDialog::getComboBox("Chat", "Choose your browser")->currentItem());

	chat_manager->changeAppearance();

/*
	Aby unikn±c problemów z niepoprawnymi localesami i pozniejszymi
	k³opotami które moga wynikn±c z tego, musimy zamienic dwie mozliwe
	mozliwo¶ci na _puste_pole_ przez co uzyskamy ze kadu i tak bedzie
	dynamicznie reagowac na zmiany localesów nie zaleznie jaka wersja
	by³a zapisana przed ustawieniem ustawien domyslnych(moze nie za
	dobrze to wyjasnione, ale konieczne. Nie dotyczy to dwóch zmiennych
	config.panelsyntax i config.conferencesyntax, bo pierwotnie zawieraj
	TYLKO sam± sk³adnie)
*/

	QLineEdit *e_chatsyntax= ConfigDialog::getLineEdit("Look", "Chat window title syntax:");
	QLineEdit *e_conferenceprefix= ConfigDialog::getLineEdit("Look", "Conference window title prefix:");

	if (e_chatsyntax->text() == tr("Chat with ")+"%a (%s[: %d])" || e_chatsyntax->text() == "Chat with %a (%s[: %d])")
		config_file.writeEntry("Look", "ChatContents", "");

	if (e_conferenceprefix->text() == tr("Conference with ") || e_conferenceprefix->text() == "Conference with ")
		config_file.writeEntry("Look", "ConferencePrefix", "");
	kdebugf2();
}

void ChatSlots::chooseColor(const char* name, const QColor& color)
{
	kdebugf();
	QLabel *preview1= ConfigDialog::getLabel("Look", "<b>Me</b> 00:00:00", "chat_me");
	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Other party</b> 00:00:02", "chat_other");
	if (QString(name)=="own_bg_color")
		preview1->setPaletteBackgroundColor(color);
	else if (QString(name)=="his_bg_color")
		preview2->setPaletteBackgroundColor(color);
	else if (QString(name)=="own_font_color")
		preview1->setPaletteForegroundColor(color);
	else if (QString(name)=="his_font_color")
		preview2->setPaletteForegroundColor(color);
	else
		kdebugm(KDEBUG_ERROR, "chooseColor: label '%s' not known!\n", name);
	kdebugf2();
}

void ChatSlots::chooseFont(const char* name, const QFont& font)
{
	kdebugf();
	QLabel *preview1= ConfigDialog::getLabel("Look", "<b>Me</b> 00:00:00", "chat_me");
	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Other party</b> 00:00:02", "chat_other");
	if (QString(name)=="chat_font_box")
	{
		preview1->setFont(font);
		preview2->setFont(font);
	}
	kdebugf2();
}

void ChatSlots::chooseEmoticonsStyle(int index)
{
	ConfigDialog::getComboBox("Chat","Emoticons theme")->setEnabled(index!=0);
}

void ChatSlots::updatePreview()
{
	kdebugf();
	QLabel *preview1= ConfigDialog::getLabel("Look", "<b>Me</b> 00:00:00", "chat_me");
	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Other party</b> 00:00:02", "chat_other");
	preview1->setFont(config_file.readFontEntry("Look", "ChatFont"));
	preview1->setPaletteForegroundColor(config_file.readColorEntry("Look", "ChatMyFontColor"));
	preview1->setPaletteBackgroundColor(config_file.readColorEntry("Look", "ChatMyBgColor"));
	preview1->setAlignment(Qt::AlignLeft);
	preview2->setFont(config_file.readFontEntry("Look", "ChatFont"));
	preview2->setPaletteForegroundColor(config_file.readColorEntry("Look", "ChatUsrFontColor"));
	preview2->setPaletteBackgroundColor(config_file.readColorEntry("Look", "ChatUsrBgColor"));
	preview2->setAlignment(Qt::AlignLeft);
	kdebugf2();
}

void ChatSlots::findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath)
{
	kdebugf();
	/*
		UWAGA1: obs³uga mozilli i firefoksa jest dosyæ skomplikowana, wiêc przy rozbudowie tej funkcji
		nale¿y najpierw zrozumieæ jej dzia³anie dla tych dwóch przegl±darek, bo mo¿na siê naci±æ...

		UWAGA2: w tej funkcji NIE WOLNO korzystaæ z klasy ConfigDialog
	*/
	QString prevBrowser=browserPath->text();
	browserOptionsCombo->setEnabled(false);	//blokujemy combo

	if (selectedBrowser==0)
	{
		browserPath->setReadOnly(false);
		return;
	}
	else
		browserPath->setReadOnly(true);

	QString homePath=getenv("HOME");
	QString browserName;

	QStringList searchPath=QStringList::split(":", QString(getenv("PATH")));

	switch (selectedBrowser)
	{
		case 1: //konqueror
		{
			browserName="dcop";
			searchPath.append("/opt/kde/bin");
			searchPath.append("/opt/kde3/bin");
			browserOptionsCombo->clear();
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 2://opera
		{
			browserName="opera";
			searchPath.append("/opt/opera");
			browserOptionsCombo->clear();
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			browserOptionsCombo->insertItem(tr("Open in background tab"));
			browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 3: //mozilla
		{
			browserName="mozilla-xremote-client";

			QStringList dirList=QDir("/usr/lib").entryList("mozilla*", QDir::All, QDir::Name|QDir::Reversed);
			for (QStringList::iterator it=dirList.begin(); it!=dirList.end(); it++)
				searchPath.append("/usr/lib/"+(*it));

			searchPath.append("/usr/local/Mozilla");
			searchPath.append("/usr/local/mozilla");
			searchPath.append(homePath+"/Mozilla");
			searchPath.append(homePath+"/mozilla");
			browserOptionsCombo->clear();
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 4:	//firefox
		{
			browserName="mozilla-xremote-client";

			QStringList dirList=QDir("/usr/lib").entryList("firefox*", QDir::All, QDir::Name|QDir::Reversed);
			for (QStringList::iterator it=dirList.begin(); it!=dirList.end(); it++)
				searchPath.append("/usr/lib/"+(*it));

			dirList=QDir("/usr/lib").entryList("mozilla-firefox*", QDir::All, QDir::Name|QDir::Reversed);
			for (QStringList::iterator it=dirList.begin(); it!=dirList.end(); it++)
				searchPath.append("/usr/lib/"+(*it));
			if (!dirList.empty())//jeste¶my na debianie, gdzie zmienili nazwê skryptu, grrr :|
				browserName="mozilla-firefox-xremote-client";

			searchPath.append("/usr/local/Firefox:");
			searchPath.append("/usr/local/firefox:");
			searchPath.append(homePath+"/Firefox:");
			searchPath.append(homePath+"/firefox:");

			dirList=QDir("/usr/lib").entryList("mozilla*", QDir::All, QDir::Name|QDir::Reversed);
			for (QStringList::iterator it=dirList.begin(); it!=dirList.end(); it++)
				searchPath.append("/usr/lib/"+(*it));

			browserOptionsCombo->clear();
			browserOptionsCombo->insertItem(tr("Open in new window"));
			browserOptionsCombo->insertItem(tr("Open in new tab"));
			browserOptionsCombo->setCurrentItem(0);
			browserOptionsCombo->setEnabled(true);
			break;
		}
		case 5: browserName="dillo"; break;
		case 6: browserName="galeon"; break;
		default: return;
	}
	QFile browserFile;
	QString path, testPath;

	bool browserFound=false;
	QStringList::iterator dir=searchPath.begin();
	QStringList::iterator endDir=searchPath.end();

	kdebugm(KDEBUG_INFO, "search path: %s\n", searchPath.join(" ").local8Bit().data());
	while (!browserFound && dir!=endDir)
	{
		testPath=(*dir)+"/"+browserName;
		if (QFile::exists(testPath))
		{
			if (selectedBrowser==1) //konqueror
			{
				if (browserName=="kfmclient")
					path.replace(QRegExp("kfmclient"), testPath);
				else
				{
					path="ok=0;for i in `dcop|grep konqueror`; do shown=`dcop $i konqueror-mainwindow#1 shown`; if [ \"$shown\" == \"true\" ];then dcop $i KonquerorIface openBrowserWindow \"%1\" && ok=1; fi; if [ \"$ok\" == \"1\" ]; then break; fi done; if [ \"$ok\" != \"1\" ]; then kfmclient openURL \"%1\"; fi;";
					path.replace(QRegExp("dcop"), testPath);
					browserName="kfmclient";
					dir=searchPath.begin();
					continue;
				}
			}
			else if (selectedBrowser==3) //mozilla
			{
				if (browserName=="mozilla")
					path=path+testPath+" \"%1\"";
				else
				{
					path=testPath+" -a mozilla \"openURL(%1,new-window)\" || ";
					browserName="mozilla";
					dir=searchPath.begin();
					continue;
				}
			}
			else if (selectedBrowser==4) //firefox
			{
				if (browserName=="firefox")
					path=path+testPath+" \"%1\"";
				else
				{
					path=testPath+" \"openURL(%1,new-window)\" || ";
					browserName="firefox";
					dir=searchPath.begin();
					continue;
				}
			}
			else
				path=testPath;
			browserPath->setText(path);
			browserFound=true;
			kdebugm(KDEBUG_INFO, "browser found! '%s'\n", path.local8Bit().data());
		}
		dir++;
	}
	if (!browserFound)
	{
		MessageBox::msg(tr("I didn't find the browser you selected! The path to it doesn't exists in $PATH variable. \nYou may add it to $PATH or specify location using Specify path option."));
		browserCombo->setCurrentItem(0);	//ustawiamy na default
		browserOptionsCombo->clear();	//czyscimy opcje
		browserPath->setText(prevBrowser);
//		browserPath->clear(); 	//no i czyscimy LineEdita
	}
	kdebugf2();
}

void ChatSlots::findAndSetWebBrowser(int selectedBrowser)
{
	kdebugf();

	QComboBox *browserCombo=ConfigDialog::getComboBox("Chat", "Choose your browser");
	QComboBox *browserOptionsCombo=ConfigDialog::getComboBox("Chat", "Browser options");
	QLineEdit *browserPath= ConfigDialog::getLineEdit("Chat", "Custom Web browser");
	findBrowser(selectedBrowser, browserCombo, browserOptionsCombo, browserPath);

	kdebugf2();
}

void ChatSlots::setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser)
{
	kdebugf();
	/*
		UWAGA: w tej funkcji NIE WOLNO korzystaæ z klasy ConfigDialog
	*/
	QString browserPath=browserPathEdit->text();
	switch(chosenBrowser)
	{
		case 1: //Konqueror
		{
			if (selectedOption==1)
				browserPath.replace("KonquerorIface openBrowserWindow", "konqueror-mainwindow#1 newTab");
			else
				browserPath.replace("konqueror-mainwindow#1 newTab", "KonquerorIface openBrowserWindow");
			browserPathEdit->setText(browserPath);
			break;
		}
		case 2:		//Opera
		{
			browserPath.remove(" -newwindow");
			browserPath.remove(" -newpage");
			browserPath.remove(" -backgroundpage");
			switch(selectedOption)
			{
				case 0: browserPath.append(" -newwindow"); break;
				case 1: browserPath.append(" -newpage"); break;
				case 2: browserPath.append(" -backgroundpage"); break;
			}
			browserPathEdit->setText(browserPath);
			break;
		}
		case 3: 	//Mozilla
		case 4:		//Firefox
		{
			if (selectedOption==1)
				browserPath.replace("new-window", "new-tab");
			else
				browserPath.replace("new-tab", "new-window");
			browserPathEdit->setText(browserPath);
			break;
		}
	}
	kdebugf2();
}

void ChatSlots::findAndSetBrowserOption(int selectedOption)
{
	kdebugf();
	setBrowserOption(selectedOption,
		ConfigDialog::getLineEdit("Chat", "Custom Web browser"),
		ConfigDialog::getComboBox("Chat", "Choose your browser")->currentItem());
	kdebugf2();
}
