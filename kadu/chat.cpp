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

#include <math.h>
#include <sys/stat.h>
#include <stdlib.h>
//
#include "config_dialog.h"
#include "config_file.h"
#include "kadu.h"
#include "events.h"
#include "chat.h"
#include "search.h"
#include "history.h"
#include "emoticons.h"
#include "pending_msgs.h"
#include "dock_widget.h"
#include "debug.h"
#include "sound.h"
#include "gadu.h"
#include "hints.h"
#ifdef HAVE_OPENSSL
extern "C"
{
#include "simlite.h"
};
#endif
//

ChatManager::ChatManager() : QObject()
{
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
	for(int i=0; i<Chats.count(); i++)
		if(Chats[i]==chat)
		{
			Chats.remove(Chats.at(i));
			return;
		}
}

void ChatManager::refreshTitles()
{
	for (int i = 0; i < Chats.count(); i++)
		Chats[i]->setTitle();
}

void ChatManager::refreshTitlesForUin(uin_t uin)
{
	for (int i = 0; i < Chats.count(); i++)
		if (Chats[i]->uins().contains(uin))
			Chats[i]->setTitle();
}

void ChatManager::changeAppearance()
{
	for (int i = 0; i < Chats.count(); i++)
		Chats[i]->changeAppearance();
}

void ChatManager::enableEncryptionBtnForUins(UinsList uins)
{
	for(int i=0; i<Chats.count(); i++)
		if(Chats[i]->uins().equals(uins))
		{
			Chats[i]->setEncryptionBtnEnabled(true);
			return;
		}
}

Chat* ChatManager::findChatByUins(UinsList uins)
{
	for(int i=0; i<Chats.count(); i++)
		if(Chats[i]->uins().equals(uins))
			return Chats[i];
	return NULL;
}

int ChatManager::openChat(UinsList senders,time_t time)
{
	for (int i = 0; i < Chats.count(); i++)
		if (Chats[i]->uins().equals(senders))
		{
			Chats[i]->raise();
			Chats[i]->setActiveWindow();
			return i;	
		}
	Chat* chat = new Chat(senders, 0);
	chat->setTitle();
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
	for (int j = 0; j < p.uins.count(); j++)
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
	{
		k = openChat(uins,0);
	}
}

void ChatManager::openPendingMsgs()
{
	UinsList uins;
	int i, k = -1;
	PendingMsgs::Element elem;
	QString toadd;
	bool stop = false;
	UserListElement e;

	kdebug("ChatManager::openPendingMsgs()\n");

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
		kdebug("ChatManager::openPendingMsgs() end\n");
		Chats[k]->scrollMessages(toadd);
		UserBox::all_refresh();
	}
}

void ChatManager::sendMessage(uin_t uin,UinsList selected_uins)
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

KaduTextBrowser::KaduTextBrowser(QWidget *parent, const char *name)
	: QTextBrowser(parent, name) {

}

void KaduTextBrowser::setSource(const QString &name) {
}

CustomInput::CustomInput(QWidget *parent, const char *name) : QMultiLineEdit(parent, name) {
}

void CustomInput::keyPressEvent(QKeyEvent * e) {
	kdebug("CustomInput::keyPressEvent()\n");
	if (autosend_enabled && ((HotKey::shortCut(e,"chat_newline")) || e->key()==Key_Enter)&& !(e->state() & ShiftButton)) {
		kdebug("CustomInput::keyPressEvent(): emit enterPressed()\n");
		emit sendMessage();
		}
	else {
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
		if (HotKey::shortCut(e,"chat_bold"))	
			{
				emit specialKeyPressed(CustomInput::KEY_BOLD);
				return;
			}
		else if (HotKey::shortCut(e,"chat_italic"))
			{
				emit specialKeyPressed(CustomInput::KEY_ITALIC);
				return;
			}
		else if (HotKey::shortCut(e,"chat_underline"))
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

Chat::Chat(UinsList uins, QWidget *parent, const char *name)
 : QWidget(parent, name, Qt::WDestructiveClose), Uins(uins)
{
	int i;
	QValueList<int> sizes;

	emoticon_selector = NULL;
	color_selector = NULL;

	title_timer = new QTimer(this);
	connect(title_timer,SIGNAL(timeout()),this,SLOT(changeTitle()));
  
	/* register us in the chats registry... */
	index=chat_manager->registerChat(this);

	QSplitter *split1, *split2;

	split1 = new QSplitter(Qt::Vertical, this);

	if (uins.count() > 1) {
		split2 = new QSplitter(Qt::Horizontal, split1);
		body = new KaduTextBrowser(split2);
		}
	else 
		body = new KaduTextBrowser(split1);
		
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")==EMOTS_ANIMATED)
		body->setStyleSheet(new AnimStyleSheet(body,emoticons.themePath()));
	
	body->setMinimumSize(QSize(100,100));
	body->setFont(config_file.readFontEntry("Look","ChatFont"));
	QObject::connect(body, SIGNAL(linkClicked(const QString &)), this, SLOT(hyperlinkClicked(const QString &)));

	QPoint pos = QCursor::pos();
	
	if (uins.count() > 1) {
		setGeometry((pos.x() + 550) / 2, (pos.y() + 400) / 2, 550, 400);
		userbox = new UserBox(split2);
		userbox->setMinimumSize(QSize(30,30));
		userbox->setPaletteBackgroundColor(config_file.readColorEntry("Look","UserboxBgColor"));
		userbox->setPaletteForegroundColor(config_file.readColorEntry("Look","UserboxFgColor"));
		userbox->QListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));

		for (i = 0; i < uins.count(); i++)
			userbox->addUser(userlist.byUin(uins[i]).altnick);
		userbox->refresh();

		connect(userbox, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)),
		UserBox::userboxmenu, SLOT(show(QListBoxItem *)));

		sizes.append(3);
		sizes.append(1);
		split2->setSizes(sizes);
		}
	else {
		setGeometry((pos.x() + 400) / 2, (pos.y() + 400) / 2, 400, 400);
		userbox = NULL;
		}
		
	QVBox *downpart = new QVBox(split1);
	QHBox *edtbuttontray = new QHBox(downpart);

	QLabel *edt = new QLabel(tr("Edit window:"),edtbuttontray);
	QToolTip::add(edt, tr("This is where you type in the text to be sent"));

	buttontray = new QHBox(edtbuttontray);

	autosend = new QPushButton(buttontray);
	autosend->setPixmap(loadIcon("key_enter.png"));
	autosend->setToggleButton(true);
	QToolTip::add(autosend, tr("Enter key sends message"));

	lockscroll = new QPushButton(buttontray);
	lockscroll->setPixmap(loadIcon("scroll_lock.png"));
	lockscroll->setToggleButton(true);
	QToolTip::add(lockscroll, tr("Blocks scrolling"));

