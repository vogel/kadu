/**************OC*************************************************************
                          chat.cpp  -  description
                             -------------------
    begin                : Sat Sep 8 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <klocale.h>
#include <kicontheme.h>
#include <kiconloader.h>
#include <qregexp.h>
#include <qlayout.h>
#include <qaccel.h>
#include <qtimer.h>
#include <qtoolbutton.h>

//
#include "kadu.h"
#include "pixmaps.h"
#include "chat.h"
#include "search.h"
#include "history.h"
#include "misc.h"
//

Chat::Chat(UinsList uins, QDialog *parent) : QDialog (parent), uins(uins) {
	int i;
	struct chats chat;

	setWFlags(Qt::WDestructiveClose);
	iconsel_ptr = NULL;
	autosend_enabled = false;

	/* register us in the chats registry... */
	chat.uins = uins;
	chat.ptr = this;
	chats.append(chat);
	index = chats.count() - 1;

	setTitle();

	body = new KTextBrowser(this);
	if (uins.count() > 1) {
  	resize(550,400);
		userbox = new UserBox(this);

		userbox->setPaletteBackgroundColor(QColor(config.colors.userboxBgR,config.colors.userboxBgG,config.colors.userboxBgB));
		userbox->setPaletteForegroundColor(QColor(config.colors.userboxFgR,config.colors.userboxFgG,config.colors.userboxFgB));
		userbox->QListBox::setFont(QFont(config.colors.userboxFont, config.colors.userboxFontSize));

		for (i = 0; i < uins.count(); i++)
			userbox->addUin(uins[i]);
		userbox->refresh();
		}
	else {
  	resize(400,400);
		userbox = NULL;
    }
		
	edit = new CustomInput(this);
	edit->setGeometry(5,215, 390, 150);
	edit->setWordWrap(QMultiLineEdit::WidgetWidth);

	KIconLoader *loader = KGlobal::iconLoader();

	sendbtn = new QPushButton(this);
	sendbtn->setText(i18n("&Send"));
	sendbtn->setGeometry(320, 375, 75, 20);
	sendbtn->setIconSet(QIconSet( loader->loadIcon("forward", KIcon::Small) ));
	connect(sendbtn, SIGNAL(clicked()), this, SLOT(sendMessage()));
	QAccel *acc = new QAccel( this );
	acc->connectItem(acc->insertItem(Key_Return+CTRL), this, SLOT(sendMessage()));

	QPushButton *closebtn = new QPushButton(this);
	closebtn->setText(i18n("&Close"));
	closebtn->setGeometry(245, 375, 70, 20);
	closebtn->setIconSet(QIconSet(loader->loadIcon("stop", KIcon::Small)));
	connect(closebtn, SIGNAL(clicked()), this, SLOT(cleanUp()));

	QLabel *edt = new QLabel(this);
	edt->setText(i18n("Edit window:"));
	QToolTip::add(edt, i18n("This is where you type in the text to be sent"));

	buttontray = new QHBox(this);

	autosend = new QPushButton(buttontray);
	autosend->setPixmap(loader->loadIcon("key_enter", KIcon::Small));
	autosend->setToggleButton(true);
	QToolTip::add(autosend, i18n("Enter key sends message"));
	if (config.autosend) {
		autosend->setOn(true);
		autosend_enabled = true;
		}

	iconsel = new QPushButton(buttontray);
	iconsel->setPixmap(loader->loadIcon("icons", KIcon::Small));
	iconsel->setToggleButton(true);

	if (config.emoticons)
		QToolTip::add(iconsel, i18n("Insert emoticon"));
	else {
		QToolTip::add(iconsel, i18n("Insert emoticon - enable in configuration"));
		iconsel->setEnabled(false);
		}

	QPushButton *history = new QPushButton(buttontray);
	history->setPixmap(QPixmap((const char**)history_xpm));
	QToolTip::add(history, i18n("Show history"));

	QPushButton *whois = new QPushButton(buttontray);
	QPixmap p_whois;
	p_whois = loader->loadIcon("viewmag", KIcon::Small);
	whois->setPixmap(p_whois);
	QToolTip::add(whois, i18n("Lookup user info"));

	connect(autosend, SIGNAL(clicked()), this, SLOT(regAutosend()));
	connect(history, SIGNAL(clicked()), this, SLOT(HistoryBox()));
	connect(iconsel, SIGNAL(clicked()), this, SLOT(insertEmoticon()));
	connect(whois, SIGNAL(clicked()), this, SLOT(userWhois()));

	QGridLayout *grid = new QGridLayout (this, 5, 4, 3, 3);
	QHBoxLayout *subgrid = new QHBoxLayout(grid);
	subgrid->addWidget(body, 3);
	if (userbox)
		subgrid->addWidget(userbox, 1);
	grid->addMultiCellLayout(subgrid, 0, 0, 0, 3);
	grid->addWidget(buttontray, 2,3,Qt::AlignRight);
	grid->addMultiCellWidget(edt, 2, 2, 0, 2, Qt::AlignLeft);
	grid->addMultiCellWidget(edit, 3, 3, 0, 3);
	grid->addWidget(closebtn, 4, 2);
	grid->addWidget(sendbtn, 4, 3);
	grid->addRowSpacing(1, 5);
	grid->setRowStretch(0,2);

	QMimeSourceFactory *bodyformat;
	bodyformat = new QMimeSourceFactory;
	if (config.emoticons)
		bodyformat->addFilePath(config.emoticonspath);

	body->setMimeSourceFactory(bodyformat);
	body->setTextFormat(Qt::RichText);

	totaloccurences = 0;

	QObject::connect(edit, SIGNAL(textChanged()), this, SLOT(timerReset()));
}

