/***************************************************************************
                          config.cpp  -  description
                             -------------------
    begin                : Thu Feb 14 2002
    copyright            : (C) 2002 by tomee
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

#include <qfiledialog.h>
#include <math.h>
#include <qvgroupbox.h>
#include <qvbox.h>
#include <qhgroupbox.h>
#include <kglobal.h>
#include <qpushbutton.h>
#include <kicontheme.h>
#include <kiconloader.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <klocale.h>
#include <kconfig.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//
#include "kadu.h"
#include "misc.h"
#include "config.h"
//

void loadKaduConfig(void) {  	
	/* first read our own config file... */
	fprintf(stderr,"KK loadKaduConfig(): Touching config file...\n");
	KConfig * konf;
	konf = new KConfig(preparePath("kadu.conf"));

	konf->setGroup("Global");
	config.uin = konf->readNumEntry("UIN",0);
	config.password = pwHash(konf->readEntry("Password",""));
	fprintf(stderr,"KK Read user data: uin %d password %s\n",config.uin,config.password);
	config.soundprog = strdup(konf->readEntry("SoundPlayer",""));
	config.soundmsg = strdup(konf->readEntry("Message_sound",""));
	config.soundvolctrl = konf->readBoolEntry("VolumeControl",false);
	config.soundvol = konf->readDoubleNumEntry("SoundVolume",1.0);
	config.soundchat = strdup(konf->readEntry("Chat_sound",""));

	config.defaultstatus = konf->readNumEntry("DefaultStatus", GG_STATUS_NOT_AVAIL);

	if (!config.defaultstatus)
		config.defaultstatus = GG_STATUS_NOT_AVAIL;
	config.defaultdescription = konf->readEntry("DefaultDescription", i18n("I am busy."));

	config.logmessages = konf->readBoolEntry("Logging",true);
	config.savegeometry = konf->readBoolEntry("SaveGeometry",true);
	config.playsoundchat = konf->readBoolEntry("PlaySoundChat",true);
	config.playsoundchatinvisible = konf->readBoolEntry("PlaySoundChatInvisible",true);
	config.playsound = konf->readBoolEntry("PlaySound",false);
	config.playartsdsp = konf->readBoolEntry("PlaySoundArtsDsp",false);
	config.sysmsgidx = konf->readNumEntry("SystemMsgIndex",0);
	config.autoaway = konf->readBoolEntry("AutoAway", false);
	config.autoawaytime = konf->readNumEntry("AutoAwayTime", 300);
	config.allowdcc = konf->readBoolEntry("AllowDCC",false);
	config.extip = strdup(konf->readEntry("ExternalIP", "0.0.0.0"));
	config.extport = konf->readNumEntry("ExternalPort", 0);
	config.server = strdup(konf->readEntry("Server", "0.0.0.0"));
	config.dock = konf->readBoolEntry("UseDocking",true);
	config.raise = konf->readBoolEntry("AutoRaise",false);
	config.privatestatus = konf->readBoolEntry("PrivateStatus", false);

	if (config.savegeometry)
		config.geometry = konf->readRectEntry("Geometry");

	konf->setGroup("SMS");
	config.smsapp = strdup(konf->readEntry("SmsApp",""));
	config.smscustomconf = konf->readBoolEntry("UseCustomString",false);
	config.smsconf = strdup(konf->readEntry("SmsString",""));

	konf->setGroup("Other");
	config.emoticons = konf->readBoolEntry("UseEmoticons",false);
	config.autosend = konf->readBoolEntry("AutoSend",false);
	config.scrolldown = konf->readBoolEntry("ScrollDown",true);
	config.emoticonspath = strdup(konf->readEntry("EmoticonsPath",""));
	config.chatprune = konf->readBoolEntry("ChatPrune",false);
	config.chatprunelen = konf->readNumEntry("ChatPruneLen",20);
	config.msgacks = konf->readBoolEntry("MessageAcks", true);
	
	konf->setGroup("Notify");
	config.soundnotify = strdup(konf->readEntry("NotifySound",""));
	config.notifyglobal = konf->readBoolEntry("NotifyStatusChange",false);
	config.notifydialog = konf->readBoolEntry("NotifyWithDialogBox",false);
	config.notifysound = konf->readBoolEntry("NotifyWithSound",false);
	config.notifies = konf->readListEntry("NotifyUsers");

	konf->setGroup("Proxy");
	config.useproxy = konf->readBoolEntry("UseProxy",false);
	config.proxyaddr = strdup(konf->readEntry("ProxyHost",""));
	config.proxyport = konf->readNumEntry("ProxyPort",0);

	konf->setGroup("Colors");
	config.colors.userboxBgR = konf->readNumEntry("UserboxBgR",255);
	config.colors.userboxBgG = konf->readNumEntry("UserboxBgG",255);
	config.colors.userboxBgB = konf->readNumEntry("UserboxBgB",255);
	config.colors.userboxFgR = konf->readNumEntry("UserboxFgR",0);
	config.colors.userboxFgG = konf->readNumEntry("UserboxFgG",0);
	config.colors.userboxFgB = konf->readNumEntry("UserboxFgB",0);
	config.colors.userboxFont = strdup(konf->readEntry("UserboxFont","sans-serif"));
	config.colors.userboxFontSize = konf->readNumEntry("UserboxFontSize",12);

	/* no need for it anymore */
	delete konf;
}