#ifdef HAVE_OPENSSL
	encryption = new QPushButton(buttontray);
	connect(encryption, SIGNAL(clicked()), this, SLOT(regEncryptSend()));

	QString keyfile_path;
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(uins[0]));
	keyfile_path.append(".pem");
	QFileInfo keyfile(keyfile_path);
	bool encryption_possible =
		(keyfile.permission(QFileInfo::ReadUser) && uins.count() == 1);

	setupEncryptButton(config_file.readBoolEntry("Chat", "Encryption") && encryption_possible);
	
	encryption->setEnabled(encryption_possible);	
#endif
	
	QPushButton *clearchat= new QPushButton(buttontray);
	clearchat->setPixmap(loadIcon("eraser.png"));
	QToolTip::add(clearchat, tr("Clear messages in chat window"));

	iconsel = new QPushButton(buttontray);
	iconsel->setPixmap(loadIcon("icons.png"));
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")==EMOTS_NONE)
	{
		QToolTip::add(iconsel, tr("Insert emoticon - enable in configuration"));
		iconsel->setEnabled(false);
	}
	else
		QToolTip::add(iconsel, tr("Insert emoticon"));

	QPushButton *history = new QPushButton(buttontray);
	history->setPixmap(loadIcon("history.png"));
	QToolTip::add(history, tr("Show history"));

	QPushButton *whois = new QPushButton(buttontray);
	whois->setPixmap(loadIcon("viewmag.png"));
	QToolTip::add(whois, tr("Lookup user info"));

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
	boldbtn = new QPushButton("B", btnpart);
	boldbtn->setToggleButton(true);
	afont.setBold(true);
	boldbtn->setFont(afont);
	italicbtn = new QPushButton("I", btnpart);
	italicbtn->setToggleButton(true);
	afont.setBold(false);
	afont.setItalic(true);
	italicbtn->setFont(afont);
	underlinebtn = new QPushButton("U", btnpart);
	underlinebtn->setToggleButton(true);
	afont.setItalic(false);
	afont.setUnderline(true);
	underlinebtn->setFont(afont);
	colorbtn = new QPushButton(btnpart);
//	colorbtn->setMinimumSize(boldbtn->width(), boldbtn->height());
	QPixmap p(16, 16);
	p.fill(colors[15]);
	colorbtn->setPixmap(p);

	QHBox *fillerbox = new QHBox(btnpart);

	sendbtn = new QPushButton(QIconSet(loadIcon("forward.png")),tr("&Send"),btnpart);
	sendbtn->setFixedWidth(120);
	connect(sendbtn, SIGNAL(clicked()), this, SLOT(sendMessage()));
	QAccel *acc = new QAccel(this);
	acc->connectItem(acc->insertItem(Key_Return + CTRL), this, SLOT(sendMessage()));

	acc = new QAccel(this);
	acc->connectItem(acc->insertItem(Key_PageUp + SHIFT), this, SLOT(pageUp()));
	acc = new QAccel(this);
	acc->connectItem(acc->insertItem(Key_PageDown + SHIFT), this, SLOT(pageDown()));

	btnpart->setStretchFactor(boldbtn, 1);
	btnpart->setStretchFactor(italicbtn, 1);
	btnpart->setStretchFactor(underlinebtn, 1);
	btnpart->setStretchFactor(colorbtn, 1);
	btnpart->setStretchFactor(fillerbox, 50);
	btnpart->setStretchFactor(sendbtn, 1);

	sizes.clear();
	sizes.append(3);
	sizes.append(2);
	split1->setSizes(sizes);

	QGridLayout *grid = new QGridLayout (this, 5, 4, 3, 3);
	grid->addMultiCellWidget(split1, 0, 4, 0, 3);
	grid->addRowSpacing(1, 5);
	grid->setRowStretch(0, 2);

	QMimeSourceFactory *bodyformat;
	bodyformat = new QMimeSourceFactory;

	body->setMimeSourceFactory(bodyformat);
	body->setTextFormat(Qt::RichText);
	edit->setMimeSourceFactory(bodyformat);
	edit->setTextFormat(Qt::RichText);

	connect(autosend, SIGNAL(toggled(bool)), edit, SLOT(setAutosend(bool)));
	connect(history, SIGNAL(clicked()), this, SLOT(HistoryBox()));
	connect(iconsel, SIGNAL(clicked()), this, SLOT(insertEmoticon()));
	connect(whois, SIGNAL(clicked()), this, SLOT(userWhois()));
	connect(clearchat, SIGNAL(clicked()), this, SLOT(clearChatWindow()));
	connect(boldbtn, SIGNAL(toggled(bool)), this, SLOT(toggledBold(bool)));
	connect(italicbtn, SIGNAL(toggled(bool)), this, SLOT(toggledItalic(bool)));
	connect(underlinebtn, SIGNAL(toggled(bool)), this, SLOT(toggledUnderline(bool)));
	connect(colorbtn, SIGNAL(clicked()), this, SLOT(changeColor()));
	connect(edit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(curPosChanged(int, int)));
	connect(edit, SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(edit, SIGNAL(specialKeyPressed(int)), this, SLOT(specialKeyPressed(int)));

	totaloccurences = 0;

	edit->setFocus();
}

Chat::~Chat() {
	chat_manager->unregisterChat(this);

	disconnect(&event_manager, SIGNAL(ackReceived(int)),
		this, SLOT(ackReceivedSlot(int)));

	if (userbox)
		delete userbox;
		
	kdebug("Chat::~Chat: chat destroyed: index %d\n", index);
}

void Chat::specialKeyPressed(int key) {
	kdebug("Chat::specialKeyPressed()\n");
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
	kdebug("Chat::toggledBold()\n");
	edit->setBold(on);
}

void Chat::toggledItalic(bool on) {
	kdebug("Chat::toggledItalic()\n");
	edit->setItalic(on);
}