Chat::~Chat() {
	int i,j;

	i = 0;
	while (i < chats.count() && chats[i].ptr != this)
		i++;
	chats.remove(chats.at(i));

	i = 0;
	while (i < acks.size() && acks[i].ptr != this)
		i++;
	if (i < acks.size()) {
		for (j = i + 1; j < acks.size(); j++) {
			acks[j-1].ack = acks[j].ack;
			acks[j-1].seq = acks[j].seq;
			acks[j-1].ptr = acks[j].ptr;
			acks[j-1].type = acks[j].type;
			}
		acks.resize(acks.size() - 1);
		}
	if (userbox)
		delete userbox;
		
	fprintf(stderr, "KK Chat::~Chat: chat destroyed: index %d\n", index);
}

void Chat::setTitle() {
	QString name;
	QString title;
	int i,j,k;
	UserListElement user;

	title = i18n("Chat with ");

	for (k = 0; k < uins.size(); k++) {
		if (k)
			title.append(", ");
	    
		name = userlist.byUin(uins[k]).altnick;
		title.append(name);
		user = userlist.byUin(uins[k]);
		j = statusGGToStatusNr(user.status);

		title.append(" (");
		title.append(i18n(statustext[j]));
		if (j & 1)
			title.append(i18n(": %1)").arg(user.description));
		else
			title.append(")");
		}
	title.replace(QRegExp("\n"), " ");

	setCaption(title);
}

/* register/unregister sending with Return key */
void Chat::regAutosend(void) {
	autosend_enabled = !autosend_enabled;
}

CustomInput::CustomInput(Chat* parent, const char *name) : QMultiLineEdit(parent, name) {
	tata = parent;
}

void CustomInput::keyPressEvent(QKeyEvent * e) {
	if (tata->autosend_enabled && ((e->key() == Key_Return) || (e->key() == Key_Enter))
		&& !(e->state() & ShiftButton))
		tata->sendMessage();
	else
		QMultiLineEdit::keyPressEvent(e);
}

