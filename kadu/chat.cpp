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
//
#include "misc.h"
#include "config_dialog.h"
#include "config_file.h"
#include "kadu.h"
#include "events.h"
#include "chat.h"
#include "search.h"
#include "history.h"
#include "emoticons.h"
#include "pending_msgs.h"
#include "debug.h"
#include "gadu.h"
#include "hints.h"
#include "status.h"
#include "message_box.h"

ChatManager::ChatManager() : QObject()
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
}

ChatManager::~ChatManager()
{
	kdebugf();
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
	for(unsigned int i=0; i<Chats.count(); i++)
		if(Chats[i]==chat)
		{
			ChatInfo info;
			info.uins=chat->uins();
			
			info.geometry.setX(chat->pos().x());
			info.geometry.setY(chat->pos().y());
			info.geometry.setWidth(chat->size().width());
			info.geometry.setHeight(chat->size().height());

			for (QValueList<ChatInfo>::iterator j=sizes.begin(); j!=sizes.end(); j++)
				if ((*j).uins.equals(info.uins))
				{
					sizes.remove(j);
					break;
				}

			info.vertSizes=chat->vertSplit->sizes();
			if (chat->horizSplit)
				info.horizSizes=chat->horizSplit->sizes();

			sizes.push_front(info);

			emit chatDestroying(chat->uins());
			Chats.remove(Chats.at(i));
			emit chatDestroyed(chat->uins());
			return;
		}
}

void ChatManager::refreshTitles()
{
	for (unsigned int i = 0; i < Chats.count(); i++)
		Chats[i]->setTitle();
}

void ChatManager::refreshTitlesForUin(UinType uin)
{
	for (unsigned int i = 0; i < Chats.count(); i++)
		if (Chats[i]->uins().contains(uin))
			Chats[i]->setTitle();
}

void ChatManager::changeAppearance()
{
	for (unsigned int i = 0; i < Chats.count(); i++)
		Chats[i]->changeAppearance();
}

Chat* ChatManager::findChatByUins(UinsList uins)
{
	for(unsigned int i=0; i<Chats.count(); i++)
		if(Chats[i]->uins().equals(uins))
			return Chats[i];
	kdebugm(KDEBUG_WARNING, "return NULL\n");
	return NULL;
}

int ChatManager::openChat(UinsList senders,time_t time)
{
	for (unsigned int i = 0; i < Chats.count(); i++)
		if (Chats[i]->uins().equals(senders))
		{
			Chats[i]->raise();
			Chats[i]->setActiveWindow();
			return i;	
		}
	Chat* chat = new Chat(senders, 0);
	chat->setTitle();

	bool found=false;
	for (QValueList<ChatInfo>::iterator j=sizes.begin(); j!=sizes.end(); j++)
		if ((*j).uins.equals(senders))
		{
			found=true;
			chat->setGeometry((*j).geometry);
			chat->vertSplit->setSizes((*j).vertSizes);
			if (chat->horizSplit)
				chat->horizSplit->setSizes((*j).horizSizes);
			break;
		}
	if (!found)
	{
		QPoint pos = QCursor::pos();
		if (senders.count()>1)
			chat->setGeometry((pos.x() + 550) / 2, (pos.y() + 400) / 2, 550, 400);
		else
			chat->setGeometry((pos.x() + 400) / 2, (pos.y() + 400) / 2, 400, 400);
	}

	chat->show();
	chat->writeMessagesFromHistory(senders, time);
	emit chatCreated(senders);
	return Chats.count()-1;
}

int ChatManager::openPendingMsg(int index,QString& to_add)
{
	PendingMsgs::Element p = pending[index];
	// jesli ktoregos z nadawcow nie mamy na liscie to dodajemy
	// go tam jako anonymous
	for (unsigned int j = 0; j < p.uins.count(); j++)
		if (!userlist.containsUin(p.uins[j]))
			userlist.addAnonymous(p.uins[j]);
	// otwieramy chat (jesli nie istnieje)
	int k = openChat(p.uins,p.time);
	// dopisujemy nowa wiadomosc do to_add
	Chats[k]->formatMessage(false, userlist.byUin(p.uins[0]).altnick,p.msg, timestamp(p.time), to_add);
	// kasujemy wiadomosc z pending
	pending.deleteMsg(index);
	// zwracamy indeks okna chat
	return k;
}

void ChatManager::openPendingMsgs(UinsList uins)
{
	PendingMsgs::Element elem;
	int k;
	bool stop = false;
	QString toadd;
	for (int i = 0; i < pending.count(); i++)
	{
		elem = pending[i];
		if (elem.uins.equals(uins))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| !elem.msgclass)
			{
				k=openPendingMsg(i,toadd);				
				i--;
				uins = elem.uins;
				stop = true;
			}
	}
	if (stop)
	{
		Chats[k]->scrollMessages(toadd);
		UserBox::all_refresh();
	}
	else
		k = openChat(uins,0);
}

void ChatManager::openPendingMsgs()
{
	UinsList uins;
	int i, k = -1;
	PendingMsgs::Element elem;
	QString toadd;
	bool stop = false;
	UserListElement e;

	kdebugf();

	for(i = 0; i<pending.count(); i++)
	{
		elem = pending[i];
		if (!uins.count() || elem.uins.equals(uins))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| (!elem.msgclass))
			{
				if (!uins.count())
					uins = elem.uins;
				k=openPendingMsg(i,toadd);
				i--;
				stop = true;
			}
	}
	if(stop)
	{
		kdebugm(KDEBUG_INFO, "ChatManager::openPendingMsgs() stopped\n");
		Chats[k]->scrollMessages(toadd);
		UserBox::all_refresh();
	}
	kdebugf2();
}

