/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qpushbutton.h>
#include <qhbuttongroup.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qcstring.h>
#include <qaccel.h>
#include <qmessagebox.h>

//
#include "misc.h"
#include "kadu.h"
#include "pixmaps.h"
#include "message.h"
#include "history.h"
#ifdef HAVE_OPENSSL
extern "C"
{
#include "simlite.h"
};
#endif
//

char tmprcvr[255];

/* is it a chat or is it not? */
void rMessage::replyMessage(void) {
	Message *msg;
	if (tchat)
		msg = new Message(tmprcvr, true);
	else
		msg = new Message(tmprcvr);
	msg->show();
	close(true);
}

/* a message! someone loves us! */
rMessage::rMessage(const QString & nick, int msgclass, UinsList uins, QString &msg, QWidget *parent, const char *name)
 : QDialog(parent, name, Qt::WDestructiveClose) {
	bool sysmsg = false;
	int j;
#ifdef HAVE_OPENSSL
	QCString tmp(msg.local8Bit());
	unsigned char *utmp = (unsigned char *)tmp.data();

	char* decrypted=sim_message_decrypt((unsigned char *)utmp, uins[0]);
#endif

	PendingMsgs::Element elem;

	sender = nick;
	if (sender.compare("System")==0)
		sysmsg = true;

	lejbel = new QLabel(this);
	lejbel->setText(i18n("Sender: "));
	lejbel->setGeometry(5,5,200,10);

	QLineEdit *sendere = new QLineEdit(this);
	sendere->setText(sender);
	sendere->setReadOnly(true);

	QLabel *klejbel = new QLabel(this);
	klejbel->setText(i18n("Class: "));

	QLineEdit *msgclasse = new QLineEdit(this);
	if (msgclass == GG_CLASS_MSG) {
		msgclasse->setText(i18n("Message"));
		tchat = false;
		}
	else
		if (msgclass == GG_CLASS_CHAT) {
			msgclasse->setText(i18n("Chat"));
			tchat = true;
			}
		else
			if (uins[0] == config.uin)
				msgclasse->setText(i18n("System"));
			else
				if (msgclass == GG_CLASS_OFFLINE)
					msgclasse->setText(i18n("Queued"));
				else
					msgclasse->setText(i18n("Unknown"));		
	msgclasse->setReadOnly(true);

/*    bool __chat = false;
    QPushButton * chatbtn;
    if (pending[i].msgclass == GG_CLASS_CHAT) {
	chatbtn = new QPushButton (this);
	chatbtn->setText(i18n("C&hat"));
	connect(chatbtn, SIGNAL( clicked() ), this, SLOT( openChat() ));
	 __chat = true;
	}*/

	body = new QMultiLineEdit(this);
	body->setGeometry(5,20,305,170);
#ifdef HAVE_OPENSSL
	if (decrypted != NULL)
	{
               body->setText(decrypted);
	       free(decrypted);
	}       
	else
#endif
               body->setText(msg);

	body->setReadOnly(true);
	body->setWordWrap(QMultiLineEdit::WidgetWidth);
	body->setFont(config.fonts.chat);

	fprintf(stderr,"KK rMessage::rMessage(): message in slot\n");

//	deletePendingMessage(i);

//	UserBox::all_refresh();

	QPushButton * btn;
	btn = new QPushButton (this);
	btn->setGeometry(220,195,80,20);
	btn->setText(i18n("&Reply"));
	btn->setIconSet(QIconSet(QPixmap((const char**)reply_ico)));
	connect(btn, SIGNAL(clicked()), this, SLOT(replyMessage()));

	if (sysmsg)
		btn->setEnabled(false);
	else
		strncpy(tmprcvr, sender, 255);

	QPushButton *closebtn;
	closebtn = new QPushButton(this);
	closebtn->setText(i18n("&Close"));
	closebtn->setIconSet(QIconSet(loadIcon("stop.png")));
	connect(closebtn, SIGNAL(clicked()), this, SLOT(cleanUp()));

	QGridLayout *grid = new QGridLayout(this,3,5,3,3);
	grid->addWidget(lejbel,0,0);
	grid->addMultiCellWidget(sendere,0,0,1,2);
	grid->addWidget(klejbel,0,3,Qt::AlignRight);
	grid->addWidget(msgclasse,0,4);
	grid->addMultiCellWidget(body,1,1,0,4);
	grid->addWidget(btn,2,4);
	grid->addWidget(closebtn,2,3);
//    if (__chat)
//	grid->addWidget(chatbtn,2,2);
}