/* convert special characters into emoticons, HTML into plain text and so forth */
QString Chat::convertCharacters(QString edit) {
	if (config.emoticons) {
		edit.replace(QRegExp(__c2q("(<p³acze>|<placze>)")), "__escaped_lt__IMG SRC=cry.gif /__escaped_gt__");
		edit.replace(QRegExp(__c2q("<zdziwiony>")), "__escaped_lt__IMG SRC=surprised.gif /__escaped_gt__");
		edit.replace(QRegExp(__c2q("(<ró¿a>|<roza>)")), "__escaped_lt__IMG SRC=rose.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<kwiatek>")), "__escaped_lt__IMG SRC=flower.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<piwo>")), "__escaped_lt__IMG SRC=beer.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<OK>")), "__escaped_lt__IMG SRC=thumbsup.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<do bani>")), "__escaped_lt__IMG SRC=thumbsdown.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<serce>")), "__escaped_lt__IMG SRC=heart.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<serduszka>")), "__escaped_lt__IMG SRC=hearts.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<kawa>")), "__escaped_lt__IMG SRC=coffee.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<komórka>")), "__escaped_lt__IMG SRC=cell.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<prezent>")), "__escaped_lt__IMG SRC=gift.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<telefon>")), "__escaped_lt__IMG SRC=phone.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<cmok>")), "__escaped_lt__IMG SRC=kiss.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<zawstydzony>")), "__escaped_lt__IMG SRC=blush.gif /__escaped_gt__");
		edit.replace( QRegExp(__c2q("<papa>")), "__escaped_lt__IMG SRC=bye.gif /__escaped_gt__");
		edit.replace( QRegExp(":\\)+"), "__escaped_lt__IMG SRC=smile.gif /__escaped_gt__");
		edit.replace( QRegExp(":-\\)+"), "__escaped_lt__IMG SRC=smile.gif /__escaped_gt__");
		edit.replace( QRegExp(";\\)+"), "__escaped_lt__IMG SRC=wink.gif /__escaped_gt__");
		edit.replace( QRegExp(";-\\)+"), "__escaped_lt__IMG SRC=wink.gif /__escaped_gt__");
		edit.replace( QRegExp(":\\(+"), "__escaped_lt__IMG SRC=sad.gif /__escaped_gt__");
		edit.replace( QRegExp(";\\(+"), "__escaped_lt__IMG SRC=sad.gif /__escaped_gt__");
		edit.replace( QRegExp(":-\\(+"), "__escaped_lt__IMG SRC=sad.gif /__escaped_gt__");
		edit.replace( QRegExp(":P+"), "__escaped_lt__IMG SRC=grin.gif /__escaped_gt__");
		edit.replace( QRegExp(":-P+"), "__escaped_lt__IMG SRC=grin.gif /__escaped_gt__");
		edit.replace( QRegExp(";P+"), "__escaped_lt__IMG SRC=grin.gif /__escaped_gt__");
		edit.replace( QRegExp(";-P+"), "__escaped_lt__IMG SRC=grin.gif /__escaped_gt__");
		edit.replace( QRegExp(":,\\(+"), "__escaped_lt__IMG SRC=cry.gif /__escaped_gt__");
		edit.replace( QRegExp(":x+"), "__escaped_lt__IMG SRC=kiss.gif /__escaped_gt__");
		}

	edit.replace( QRegExp("<"), "&lt;" );
	edit.replace( QRegExp(">"), "&gt;" );
	edit.replace( QRegExp("__escaped_lt__"), "<");
	edit.replace( QRegExp("__escaped_gt__"), ">");
	edit.replace( QRegExp("  "), " &nbsp;" );
	edit.replace( QRegExp("\n"), "<BR>" );

	int s = 0;
	int p,l,q;
	for (;;) {
		// find next url
		if (s >= (int)edit.length())
			break;
		p = edit.find(QRegExp("(http://|www\\.|ftp://ftp\\.)[a-zA-Z0-9\\-\\.]+\\.[a-zA-Z]{1,4}"), s);

		if (p < 0) 
			break;

		// clean up;
		l = (edit.find(" ", p) < 0) ? edit.length() - p : edit.find(" ", p) - p;
		QChar c = edit[l+p-1];
		while ( !( ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) ) )
			c = edit [p - 1 + (--l)];

		// urlize
		q = edit.find("http://", p);

		edit = edit.left(p) + "<a href=\"" + ((q < 0) ? "http://" : "") + edit.mid(p,l) + "\">" +
		edit.mid(p,l) + "</a>" + edit.mid(p+l);
		if (q < 0)
			s = p + 2*l + 22;
		else
			s = p + 2*l + 15;
		}

	return edit;
}