void ChatManager::sendMessage(UinType uin,UinsList selected_uins)
{
	QString tmp;
	int i, k = -1;
	bool stop = false;
	PendingMsgs::Element elem;
	UinsList uins;
	QString toadd;

	for (i = 0; i < pending.count(); i++)
	{
		elem = pending[i];
		if ((!uins.count() && elem.uins.contains(uin)) || (uins.count() && elem.uins.equals(uins)))
			if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
				|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
				|| !elem.msgclass)
			{
				if (!uins.count())
					uins = elem.uins;					
				k=openPendingMsg(i,toadd);
				i--;
				stop = true;
			}
	}
	if (stop)
	{
		Chats[k]->scrollMessages(toadd);
		UserBox::all_refresh();
	}
	else
	{
		// zawsze otwieraja sie czaty
		uins = selected_uins;
		k = openChat(uins,0);
	}
}

void ChatManager::chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab)
{
	Chat* chat=findChatByUins(senders);
	if(chat!=NULL)
	{
		QString toadd;
		chat->checkPresence(senders, msg, time, toadd);
		chat->alertNewMessage();
		if (!chat->isActiveWindow() && config_file.readBoolEntry("Hints","NotifyNewMessage"))
			hintmanager->addHintNewMsg(userlist.byUinValue(senders[0]).altnick, msg);
		grab=true;
	}
}

ChatManager* chat_manager=NULL;


const char *colors[16] = {"#FF0000", "#A00000", "#00FF00", "#00A000", "#0000FF", "#0000A0", "#FFFF00",
	"#A0A000", "#FF00FF", "#A000A0", "#00FFFF", "#00A0A0", "#FFFFFF", "#A0A0A0", "#808080", "#000000"};

CustomInput::CustomInput(QWidget *parent, const char *name) : QMultiLineEdit(parent, name) {
	QStyleSheet *style=styleSheet();
	style->item("p")->setMargin(QStyleSheetItem::MarginVertical, 0);
	setStyleSheet(style);
}

void CustomInput::keyPressEvent(QKeyEvent * e) {
//	kdebugf();
	emit keyPressed(e, this);
	if (autosend_enabled && ((HotKey::shortCut(e,"ShortCuts", "chat_newline")) || e->key()==Key_Enter)&& !(e->state() & ShiftButton))
	{
		kdebugm(KDEBUG_INFO, "CustomInput::keyPressEvent(): emit sendMessage()\n");
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
			emit specialKeyPressed(CustomInput::KEY_UNDERLINE);
		QMultiLineEdit::keyPressEvent(e);
	}
	// przekazanie event'a do qwidget 
	// aby obsluzyc skroty klawiszowe (definiowane sa dla okna chat)
	QWidget::keyPressEvent(e);
}

void CustomInput::setAutosend(bool on) {
	autosend_enabled = on;
}

void CustomInput::paste() {
	pasteSubType("plain");
}

QValueList<Chat::RegisteredButton> Chat::RegisteredButtons;

KaduSplitter::KaduSplitter(QWidget *parent, const char *name) : QSplitter (parent, name)
{
}

KaduSplitter::KaduSplitter(Orientation o, QWidget *parent, const char *name) : QSplitter(o,parent,name)
{
}