void rMessage::init(void) {
	resize(345,220);
	setCaption(i18n("Message from ") + sender);
}

/* clean up the mess */
void rMessage::cleanUp(void) {
	/* should we add some more? */
	close(true);
}

/* let's chat some more */
void rMessage::openChat(void) {
	QString tmp = __c2q(tmprcvr);    
	UinsList uins;

	uins.append(userlist.byAltNick(tmp).uin);
	kadu->openChat(uins);
	close(true);
}

/* send a message, send me a message */
Message::Message (const QString & nick, bool tchat, QWidget *parent, const char *name)
 : QDialog(parent, name) {
	QAccel *acc = new QAccel( this );
	acc->connectItem( acc->insertItem(Key_Return+CTRL), this, SLOT(commitSend()) );

	nicksnd = nick;
	QPushButton *closebtn;
	QLabel *statuslab;
	QLineEdit *nickedit;
	QLineEdit *statusedit;

	acks.resize(acks.size() + 1);
	index = acks.size() - 1;
	acks[index].ack = 1;
	acks[index].seq = 0;
	acks[index].type = 0;
	acks[index].ptr = NULL;

	nicknamelab = new QLabel(this);
	nicknamelab->setText(i18n("Recipient"));
	nicknamelab->setGeometry(5,5,65,20);

	nickedit = new QLineEdit(this);
	nickedit->setGeometry(70, 5, 80, 20);
	nickedit->setText(nicksnd);
	nickedit->setReadOnly(true);

	statuslab = new QLabel(this);
	statuslab->setText(i18n("Status"));
	statuslab->setGeometry(180, 5, 55, 20);

	statusedit = new QLineEdit(this);
	statusedit->setGeometry(230, 5, 60, 20);

	unsigned int status = userlist.byAltNick(nicksnd).status;
	if (status != -1)
		statusedit->setText(i18n(statustext[statusGGToStatusNr(status)]));
	else	
		statusedit->setText(i18n("(Unknown)"));
	statusedit->setReadOnly(true);

	body = new QMultiLineEdit(this);
	body->setGeometry(5,30,295,150);
	body->setWordWrap(QMultiLineEdit::WidgetWidth);
	body->setFont(config.fonts.chat);

	sendbtn = new QPushButton (this);
	sendbtn->setGeometry(240,185,60,20);
	sendbtn->setText(i18n("&Send"));
	sendbtn->setIconSet(QIconSet(loadIcon("forward.png")));
	connect(sendbtn, SIGNAL(clicked()), this, SLOT(commitSend()));
	QToolTip::add(sendbtn, i18n("Sends message. Hit CTRL+Enter or ALT+S for shortcut"));

	closebtn = new QPushButton (this);
	closebtn->setGeometry(170, 185, 60, 20);
	closebtn->setText(i18n("&Close"));
	closebtn->setIconSet(QIconSet(loadIcon("stop.png")));
	connect(closebtn, SIGNAL(clicked()), this, SLOT(reject()));
	QHButtonGroup *btngrp = new QHButtonGroup(this);

	b_chat = new QRadioButton(btngrp);
	b_chat->setText(i18n("C&hat"));
	QToolTip::add(b_chat, i18n("This option sends the message as a chat event"));

	QRadioButton *b_msg = new QRadioButton(btngrp);
	b_msg->setText(i18n("&Mesg"));
	if (!tchat)
		b_msg->setChecked(true);
	else
		b_chat->setChecked(true);
	QToolTip::add(b_msg, i18n("This option sends the message ordinarily"));

#ifdef HAVE_OPENSSL
	int uin;
	QString keyfile_path;

	uin = userlist.byAltNick(nick).uin;

	b_encryptmsg = new QCheckBox(btngrp);
	b_encryptmsg->setText(i18n("Encrypted"));

	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(uin));
	keyfile_path.append(".pem");

	QFileInfo keyfile(keyfile_path);

	if (keyfile.permission(QFileInfo::ReadUser))
		b_encryptmsg->setChecked(config.encryption);
	else
		b_encryptmsg->setEnabled(false);
	btngrp->insert(b_encryptmsg, 3);