/* unregister us */
void Chat::closeEvent(QCloseEvent *e) {
	fprintf(stderr, "KK Chat::closeEvent()\n");	
	QWidget::closeEvent(e);
}

/* look up party's info */
void Chat::userWhois(void) {
	SearchDialog *sd;
	QString tmp;
	uin_t uin;
	
	if (!userbox)
		uin = uins[0];
	else
		if (userbox->currentItem() == -1)
			uin = uins[0];
		else
			uin = userlist.byAltNick(userbox->currentText()).uin;
	sd = new SearchDialog(0, "User info", uin);
	sd->show();
	sd->doSearch();
}

/* clean us up */
void Chat::cleanUp(void) {
	fprintf(stderr, "KK Chat::cleanUp: chat destroying: %d\n", index);
	close();
}

/* reset autoaway timer, something was pressed */
void Chat::timerReset(void) {
	kadu->autoaway->stop();
	kadu->autoaway->start(config.autoawaytime * 1000, TRUE);
}	

/* invoked from outside when new message arrives, this is the window to the world */
int Chat::checkPresence(UinsList senders, QString *msg, time_t time) {
	kadu->autoaway->stop();
	kadu->autoaway->start(config.autoawaytime * 1000, TRUE);

	QString toadd;
	QString editext = convertCharacters(*msg);
	toadd.append("<TABLE width=\"100%\"><TR><TD bgcolor=\"#F0F0F0\"><B>");
	toadd.append(userlist.byUin(senders[0]).altnick);
	toadd.append(" ");
	toadd.append(__c2q(timestamp(time)));
	toadd.append("</B><BR>");
	toadd.append(editext);
	toadd.append("</TD></TR></TABLE>");

	if (config.chatprune)
		pruneWindow();

	if (!config.scrolldown)
		body->setText(toadd + body->text());
	else {
		body->setText(body->text() + toadd);
		body->scrollToBottom();
		}

	return 0;
}

void Chat::playChatSound() {
	if (config.playsoundchatinvisible && config.playsoundchat) {
		if (!isActiveWindow())
			alertNewMessage();
		return;
		}
	else
		if (config.playsoundchat)
			alertNewMessage();
}

void Chat::alertNewMessage(void) {
	playSound(config.soundchat);
}

void Chat::writeMyMessage() {
	QString toadd;
	QString editext = convertCharacters(edit->text());

	toadd.append("<TABLE WIDTH=\"100%\"><TR><TD bgcolor=\"#E0E0E0\"><B>");
	toadd.append(config.nick);
	toadd.append(" ");
	toadd.append(__c2q(timestamp()));
	toadd.append("</B><BR>");
	toadd.append(editext);
	toadd.append("</TD></TR></TABLE>");

	if (config.chatprune)
		pruneWindow();

	if (!config.scrolldown)
		body->setText(toadd + body->text());
	else {
		body->setText(body->text() + toadd);
		body->scrollToBottom();    
		}

	edit->clear();
	edit->setReadOnly(false);
	edit->setEnabled(true);
}

void Chat::addMyMessageToHistory() {
	int uin;
	QString text;
	text = edit->text();
	QCString tmp(text.local8Bit());
	unsigned char *utmp = (unsigned char *) tmp.data();

	uin = uins[0];
	if (config.logmessages)
		appendHistory(uin, (unsigned char *)utmp, true);
}