void Chat::toggledUnderline(bool on) {
	kdebug("Chat::toggledUnderline()\n");
	edit->setUnderline(on);
}

void Chat::curPosChanged(int para, int pos) {
	int i;

	kdebug("Chat::curPosChanged()\n");
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

void Chat::setupEncryptButton(bool enabled) {
#ifdef HAVE_OPENSSL
	encrypt_enabled = enabled;
	QToolTip::remove(encryption);
	if (enabled) {
		QToolTip::add(encryption, tr("Disable encryption for this conversation"));
		encryption->setPixmap(loadIcon("encrypted.png"));
		}
	else {
		QToolTip::add(encryption, tr("Enable encryption for this conversation"));
		encryption->setPixmap(loadIcon("decrypted.png"));
		}
#endif		
}

void Chat::pageUp() {
	body->scrollBy(0, (body->height() * -2) / 3);
}

void Chat::pageDown() {
	body->scrollBy(0, (body->height() * 2) / 3);
}

void Chat::setEncryptionBtnEnabled(bool enabled) {
#ifdef HAVE_OPENSSL
	encryption->setEnabled(enabled && config_file.readBoolEntry("Chat", "Encryption"));
#endif
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
		kdebug("Chat::setTitle(): Uins.size() > 1\n");
		if (config_file.readEntry("Look","ConferencePrefix").isEmpty())
			title = tr("Conference with ");
		else
			title = config_file.readEntry("Look","ConferencePrefix");
		for (int k = 0; k < Uins.size(); k++) {
			if (k)
				title.append(", ");
			title.append(parse(config_file.readEntry("Look","ConferenceContents"),userlist.byUinValue(Uins[k]),false));
		}
		setIcon(*icons->loadIcon("online"));
	}
	else {
		kdebug("Chat::setTitle()\n");
		if (config_file.readEntry("Look","ChatContents").isEmpty())
			title = parse(tr("Chat with ")+"%a (%s[: %d])",userlist.byUinValue(Uins[0]),false);
		else
			title = parse(config_file.readEntry("Look","ChatContents"),userlist.byUinValue(Uins[0]),false);
		setIcon(*icons->loadIcon(gg_icons[statusGGToStatusNr(userlist.byUinValue(Uins[0]).status)]));
	}

	title.replace(QRegExp("\n"), " ");

	setCaption(title);
	title_buffer = title;
}

void Chat::changeTitle() {
	if(!isActiveWindow()){
		if (caption() == "  "){
			setCaption(title_buffer);
			title_timer->start(500,TRUE);
		}
		else{    
			setCaption("  ");
			title_timer->start(500,TRUE);
		}
	}
}

void Chat::windowActivationChange(bool oldActive) {
	if (isActiveWindow() && title_timer->isActive()){
		title_timer->stop();
		setCaption(title_buffer);
	}
}

void Chat::keyPressEvent(QKeyEvent *e) {
	if (HotKey::shortCut(e,"chat_clear"))
		clearChatWindow();
	else
	if (HotKey::shortCut(e,"chat_close"))
		close();
	else
	if (HotKey::shortCut(e,"kadu_viewhistory"))
		HistoryBox();
	else
	if (HotKey::shortCut(e,"kadu_searchuser"))
		userWhois();
	QWidget::keyPressEvent(e);
}

void Chat::regEncryptSend(void) {
#ifdef HAVE_OPENSSL
	encrypt_enabled = !encrypt_enabled;
	setupEncryptButton(encrypt_enabled);
#endif
}

/* convert special characters into emoticons, HTML into plain text and so forth */
QString Chat::convertCharacters(QString edit, bool me) {

	// zmieniamy windowsowe \r\n na unixowe \n
	edit.replace( QRegExp("\r\n"), "\n" );
	edit.replace( QRegExp("\n"), "<BR>" );

	HtmlDocument doc;
	doc.parseHtml(edit);

	// detekcja adresow url
	QRegExp url_regexp("(http://|https://|www\\.|ftp://)[a-zA-Z0-9\\-\\._/~?=&#\\+%:;,!]+");
	for(int i=0; i<doc.countElements(); i++)
	{
		if(doc.isTagElement(i))
			continue;
		QString text=doc.elementText(i);
		int p=url_regexp.search(text);
		if (p < 0)
			continue;
		int l=url_regexp.matchedLength();
		QString link="<a href=\""+text.mid(p,l)+"\">"+
			text.mid(p,l)+"</a>";
		doc.splitElement(i,p,l);
		doc.setElementValue(i,link,true);
	};

	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle")!=EMOTS_NONE)
	{
		body->mimeSourceFactory()->addFilePath(emoticons.themePath());
		if (me)
			emoticons.expandEmoticons(doc,config_file.readColorEntry("Look","ChatMyBgColor"));
		else
			emoticons.expandEmoticons(doc,config_file.readColorEntry("Look","ChatUsrBgColor"));
	};
	
	edit=doc.generateHtml();
	return edit;
}

/* unregister us */
void Chat::closeEvent(QCloseEvent *e) {
	kdebug("Chat::closeEvent()\n");	
	QWidget::closeEvent(e);
}