void KaduSplitter::drawContents(QPainter *p)
{
	QSplitter::drawContents(p);
	kdebugf();
	for (QValueList<KaduTextBrowser *>::iterator i=list.begin(); i!=list.end(); i++)
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

Chat::Chat(UinsList uins, QWidget *parent, const char *name)
 : QWidget(parent, name, Qt::WDestructiveClose),Uins(uins)
{
//	int i;
	QValueList<int> sizes;

	emoticon_selector = NULL;
	color_selector = NULL;

	title_timer = new QTimer(this);
	connect(title_timer,SIGNAL(timeout()),this,SLOT(changeTitle()));

	/* register us in the chats registry... */
	index=chat_manager->registerChat(this);

	vertSplit = new KaduSplitter(Qt::Vertical, this);

	if (uins.count() > 1)
	{
		horizSplit = new KaduSplitter(Qt::Horizontal, vertSplit);
		body = new KaduTextBrowser(horizSplit);
	}
	else 
	{
		horizSplit=NULL;
		body = new KaduTextBrowser(vertSplit);
	}
		
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")==EMOTS_ANIMATED)
		body->setStyleSheet(new AnimStyleSheet(body,emoticons->themePath()));
	
	body->setMinimumSize(QSize(100,100));
	body->setFont(config_file.readFontEntry("Look","ChatFont"));

	QPoint pos = QCursor::pos();
	
	if (uins.count() > 1)
	{
		userbox = new UserBox(horizSplit);
		userbox->setMinimumSize(QSize(30,30));
		userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
		userbox->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
		userbox->QListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));

		for (unsigned i = 0; i < uins.count(); i++)
			userbox->addUser(userlist.byUin(uins[i]).altnick);
		userbox->refresh();

		connect(userbox, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)),
		UserBox::userboxmenu, SLOT(show(QListBoxItem *)));

		sizes.append(3);
		sizes.append(1);
		horizSplit->setSizes(sizes);
	}
	else
		userbox = NULL;
		
	QVBox *downpart = new QVBox(vertSplit);
	QHBox *edtbuttontray = new QHBox(downpart);

	QLabel *edt = new QLabel(tr("Edit window:"),edtbuttontray);
	QToolTip::add(edt, tr("This is where you type in the text to be sent"));

	buttontray = new QHBox(edtbuttontray);

	autosend = new QPushButton(buttontray);
	autosend->setPixmap(icons_manager.loadIcon("AutoSendMessage"));
	autosend->setToggleButton(true);
	QToolTip::add(autosend, tr("Enter key sends message"));

	lockscroll = new QPushButton(buttontray);
	lockscroll->setPixmap(icons_manager.loadIcon("ScrollLock"));
	lockscroll->setToggleButton(true);
	QToolTip::add(lockscroll, tr("Blocks scrolling"));

	for(unsigned int i=0; i<RegisteredButtons.size(); i++)
	{
		RegisteredButton& b=RegisteredButtons[i];
		QPushButton* btn=new QPushButton(buttontray,b.name.local8Bit().data());
		connect(btn, SIGNAL(clicked()), b.receiver, b.slot.local8Bit().data());
		Buttons.insert(b.name,btn);
	}
	
	QPushButton *clearchat= new QPushButton(buttontray);
	clearchat->setPixmap(icons_manager.loadIcon("ClearChat"));
	QToolTip::add(clearchat, tr("Clear messages in chat window"));

	iconsel = new QPushButton(buttontray);
	iconsel->setPixmap(icons_manager.loadIcon("Emoticon"));
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")==EMOTS_NONE)
	{
		QToolTip::add(iconsel, tr("Insert emoticon - enable in configuration"));
		iconsel->setEnabled(false);
	}
	else
		QToolTip::add(iconsel, tr("Insert emoticon"));

	QPushButton *history = new QPushButton(buttontray);
	history->setPixmap(icons_manager.loadIcon("History"));
	QToolTip::add(history, tr("Show history"));

	QPushButton *whois = new QPushButton(buttontray);
	whois->setPixmap(icons_manager.loadIcon("LookupUserInfo"));
	QToolTip::add(whois, tr("Lookup user info"));

	QPushButton* insertimage = new QPushButton(buttontray);
	insertimage->setPixmap(icons_manager.loadIcon("ChooseEmoticons"));
	QToolTip::add(insertimage, tr("Insert image"));

	edtbuttontray->setStretchFactor(edt, 50);
	edtbuttontray->setStretchFactor(buttontray, 1);

	edit = new CustomInput(downpart);
	edit->setMinimumHeight(1);
	edit->setWordWrap(QMultiLineEdit::WidgetWidth);
	edit->setFont(config_file.readFontEntry("Look","ChatFont"));

	if (config_file.readBoolEntry("Chat","AutoSend"))
		autosend->setOn(true);
	edit->setAutosend(config_file.readBoolEntry("Chat","AutoSend"));

	QHBox *btnpart = new QHBox(downpart);

	QFont afont = QApplication::font();
	QSize s=QFontMetrics(afont).size(0, "B")*6;

	boldbtn = new QPushButton("B", btnpart);
	boldbtn->setToggleButton(true);
	afont.setBold(true);
	boldbtn->setFont(afont);
	boldbtn->setMaximumSize(s);

	italicbtn = new QPushButton("I", btnpart);
	italicbtn->setToggleButton(true);
	afont.setBold(false);
	afont.setItalic(true);
	italicbtn->setFont(afont);
	italicbtn->setMaximumSize(s);

	underlinebtn = new QPushButton("U", btnpart);
	underlinebtn->setToggleButton(true);
	afont.setItalic(false);
	afont.setUnderline(true);
	underlinebtn->setFont(afont);
	underlinebtn->setMaximumSize(s);

	colorbtn = new QPushButton(btnpart);
//	colorbtn->setMinimumSize(boldbtn->width(), boldbtn->height());
	QPixmap p(16, 16);
	p.fill(colors[15]);
	colorbtn->setPixmap(p);

	QWidget *filler=new QWidget(btnpart);
	filler->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	sendbtn = new QPushButton(QIconSet(icons_manager.loadIcon("SendMessage")),tr("&Send"),btnpart);
	sendbtn->setFixedWidth(120);
	connect(sendbtn, SIGNAL(clicked()), this, SLOT(sendMessage()));
	QAccel *acc = new QAccel(this);
	acc->connectItem(acc->insertItem(Key_Return + CTRL), this, SLOT(sendMessage()));

	acc = new QAccel(this);
	acc->connectItem(acc->insertItem(Key_PageUp + SHIFT), this, SLOT(pageUp()));
	acc = new QAccel(this);
	acc->connectItem(acc->insertItem(Key_PageDown + SHIFT), this, SLOT(pageDown()));

	sizes.clear();
	sizes.append(3);
	sizes.append(2);
	vertSplit->setSizes(sizes);

	QGridLayout *grid = new QGridLayout (this, 5, 4, 3, 3);
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

	connect(&event_manager, SIGNAL(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)),
		this, SLOT(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)));

	totaloccurences = 0;

	edit->setFocus();
}

Chat::~Chat() {
	kdebugf();
	chat_manager->unregisterChat(this);

	disconnect(&event_manager, SIGNAL(ackReceived(int)),
		this, SLOT(ackReceivedSlot(int)));
	disconnect(&event_manager, SIGNAL(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)),
		this, SLOT(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)));

	if (userbox)
		delete userbox;
		
	kdebugm(KDEBUG_FUNCTION_END, "Chat::~Chat: chat destroyed: index %d\n", index);
}

void Chat::registerButton(const QString& name,QObject* receiver,const QString& slot)
{
	RegisteredButton b;
	b.name=name;
	b.receiver=receiver;
	b.slot=slot;
	RegisteredButtons.append(b);
}

void Chat::unregisterButton(const QString& name)
{
	for(unsigned int i=0; i<RegisteredButtons.size(); i++)
		if(RegisteredButtons[i].name==name)
		{
			RegisteredButtons.remove(RegisteredButtons.at(i));
			break;
		}
	for(unsigned int i=0; i<chat_manager->chats().size(); i++)
	{
		Chat* chat=chat_manager->chats()[i];
		if(chat->Buttons.contains(name))
		{
			delete chat->Buttons[name];
			chat->Buttons.remove(name);
		}
	}
}