void saveKaduConfig(void) {
	fprintf(stderr,"KK saveKaduConfig(): Writing config files...\n");
	KConfig * konf;
	konf = new KConfig(preparePath("kadu.conf"));

	konf->setGroup("Global");
	konf->writeEntry("UIN",config.uin);
	konf->writeEntry("Password",pwHash(config.password));
	konf->writeEntry("Geometry",kadu->geometry());
	konf->writeEntry("Message_sound",config.soundmsg);
	konf->writeEntry("Chat_sound",config.soundchat);
	konf->writeEntry("Logging",config.logmessages);
	konf->writeEntry("DefaultStatus",config.defaultstatus);
	konf->writeEntry("DefaultDescription", config.defaultdescription);
	konf->writeEntry("SystemMsgIndex",config.sysmsgidx);
	konf->writeEntry("SaveGeometry",config.savegeometry);
	konf->writeEntry("PlaySoundChat",config.playsoundchat);
	konf->writeEntry("PlaySoundChatInvisible",config.playsoundchatinvisible);
	konf->writeEntry("SoundPlayer",config.soundprog);
	konf->writeEntry("PlaySound",config.playsound);
	konf->writeEntry("PlaySoundArtsDsp",config.playartsdsp);
	konf->writeEntry("SoundVolume",config.soundvol);
	konf->writeEntry("VolumeControl",config.soundvolctrl);
	konf->writeEntry("AutoAway",config.autoaway);
	konf->writeEntry("AutoAwayTime",config.autoawaytime);
	konf->writeEntry("AllowDCC",config.allowdcc);
	konf->writeEntry("ExternalIP", config.extip);
	konf->writeEntry("ExternalPort", config.extport);
	konf->writeEntry("Server", config.server);
	konf->writeEntry("UseDocking",config.dock);
	konf->writeEntry("AutoRaise",config.raise);
	konf->writeEntry("PrivateStatus",config.privatestatus);

	konf->setGroup("SMS");
	konf->writeEntry("SmsApp",config.smsapp);
	konf->writeEntry("SmsString",config.smsconf);
	konf->writeEntry("UseCustomString",config.smscustomconf);

	konf->setGroup("Other");
	konf->writeEntry("UseEmoticons",config.emoticons);
	konf->writeEntry("AutoSend",config.autosend);
	konf->writeEntry("ScrollDown",config.scrolldown);
	konf->writeEntry("EmoticonsPath",config.emoticonspath);
	konf->writeEntry("ChatPrune",config.chatprune);
	konf->writeEntry("ChatPruneLen",config.chatprunelen);
	konf->writeEntry("MessageAcks", config.msgacks);
	
	konf->setGroup("Proxy");
	konf->writeEntry("UseProxy",config.useproxy);
	konf->writeEntry("ProxyHost",config.proxyaddr);
	konf->writeEntry("ProxyPort",config.proxyport);

	konf->setGroup("Notify");
	konf->writeEntry("NotifyUsers",config.notifies);
	konf->writeEntry("NotifySound",config.soundnotify);
	konf->writeEntry("NotifyStatusChange",config.notifyglobal);
	konf->writeEntry("NotifyWithDialogBox",config.notifydialog);
	konf->writeEntry("NotifyWithSound",config.notifysound);

	konf->sync();
	delete konf;
}