/* look up party's info */
void Chat::userWhois(void) {
	uin_t uin;
	
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

void Chat::hyperlinkClicked(const QString &link) {
	openWebBrowser(link);
}

void Chat::formatMessage(bool me, const QString &altnick, const QString &msg, const QString &time, QString &toadd) {
	QString editext = convertCharacters(msg,me);

	toadd.append("<TABLE width=\"100%\"><TR><TD bgcolor=\"");
	if (me)
	    	toadd.append(config_file.readColorEntry("Look","ChatMyBgColor").name());
	else
	    	toadd.append(config_file.readColorEntry("Look","ChatUsrBgColor").name());
	toadd.append("\"><FONT color=\"");
	if (me)
		toadd.append(config_file.readColorEntry("Look","ChatMyFontColor").name());
	else
		toadd.append(config_file.readColorEntry("Look","ChatUsrFontColor").name());
	toadd.append("\"><B>");
	toadd.append(altnick);
	toadd.append(" :: ");
	toadd.append(time + "</B><BR>" + editext + "</TD></TR></TABLE></FONT>");
}

void Chat::scrollMessages(QString &toadd) {
	if (config_file.readBoolEntry("Chat","ChatPrune"))
		pruneWindow();

	body->viewport()->setUpdatesEnabled(false);
	if (!config_file.readBoolEntry("Chat","ScrollDown"))
		body->setText(toadd + body->text());
	else {
		body->setText(body->text() + toadd);
		if (!lockscroll->isOn())
			body->scrollToBottom();
		}
	body->viewport()->setUpdatesEnabled(true);
	body->viewport()->repaint();
}

void Chat::writeMessagesFromHistory(UinsList senders, time_t time) {
	QString toadd;
	QValueList<HistoryEntry> entries;
	QValueList<HistoryEntry> entriestmp;
	QDateTime date;
	int i, from, end, count;
	
	kdebug("Chat::writeMessageFromHistory()\n");

	date.setTime_t(time);
	count = history.getHistoryEntriesCount(senders);
	end = count - 1;
	
	while (end >= 0 && entries.count() < config_file.readNumEntry("History","ChatHistoryCitation")) {
		from = (end < config_file.readNumEntry("History", "ChatHistoryCitation")) ? 0 : end - config_file.readNumEntry("History","ChatHistoryCitation") + 1;
		entriestmp = history.getHistoryEntries(senders, from, end - from + 1, HISTORYMANAGER_ENTRY_CHATSEND
			| HISTORYMANAGER_ENTRY_MSGSEND | HISTORYMANAGER_ENTRY_CHATRCV | HISTORYMANAGER_ENTRY_MSGRCV);
		kdebug("Chat::writeMessageFromHistory(): temp entries = %d\n", entriestmp.count());
		if (time) {
			QValueList<HistoryEntry>::iterator it = entriestmp.begin();
			while (it != entriestmp.end()) {
				if ((*it).type == HISTORYMANAGER_ENTRY_CHATRCV
					|| (*it).type == HISTORYMANAGER_ENTRY_MSGRCV) {
						kdebug("Chat::writeMessageFromHistory(): %s %s\n",
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
		kdebug("Chat::writeMessageFromHistory(): entries = %d\n", entries.count());
		end = from - 1;
		}
	from = (entries.count() < config_file.readNumEntry("History","ChatHistoryCitation")) ? 0 : entries.count() - config_file.readNumEntry("History","ChatHistoryCitation");
	for (i = from; i < entries.count(); i++)
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


	if (config_file.readBoolEntry("Sounds","PlaySoundChat"))
		{
		    if (config_file.readBoolEntry("Sounds","PlaySoundChatInvisible")){
			if (isActiveWindow())
				return;
										     }

	QString chatsound;
	if (config_file.readEntry("Sounds", "SoundTheme") == "Custom")
		chatsound=config_file.readEntry("Sounds", "Chat_sound");
	else 
		chatsound=soundmanager.themePath()+ "/" +soundmanager.getThemeEntry("Chat");

	soundmanager.playSound(chatsound);
			
		}
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
	sendbtn->setIconSet(QIconSet(loadIcon("forward.png")));
	sendbtn->setText(tr("&Send"));
}

void Chat::ackReceivedSlot(int Seq) {
	kdebug("Chat::ackReceivedSlot()\n");
	if (seq != Seq)
		return;
	kdebug("Chat::ackReceivedSlot(): This is my ack.\n");
	writeMyMessage();
	seq = 0;
	disconnect(&event_manager, SIGNAL(ackReceived(int)),
		this, SLOT(ackReceivedSlot(int)));
}

/* sends the message typed */
void Chat::sendMessage(void) {
	QString mesg;

	if (getActualStatus() == GG_STATUS_NOT_AVAIL) {
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
			(void *)((char *)(myLastFormats) + sizeof(struct gg_msg_richtext)));
	else
		escapeSpecialCharacters(myLastMessage);
	kdebug("Chat::sendMessage():\n%s\n", myLastMessage.latin1());
	myLastMessage.replace(QRegExp("\r\n"), "\n");

	if (!mesg.length() || mesg.length() >= 2000)
		return;

	addMyMessageToHistory();
	// zmieniamy unixowe \n na windowsowe \r\n

	if (config_file.readBoolEntry("Chat","MessageAcks")) {
		edit->setReadOnly(true);	
		edit->setEnabled(false);
		disconnect(sendbtn, SIGNAL(clicked()), this, SLOT(sendMessage()));
		connect(sendbtn, SIGNAL(clicked()), this, SLOT(cancelMessage()));
		sendbtn->setIconSet(QIconSet(loadIcon("stop.png")));
		sendbtn->setText(tr("&Cancel"));
		}

	char* tmp = strdup(unicode2cp(mesg).data());
	
#ifdef HAVE_OPENSSL
	if (Uins.count()==1 && encrypt_enabled)
	{
		char* encrypted = sim_message_encrypt((unsigned char *)tmp, Uins[0]);
		free(tmp);
		tmp=encrypted;		
	}	
#endif

	if (tmp != NULL)
	{
		if (myLastFormatsLength)
			seq = gadu->sendMessageRichText(Uins, tmp,
				(unsigned char *)myLastFormats, myLastFormatsLength);
		else
			seq = gadu->sendMessage(Uins, tmp);
		free(tmp);
	}

 	if (config_file.readBoolEntry("Chat","MessageAcks"))
	{
		connect(&event_manager, SIGNAL(ackReceived(int)),
			this, SLOT(ackReceivedSlot(int)));
	}
	else
	{
		writeMyMessage();
	}	

	if (myLastFormats)
		delete [](char *)myLastFormats;

	if (sess->check & GG_CHECK_WRITE)
		kadusnw->setEnabled(true);		
}

/* prunes messages */
void Chat::pruneWindow(void) {
	int index,occurences;

	occurences = 0;
	if (config_file.readBoolEntry("Chat","ScrollDown")) {
		index = -1;
		while (occurences != config_file.readNumEntry("Chat","ChatPruneLen") && totaloccurences > config_file.readNumEntry("Chat","ChatPruneLen") - 1) {
			index = body->text().findRev(QString("<TABLE"), index - 8);
			occurences++;
			}
		totaloccurences++;

		body->setText(body->text().right(body->text().length() - index));	
		}
	else {
		index = 0;
		while (occurences != config_file.readNumEntry("Chat","ChatPruneLen") && totaloccurences > config_file.readNumEntry("Chat","ChatPruneLen") ) {
			if (occurences == 0)
				index = body->text().find(QString("<TABLE"), 0);
			else
				index = body->text().find(QString("<TABLE"), index + 8);

			occurences++;
			}
		totaloccurences++;

		if (totaloccurences > config_file.readNumEntry("Chat","ChatPruneLen") && index != -1 && index != 0)
			body->setText(body->text().left(index));
		}
}

/* opens messages history */
void Chat::HistoryBox(void) {
	History *hb;

	hb = new History(Uins);
	hb->show();
}

void Chat::insertEmoticon(void)
{
	if (emoticon_selector == NULL)
	{
		emoticon_selector = new EmoticonSelector(NULL, "Emoticon selector", this);
		emoticon_selector->alignTo(iconsel);
		emoticon_selector->show();
	}
	else
	{
		emoticon_selector->close();
		emoticon_selector = NULL;
	}
}

void Chat::changeColor(void)
{
	if (color_selector == NULL) {
		color_selector = new ColorSelector(this);
		color_selector->alignTo(colorbtn);
		color_selector->show();
		connect(color_selector, SIGNAL(colorSelect(const QColor&)), this, SLOT(colorChanged(const QColor&)));
		connect(color_selector, SIGNAL(aboutToClose()), this, SLOT(aboutToClose()));
		}
	else
		color_selector = NULL;
}

void Chat::aboutToClose() {
	kdebug("Chat::aboutToClose()\n");
	color_selector = NULL;
}

void Chat::colorChanged(const QColor& color) {
	color_selector = NULL;
	QPixmap p(16, 16);
	p.fill(color);
	colorbtn->setPixmap(p);
	edit->setColor(color);
	actcolor = color;
}

/* adds an emoticon code to the edit window */
void Chat::addEmoticon(QString string) {

	if (string.length()) {
		string.replace(QRegExp("&lt;"), "<");
		string.replace(QRegExp("&gt;"), ">");
		edit->insert(string);
		edit->setFocus();
		}
	emoticon_selector = NULL;
}



void Chat::initModule()
{
	QT_TRANSLATE_NOOP("@default", "General");
	QT_TRANSLATE_NOOP("@default", "Open chat window on new message");
	QT_TRANSLATE_NOOP("@default", "Define keys");
	QT_TRANSLATE_NOOP("@default", "New line / send message:");
	QT_TRANSLATE_NOOP("@default", "Clear Chat:");
	QT_TRANSLATE_NOOP("@default", "Close Chat:");
	QT_TRANSLATE_NOOP("@default", "Bold text:");
	QT_TRANSLATE_NOOP("@default", "Italic text:");
	QT_TRANSLATE_NOOP("@default", "Underline text:");
	QT_TRANSLATE_NOOP("@default", "Emoticons");
	QT_TRANSLATE_NOOP("@default", "Emoticons:");
	QT_TRANSLATE_NOOP("@default", "Emoticons theme");
	QT_TRANSLATE_NOOP("@default", "WWW options");
	QT_TRANSLATE_NOOP("@default", "Use default Web browser");
	QT_TRANSLATE_NOOP("@default", "Custom Web browser");
	QT_TRANSLATE_NOOP("@default", "Automatically prune chat messages");
	QT_TRANSLATE_NOOP("@default", "Message pruning");
	QT_TRANSLATE_NOOP("@default", "Reduce the number of visible messages to");
	QT_TRANSLATE_NOOP("@default", "Use encryption");
	QT_TRANSLATE_NOOP("@default", "Encryption properties");
	QT_TRANSLATE_NOOP("@default", "Keys length");
	QT_TRANSLATE_NOOP("@default", "Generate keys");
	QT_TRANSLATE_NOOP("@default", "Open chat window on new message");
	QT_TRANSLATE_NOOP("@default", "Scroll chat window downward, not upward");
	QT_TRANSLATE_NOOP("@default", "\"Enter\" key in chat sends message by default");
	QT_TRANSLATE_NOOP("@default", "Message acknowledgements (wait for delivery)");
	QT_TRANSLATE_NOOP("@default", "Flash chat title on new message");
	QT_TRANSLATE_NOOP("@default", "Ignore messages from anonymous users");
	QT_TRANSLATE_NOOP("@default", "Syntax the same as in information panel.");
	QT_TRANSLATE_NOOP("@default", "%1 - Url clicked in chat window");
	QT_TRANSLATE_NOOP("@default", "<b>Me</b> 00:00:00");
	QT_TRANSLATE_NOOP("@default", "<b>Other party</b> 00:00:02");

	ConfigDialog::addTab("ShortCuts");
	ConfigDialog::addVGroupBox("ShortCuts", "ShortCuts", "Define keys");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "New line / send message:", "chat_newline", "Return");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Clear Chat:", "chat_clear", "F9");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Close Chat:", "chat_close", "Esc");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Bold text:", "chat_bold", "Ctrl+B");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Italic text:", "chat_italic", "Ctrl+I");
	ConfigDialog::addHotKeyEdit("ShortCuts", "Define keys", "Underline text:", "chat_underline", "Ctrl+U");
	

	ConfigDialog::addTab("Chat");
	ConfigDialog::addVGroupBox("Chat", "Chat", "Emoticons");
	ConfigDialog::addComboBox("Chat", "Emoticons", "Emoticons:");
	ConfigDialog::addComboBox("Chat", "Emoticons", "Emoticons theme");
	ConfigDialog::addVGroupBox("Chat", "Chat", "WWW options");
	ConfigDialog::addCheckBox("Chat", "WWW options", "Use default Web browser", "DefaultWebBrowser", true);
	ConfigDialog::addLineEdit("Chat", "WWW options", "Custom Web browser", "WebBrowser", "", "%1 - Url clicked in chat window");
	ConfigDialog::addCheckBox("Chat", "Chat", "Automatically prune chat messages", "ChatPrune", false);
	ConfigDialog::addHGroupBox("Chat", "Chat", "Message pruning");
	ConfigDialog::addLineEdit("Chat", "Message pruning", "Reduce the number of visible messages to", "ChatPruneLen", "20");
#ifdef HAVE_OPENSSL		
	ConfigDialog::addCheckBox("Chat", "Chat", "Use encryption", "Encryption", false);	
	ConfigDialog::addHGroupBox("Chat", "Chat", "Encryption properties");
	ConfigDialog::addComboBox("Chat", "Encryption properties", "Keys length");
	ConfigDialog::addPushButton("Chat", "Encryption properties", "Generate keys");
#endif
	ConfigDialog::addCheckBox("Chat", "Chat", "Open chat window on new message", "OpenChatOnMessage");
	ConfigDialog::addCheckBox("Chat", "Chat", "Scroll chat window downward, not upward", "ScrollDown", true);
	ConfigDialog::addCheckBox("Chat", "Chat", "\"Enter\" key in chat sends message by default", "AutoSend", true);
	ConfigDialog::addCheckBox("Chat", "Chat", "Message acknowledgements (wait for delivery)", "MessageAcks", true);
	ConfigDialog::addCheckBox("Chat", "Chat", "Flash chat title on new message", "BlinkChatTitle", true);
	ConfigDialog::addCheckBox("Chat", "Chat", "Ignore messages from anonymous users", "IgnoreAnonymousUsers", false);


// pierwsze uruchomienie kadu
	QColor color;
	color=QColor("#E0E0E0");
	config_file.addVariable("Look", "ChatMyBgColor", &color);
	color=QColor("#F0F0F0");
	config_file.addVariable("Look", "ChatUsrBgColor", &color);
	color=QColor("#000000");
	config_file.addVariable("Look", "ChatMyFontColor", &color);
	color=QColor("#000000");
	config_file.addVariable("Look", "ChatUsrFontColor", &color);

	QFontInfo info(qApp->font());
	QFont def_font(info.family(),info.pointSize());
	config_file.addVariable("Look", "ChatFont", &def_font);
//

	QT_TRANSLATE_NOOP("@default", "Look");
	QT_TRANSLATE_NOOP("@default", "Chat properties");
	QT_TRANSLATE_NOOP("@default", "Font");
	QT_TRANSLATE_NOOP("@default", "Font size");
	QT_TRANSLATE_NOOP("@default", "Other");
	QT_TRANSLATE_NOOP("@default", "Information panel syntax:");
	QT_TRANSLATE_NOOP("@default", "Chat window title syntax:");
	QT_TRANSLATE_NOOP("@default", "Conference window title prefix:");
	QT_TRANSLATE_NOOP("@default", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - frist name\n%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n%v - revDNS, %p - port %e - email\nIf you leave blank, default settings will be used");
	QT_TRANSLATE_NOOP("@default", "This text will be before syntax.\nIf you leave blank, default settings will be used.");
	QT_TRANSLATE_NOOP("@default", "Conference window title prefix");
	QT_TRANSLATE_NOOP("@default", "syntax:");
	QT_TRANSLATE_NOOP("@default", "Preview chat");

	ConfigDialog::addTab("Look");
	ConfigDialog::addVGroupBox("Look","Look","Chat properties");
	ConfigDialog::addGrid("Look", "Chat properties", "previewgrid2", 2);
	ConfigDialog::addVGroupBox("Look","previewgrid2","Preview chat");
	ConfigDialog::addLabel("Look","Preview chat", "<b>Me</b> 00:00:00", "chat");
	ConfigDialog::addLabel("Look","Preview chat", "<b>Other party</b> 00:00:02", "chat2");
	ConfigDialog::addHBox("Look", "Chat properties", "--");
	ConfigDialog::addComboBox("Look", "--", "", "", "combobox1");
	ConfigDialog::addLineEdit2("Look", "--", "", "", "", "line1");
	ConfigDialog::addColorButton("Look", "--","ColorButton1", QColor(config_file.readEntry("Look","ChatMyBgColor")));
	ConfigDialog::addHBox("Look", "Chat properties", "font&size2");
	ConfigDialog::addComboBox("Look", "font&size2", "Font", "", "font");
	ConfigDialog::addComboBox("Look", "font&size2", "Font size", "", "size");
	ConfigDialog::addVGroupBox("Look", "Look", "Other");
	ConfigDialog::addVBox("Look", "Other", "syntax");
	ConfigDialog::addLineEdit("Look", "syntax", "Information panel syntax:", "PanelContents", "[#%u][, %f] %r [- %d] [ (%i)]", "Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - frist name\n%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n%v - revDNS, %p - port %e - email\nIf you leave blank, default settings will be used");
	ConfigDialog::addLineEdit("Look", "syntax", "Chat window title syntax:", "ChatContents", "", "Syntax the same as in information panel.");
	ConfigDialog::addHBox("Look", "syntax", "conference");
	ConfigDialog::addLineEdit("Look", "conference", "Conference window title prefix:", "ConferencePrefix", "", "This text will be before syntax.\nIf you leave blank, default settings will be used.");
	ConfigDialog::addLineEdit("Look", "conference", "syntax:", "ConferenceContents", "%a (%s[: %d])", "Syntax the same as in information panel.");

	config_file.addVariable("Chat", "EmoticonsStyle", EMOTS_ANIMATED);
	emoticons.setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));
	
	ChatSlots *chatslots =new ChatSlots();
	ConfigDialog::registerSlotOnCreate(chatslots,SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(chatslots,SLOT(onDestroyConfigDialog()));
	ConfigDialog::connectSlot("Chat", "Emoticons:", SIGNAL(activated(int)), chatslots, SLOT(chooseEmoticonsStyle(int)));

#ifdef HAVE_OPENSSL	
	ConfigDialog::connectSlot("Chat", "Generate keys", SIGNAL(clicked()), chatslots, SLOT(generateMyKeys()));
	ConfigDialog::connectSlot("Chat", "Use encryption", SIGNAL(toggled(bool)), chatslots, SLOT(onUseEncryption(bool)));
#endif
	ConfigDialog::connectSlot("Chat", "Use default Web browser", SIGNAL(toggled(bool)), chatslots, SLOT(onDefWebBrowser(bool)));

	ConfigDialog::connectSlot("Chat", "Automatically prune chat messages", SIGNAL(toggled(bool)), chatslots, SLOT(onPruneChat(bool)));
	
	ConfigDialog::connectSlot("Look", "ColorButton1", SIGNAL(changed()), chatslots, SLOT(chooseColorGet()));
	ConfigDialog::connectSlot("Look", "", SIGNAL(textChanged(const QString&)), chatslots, SLOT(chooseColorGet(const QString&)), "line1");
	ConfigDialog::connectSlot("Look", "", SIGNAL(activated(int)), chatslots, SLOT(chooseChatSelect(int)), "combobox1");
	
	chat_manager=new ChatManager();
	connect(&event_manager,SIGNAL(chatMsgReceived1(UinsList,const QString&,time_t,bool&)),
		chat_manager,SLOT(chatMsgReceived(UinsList,const QString&,time_t,bool&)));
};

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
};

ColorSelector::ColorSelector(QWidget *parent, const char *name) : QWidget (parent, name,Qt::WType_Popup)
{
	setWFlags(Qt::WDestructiveClose);

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
	};
}