QPushButton* Chat::button(const QString& name)
{
	if(Buttons.contains(name))
		return Buttons[name];

	kdebugm(KDEBUG_WARNING, "return NULL\n");
	return NULL;
}

void Chat::specialKeyPressed(int key) {
	kdebugf();
	switch (key) {
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
}

void Chat::toggledBold(bool on) {
	kdebugf();
	edit->setBold(on);
}

void Chat::toggledItalic(bool on) {
	kdebugf();
	edit->setItalic(on);
}

void Chat::toggledUnderline(bool on) {
	kdebugf();
	edit->setUnderline(on);
}

void Chat::curPosChanged(int para, int pos) {
	int i;

	kdebugf();
	if (edit->bold() != boldbtn->isOn())
		boldbtn->setOn(edit->bold());
	if (edit->italic() != italicbtn->isOn())
		italicbtn->setOn(edit->italic());
	if (edit->underline() != underlinebtn->isOn())
		underlinebtn->setOn(edit->underline());
	if (edit->color() != actcolor) {
		for (i = 0; i < 16; i++)
			if (edit->color() == QColor(colors[i]))
				break;
		QPixmap p(16, 16);
		if (i >= 16)
			i = 15;
		p.fill(colors[i]);
		actcolor = colors[i];
		colorbtn->setPixmap(p);
		}
	
}

void Chat::pageUp() {
	body->scrollBy(0, (body->height() * -2) / 3);
}

void Chat::pageDown() {
	body->scrollBy(0, (body->height() * 2) / 3);
}

void Chat::insertImage()
{
	ImageDialog* id = new ImageDialog(this);
	id->setCaption(tr("Insert image"));
	if(id->exec() == QDialog::Accepted)
		edit->insert(QString("[IMAGE ")+id->selectedFile()+"]");
	delete id;
	edit->setFocus();
}

void Chat::imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path)
{
	kdebugf();
	body->setText(
		gadu_images_manager.replaceLoadingImages(
			body->text(),sender,size,crc32));
	kdebugf2();
}

void Chat::changeAppearance() {
	if (Uins.count() > 1 && userbox) {
		userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
		userbox->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
		userbox->QListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));
		}
	body->setFont(config_file.readFontEntry("Look","ChatFont"));
	edit->setFont(config_file.readFontEntry("Look","ChatFont"));
}

void Chat::setTitle() {

	QString title;

	if (Uins.size() > 1) {
		kdebugm(KDEBUG_INFO, "Chat::setTitle(): Uins.size() > 1\n");
		if (config_file.readEntry("Look","ConferencePrefix").isEmpty())
			title = tr("Conference with ");
		else
			title = config_file.readEntry("Look","ConferencePrefix");
		title.append(parse(config_file.readEntry("Look","ConferenceContents"),userlist.byUinValue(Uins[0]),false));
		for (unsigned int k = 1; k < Uins.size(); k++) {
			title.append(", ");
			title.append(parse(config_file.readEntry("Look","ConferenceContents"),userlist.byUinValue(Uins[k]),false));
		}
		setIcon(icons_manager.loadIcon("Online"));
	}
	else {
		kdebugm(KDEBUG_INFO, "Chat::setTitle()\n");
		if (config_file.readEntry("Look","ChatContents").isEmpty())
			title = parse(tr("Chat with ")+"%a (%s[: %d])",userlist.byUinValue(Uins[0]),false);
		else
			title = parse(config_file.readEntry("Look","ChatContents"),userlist.byUinValue(Uins[0]),false);
		setIcon(icons_manager.loadIcon(gg_icons[statusGGToStatusNr(userlist.byUinValue(Uins[0]).status)]));
	}

	title.replace(QRegExp("<br/>"), " ");
	title.replace(QRegExp("&nbsp;"), " ");

	setCaption(title);
	title_buffer = title;
}

void Chat::changeTitle() {
	if(!isActiveWindow()){
		if (caption() == "  ")
			setCaption(title_buffer);
		else
			setCaption("  ");
		title_timer->start(500,TRUE);
	}
}

void Chat::windowActivationChange(bool oldActive) {
	if (isActiveWindow() && title_timer->isActive()){
		title_timer->stop();
		setCaption(title_buffer);
	}
}