/* sends the message typed */
void Chat::sendMessage(void) {
	int i,j;
	uin_t *users;

	kadu->autoaway->stop();
	kadu->autoaway->start(config.autoawaytime * 1000, TRUE);

	if (!QString::compare(edit->text().local8Bit(),""))
		return;

	if (edit->length() >= 2000)
		return;

	if (config.msgacks) {
		edit->setReadOnly(true);	
		edit->setEnabled(false);
		}
		
	QString text;
	text = edit->text();
	QCString tmp(text.local8Bit());
	unsigned char *utmp = (unsigned char *) tmp.data();

	addMyMessageToHistory();

	iso_to_cp(utmp);
	
	users = new (uin_t)[uins.count()];
	if (config.msgacks) {
		acks.resize(acks.size() + 1);
		i = acks.size() - 1;
		if (uins.count() > 1) {
			for (j = 0; j < uins.count(); j++)
				users[j] = uins[j];
			acks[i].seq = gg_send_message_confer(&sess, GG_CLASS_CHAT,
				uins.count(), users, (unsigned char *)utmp);    
			acks[i].ack = uins.count();
			}
		else {
			acks[i].seq = gg_send_message(&sess, GG_CLASS_CHAT, uins[0], (unsigned char *)utmp);
			acks[i].ack = 1;
			}
		acks[i].type = 2;
		acks[i].ptr = this;
		}
	else {
		if (uins.count() > 1) {
			for (j = 0; j < uins.count(); j++)
				users[j] = uins[j];
			gg_send_message_confer(&sess, GG_CLASS_CHAT,
				uins.count(), users, (unsigned char *)utmp);    
			}
		else
			gg_send_message(&sess, GG_CLASS_CHAT, uins[0], (unsigned char *)utmp);
		writeMyMessage();	
		}
	delete users;

	if (sess.check & GG_CHECK_WRITE)
		kadusnw->setEnabled(true);
}

/* prunes messages */
void Chat::pruneWindow(void) {
	int index,occurences;

	occurences = 0;
	if (config.scrolldown) {
		index = -1;
		while (occurences != config.chatprunelen && totaloccurences > config.chatprunelen - 1) {
			index = body->text().findRev(QString("<TABLE"), index - 8);
			occurences++;
			}
		totaloccurences++;

		body->setText(body->text().right(body->text().length() - index));	
		}
	else {
		index = 0;
		while (occurences != config.chatprunelen && totaloccurences > config.chatprunelen ) {
			if (occurences == 0)
				index = body->text().find(QString("<TABLE"), 0);
			else
				index = body->text().find(QString("<TABLE"), index + 8);

			occurences++;
			}
		totaloccurences++;

		if (totaloccurences > config.chatprunelen && index != -1 && index != 0)
			body->setText(body->text().left(index));
		}
}

/* opens messages history */
void Chat::HistoryBox(void) {
	History *hb;

	hb = new History(uins[0]);
	hb->show();
}

/* this nifty icon selector */
void Chat::insertEmoticon(void) {
	if (iconsel_ptr == NULL) {
		iconsel->setOn(true);
		iconsel_ptr = new IconSelector(this,"Icon selector",this);
		iconsel_ptr->show();
		}
	else {
		iconsel->setOn(false);
		iconsel_ptr->close();
		iconsel_ptr = NULL;
		}
}

/* adds an emoticon code to the edit window */
void Chat::addEmoticon(QString string) {
	edit->setText(edit->text() + string);
	edit->end();
	iconsel_ptr = NULL;
	iconsel->setOn(false);
}