void ColorSelector::iconClicked(const QColor& color)
{
	emit colorSelect(color);
	close();
};

void ColorSelector::closeEvent(QCloseEvent *e) {
	kdebug("ColorSelector::closeEvent()\n");
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
};

void ChatSlots::onCreateConfigDialog()
{
	kdebug("ChatSlots::onCreateConfigDialog()\n");
	QComboBox *cb_emoticons_style= ConfigDialog::getComboBox("Chat", "Emoticons:");
	cb_emoticons_style->insertItem(tr("None"));
	cb_emoticons_style->insertItem(tr("Static"));
	cb_emoticons_style->insertItem(tr("Animated"));
	cb_emoticons_style->setCurrentItem(config_file.readNumEntry("Chat", "EmoticonsStyle"));

	QComboBox* cb_emoticons_theme= ConfigDialog::getComboBox("Chat", "Emoticons theme");
	cb_emoticons_theme->insertStringList(emoticons.themes());
	cb_emoticons_theme->setCurrentText(config_file.readEntry("Chat", "EmoticonsTheme"));

	if ((EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") == EMOTS_NONE)
		(cb_emoticons_theme)->setEnabled(false);

#ifdef HAVE_OPENSSL
	QComboBox* cb_keylength= ConfigDialog::getComboBox("Chat", "Keys length");
	cb_keylength->insertItem("1024");
#endif

	QCheckBox *c_defweb= ConfigDialog::getCheckBox("Chat", "Use default Web browser");
	QLineEdit *l_webbrow= ConfigDialog::getLineEdit("Chat", "Custom Web browser");
	
	if (c_defweb->isChecked())
	    ((QHBox*)l_webbrow->parent())->setEnabled(false);

#ifdef HAVE_OPENSSL
	QCheckBox *c_useencryption= ConfigDialog::getCheckBox("Chat", "Use encryption");
	QHGroupBox *h_encryption= ConfigDialog::getHGroupBox("Chat", "Encryption properties");
	h_encryption->setEnabled(c_useencryption->isChecked());
#endif
	
	QCheckBox *c_prunechat= ConfigDialog::getCheckBox("Chat", "Automatically prune chat messages");
	QHGroupBox *h_prune= ConfigDialog::getHGroupBox("Chat", "Message pruning");
	
	h_prune->setEnabled(c_prunechat->isChecked());


	vl_chatcolor.clear();
	vl_chatcolor.append(config_file.readColorEntry("Look", "ChatMyBgColor"));
	vl_chatcolor.append(config_file.readColorEntry("Look", "ChatMyFontColor"));
	vl_chatcolor.append(config_file.readColorEntry("Look", "ChatUsrBgColor"));
	vl_chatcolor.append(config_file.readColorEntry("Look", "ChatUsrFontColor"));
	
	vl_chatfont.clear();
	vl_chatfont.append(config_file.readFontEntry("Look", "ChatFont"));

	QLineEdit *l_color= ConfigDialog::getLineEdit("Look", "", "line1");
	l_color->setMaxLength(7);
	l_color->setText(vl_chatcolor[0].name());
	ColorButton *colorbutton= ConfigDialog::getColorButton("Look", "ColorButton1");
	colorbutton->setColor(vl_chatcolor[0]);
	QComboBox *cb_chatselect= ConfigDialog::getComboBox("Look", "","combobox1");

	cb_chatselect->insertItem(tr("Your background color"));
	cb_chatselect->insertItem(tr("Your font color"));
	cb_chatselect->insertItem(tr("User background color"));
	cb_chatselect->insertItem(tr("User font color"));
	cb_chatselect->insertItem(tr("Font in chat window"));
	cb_chatselect->setCurrentItem(0);

	updatePreview();
	QHBox *h_fontsize= ConfigDialog::getHBox("Look", "font&size2");
	h_fontsize->hide();

	QComboBox *cb_chatfont= ConfigDialog::getComboBox("Look", "Font", "font");
	QComboBox *cb_chatfontsize= ConfigDialog::getComboBox("Look", "Font size", "size");

	QFontDatabase fdb;
	QValueList<int> vl;
	
	vl = fdb.pointSizes(vl_chatfont[0].family(),"Normal");
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
		cb_chatfontsize->insertItem(QString::number(*points));
	
	cb_chatfontsize->setCurrentText(QString::number(vl_chatfont[0].pointSize()));
	cb_chatfont->insertStringList(fdb.families());
	cb_chatfont->setCurrentText(vl_chatfont[0].family());
	
	connect(cb_chatfont, SIGNAL(activated(int)), this, SLOT(chooseChatFont(int)));
	connect(cb_chatfontsize, SIGNAL(activated(int)), this, SLOT(chooseChatFontSize(int)));


}

void ChatSlots::onPruneChat(bool toggled)
{
	QHGroupBox *h_prune= ConfigDialog::getHGroupBox("Chat", "Message pruning");
	h_prune->setEnabled(toggled);


}

void ChatSlots::onDefWebBrowser(bool toggled)
{
	QLineEdit *l_webbrow= ConfigDialog::getLineEdit("Chat", "Custom Web browser");
	    ((QHBox*)l_webbrow->parent())->setEnabled(!toggled);
	
}

void ChatSlots::onUseEncryption(bool toggled)
{
#ifdef HAVE_OPENSSL
	QHGroupBox *h_encryption= ConfigDialog::getHGroupBox("Chat", "Encryption properties");
	h_encryption->setEnabled(toggled);
#endif
}


void ChatSlots::onDestroyConfigDialog()
{
	kdebug("ChatSlots::onDestroyConfigDialog()\n");
	
	QComboBox* cb_emoticons_theme= ConfigDialog::getComboBox("Chat", "Emoticons theme");
	config_file.writeEntry("Chat", "EmoticonsTheme",cb_emoticons_theme->currentText());
	emoticons.setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));
	QComboBox *cb_emoticons_style= ConfigDialog::getComboBox("Chat", "Emoticons:");
	config_file.writeEntry("Chat", "EmoticonsStyle", cb_emoticons_style->currentItem());

	config_file.writeEntry("Look","ChatMyBgColor",vl_chatcolor[0]);
	config_file.writeEntry("Look","ChatMyFontColor",vl_chatcolor[1]);
	config_file.writeEntry("Look","ChatUsrBgColor", vl_chatcolor[2]);
	config_file.writeEntry("Look","ChatUsrFontColor", vl_chatcolor[3]);
	config_file.writeEntry("Look", "ChatFont", vl_chatfont[0]);

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

void ChatSlots::chooseEmoticonsStyle(int index) {
    QComboBox *emotheme_box= ConfigDialog::getComboBox("Chat","Emoticons theme");
	if (!index)
	    emotheme_box->setEnabled(false);
	else
	    emotheme_box->setEnabled(true);
}


void ChatSlots::generateMyKeys(void) {
#ifdef HAVE_OPENSSL
	QString keyfile_path;

	
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(config_file.readNumEntry("General","UIN")));
	keyfile_path.append(".pem");
	
	QFileInfo keyfile(keyfile_path);
	
	if (keyfile.permission(QFileInfo::WriteUser))
		if(QMessageBox::warning(0, "Kadu",
			tr("Keys exist. Do you want to overwrite them?"),
			tr("Yes"), tr("No"),QString::null, 0, 1)==1)
				return;
	
	QCString tmp=ggPath("keys").local8Bit();
	mkdir(tmp.data(), 0700);

	if (sim_key_generate(config_file.readNumEntry("General","UIN")) < 0) {
		QMessageBox::critical(0, "Kadu", tr("Error generating keys"), tr("OK"), QString::null, 0);
		return;
	}

	QMessageBox::information(0, "Kadu", tr("Keys have been generated and written"), tr("OK"), QString::null, 0);
#endif
}