void Chat::keyPressEvent(QKeyEvent *e) {
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

/* convert special characters into emoticons, HTML into plain text and so forth */
QString Chat::convertCharacters(QString edit, bool me) {

	// zmieniamy windowsowe \r\n na unixowe \n
	edit.replace( QRegExp("\r\n"), "<br/>" );
	edit.replace( QRegExp("\n"), "<br/>" );

	HtmlDocument doc;
	doc.parseHtml(edit);

	// detekcja adresow url
	doc.convertUrlsToHtml();

	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")!=EMOTS_NONE)
	{
		body->mimeSourceFactory()->addFilePath(emoticons->themePath());
		if (me)
			emoticons->expandEmoticons(doc,config_file.readColorEntry("Look","ChatMyBgColor"));
		else
			emoticons->expandEmoticons(doc,config_file.readColorEntry("Look","ChatUsrBgColor"));
	}
	
	edit=doc.generateHtml();
	return edit;
}

/* unregister us */
void Chat::closeEvent(QCloseEvent *e) {
	kdebugf();	
	QWidget::closeEvent(e);
}

/* look up party's info */
void Chat::userWhois(void) {
	UinType uin;
	
	if (!userbox)
		uin = Uins[0];
	else
		if (userbox->currentItem() == -1)
			uin = Uins[0];
		else
			uin = userlist.byAltNick(userbox->currentText()).uin;
	SearchDialog *sd = new SearchDialog(0, "User info", uin);
	sd->show();
	sd->firstSearch();
}

void Chat::formatMessage(bool me, const QString &altnick, const QString &msg, const QString &time, QString &toadd) {
	QString editext = convertCharacters(msg,me);

	toadd.append("<table width=\"100%\"><tr><td bgcolor=\"");
	if (me)
		toadd.append(config_file.readColorEntry("Look","ChatMyBgColor").name());
	else
		toadd.append(config_file.readColorEntry("Look","ChatUsrBgColor").name());
	toadd.append("\"><font color=\"");
	if (me)
		toadd.append(config_file.readColorEntry("Look","ChatMyFontColor").name());
	else
		toadd.append(config_file.readColorEntry("Look","ChatUsrFontColor").name());
	toadd.append("\"><b>");
	toadd.append(altnick);
	toadd.append(" :: ");
	toadd.append(time + "</b><br/>" + editext + "</font></td></tr></table>");
}

void Chat::scrollMessages(QString &toadd)
{
	if (config_file.readBoolEntry("Chat","ChatPrune"))
		pruneWindow();

	body->viewport()->setUpdatesEnabled(false);
	if (!config_file.readBoolEntry("Chat","ScrollDown"))
		body->setText(toadd + body->text());
	else
	{
		body->setText(body->text() + toadd);
		if (!lockscroll->isOn())
			body->scrollToBottom();
	}
	body->viewport()->setUpdatesEnabled(true);
	body->viewport()->repaint();
}

void Chat::writeMessagesFromHistory(UinsList senders, time_t time)
{
	QString toadd;
	QValueList<HistoryEntry> entries;
	QValueList<HistoryEntry> entriestmp;
	QDateTime date;
	unsigned int from, end, count;
	
	kdebugf();

	date.setTime_t(time);
	count = history.getHistoryEntriesCount(senders);
	end = count - 1;
	
	from = count;
	while (from >= 1 && entries.count() < config_file.readUnsignedNumEntry("History","ChatHistoryCitation")) {
		from = (end < config_file.readUnsignedNumEntry("History", "ChatHistoryCitation")) ? 0 : end - config_file.readUnsignedNumEntry("History","ChatHistoryCitation") + 1;
		entriestmp = history.getHistoryEntries(senders, from, end - from + 1, HISTORYMANAGER_ENTRY_CHATSEND
			| HISTORYMANAGER_ENTRY_MSGSEND | HISTORYMANAGER_ENTRY_CHATRCV | HISTORYMANAGER_ENTRY_MSGRCV);
		kdebugm(KDEBUG_INFO, "Chat::writeMessageFromHistory(): temp entries = %d\n", entriestmp.count());
		if (time) {
			QValueList<HistoryEntry>::iterator it = entriestmp.begin();
			while (it != entriestmp.end()) {
				if ((*it).type == HISTORYMANAGER_ENTRY_CHATRCV
					|| (*it).type == HISTORYMANAGER_ENTRY_MSGRCV) {
						kdebugm(KDEBUG_INFO, "Chat::writeMessageFromHistory(): %s %s\n",
							(const char *)date.toString("dd.MM.yyyy hh:mm:ss").local8Bit(),
							(const char *)(*it).sdate.toString("dd.MM.yyyy hh:mm:ss").local8Bit());
						if (date <= (*it).sdate)
							it = entriestmp.remove(it);
						else
							it++;
						}
				else
					it++;
				}
			}
		if (entriestmp.count())
			entries = entriestmp + entries;
		kdebugm(KDEBUG_INFO, "Chat::writeMessageFromHistory(): entries = %d\n", entries.count());
		end = from - 1;
		}
	from = (entries.count() < config_file.readUnsignedNumEntry("History","ChatHistoryCitation")) ? 0 : entries.count() - config_file.readUnsignedNumEntry("History","ChatHistoryCitation");
	for (unsigned int i = from; i < entries.count(); i++)
		if (entries[i].date.secsTo(QDateTime::currentDateTime()) <= -config_file.readNumEntry("History","ChatHistoryQuotationTime") * 3600)
			if (entries[i].type == HISTORYMANAGER_ENTRY_MSGSEND
				|| entries[i].type == HISTORYMANAGER_ENTRY_CHATSEND)
				formatMessage(true, config_file.readEntry("General","Nick"), entries[i].message,
					printDateTime(entries[i].date), toadd);
			else
				formatMessage(false, entries[i].nick, entries[i].message,
					printDateTime(entries[i].date) + QString(" / S ")
					+ printDateTime(entries[i].sdate), toadd);
	if (toadd.length())
		scrollMessages(toadd);
}

/* invoked from outside when new message arrives, this is the window to the world */
void Chat::checkPresence(UinsList senders, const QString &msg, time_t time, QString &toadd) {
	formatMessage(false, userlist.byUin(senders[0]).altnick, msg, timestamp(time), toadd);

	scrollMessages(toadd);
}

void Chat::alertNewMessage(void) {

	if (config_file.readBoolEntry("Chat","BlinkChatTitle"))
		if (!isActiveWindow() && !title_timer->isActive())
			changeTitle();
}

void Chat::writeMyMessage() {
	QString toadd;
	formatMessage(true,config_file.readEntry("General","Nick"), myLastMessage, timestamp(), toadd);
	scrollMessages(toadd);
	if (!edit->isEnabled())
		cancelMessage();
	edit->clear();
	if (boldbtn->isOn())
		edit->setBold(true);
	if (italicbtn->isOn())
		edit->setItalic(true);
	if (underlinebtn->isOn())
		edit->setUnderline(true);
}

void Chat::addMyMessageToHistory() {
	int uin;

	uin = Uins[0];
	if (config_file.readBoolEntry("General","Logging"))
		history.appendMessage(Uins, uin, myLastMessage, true);
}

void Chat::clearChatWindow(void) {
	body->clear();
	totaloccurences = 0;
}

void Chat::cancelMessage(void) {
	seq = 0;
	disconnect(&event_manager, SIGNAL(ackReceived(int)),
		this, SLOT(ackReceivedSlot(int)));
	edit->setReadOnly(false);
	edit->setEnabled(true);
	edit->setFocus();
	disconnect(sendbtn, SIGNAL(clicked()), this, SLOT(cancelMessage()));
	connect(sendbtn, SIGNAL(clicked()), this, SLOT(sendMessage()));
	sendbtn->setIconSet(QIconSet(icons_manager.loadIcon("SendMessage")));
	sendbtn->setText(tr("&Send"));
}

void Chat::ackReceivedSlot(int Seq) {
	kdebugf();
	if (seq != Seq)
		return;
	kdebugm(KDEBUG_INFO, "Chat::ackReceivedSlot(): This is my ack.\n");
	writeMyMessage();
	seq = 0;
	disconnect(&event_manager, SIGNAL(ackReceived(int)),
		this, SLOT(ackReceivedSlot(int)));
}

/* sends the message typed */
void Chat::sendMessage(void) {

	if (edit->text() == "")
		return;
	
	emit messageSendRequested(this);

	QString mesg;

	if (getCurrentStatus() == GG_STATUS_NOT_AVAIL) {
		QMessageBox::critical(this, tr("Send message error"),
			tr("Application encountered network error."));
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
		escapeSpecialCharacters(myLastMessage);
	kdebugm(KDEBUG_INFO, "Chat::sendMessage():\n%s\n", (const char *)unicode2latin(myLastMessage));
	// zmieniamy unixowe \n na windowsowe \r\n
	myLastMessage.replace(QRegExp("\r\n"), "\n");

	if (mesg.length() >= 2000)
	{
		MessageBox::wrn(tr("Message too long (%1>=%2)").arg(mesg.length()).arg(2000));
		return;
	}

	addMyMessageToHistory();

	QCString msg = unicode2cp(mesg);
	
	bool stop=false;
	emit messageFiltering(Uins,msg,stop);
	if(stop)
		return;

	if (mesg.length() >= 2000)
	{
		MessageBox::wrn(tr("Filtered message too long (%1>=%2)").arg(mesg.length()).arg(2000));
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

 	if (config_file.readBoolEntry("Chat","MessageAcks"))
		connect(&event_manager, SIGNAL(ackReceived(int)), this, SLOT(ackReceivedSlot(int)));
	else
		writeMyMessage();

	if (myLastFormats)
		delete [](char *)myLastFormats;

	if (sess->check & GG_CHECK_WRITE)
		kadusnw->setEnabled(true);
		
	emit messageSent(this);
}

/* prunes messages */
void Chat::pruneWindow(void) {
	int index,occurences;

	occurences = 0;
	int chatPruneLen=config_file.readNumEntry("Chat","ChatPruneLen");
	if (config_file.readBoolEntry("Chat","ScrollDown")) {
		index = -1;
		while (occurences !=chatPruneLen  && totaloccurences > chatPruneLen - 1) {
			index = body->text().findRev(QString("<table"), index - 8);
			occurences++;
			}
		totaloccurences++;

		body->setText(body->text().right(body->text().length() - index));	
		}
	else {
		index = 0;
		while (occurences != chatPruneLen && totaloccurences > chatPruneLen ) {
			if (occurences == 0)
				index = body->text().find(QString("<table"), 0);
			else
				index = body->text().find(QString("<table"), index + 8);

			occurences++;
			}
		totaloccurences++;

		if (totaloccurences > chatPruneLen && index != -1 && index != 0)
			body->setText(body->text().left(index));
		}
}

/* opens messages history */
void Chat::HistoryBox(void) {
	History *hb;

	hb = new History(Uins);
	hb->show();
}

void Chat::emoticonSelectorClicked(void)
{
	//emoticons_selector zawsze bêdzie NULLem gdy wchodzimy do tej funkcji
	//bo EmoticonSelector ma ustawione flagi Qt::WDestructiveClose i Qt::WType_Popup
	//akcj± na opuszczenie okna jest ustawienie zmiennej emoticons_selector w Chacie na NULL
	emoticon_selector = new EmoticonSelector(NULL, "Emoticon selector", this);
	emoticon_selector->alignTo(iconsel);
	emoticon_selector->show();
}

void Chat::changeColor(void)
{
	//sytuacja podobna jak w przypadku emoticon_selectora
	color_selector = new ColorSelector(this);
	color_selector->alignTo(colorbtn);
	color_selector->show();
	connect(color_selector, SIGNAL(colorSelect(const QColor&)), this, SLOT(colorChanged(const QColor&)));
	connect(color_selector, SIGNAL(aboutToClose()), this, SLOT(colorSelectorAboutToClose()));
}

void Chat::colorSelectorAboutToClose() {
	kdebugf();
	color_selector = NULL;
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

	ConfigDialog::addTab("ShortCuts");
	ConfigDialog::addVGroupBox("ShortCuts", "ShortCuts", QT_TRANSLATE_NOOP("@default", "Define keys"));
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "New line / send message:"), "chat_newline", "Return");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Clear Chat:"), "chat_clear", "F9");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Close Chat:"), "chat_close", "Esc");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Bold text:"), "chat_bold", "Ctrl+B");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Italic text:"), "chat_italic", "Ctrl+I");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", QT_TRANSLATE_NOOP("@default", "Underline text:"), "chat_underline", "Ctrl+U");
	
	ConfigDialog::addTab("Chat");
	ConfigDialog::addVGroupBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "Emoticons"));
	ConfigDialog::addComboBox("Chat", "Emoticons", QT_TRANSLATE_NOOP("@default", "Emoticons:"));
	ConfigDialog::addComboBox("Chat", "Emoticons", QT_TRANSLATE_NOOP("@default", "Emoticons theme"));
	ConfigDialog::addVGroupBox("Chat", "Chat", QT_TRANSLATE_NOOP("@default", "WWW options"));
	ConfigDialog::addCheckBox("Chat", "WWW options", QT_TRANSLATE_NOOP("@default", "Use default Web browser"), "DefaultWebBrowser", true);
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

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Look"));
	
	ConfigDialog::addGrid("Look", "Look", "varOpts", 2);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show status button"), "ShowStatusButton", true);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Display group tabs"), "DisplayGroupTabs", true);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show description in userbox"), "ShowDesc", true);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Multiline description in userbox"), "ShowMultilineDesc", true);
		ConfigDialog::addCheckBox("Look", "varOpts", QT_TRANSLATE_NOOP("@default", "Show available users in bold"), "ShowBold", true, QT_TRANSLATE_NOOP("@default","Displays users that are not offline using a bold font"));
	ConfigDialog::addVBox("Look", "Look", "varOpts2");//potrzebne userboksowi
	
	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Colors"));
		ConfigDialog::addVGroupBox("Look", "Colors", QT_TRANSLATE_NOOP("@default", "Chat window"));
			ConfigDialog::addHBox("Look", "Chat window", "own_bg_color_box");
				ConfigDialog::addLabel("Look", "own_bg_color_box", QT_TRANSLATE_NOOP("@default", "Your background color"));
				ConfigDialog::addColorButton("Look", "own_bg_color_box", "", "ChatMyBgColor", config_file.readColorEntry("Look","ChatMyBgColor"), "", "own_bg_color");
			ConfigDialog::addHBox("Look", "Chat window", "his_bg_color_box");
				ConfigDialog::addLabel("Look", "his_bg_color_box", QT_TRANSLATE_NOOP("@default", "User background color"));
				ConfigDialog::addColorButton("Look", "his_bg_color_box", "", "ChatUsrBgColor", config_file.readColorEntry("Look","ChatUsrBgColor"), "", "his_bg_color");
			ConfigDialog::addHBox("Look", "Chat window", "own_font_color_box");
				ConfigDialog::addLabel("Look", "own_font_color_box", QT_TRANSLATE_NOOP("@default", "Your font color"));
				ConfigDialog::addColorButton("Look", "own_font_color_box", "", "ChatMyFontColor", config_file.readColorEntry("Look","ChatMyFontColor"), "", "own_font_color");
			ConfigDialog::addHBox("Look", "Chat window", "his_font_color_box");
				ConfigDialog::addLabel("Look", "his_font_color_box", QT_TRANSLATE_NOOP("@default", "User font color"));
				ConfigDialog::addColorButton("Look", "his_font_color_box", "", "ChatUsrFontColor", config_file.readColorEntry("Look","ChatUsrFontColor"), "", "his_font_color");
	
	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Fonts"));
		ConfigDialog::addSelectFont("Look", "Fonts", QT_TRANSLATE_NOOP("@default", "Font in chat window"), "ChatFont", def_font.toString(), "", "chat_font_box");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Previews"));
		ConfigDialog::addVGroupBox("Look", "Previews", QT_TRANSLATE_NOOP("@default", "Chat preview"));
			ConfigDialog::addHBox("Look", "Chat preview", "chat_prvw");
				ConfigDialog::addLabel("Look", "chat_prvw", QT_TRANSLATE_NOOP("@default", "<b>Me</b> 00:00:00"), "chat_me");
				ConfigDialog::addLabel("Look", "chat_prvw", QT_TRANSLATE_NOOP("@default", "<b>Other party</b> 00:00:02"), "chat_other");

	ConfigDialog::addVGroupBox("Look", "Look", QT_TRANSLATE_NOOP("@default", "Other"));
		ConfigDialog::addLineEdit("Look", "Other", QT_TRANSLATE_NOOP("@default", "Chat window title syntax:"), "ChatContents", "", "Syntax the same as in information panel.");
		ConfigDialog::addHBox("Look", "Other", "conference");
			ConfigDialog::addLineEdit("Look", "conference", QT_TRANSLATE_NOOP("@default", "Conference window title prefix:"), "ConferencePrefix", "", QT_TRANSLATE_NOOP("@default", "This text will be before syntax.\nIf you leave blank, default settings will be used."));
			ConfigDialog::addLineEdit("Look", "conference", QT_TRANSLATE_NOOP("@default", "syntax:"), "ConferenceContents", "%a (%s[: %d])", QT_TRANSLATE_NOOP("@default", "Syntax the same as in information panel."));

	config_file.addVariable("Chat", "EmoticonsStyle", EMOTS_ANIMATED);
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));
	
	ChatSlots *chatslots =new ChatSlots();
	ConfigDialog::registerSlotOnCreate(chatslots,SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(chatslots,SLOT(onDestroyConfigDialog()));
	ConfigDialog::connectSlot("Chat", "Emoticons:", SIGNAL(activated(int)), chatslots, SLOT(chooseEmoticonsStyle(int)));
	ConfigDialog::connectSlot("Chat", "Use default Web browser", SIGNAL(toggled(bool)), chatslots, SLOT(onDefWebBrowser(bool)));
	ConfigDialog::connectSlot("Chat", "Automatically prune chat messages", SIGNAL(toggled(bool)), chatslots, SLOT(onPruneChat(bool)));
	ConfigDialog::connectSlot("Chat", "Automatically fold links", SIGNAL(toggled(bool)), chatslots, SLOT(onFoldLink(bool)));
	
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), chatslots, SLOT(chooseColor(const char *, const QColor&)), "own_bg_color");
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), chatslots, SLOT(chooseColor(const char *, const QColor&)), "his_bg_color");
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), chatslots, SLOT(chooseColor(const char *, const QColor&)), "own_font_color");
	ConfigDialog::connectSlot("Look", "", SIGNAL(changed(const char *, const QColor&)), chatslots, SLOT(chooseColor(const char *, const QColor&)), "his_font_color");

	ConfigDialog::connectSlot("Look", "Font in chat window", SIGNAL(changed(const char *, const QFont&)), chatslots, SLOT(chooseFont(const char *, const QFont&)), "chat_font_box");
	
	chat_manager=new ChatManager();
	connect(&event_manager,SIGNAL(chatMsgReceived1(UinsList,const QString&,time_t,bool&)),
		chat_manager,SLOT(chatMsgReceived(UinsList,const QString&,time_t,bool&)));
}