/* the icon selector itself */
IconSelector::IconSelector(QWidget *parent, const char *name, Chat * caller) : QWidget (parent, name) {
	callingwidget = caller;
	setWFlags(Qt::WDestructiveClose||Qt::WStyle_NoBorder||Qt::WStyle_NoBorderEx||Qt::WX11BypassWM);

	QString path;
	path.append(config.emoticonspath);
	QGridLayout *grid = new QGridLayout(this, 5, 4, 0, 0);

	QToolButton *icon_1_1 = new QToolButton(this);
	icon_1_1->setPixmap(QPixmap(path + "smile.gif"));
	icon_1_1->setAutoRaise(true);
	grid->addWidget(icon_1_1, 0, 0);
	QObject::connect(icon_1_1, SIGNAL(clicked()), this, SLOT(slot_1_1()));

	QToolButton *icon_1_2 = new QToolButton(this);
	icon_1_2->setPixmap(QPixmap(path + "sad.gif"));
	icon_1_2->setAutoRaise(true);
	grid->addWidget(icon_1_2, 0, 1);
	QObject::connect(icon_1_2, SIGNAL(clicked()), this, SLOT(slot_1_2()));

	QToolButton *icon_1_3 = new QToolButton(this);
	icon_1_3->setPixmap(QPixmap(path + "surprised.gif"));
	icon_1_3->setAutoRaise(true);
	grid->addWidget(icon_1_3, 0, 2);
	QObject::connect(icon_1_3, SIGNAL(clicked()), this, SLOT(slot_1_3()));

	QToolButton *icon_1_4 = new QToolButton(this);
	icon_1_4->setPixmap(QPixmap(path + "wink.gif"));
	icon_1_4->setAutoRaise(true);
	grid->addWidget(icon_1_4, 0, 3);
	QObject::connect(icon_1_4, SIGNAL(clicked()), this, SLOT(slot_1_4()));

	QToolButton *icon_2_1 = new QToolButton(this);
	icon_2_1->setPixmap(QPixmap(path + "beer.gif"));
	icon_2_1->setAutoRaise(true);
	grid->addWidget(icon_2_1, 1, 0);
	QObject::connect(icon_2_1, SIGNAL( clicked() ), this, SLOT(slot_2_1()));

	QToolButton *icon_2_2 = new QToolButton(this);
	icon_2_2->setPixmap(QPixmap(path + "cry.gif"));
	icon_2_2->setAutoRaise(true);
	grid->addWidget(icon_2_2,1,1);
	QObject::connect(icon_2_2, SIGNAL(clicked()), this, SLOT(slot_2_2()));

	QToolButton *icon_2_3 = new QToolButton(this);
	icon_2_3->setPixmap(QPixmap(path + "grin.gif"));
	icon_2_3->setAutoRaise(true);
	grid->addWidget(icon_2_3, 1, 2);
	QObject::connect(icon_2_3, SIGNAL(clicked()), this, SLOT(slot_2_3()));

	QToolButton *icon_2_4 = new QToolButton(this);
	icon_2_4->setPixmap(QPixmap(path + "coffee.gif"));
	icon_2_4->setAutoRaise(true);
	grid->addWidget(icon_2_4, 1, 3);
	QObject::connect(icon_2_4, SIGNAL(clicked()), this, SLOT(slot_2_4()));

	QToolButton *icon_3_1 = new QToolButton(this);
	icon_3_1->setPixmap(QPixmap(path + "flower.gif"));
	icon_3_1->setAutoRaise(true);
	grid->addWidget(icon_3_1, 2, 0);
	QObject::connect(icon_3_1, SIGNAL(clicked()), this, SLOT(slot_3_1()));

	QToolButton *icon_3_2 = new QToolButton(this);
	icon_3_2->setPixmap(QPixmap(path + "rose.gif"));
	icon_3_2->setAutoRaise(true);
	grid->addWidget(icon_3_2, 2, 1);
	QObject::connect(icon_3_2, SIGNAL(clicked()), this, SLOT(slot_3_2()));

	QToolButton *icon_3_3 = new QToolButton(this);
	icon_3_3->setPixmap(QPixmap(path + "heart.gif"));
	icon_3_3->setAutoRaise(true);
	grid->addWidget(icon_3_3, 2, 2);
	QObject::connect(icon_3_3, SIGNAL(clicked()), this, SLOT(slot_3_3()));

	QToolButton *icon_3_4 = new QToolButton(this);
	icon_3_4->setPixmap(QPixmap(path + "thumbsup.gif"));
	icon_3_4->setAutoRaise(true);
	grid->addWidget(icon_3_4, 2, 3);
	QObject::connect(icon_3_4, SIGNAL(clicked()), this, SLOT(slot_3_4()));

	QToolButton *icon_4_1 = new QToolButton(this);
	icon_4_1->setPixmap(QPixmap(path + "thumbsdown.gif"));
	icon_4_1->setAutoRaise(true);
	grid->addWidget(icon_4_1, 3, 0);
	QObject::connect(icon_4_1, SIGNAL(clicked()), this, SLOT(slot_4_1()));

	QToolButton *icon_4_2 = new QToolButton(this);
	icon_4_2->setPixmap(QPixmap(path + "gift.gif"));
	icon_4_2->setAutoRaise(true);
	grid->addWidget(icon_4_2, 3, 1);
	QObject::connect(icon_4_2, SIGNAL(clicked()), this, SLOT(slot_4_2()));

	QToolButton *icon_4_3 = new QToolButton(this);
	icon_4_3->setPixmap(QPixmap(path + "cell.gif"));
	icon_4_3->setAutoRaise(true);
	grid->addWidget(icon_4_3, 3, 2);
	QObject::connect(icon_4_3, SIGNAL(clicked()), this, SLOT(slot_4_3()));

	QToolButton *icon_4_4 = new QToolButton(this);
	icon_4_4->setPixmap(QPixmap(path + "phone.gif"));
	icon_4_4->setAutoRaise(true);
	grid->addWidget(icon_4_4, 3, 3);
	QObject::connect(icon_4_4, SIGNAL(clicked()), this, SLOT(slot_4_4()));

	QToolButton *icon_5_1 = new QToolButton(this);
	icon_5_1->setPixmap(QPixmap(path + "hearts.gif"));
	icon_5_1->setAutoRaise(true);
	grid->addWidget(icon_5_1, 4, 0);
	QObject::connect(icon_5_1, SIGNAL(clicked()), this, SLOT(slot_5_1()));

	QToolButton *icon_5_2 = new QToolButton(this);
	icon_5_2->setPixmap(QPixmap(path + "kiss.gif"));
	icon_5_2->setAutoRaise(true);
	grid->addWidget(icon_5_2, 4, 1);
	QObject::connect(icon_5_2, SIGNAL(clicked()), this, SLOT(slot_5_2()));

	QToolButton *icon_5_3 = new QToolButton(this);
	icon_5_3->setPixmap(QPixmap(path + "blush.gif"));
	icon_5_3->setAutoRaise(true);
	grid->addWidget(icon_5_3, 4, 2);
	QObject::connect(icon_5_3, SIGNAL(clicked()), this, SLOT(slot_5_3()));

	QToolButton *icon_5_4 = new QToolButton(this);
	icon_5_4->setPixmap(QPixmap(path + "bye.gif"));
	icon_5_4->setAutoRaise(true);
	grid->addWidget(icon_5_4, 4, 3);
	QObject::connect(icon_5_4, SIGNAL(clicked()), this, SLOT(slot_5_4()));

	move(callingwidget->buttontray->x() - sizeHint().width() + icon_1_4->sizeHint().width(),
	callingwidget->buttontray->y() + callingwidget->buttontray->height());
}