ConfigDialog::ConfigDialog(QWidget *parent, const char *name) : QTabDialog(parent, name) {
	setWFlags(Qt::WDestructiveClose);

	setupTab1();
	setupTab2();
	setupTab3();
	setupTab4();
	setupTab5();

	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(updateConfig()));
	setCancelButton(i18n("Cancel"));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close()));
	setCaption(i18n("Kadu configuration"));
	resize(380,420);
}

void ConfigDialog::setupTab1(void) {
	int i;

	QVBox *box = new QVBox(this);
	box->setMargin(5);

	QHGroupBox *userinfo = new QHGroupBox(box);
	userinfo->setTitle(i18n("User data"));

	QLabel *l_uin = new QLabel(userinfo);
	l_uin->setText(i18n("Uin"));

	char uin[12];
	snprintf(uin, sizeof(uin), "%d", config.uin);
	e_uin = new QLineEdit(userinfo);
	e_uin->setText(uin);

	QLabel *l_password = new QLabel(userinfo);
	l_password->setText(i18n("Password"));

	e_password = new QLineEdit(userinfo);
	e_password->setEchoMode(QLineEdit::Password);
	e_password->setText(config.password);

	/* SMS begin */
	QVGroupBox *smsvgrp = new QVGroupBox(box);
	smsvgrp->setTitle(i18n("SMS options"));

	QHBox *smshbox1 = new QHBox(smsvgrp);
	smshbox1->setSpacing(5);
	QLabel *l_smsapp = new QLabel(smshbox1);
	l_smsapp->setText(i18n("SMS application"));
	e_smsapp = new QLineEdit(smshbox1);
	e_smsapp->setText(config.smsapp);

	b_smscustomconf = new QCheckBox(smsvgrp);
	b_smscustomconf->setText(i18n("Enable custom string formatting"));
	QToolTip::add(b_smscustomconf,i18n("Check this box if your sms application doesn't understand sms number \"message\"\nThe string formatting is %s for app, %d for number and %s for message\nand the order is %s %d %s"));

	QHBox *smshbox2 = new QHBox(smsvgrp);
	smshbox2->setSpacing(5);
	QLabel *l_smsconf = new QLabel(smshbox2);
	l_smsconf->setText(i18n("SMS custom string"));
	e_smsconf = new QLineEdit(smshbox2);
	e_smsconf->setText(config.smsconf);

	if (config.smscustomconf)
		b_smscustomconf->setEnabled(true);
	else
		smshbox2->setEnabled(false);

	QObject::connect(b_smscustomconf, SIGNAL(toggled(bool)), smshbox2, SLOT(setEnabled(bool)));
	/* SMS end */

	b_autoaway = new QCheckBox(box);
	b_autoaway->setText(i18n("Enable autoaway"));

	QHGroupBox *awygrp = new QHGroupBox(box);
	QLabel *l_autoaway = new QLabel(awygrp);
	l_autoaway->setText(i18n("Set status to away after "));

	char czas[8];
	snprintf(czas, sizeof(czas), "%d", config.autoawaytime);

	e_autoawaytime = new QLineEdit(awygrp);
	e_autoawaytime->setText(czas);

	QLabel *l_autoaway2 = new QLabel(awygrp);
	l_autoaway2->setText(i18n(" seconds"));

	if (config.autoaway)
		b_autoaway->setChecked(true);
	else
		awygrp->setEnabled(false);

	QObject::connect(b_autoaway, SIGNAL(toggled(bool)), awygrp, SLOT(setEnabled(bool)));

	QVGroupBox *gb_defstatus = new QVGroupBox(box);
	gb_defstatus->setTitle(i18n("Default Status"));
	cb_defstatus = new QComboBox(gb_defstatus);
	for (i = 0;i < 7; i++)
		cb_defstatus->insertItem(i18n(__c2q(statustext[i])));
	i = 0;
	while (i < 7 && config.defaultstatus != gg_statuses[i])
		i++;
	cb_defstatus->setCurrentItem(i);

	b_logging = new QCheckBox(box);
	b_logging->setText(i18n("Log messages"));
	if (config.logmessages)
		b_logging->setChecked(true);

	b_geometry = new QCheckBox(box);
	b_geometry->setText(i18n("Restore window geometry"));
	if (config.savegeometry)
		b_geometry->setChecked(true);

	b_dock = new QCheckBox(box);
	b_dock->setText(i18n("Enable dock icon"));
	if (config.dock)
		b_dock->setChecked(true);

	b_private = new QCheckBox(box);
	b_private->setText(i18n("Private status"));
	if (config.privatestatus)
		b_private->setChecked(true);

	addTab(box, i18n("General"));
}