void ChatSlots::chooseChatSelect(int nr)
{
	kdebug("ChatSlots::chooseChatSelect() item: %d\n", nr);
	ColorButton *colorbutton= ConfigDialog::getColorButton("Look", "ColorButton1");
	QLineEdit *l_color= ConfigDialog::getLineEdit("Look", "", "line1");
	colorbutton->setColor(vl_chatcolor[nr]);
	l_color->setText(colorbutton->color().name());

	QHBox *h_fontsize= ConfigDialog::getHBox("Look", "font&size2");
	QComboBox *cb_chatfont= ConfigDialog::getComboBox("Look", "Font", "font");
	QComboBox *cb_chatfontsize= ConfigDialog::getComboBox("Look", "Font size", "size");

	if (nr == 4)
	{
	    h_fontsize->show();
	    cb_chatfontsize->setCurrentText(QString::number(vl_chatfont[0].pointSize()));
	    cb_chatfont->setCurrentText(vl_chatfont[0].family());
	    colorbutton->setEnabled(false);
	    l_color->setEnabled(false);

	}
	else
	{
	    h_fontsize->hide();
	    colorbutton->setEnabled(true);
	    l_color->setEnabled(true);

	}
}

void ChatSlots::chooseChatFont(int nr)
{
	kdebug("ChatSlots::chooseChatFont()\n");

	QFontDatabase fdb;
	QValueList<int> vl;
	QComboBox *cb_chatfont= ConfigDialog::getComboBox("Look", "Font", "font");
	QComboBox *cb_chatfontsize= ConfigDialog::getComboBox("Look", "Font size", "size");
	
	vl = fdb.pointSizes(cb_chatfont->text(nr),"Normal");
	cb_chatfontsize->clear();
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
	    {	
		cb_chatfontsize->insertItem(QString::number(*points));
		if (*points == vl_chatfont[0].pointSize())
		cb_chatfontsize->setCurrentItem(cb_chatfontsize->count()-1);
	    }
	if (cb_chatfontsize->count() > 0)
		vl_chatfont[0] = 
		    QFont(cb_chatfont->text(nr), cb_chatfontsize->currentText().toInt());

	updatePreview();

}