/* a set of slots. tell me it can be done better. */
void IconSelector::slot_1_1() { callingwidget->addEmoticon(":)"); close(); };
void IconSelector::slot_1_2() { callingwidget->addEmoticon(":("); close(); };
void IconSelector::slot_1_3() { callingwidget->addEmoticon("<zdziwiony>"); close(); };
void IconSelector::slot_1_4() { callingwidget->addEmoticon(";)"); close(); };

void IconSelector::slot_2_1() { callingwidget->addEmoticon("<piwo>"); close(); };
void IconSelector::slot_2_2() { callingwidget->addEmoticon(__c2q("<p³acze>")); close(); };
void IconSelector::slot_2_3() { callingwidget->addEmoticon(";P"); close(); };
void IconSelector::slot_2_4() { callingwidget->addEmoticon("<kawa>"); close(); };

void IconSelector::slot_3_1() { callingwidget->addEmoticon("<kwiatek>"); close(); };
void IconSelector::slot_3_2() { callingwidget->addEmoticon("<roza>"); close(); };
void IconSelector::slot_3_3() { callingwidget->addEmoticon("<serce>"); close(); };
void IconSelector::slot_3_4() { callingwidget->addEmoticon("<OK>"); close(); };

void IconSelector::slot_4_1() { callingwidget->addEmoticon("<do bani>"); close(); };
void IconSelector::slot_4_2() { callingwidget->addEmoticon("<prezent>"); close(); };
void IconSelector::slot_4_3() { callingwidget->addEmoticon("<komórka>"); close(); };
void IconSelector::slot_4_4() { callingwidget->addEmoticon("<telefon>"); close(); };

void IconSelector::slot_5_1() { callingwidget->addEmoticon("<serduszka>"); close(); };
void IconSelector::slot_5_2() { callingwidget->addEmoticon("<cmok>"); close(); };
void IconSelector::slot_5_3() { callingwidget->addEmoticon("<zawstydzony>"); close(); };
void IconSelector::slot_5_4() { callingwidget->addEmoticon("<papa>"); close(); };

#include "chat.moc"