void ConfigDialog::setupTab2(void) {
	KIconLoader *loader = KGlobal::iconLoader();
	QVBox *box2 = new QVBox(this);
	box2->setMargin(5);	

	b_playsound = new QCheckBox(box2);
	b_playsound->setText(i18n("Play sounds"));

	b_playartsdsp = new QCheckBox(box2);
	b_playartsdsp->setText(i18n("Play sounds using aRts! server"));
	if (config.playartsdsp)
		b_playartsdsp->setChecked(true);

	if (config.playsound)
		b_playsound->setChecked(true);
	else
		b_playartsdsp->setEnabled(false);

	QObject::connect(b_playsound, SIGNAL(toggled(bool)), b_playartsdsp, SLOT(setEnabled(bool)));

	QHGroupBox *sndgroup = new QHGroupBox(box2);
	sndgroup->setTitle(i18n("Sound player"));
	e_soundprog = new QLineEdit(sndgroup);
	e_soundprog->setText(config.soundprog);

	QPushButton *f_soundprog = new QPushButton(sndgroup);
	f_soundprog->setPixmap(loader->loadIcon("fileopen", KIcon::Small));
	connect(f_soundprog, SIGNAL(clicked()), this, SLOT(choosePlayerFile()));

	b_soundvolctrl = new QCheckBox(box2);
	b_soundvolctrl->setText(i18n("Enable volume control (player must support it)"));

	QHBox *volbox = new QHBox(box2);
	volbox->setSpacing(5);
	QLabel *l_vol = new QLabel(volbox);
	l_vol->setText(i18n("Volume"));
	s_volume = new QSlider(0,400,1,(int)floor(config.soundvol * 100),Qt::Horizontal,volbox);
	s_volume->setTickmarks(QSlider::Below);
	s_volume->setTickInterval(50);

	if (config.soundvolctrl)
		b_soundvolctrl->setChecked(true);
	else
		volbox->setEnabled(false);

	QObject::connect(b_soundvolctrl, SIGNAL(toggled(bool)), volbox, SLOT(setEnabled(bool)));

	QHGroupBox *msggroup = new QHGroupBox(box2);
	msggroup->setTitle(i18n("Message sound"));

	e_msgfile = new QLineEdit(msggroup);
	e_msgfile->setText(config.soundmsg);

	QPixmap icon;
	icon = loader->loadIcon("fileopen", KIcon::Small);
	QPushButton *msgsnd = new QPushButton(msggroup);
	msgsnd->setPixmap(icon);
	connect(msgsnd, SIGNAL(clicked()), this, SLOT(chooseMsgFile()));

	b_playchat = new QCheckBox(box2);
	b_playchat->setText(i18n("Play sounds from a person whilst chatting"));
	b_playchatinvisible = new QCheckBox(box2);
	b_playchatinvisible->setText(i18n("Play chat sounds only when window is invisible"));
	QObject::connect(b_playchat, SIGNAL(toggled(bool)), b_playchatinvisible, SLOT(setEnabled(bool)));

	if (config.playsoundchat)
		b_playchat->setChecked(true);
	else
		b_playchatinvisible->setEnabled(false);

	if (config.playsoundchatinvisible)
		b_playchatinvisible->setChecked(true);

	QHGroupBox *chatgroup = new QHGroupBox(box2);
	chatgroup->setTitle(i18n("Chat sound"));

	e_chatfile = new QLineEdit(chatgroup);
	e_chatfile->setText(config.soundchat);

	QPushButton *chatsnd = new QPushButton(chatgroup);
	chatsnd->setPixmap(icon);
	connect(chatsnd, SIGNAL(clicked()), this, SLOT(chooseChatFile()));

	addTab(box2, i18n("Sounds"));
}