void ChatSlots::chooseChatFontSize(int nr)
{
	QComboBox *cb_chatfontsize= ConfigDialog::getComboBox("Look", "Font size", "size");
	QComboBox *cb_chatfont= ConfigDialog::getComboBox("Look", "Font", "font");
	
	vl_chatfont[0]= 
	    QFont(cb_chatfont->currentText(), cb_chatfontsize->currentText().toInt());
	updatePreview();
}

void ChatSlots::chooseColorGet(const QString &text)
{
	kdebug("ChatSlots::chooseColorGet(QString)\n");
	if ((text.length() == 7)&& (QColor(text).isValid()))
	    {
	    	ColorButton *colorbutton= ConfigDialog::getColorButton("Look", "ColorButton1");
		QLineEdit *l_color= ConfigDialog::getLineEdit("Look", "", "line1");
		colorbutton->setColor(QColor(text));
		int pos=l_color->cursorPosition();
		chooseColorGet();
		l_color->setCursorPosition(pos);
		updatePreview();
	    }
}

void ChatSlots::chooseColorGet()
{
	kdebug("ChatSlots::chooseColorGet()\n");
	ColorButton *colorbutton= ConfigDialog::getColorButton("Look", "ColorButton1");
	QLineEdit *l_color= ConfigDialog::getLineEdit("Look", "", "line1");
	QComboBox *cb_chatselect= ConfigDialog::getComboBox("Look", "","combobox1");
	
	l_color->setText(colorbutton->color().name());
	vl_chatcolor[cb_chatselect->currentItem()]=colorbutton->color();
	updatePreview();
}

void ChatSlots::updatePreview()
{
	QLabel *preview= ConfigDialog::getLabel("Look", "<b>Me</b> 00:00:00", "chat");
	QLabel *preview2= ConfigDialog::getLabel("Look", "<b>Other party</b> 00:00:02", "chat2");
	preview->setFont(vl_chatfont[0]);
	preview->setPaletteForegroundColor(vl_chatcolor[1]);
	preview->setPaletteBackgroundColor(vl_chatcolor[0]);
	preview->setAlignment(Qt::AlignLeft);
	preview2->setFont(vl_chatfont[0]);
	preview2->setPaletteForegroundColor(vl_chatcolor[3]);
	preview2->setPaletteBackgroundColor(vl_chatcolor[2]);
	preview2->setAlignment(Qt::AlignLeft);
}