#endif

	btngrp->insert(b_chat, 1);
	btngrp->insert(b_msg, 2);

	QPushButton *history = new QPushButton(this);
	history->setPixmap( QPixmap((const char**)history_xpm) );
	QToolTip::add(history, i18n("Show history"));

	connect(history, SIGNAL(clicked()), this, SLOT(HistoryBox()));

	QGridLayout *grid = new QGridLayout (this, 4, 5, 5, 5);
	grid->addWidget(nicknamelab, 0, 0);
	grid->addWidget(nickedit, 0, 1);
	grid->addWidget(history, 0, 2);
	grid->addWidget(statuslab, 0, 3, Qt::AlignRight);
	grid->addWidget(statusedit, 0, 4);
	grid->addMultiCellWidget(body, 2, 2, 0, 4);
	grid->addMultiCellWidget(btngrp, 3,3, 0,2);
	grid->addWidget(closebtn, 3, 3);
	grid->addWidget(sendbtn, 3, 4);

	grid->addRowSpacing(1, 5);
	grid->addRowSpacing(2, 20);

	body->setFocus();
}

void Message::init(void) {
	resize(355,235);
	setCaption(i18n("Message for ") + nicksnd);
}

/* the actual send */
void Message::commitSend(void) {
	if (getActualStatus() == GG_STATUS_NOT_AVAIL) {
		QMessageBox::critical(this, i18n("Send message error"),
			i18n("Application encountered network error."));
		return;
		}

	sendbtn->setDisabled(true);

	int uin;
	QString text;
	text = body->text();
	if ((text.compare("") == 0) || (text.compare(" ") == 0))
		return;

	// zmieniamy unixowe \n na windowsowe \r\n
	text.replace(QRegExp("\n"), "\r\n");
	QCString tmp(text.local8Bit());
	unsigned char *utmp = (unsigned char *) tmp.data();	

	body->setReadOnly(true);
	uin = userlist.byAltNick(nicksnd).uin;

	int seq;
	UinsList uins;
	uins.append(uin);
	if (config.logmessages)
		appendHistory(uins, uin, utmp, TRUE);
	iso_to_cp(utmp);

#ifdef HAVE_OPENSSL
	char* encrypted = sim_message_encrypt((unsigned char *)utmp, uin);
#endif
	if (b_chat->isChecked()) {
#ifdef HAVE_OPENSSL
               if (b_encryptmsg->isEnabled() && b_encryptmsg->isChecked()) {
                       seq = gg_send_message(sess, GG_CLASS_CHAT, uin, (unsigned char *)encrypted);
		       free(encrypted);
                       fprintf(stderr,"seq: %d\n", seq);
               } else {
#endif
                       seq = gg_send_message(sess, GG_CLASS_CHAT, uin, utmp);
                       fprintf(stderr,"seq: %d\n", seq);
#ifdef HAVE_OPENSSL
		}
#endif
	} else
#ifdef HAVE_OPENSSL
		if (b_encryptmsg->isEnabled() && b_encryptmsg->isChecked())
		{
			seq = gg_send_message(sess, GG_CLASS_MSG, uin, (unsigned char *)encrypted);
			free(encrypted);
		}
		else
#endif
		seq = gg_send_message(sess, GG_CLASS_MSG, uin, utmp);
	
	if (sess->check & GG_CHECK_WRITE)
		kadusnw->setEnabled(true);

	acks[index].ack = 1;
	acks[index].seq = seq;
	acks[index].ptr = this;
	acks[index].type = 1;

	fprintf(stderr,"KK Message::commitSend(): Message to uin %i queued for delivery\n", uin);
	setCaption(i18n("Message for ") + nicksnd + i18n(" [sending...]"));
	body->setReadOnly(false);	
}

Message::~Message() {
	fprintf(stderr, "KK Message::~Message()\n");
	for (int i = index + 1; i < acks.size(); i++) {
		acks[i-1].ack = acks[i].ack;
		acks[i-1].seq = acks[i].seq;
		acks[i-1].ptr = acks[i].ptr;
		acks[i-1].type = acks[i].type;
		}
	acks.resize(acks.size() - 1);
}

/* KGB informs: the message was delivered. let's close the window */
/* this function is called back from the network events handler */
void Message::gotAck(void) {
	accept();
}

/* for sanity's sake 
void Message::accept() {
	close(true);
}

void Message::reject() {
	close(true);
}
*/

void Message::HistoryBox (void) {
	History *hb;
	UinsList uins;

	uins.append(userlist.byAltNick(nicksnd).uin);
	hb = new History(uins);
	hb->show();
}