void ConfigDialog::setupTab3(void) {
	KIconLoader *loader = KGlobal::iconLoader();
	QVBox *box3 = new QVBox(this);
	box3->setMargin(5);	

	b_emoticons = new QCheckBox(box3);
	b_emoticons->setText(i18n("Enable emoticons in chat window"));

	QHGroupBox *emogroup = new QHGroupBox(box3);
	emogroup->setTitle(i18n("Emoticons path"));

	e_emoticonspath = new QLineEdit(emogroup);
	e_emoticonspath->setText(config.emoticonspath);

	QPushButton *emoget = new QPushButton(emogroup);
	emoget->setPixmap(loader->loadIcon("fileopen", KIcon::Small));
	connect(emoget, SIGNAL(clicked()), this, SLOT(chooseEmoticonsPath()));

	if (config.emoticons)
		b_emoticons->setChecked(true);
	else
		emogroup->setEnabled(true);

	QObject::connect(b_emoticons,SIGNAL(toggled(bool)), emogroup, SLOT(setEnabled(bool)));
	QObject::connect(b_emoticons, SIGNAL(toggled(bool)), this, SLOT(emoticonsEnabled(bool)));

	b_chatprune = new QCheckBox(box3);
	b_chatprune->setText(i18n("Automatically prune chat messages"));

	QHGroupBox *prunebox = new QHGroupBox(box3);
	prunebox->setTitle(i18n("Message pruning"));

	QLabel *l_chatprunedsc1 = new QLabel(prunebox);
	l_chatprunedsc1->setText(i18n("Reduce the number of visible messages to"));

	e_chatprunelen = new QLineEdit(prunebox);
	e_chatprunelen->setText(QString::number(config.chatprunelen));

	if (config.chatprune)
		b_chatprune->setChecked(true);
	else
		prunebox->setEnabled(false);

	QObject::connect(b_chatprune, SIGNAL(toggled(bool)), prunebox, SLOT(setEnabled(bool)));

	b_scrolldown = new QCheckBox(box3);
	b_scrolldown->setText(i18n("Scroll chat window downward, not upward"));
	if (config.scrolldown)
		b_scrolldown->setChecked(true);

	b_autosend = new QCheckBox(box3);
	b_autosend->setText(i18n("\"Enter\" key in chat sends message by default"));
	if (config.autosend)
		b_autosend->setChecked(true);
	
	b_msgacks = new QCheckBox(box3);
	b_msgacks->setText(i18n("Message acknowledgements"));
	b_msgacks->setChecked(config.msgacks);
	
	addTab(box3, i18n("Chat"));
}