const UinsList& Chat::uins()
{
	return Uins;
}

ColorSelectorButton::ColorSelectorButton(QWidget* parent, const QColor& qcolor) : QToolButton(parent)
{
	QPixmap p(15,15);
	p.fill(qcolor);
	color = qcolor;
	setPixmap(p);
//	setAutoRaise(true);
	setMouseTracking(true);
	QToolTip::add(this,color.name());
	connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void ColorSelectorButton::buttonClicked()
{
	emit clicked(color);
}

ColorSelector::ColorSelector(QWidget *parent, const char *name) : QWidget (parent, name,Qt::WType_Popup|Qt::WDestructiveClose)
{
	QValueList<QColor> qcolors;
	int i;

	for (i = 0; i < 16; i++)
		qcolors.append(colors[i]);

	int selector_count=qcolors.count();
	int selector_width=(int)sqrt((double)selector_count);
	int btn_width=0;
	QGridLayout *grid = new QGridLayout(this, 0, selector_width, 0, 0);

	for(int i=0; i<selector_count; i++)
	{
		ColorSelectorButton* btn = new ColorSelectorButton(this, qcolors[i]);
		btn_width=btn->sizeHint().width();
		grid->addWidget(btn, i/selector_width, i%selector_width);
		connect(btn,SIGNAL(clicked(const QColor&)),this,SLOT(iconClicked(const QColor&)));
	}
}

void ColorSelector::iconClicked(const QColor& color)
{
	emit colorSelect(color);
	close();
}

void ColorSelector::closeEvent(QCloseEvent *e) {
	kdebugf();
	emit aboutToClose();
	QWidget::closeEvent(e);
}

void ColorSelector::alignTo(QWidget* w)
{
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
}

void ChatSlots::onCreateConfigDialog()
{
	kdebugf();
	QComboBox *cb_emoticons_style= ConfigDialog::getComboBox("Chat", "Emoticons:");
	cb_emoticons_style->insertItem(tr("None"));
	cb_emoticons_style->insertItem(tr("Static"));
	cb_emoticons_style->insertItem(tr("Animated"));
	cb_emoticons_style->setCurrentItem(config_file.readNumEntry("Chat", "EmoticonsStyle"));

	QComboBox* cb_emoticons_theme= ConfigDialog::getComboBox("Chat", "Emoticons theme");
	cb_emoticons_theme->insertStringList(emoticons->themes());
	cb_emoticons_theme->setCurrentText(config_file.readEntry("Chat", "EmoticonsTheme"));

	if ((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") == EMOTS_NONE)
		(cb_emoticons_theme)->setEnabled(false);

	QCheckBox *c_defweb= ConfigDialog::getCheckBox("Chat", "Use default Web browser");
	QLineEdit *l_webbrow= ConfigDialog::getLineEdit("Chat", "Custom Web browser");
	
	if (c_defweb->isChecked())
		((QHBox*)l_webbrow->parent())->setEnabled(false);
	
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
}

void ChatSlots::onPruneChat(bool toggled)
{
	ConfigDialog::getHGroupBox("Chat", "Message pruning")->setEnabled(toggled);
}

void ChatSlots::onFoldLink(bool toggled)
{
	ConfigDialog::getHGroupBox("Chat", "Link folding")->setEnabled(toggled);
}

void ChatSlots::onDefWebBrowser(bool toggled)
{
	QLineEdit *l_webbrow= ConfigDialog::getLineEdit("Chat", "Custom Web browser");
	((QHBox*)l_webbrow->parent())->setEnabled(!toggled);
}

void ChatSlots::onDestroyConfigDialog()
{
	kdebugf();
	
	QComboBox* cb_emoticons_theme= ConfigDialog::getComboBox("Chat", "Emoticons theme");
	config_file.writeEntry("Chat", "EmoticonsTheme",cb_emoticons_theme->currentText());
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));
	QComboBox *cb_emoticons_style= ConfigDialog::getComboBox("Chat", "Emoticons:");
	config_file.writeEntry("Chat", "EmoticonsStyle", cb_emoticons_style->currentItem());

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
}

void ChatSlots::chooseColor(const char *name, const QColor &color)
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
}

void ChatSlots::chooseFont(const char *name, const QFont &font)
{
	kdebugf();
	QLabel *preview1= ConfigDialog::getLabel("Look", "<b>Me</b> 00:00:00", "chat_me");
	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Other party</b> 00:00:02", "chat_other");
	if (QString(name)=="chat_font_box")
	{
		preview1->setFont(font);
		preview2->setFont(font);
	}
}

void ChatSlots::chooseEmoticonsStyle(int index) {
	QComboBox *emotheme_box= ConfigDialog::getComboBox("Chat","Emoticons theme");
	emotheme_box->setEnabled(index!=0);
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
}