void ConfigDialog::setupTab4(void) {
	int i;
	uin_t uin;

	KIconLoader *loader = KGlobal::iconLoader();
	QVBox *box4 = new QVBox(this);
	box4->setMargin(2);	
	
	b_notifyglobal = new QCheckBox(box4);
	b_notifyglobal->setText(i18n("Notify when users become available"));

	/* two nice panes */
	QHBox *panebox = new QHBox(box4);

	QVBox *vbox1 = new QVBox(panebox);
	QLabel *_l1 = new QLabel(vbox1);
	_l1->setText(i18n("Available"));
	e_availusers = new QListBox(vbox1);
	i = 0;
	while (i < userlist.count()) {
		if (!config.notifies.contains(QString::number(userlist[i].uin)))
			e_availusers->insertItem(userlist[i].altnick);
		i++;
		}

	QVBox *vbox2 = new QVBox(panebox);
	QPushButton *_goRight = new QPushButton (vbox2);
	_goRight->setPixmap( loader->loadIcon("forward", KIcon::Small));

	QPushButton *_goLeft = new QPushButton (vbox2);
	_goLeft->setPixmap( loader->loadIcon("back", KIcon::Small));

	QObject::connect(_goRight, SIGNAL(clicked()), this, SLOT(_Right()));
	QObject::connect(_goLeft, SIGNAL(clicked()), this, SLOT(_Left()));

	QVBox *vbox3 = new QVBox(panebox);
	QLabel *_l2 = new QLabel(vbox3);
	_l2->setText(i18n("Tracked"));
	e_notifies = new QListBox(vbox3);

	QStringList::Iterator it;
	for (it = config.notifies.begin(); it != config.notifies.end(); ++it) {
		QString nick;
		uin = atoi((const char *)(*it).local8Bit());
		if (userlist.containsUin(uin)) {
			nick = userlist.byUin(uin).altnick;
			e_notifies->insertItem(nick);
			}
		else
			it = config.notifies.remove(it);
		}
	/* end two panes */

	QVGroupBox *notifybox = new QVGroupBox(box4);
	notifybox->setTitle(i18n("Notify options"));
	notifybox->setMargin(2);

	if (config.notifyglobal)
		b_notifyglobal->setChecked(true);
	else
		notifybox->setEnabled(false);

	b_notifysound = new QCheckBox(notifybox);
	b_notifysound->setText(i18n("Notify by sound"));

	QHGroupBox *soundbox = new QHGroupBox(notifybox);
	soundbox->setTitle(i18n("Notify sound"));
	e_soundnotify = new QLineEdit(soundbox);
	e_soundnotify->setText(config.soundnotify);

	QPushButton *nsndget = new QPushButton(soundbox);
	nsndget->setPixmap(loader->loadIcon("fileopen", KIcon::Small));
	connect(nsndget, SIGNAL(clicked()), this, SLOT(chooseNotifyFile()));

	b_notifydialog = new QCheckBox(notifybox);
	b_notifydialog->setText(i18n("Notify by dialog box"));

	if (config.notifydialog)
		b_notifydialog->setChecked(true);

	if (config.notifysound)
		b_notifysound->setChecked(true);
	else
		soundbox->setEnabled(false);

	QObject::connect(b_notifysound, SIGNAL(toggled(bool)), soundbox, SLOT(setEnabled(bool)));
	QObject::connect(b_notifyglobal, SIGNAL(toggled(bool)), notifybox, SLOT(setEnabled(bool)));

	addTab(box4, i18n("Users"));
}

void ConfigDialog::setupTab5(void) {
	int i;

	KIconLoader *loader = KGlobal::iconLoader();

	QVBox *box5 = new QVBox(this);
	box5->setMargin(2);	

	b_dccenabled = new QCheckBox(box5);
	b_dccenabled->setText(i18n("DCC enabled"));
	b_dccenabled->setChecked(config.allowdcc);

	b_dccfwd = new QCheckBox(box5);
	b_dccfwd->setText(i18n("DCC forwarding enabled"));
	b_dccfwd->setEnabled(config.allowdcc);

	g_fwdprop = new QVGroupBox(box5);
	g_fwdprop->setTitle(i18n("DCC forwarding properties"));
	g_fwdprop->setMargin(2);

	QHBox *extipbox = new QHBox(g_fwdprop);
	extipbox->setSpacing(5);
	QLabel *l1 = new QLabel(extipbox);
	l1->setText(i18n("External IP address:"));
	e_extip = new QLineEdit(extipbox);

	QHBox *extportbox = new QHBox(g_fwdprop);
	extportbox->setSpacing(5);
	QLabel *l2 = new QLabel(extportbox);
	l2->setText(i18n("External TCP port:"));
	e_extport = new QLineEdit(extportbox);

	b_defserver = new QCheckBox(box5);
	b_defserver->setText(i18n("Use default server"));

	g_server = new QVGroupBox(box5);
	g_server->setTitle(i18n("Server"));
	g_server->setMargin(2);

	QHBox *serverbox = new QHBox(g_server);
	serverbox->setSpacing(5);
	QLabel *l3 = new QLabel(serverbox);
	l3->setText(i18n("IP address:"));
	e_server = new QLineEdit(serverbox);

	g_fwdprop->setEnabled(inet_addr(config.extip) && config.extport > 1023);    
	if (g_fwdprop->isEnabled()) {
		b_dccfwd->setChecked(true);
		e_extip->setText(config.extip);
		e_extport->setText(QString::number(config.extport));	
		}
	g_server->setEnabled(inet_addr(config.server));
	if (!g_server->isEnabled())
		b_defserver->setChecked(true);
	else
		e_server->setText(config.server);

	QObject::connect(b_dccenabled, SIGNAL(toggled(bool)), this, SLOT(ifDccEnabled(bool)));
	QObject::connect(b_dccfwd, SIGNAL(toggled(bool)), g_fwdprop, SLOT(setEnabled(bool)));
	QObject::connect(b_defserver, SIGNAL(toggled(bool)), this, SLOT(ifDefServerEnabled(bool)));

	addTab(box5, i18n("Network"));
}

void ConfigDialog::ifDccEnabled(bool toggled) {
	b_dccfwd->setEnabled(toggled);
	if (!toggled)
		g_fwdprop->setEnabled(false);
	else
		if (b_dccfwd->isChecked())
			g_fwdprop->setEnabled(toggled);
}

void ConfigDialog::ifDefServerEnabled(bool toggled) {
	g_server->setEnabled(!toggled);
}

void ConfigDialog::_Left(void) {
	if (e_notifies->currentItem() != -1) {
		e_availusers->insertItem(e_notifies->text(e_notifies->currentItem()));
		e_notifies->removeItem(e_notifies->currentItem());
		}
}

void ConfigDialog::_Right(void) {
	if (e_availusers->currentItem() != -1) {
		e_notifies->insertItem(e_availusers->text(e_availusers->currentItem()));
		e_availusers->removeItem(e_availusers->currentItem());
		}
}

void ConfigDialog::chooseMsgFile(void) {
	QString s(QFileDialog::getOpenFileName( QString::null, "Audio Files (*.wav *.au *.raw)", this));
	e_msgfile->setText(s);
}

void ConfigDialog::chooseNotifyFile(void) {
	QString s(QFileDialog::getOpenFileName( QString::null, "Audio Files (*.wav *.au *.raw)", this));
	e_soundnotify->setText(s);
}

void ConfigDialog::chooseEmoticonsPath(void) {
	QString s(QFileDialog::getExistingDirectory( QString::null, this));
	e_emoticonspath->setText(s);
}

void ConfigDialog::chooseChatFile(void) {
	QString s(QFileDialog::getOpenFileName( QString::null, "Audio Files (*.wav *.au *.raw)", this));
	e_chatfile->setText(s);
}

void ConfigDialog::choosePlayerFile(void) {
	QString s(QFileDialog::getOpenFileName( QString::null, "All Files (*)", this));
	e_soundprog->setText(s);
}

void ConfigDialog::emoticonsEnabled(bool enabled) {
	if (enabled)
		QMessageBox::information(this, "Emoticons", i18n("You have enabled the usage of emoticons.\nPlease make sure you will have set the right path\nto the directory where all emoticons are stored") );
}

void ConfigDialog::updateConfig(void) {
	QString tmp;

	config.uin = atoi(e_uin->text().latin1());
	free(config.password);
	config.password = strdup(e_password->text().local8Bit());
	config.savegeometry = b_geometry->isChecked();
	config.logmessages = b_logging->isChecked();
	config.playsoundchat = b_playchat->isChecked();
	config.playsoundchatinvisible = b_playchatinvisible->isChecked();
	free(config.soundmsg);
	config.soundmsg = strdup(e_msgfile->text().local8Bit());
	free(config.soundchat);
	config.soundchat = strdup(e_chatfile->text().local8Bit());
	config.soundprog = strdup(e_soundprog->text().latin1());
	config.playsound = b_playsound->isChecked();
	config.playartsdsp = b_playartsdsp->isChecked();
	config.soundvol = (double) s_volume->value();
	config.soundvol /= 100;
	config.soundvolctrl = b_soundvolctrl->isChecked();
	config.autoaway = b_autoaway->isChecked();
	config.autoawaytime = atoi(e_autoawaytime->text().latin1());
	kadu->autoaway->stop();
	kadu->autoaway->start(config.autoawaytime * 1000, TRUE);
	config.dock = b_dock->isChecked();
	config.defaultstatus = gg_statuses[cb_defstatus->currentItem()];

	bool prevprivatestatus = config.privatestatus;
	config.privatestatus = b_private->isChecked();
	if (!statusppm->isItemChecked(6) && !statusppm->isItemChecked(7)
		&& prevprivatestatus != config.privatestatus) {
		statusppm->setItemChecked(8, config.privatestatus);
		kadu->setStatus(sess.status & (~GG_STATUS_FRIENDS_MASK));
		}

	config.smsapp = strdup(e_smsapp->text().latin1());
	config.smsconf = strdup(e_smsconf->text().latin1());
	config.smscustomconf = b_smscustomconf->isChecked();
	config.emoticons = b_emoticons->isChecked();
	config.autosend = b_autosend->isChecked();
	config.scrolldown = b_scrolldown->isChecked();
	config.emoticonspath = strdup(e_emoticonspath->text().latin1());
	config.chatprune = b_chatprune->isChecked();
	config.chatprunelen = atoi(e_chatprunelen->text().latin1());
	config.msgacks = b_msgacks->isChecked();
	
	free(config.soundnotify);
	config.soundnotify = strdup(e_soundnotify->text().latin1());
	config.notifyglobal = b_notifyglobal->isChecked();
	config.notifysound = b_notifysound->isChecked();
	config.notifydialog = b_notifydialog->isChecked();

	config.notifies.clear();
	for (int i = 0; i < e_notifies->count(); i++) {
		tmp = e_notifies->text(i);
		config.notifies.append(QString::number(userlist.byAltNick(tmp).uin));
		}

	delete config.extip;
	config.allowdcc = b_dccenabled->isChecked();
	if (config.allowdcc && b_dccfwd->isChecked() && inet_addr(e_extip->text().latin1()) != INADDR_NONE
		&& atoi(e_extport->text().latin1()) > 1023) {
		config.extip = strdup(e_extip->text().latin1());
		config.extport = atoi(e_extport->text().latin1());
		}
	else {
		config.extip = strdup("0.0.0.0");
		config.extport = 0;
		}
	delete config.server;
	if (!b_defserver->isChecked() && inet_addr(e_server->text().latin1()) != INADDR_NONE)
		config.server = strdup(e_server->text().latin1());
	else
		config.server = strdup("0.0.0.0");

	/* and now, save it */
	saveKaduConfig();
}

#include "config.moc"
