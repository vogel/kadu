/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qvaluelist.h>
#include <qcolordialog.h>
#include <qfontdatabase.h>
#include <qfiledialog.h>
#include <math.h>
#include <qvbox.h>
#include <qhgroupbox.h>
#include <kglobal.h>
#include <qpushbutton.h>
#include <kicontheme.h>
#include <kiconloader.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qtimer.h>
#include <qgrid.h>
#include <klocale.h>
#include <kconfig.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

//
#include "kadu.h"
#include "misc.h"
#include "emoticons.h"
#include "config.h"
#include "dock_widget.h"
#ifdef HAVE_OPENSSL
extern "C"
{
#include "simlite.h"
};
#endif
//

void loadKaduConfig(void) {  	
	/* first read our own config file... */
	fprintf(stderr,"KK loadKaduConfig(): Reading config file...\n");
	KConfig * konf;
	konf = new KConfig(ggPath(QString("kadu.conf")));

	konf->setGroup("Global");
	config.uin = konf->readNumEntry("UIN",0);
	config.password = pwHash(konf->readEntry("Password",""));
	fprintf(stderr,"KK Read user data: uin %d password :-P\n", config.uin);
	config.soundprog = strdup(konf->readEntry("SoundPlayer",""));
	config.soundmsg = strdup(konf->readEntry("Message_sound",""));
	config.soundvolctrl = konf->readBoolEntry("VolumeControl",false);
	config.soundvol = konf->readDoubleNumEntry("SoundVolume",1.0);
	config.soundchat = strdup(konf->readEntry("Chat_sound",""));
	config.nick = konf->readEntry("Nick", i18n("Me"));
	
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
	config.dccip = strdup(konf->readEntry("DccIP", "0.0.0.0"));
	config.extip = strdup(konf->readEntry("ExternalIP", "0.0.0.0"));
	config.extport = konf->readNumEntry("ExternalPort", 0);
	config.servers = QStringList::split(";", konf->readEntry("Server", ""));
	server_nr = 0;
	config.dock = konf->readBoolEntry("UseDocking",true);
	config.raise = konf->readBoolEntry("AutoRaise",false);
	config.privatestatus = konf->readBoolEntry("PrivateStatus", false);
	config.rundocked = konf->readBoolEntry("RunDocked", false);
	config.grouptabs = konf->readBoolEntry("DisplayGroupTabs", true);
	config.checkupdates = konf->readBoolEntry("CheckUpdates", true);
	config.addtodescription = konf->readBoolEntry("AddToDescription", false);
	config.showhint = konf->readBoolEntry("ShowHint",false);

	if (config.savegeometry)
		config.geometry = konf->readRectEntry("Geometry");

	konf->setGroup("WWW");
	config.defaultwebbrowser = konf->readBoolEntry("DefaultWebBrowser", true);
	config.webbrowser = konf->readEntry("WebBrowser", "");
	
	konf->setGroup("SMS");
	config.smsbuildin = konf->readBoolEntry("BuiltInApp",true);
	config.smsapp = strdup(konf->readEntry("SmsApp",""));
	config.smscustomconf = konf->readBoolEntry("UseCustomString",false);
	config.smsconf = strdup(konf->readEntry("SmsString",""));

	konf->setGroup("Other");
	config.emoticons = konf->readBoolEntry("UseEmoticons",false);
	emoticons.setEmoticonsTheme(konf->readEntry("EmoticonsTheme","kadubis"));
	config.autosend = konf->readBoolEntry("AutoSend",false);
	config.scrolldown = konf->readBoolEntry("ScrollDown",true);
	config.chatprune = konf->readBoolEntry("ChatPrune",false);
	config.chatprunelen = konf->readNumEntry("ChatPruneLen",20);
	config.msgacks = konf->readBoolEntry("MessageAcks", true);
	config.blinkchattitle = konf->readBoolEntry("BlinkChatTitle", true);
	config.ignoreanonusers = konf->readBoolEntry("IgnoreAnonymousUsers", false);
#ifdef HAVE_OPENSSL
	config.encryption = konf->readBoolEntry("Encryption", false);
	config.keyslen = konf->readNumEntry("KeysLength", 1024);
#endif

	konf->setGroup("Notify");
	config.soundnotify = strdup(konf->readEntry("NotifySound", ""));
	config.notifyglobal = konf->readBoolEntry("NotifyStatusChange", false);
	config.notifyall = konf->readBoolEntry("NotifyAboutAll", false);
	config.notifydialog = konf->readBoolEntry("NotifyWithDialogBox", false);
	config.notifysound = konf->readBoolEntry("NotifyWithSound", false);
//	config.notifies = konf->readListEntry("NotifyUsers");

	konf->setGroup("Proxy");
	config.useproxy = konf->readBoolEntry("UseProxy", false);
	config.proxyaddr = konf->readEntry("ProxyHost", "");
	config.proxyport = konf->readNumEntry("ProxyPort", 0);
	config.proxyuser = pwHash(konf->readEntry("ProxyUser", ""));
	config.proxypassword = pwHash(konf->readEntry("ProxyPassword", ""));

	konf->setGroup("Colors");
	config.colors.userboxBgColor = konf->readEntry("UserboxBgColor","#FFFFFF");
	config.colors.userboxFgColor = konf->readEntry("UserboxFgColor","#000000");
	config.colors.chatMyBgColor = konf->readEntry("ChatMyBgColor", "#E0E0E0");
	config.colors.chatUsrBgColor = konf->readEntry("ChatUsrBgColor", "#F0F0F0");
	config.colors.chatMyFontColor = konf->readEntry("ChatMyFontColor", "#000000");
	config.colors.chatUsrFontColor = konf->readEntry("ChatUsrFontColor", "#000000");

	konf->setGroup("Fonts");
	QFontInfo info(a->font());
	config.fonts.userboxFont = konf->readEntry("UserboxFont", info.family());
	config.fonts.userboxFontSize = konf->readNumEntry("UserboxFontSize", info.pointSize());
	config.fonts.chatFont = konf->readEntry("ChatFont", info.family());
	config.fonts.chatFontSize = konf->readNumEntry("ChatFontSize", info.pointSize());

	/* no need for it anymore */
	delete konf;
}

void saveKaduConfig(void) {
	fprintf(stderr,"KK saveKaduConfig(): Writing config files...\n");
	KConfig * konf;
	konf = new KConfig(ggPath(QString("kadu.conf")));

	konf->setGroup("Global");
	konf->writeEntry("UIN",config.uin);
	konf->writeEntry("Password", pwHash(config.password));
	konf->writeEntry("Nick", config.nick);
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
	konf->writeEntry("DccIP", config.dccip);
	konf->writeEntry("ExternalIP", config.extip);
	konf->writeEntry("ExternalPort", config.extport);
	konf->writeEntry("Server", config.servers.join(";"));
	konf->writeEntry("UseDocking",config.dock);
	konf->writeEntry("AutoRaise",config.raise);
	konf->writeEntry("PrivateStatus",config.privatestatus);
	konf->writeEntry("RunDocked",config.rundocked);
	konf->writeEntry("CheckUpdates", config.checkupdates);
	konf->writeEntry("DisplayGroupTabs",config.grouptabs);
	konf->writeEntry("AddToDescription",config.addtodescription);
	konf->writeEntry("ShowHint",config.showhint);

	konf->setGroup("WWW");
	konf->writeEntry("DefaultWebBrowser", config.defaultwebbrowser);	
	konf->writeEntry("WebBrowser", config.webbrowser);

	konf->setGroup("SMS");
	konf->writeEntry("BuiltInApp",config.smsbuildin);	
	konf->writeEntry("SmsApp",config.smsapp);
	konf->writeEntry("SmsString",config.smsconf);
	konf->writeEntry("UseCustomString",config.smscustomconf);

	konf->setGroup("Other");
	konf->writeEntry("UseEmoticons",config.emoticons);
	konf->writeEntry("EmoticonsTheme",config.emoticons_theme);
	konf->writeEntry("AutoSend",config.autosend);
	konf->writeEntry("ScrollDown",config.scrolldown);
	konf->writeEntry("ChatPrune",config.chatprune);
	konf->writeEntry("ChatPruneLen",config.chatprunelen);
	konf->writeEntry("MessageAcks", config.msgacks);
	konf->writeEntry("BlinkChatTitle", config.blinkchattitle);
	konf->writeEntry("IgnoreAnonymousUsers", config.ignoreanonusers);
#ifdef HAVE_OPENSSL
        konf->writeEntry("Encryption", config.encryption);
        konf->writeEntry("KeysLength", config.keyslen);
#endif

	konf->setGroup("Proxy");
	konf->writeEntry("UseProxy",config.useproxy);
	konf->writeEntry("ProxyHost",config.proxyaddr);
	konf->writeEntry("ProxyPort",config.proxyport);
	konf->writeEntry("ProxyUser", pwHash(config.proxyuser));
	konf->writeEntry("ProxyPassword", pwHash(config.proxypassword));

	konf->setGroup("Notify");
//	konf->writeEntry("NotifyUsers", config.notifies);
	konf->writeEntry("NotifySound", config.soundnotify);
	konf->writeEntry("NotifyStatusChange", config.notifyglobal);
	konf->writeEntry("NotifyAboutAll", config.notifyall);
	konf->writeEntry("NotifyWithDialogBox", config.notifydialog);
	konf->writeEntry("NotifyWithSound", config.notifysound);

	konf->setGroup("Colors");
	konf->writeEntry("UserboxBgColor", config.colors.userboxBgColor);
	konf->writeEntry("UserboxFgColor", config.colors.userboxFgColor);
	konf->writeEntry("ChatMyBgColor", config.colors.chatMyBgColor);
	konf->writeEntry("ChatUsrBgColor", config.colors.chatUsrBgColor);
	konf->writeEntry("ChatMyFontColor", config.colors.chatMyFontColor);
	konf->writeEntry("ChatUsrFontColor", config.colors.chatUsrFontColor);

	konf->setGroup("Fonts");
	konf->writeEntry("UserboxFont", config.fonts.userboxFont);
	konf->writeEntry("UserboxFontSize", config.fonts.userboxFontSize);
	konf->writeEntry("ChatFont", config.fonts.chatFont);
	konf->writeEntry("ChatFontSize", config.fonts.chatFontSize);

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
	setupTab6();

	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(updateConfig()));
	setCancelButton(i18n("Cancel"));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close()));
	setCaption(i18n("Kadu configuration"));
	resize(480,440);
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

	QLabel *l_nick = new QLabel(userinfo);
	l_nick->setText(i18n("Nick"));
	
	e_nick = new QLineEdit(userinfo);
	e_nick->setText(config.nick);
	
	/* SMS begin */
	QVGroupBox *smsvgrp = new QVGroupBox(box);
	smsvgrp->setTitle(i18n("SMS options"));

	b_smsbuildin = new QCheckBox(smsvgrp);
	b_smsbuildin->setText(i18n("Use built-in SMS application"));

	smshbox1 = new QHBox(smsvgrp);
	smshbox1->setSpacing(5);
	QLabel *l_smsapp = new QLabel(smshbox1);
	l_smsapp->setText(i18n("Custom SMS application"));
	e_smsapp = new QLineEdit(smshbox1);
	e_smsapp->setText(config.smsapp);

	smshbox2 = new QHBox(smsvgrp);
	smshbox2->setSpacing(5);
	b_smscustomconf = new QCheckBox(smshbox2);
	b_smscustomconf->setText(i18n("SMS custom string"));
	QToolTip::add(b_smscustomconf,i18n("Check this box if your sms application doesn't understand arguments: number \"message\"\nArguments should be separated with spaces. %n argument is converted to number, %m to message"));

	e_smsconf = new QLineEdit(smshbox2);
	e_smsconf->setText(config.smsconf);

	if (config.smsbuildin)
	{
		b_smsbuildin->setChecked(true);
		smshbox1->setEnabled(false);
		smshbox2->setEnabled(false);
	};
		
	if (config.smscustomconf)
		b_smscustomconf->setChecked(true);
	else
		e_smsconf->setEnabled(false);

	QObject::connect(b_smsbuildin, SIGNAL(toggled(bool)), this, SLOT(onSmsBuildInCheckToogle(bool)));
	QObject::connect(b_smscustomconf, SIGNAL(toggled(bool)), e_smsconf, SLOT(setEnabled(bool)));
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

	QGrid* grid = new QGrid(3, box);

	b_logging = new QCheckBox(grid);
	b_logging->setText(i18n("Log messages"));
	if (config.logmessages)
		b_logging->setChecked(true);

	b_geometry = new QCheckBox(grid);
	b_geometry->setText(i18n("Restore window geometry"));
	if (config.savegeometry)
		b_geometry->setChecked(true);

	b_dock = new QCheckBox(grid);
	b_dock->setText(i18n("Enable dock icon"));
	if (config.dock)
		b_dock->setChecked(true);

	b_private = new QCheckBox(grid);
	b_private->setText(i18n("Private status"));
	if (config.privatestatus)
		b_private->setChecked(true);

	b_rdocked = new QCheckBox(grid);
	b_rdocked->setText(i18n("Start docked"));
	if (config.rundocked)
		b_rdocked->setChecked(true);

	b_grptabs = new QCheckBox(grid);
	b_grptabs->setText(i18n("Display group tabs"));
	if (config.grouptabs)
		b_grptabs->setChecked(true);

	b_checkupdates = new QCheckBox(grid);
	b_checkupdates->setText(i18n("Check for updates"));
	if (config.checkupdates)
		b_checkupdates->setChecked(true);
		
	b_addtodescription = new QCheckBox(grid);
	b_addtodescription->setText(i18n("Add to description"));
	if (config.addtodescription)
		b_addtodescription->setChecked(true);		
	QToolTip::add(b_addtodescription,i18n("If a file description in gg settings directory is present, its contents will be added\nto the status description and then the file will be deleted."));

	b_showhint = new QCheckBox(grid);
	b_showhint->setText(i18n("Dock hint(experimental)"));
	if (config.showhint)
		b_showhint->setChecked(true);		

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

	QPushButton *testsoundmsg = new QPushButton(msggroup);
	testsoundmsg->setText(i18n("Testing"));
	connect(testsoundmsg, SIGNAL(clicked()), this, SLOT(chooseMsgTest()));

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

	QPushButton *testsoundchat = new QPushButton(chatgroup);
	testsoundchat->setText(i18n("Testing"));
	connect(testsoundchat, SIGNAL(clicked()), this, SLOT(chooseChatTest()));

	QObject::connect(b_playsound, SIGNAL(toggled(bool)), testsoundmsg, SLOT(setEnabled(bool)));
	QObject::connect(b_playsound, SIGNAL(toggled(bool)), testsoundchat, SLOT(setEnabled(bool)));

	addTab(box2, i18n("Sounds"));
}

void ConfigDialog::setupTab3(void) {
//	KIconLoader *loader = KGlobal::iconLoader();
	QVBox *box3 = new QVBox(this);
	box3->setMargin(5);	

	QVGroupBox *emogroup = new QVGroupBox(box3);
	emogroup->setTitle(i18n("Emoticons"));

	b_emoticons = new QCheckBox(emogroup);
	b_emoticons->setText(i18n("Enable emoticons in chat window"));

	QHBox* emotheme_box = new QHBox(emogroup);
	QLabel* l_emoticons_theme=new QLabel(emotheme_box);
	l_emoticons_theme->setText(i18n("Emoticons theme"));
	cb_emoticons_theme=new QComboBox(emotheme_box);
	cb_emoticons_theme->insertStringList(emoticons.themes());
	cb_emoticons_theme->setCurrentText(config.emoticons_theme);

	if (config.emoticons)
		b_emoticons->setChecked(true);
	else
		emotheme_box->setEnabled(false);

	QObject::connect(b_emoticons,SIGNAL(toggled(bool)), emotheme_box, SLOT(setEnabled(bool)));

	/* WWW begin */
	QVGroupBox *webvgrp = new QVGroupBox(box3);
	webvgrp->setTitle(i18n("WWW options"));

	b_defwebbrowser = new QCheckBox(webvgrp);
	b_defwebbrowser->setText(i18n("Use default Web browser"));

	webhbox1 = new QHBox(webvgrp);
	webhbox1->setSpacing(5);
	QLabel *l_webbrowser = new QLabel(webhbox1);
	l_webbrowser->setText(i18n("Custom Web browser"));
	e_webbrowser = new QLineEdit(webhbox1);
	e_webbrowser->setText(config.webbrowser);

	if (config.defaultwebbrowser) {
		b_defwebbrowser->setChecked(true);
		webhbox1->setEnabled(false);
		}
		
	QObject::connect(b_defwebbrowser, SIGNAL(toggled(bool)), this, SLOT(onDefWebBrowserToogle(bool)));
	/* WWW end */

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

#ifdef HAVE_OPENSSL
	const char* keyslens[] = { "128", "256", "512", "768", "1024", 0 };

	b_encryption = new QCheckBox(box3);
	b_encryption->setText(i18n("Use encryption"));

	QHGroupBox *encryptbox = new QHGroupBox(box3);
	encryptbox->setTitle(i18n("Encryption properties"));
	QLabel *l_lenencrypt = new QLabel(encryptbox);
	l_lenencrypt->setText(i18n("Keys length"));

	cb_keyslen = new QComboBox(encryptbox);
	cb_keyslen->insertStrList(keyslens);
	cb_keyslen->setCurrentText(QString::number(config.keyslen));

	QPushButton *pb_genkeys = new QPushButton(encryptbox);
	pb_genkeys->setText(i18n("Generate keys"));

        if (config.encryption) {
		b_encryption->setChecked(true);
	} else {
		encryptbox->setEnabled(false);
	}

	QObject::connect(b_encryption, SIGNAL(toggled(bool)), encryptbox, SLOT(setEnabled(bool)));
	QObject::connect(pb_genkeys, SIGNAL(clicked()), this, SLOT(generateMyKeys()));
#endif

	b_scrolldown = new QCheckBox(box3);
	b_scrolldown->setText(i18n("Scroll chat window downward, not upward"));
	if (config.scrolldown)
		b_scrolldown->setChecked(true);

	b_autosend = new QCheckBox(box3);
	b_autosend->setText(i18n("\"Enter\" key in chat sends message by default"));
	if (config.autosend)
		b_autosend->setChecked(true);
	
	b_msgacks = new QCheckBox(box3);
	b_msgacks->setText(i18n("Message acknowledgements (wait for delivery)"));
	b_msgacks->setChecked(config.msgacks);

	b_blinkchattitle = new QCheckBox(box3);
	b_blinkchattitle->setText(i18n("Flash chat title on new message"));
	b_blinkchattitle->setChecked(config.blinkchattitle);

	b_ignoreanonusers = new QCheckBox(box3);
	b_ignoreanonusers->setText(i18n("Ignore messages from anonymous users"));
	b_ignoreanonusers->setChecked(config.ignoreanonusers);

	addTab(box3, i18n("Chat"));
}

void ConfigDialog::setupTab4(void) {
	int i;
//	uin_t uin;

	KIconLoader *loader = KGlobal::iconLoader();
	QVBox *box4 = new QVBox(this);
	box4->setMargin(2);	
	
	b_notifyglobal = new QCheckBox(box4);
	b_notifyglobal->setText(i18n("Notify when users become available"));

	b_notifyall = new QCheckBox(box4);
	b_notifyall->setText(i18n("Notify about all users"));

	/* two nice panes */
	panebox = new QHBox(box4);

	QVBox *vbox1 = new QVBox(panebox);
	QLabel *_l1 = new QLabel(vbox1);
	_l1->setText(i18n("Available"));
	e_availusers = new QListBox(vbox1);

	QVBox *vbox2 = new QVBox(panebox);
	QPushButton *_goRight = new QPushButton (vbox2);
	_goRight->setPixmap( loader->loadIcon("forward", KIcon::Small));

	QPushButton *_goLeft = new QPushButton (vbox2);
	_goLeft->setPixmap( loader->loadIcon("back", KIcon::Small));

	QVBox *vbox3 = new QVBox(panebox);
	QLabel *_l2 = new QLabel(vbox3);
	_l2->setText(i18n("Tracked"));
	e_notifies = new QListBox(vbox3);

	i = 0;
	while (i < userlist.count()) {
		if (!userlist[i].notify)
			e_availusers->insertItem(userlist[i].altnick);
		else
			e_notifies->insertItem(userlist[i].altnick);
		i++;
		}


	QObject::connect(_goRight, SIGNAL(clicked()), this, SLOT(_Right()));
	QObject::connect(_goLeft, SIGNAL(clicked()), this, SLOT(_Left()));

	// end of two panes

	notifybox = new QVGroupBox(box4);
	notifybox->setTitle(i18n("Notify options"));
	notifybox->setMargin(2);

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

	if (config.notifyglobal)
		b_notifyglobal->setChecked(true);
	else {
		b_notifyall->setEnabled(false);
		notifybox->setEnabled(false);
		panebox->setEnabled(false);
		}

	if (config.notifyall) {
		b_notifyall->setChecked(true);
		panebox->setEnabled(false);
		}

	if (config.notifydialog)
		b_notifydialog->setChecked(true);

	if (config.notifysound)
		b_notifysound->setChecked(true);
	else
		soundbox->setEnabled(false);

	QObject::connect(b_notifysound, SIGNAL(toggled(bool)), soundbox, SLOT(setEnabled(bool)));
	QObject::connect(b_notifyall, SIGNAL(toggled(bool)), this, SLOT(ifNotifyAll(bool)));
	QObject::connect(b_notifyglobal, SIGNAL(toggled(bool)), this, SLOT(ifNotifyGlobal(bool)));

	addTab(box4, i18n("Users"));
}

void ConfigDialog::setupTab5(void) {
//	int i;

//	KIconLoader *loader = KGlobal::iconLoader();

	QVBox *box5 = new QVBox(this);
	box5->setMargin(2);	

	b_dccenabled = new QCheckBox(box5);
	b_dccenabled->setText(i18n("DCC enabled"));
	b_dccenabled->setChecked(config.allowdcc);

	b_dccip = new QCheckBox(box5);
	b_dccip->setText(i18n("DCC IP autodetection"));
	b_dccip->setChecked(!inet_addr(config.dccip));

	g_dccip = new QVGroupBox(box5);
	g_dccip->setTitle(i18n("DCC IP"));
	g_dccip->setEnabled(!b_dccip->isChecked());

	QHBox *dccipbox = new QHBox(g_dccip);
	dccipbox->setSpacing(5);
	QLabel *l4 = new QLabel(dccipbox);
	l4->setText(i18n("IP address:"));
	e_dccip = new QLineEdit(dccipbox);
	if (g_dccip->isEnabled())
		e_dccip->setText(config.dccip);

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
	b_defserver->setText(i18n("Use default servers"));

	g_server = new QVGroupBox(box5);
	g_server->setTitle(i18n("Servers"));
	g_server->setMargin(2);

	QHBox *serverbox = new QHBox(g_server);
	serverbox->setSpacing(5);
	QLabel *l3 = new QLabel(serverbox);
	l3->setText(i18n("IP addresses:"));
	e_server = new QLineEdit(serverbox);

	b_useproxy = new QCheckBox(box5);
	b_useproxy->setText(i18n("Use proxy server"));

	g_proxy = new QVGroupBox(box5);
	g_proxy->setTitle(i18n("Proxy server"));

	QHBox *proxyserverbox = new QHBox(g_proxy);
	proxyserverbox->setSpacing(5);
	QLabel *l5 = new QLabel(proxyserverbox);
	l5->setText(i18n("IP address:"));
	e_proxyserver = new QLineEdit(proxyserverbox);
	QLabel *l6 = new QLabel(proxyserverbox);
	l6->setText(i18n("Port:"));
	e_proxyport = new QLineEdit(proxyserverbox);

	QHBox *proxyuserbox = new QHBox(g_proxy);
	proxyuserbox->setSpacing(5);
	QLabel *l7 = new QLabel(proxyuserbox);
	l7->setText(i18n("Username:"));
	e_proxyuser = new QLineEdit(proxyuserbox);
	QLabel *l8 = new QLabel(proxyuserbox);
	l8->setText(i18n("Password:"));
	e_proxypassword = new QLineEdit(proxyuserbox);
	e_proxypassword->setEchoMode(QLineEdit::Password);

	g_fwdprop->setEnabled(inet_addr(config.extip) && config.extport > 1023);    
	if (g_fwdprop->isEnabled()) {
		b_dccfwd->setChecked(true);
		e_extip->setText(config.extip);
		e_extport->setText(QString::number(config.extport));	
		}
	g_server->setEnabled(config.servers.count() && inet_addr(config.servers[0].latin1()) != INADDR_NONE);
	if (!g_server->isEnabled())
		b_defserver->setChecked(true);
	else
		e_server->setText(config.servers.join(";"));
	g_proxy->setEnabled(inet_addr(config.proxyaddr) && config.proxyport > 1023 && config.useproxy);
	b_useproxy->setChecked(g_proxy->isEnabled());
	if (g_proxy->isEnabled()) {
		e_proxyserver->setText(config.proxyaddr);
		e_proxyport->setText(QString::number(config.proxyport));
		e_proxyuser->setText(config.proxyuser);
		e_proxypassword->setText(config.proxypassword);
		}

	QObject::connect(b_dccenabled, SIGNAL(toggled(bool)), this, SLOT(ifDccEnabled(bool)));
	QObject::connect(b_dccip, SIGNAL(toggled(bool)), this, SLOT(ifDccIpEnabled(bool)));
	QObject::connect(b_dccfwd, SIGNAL(toggled(bool)), g_fwdprop, SLOT(setEnabled(bool)));
	QObject::connect(b_defserver, SIGNAL(toggled(bool)), this, SLOT(ifDefServerEnabled(bool)));
	QObject::connect(b_useproxy, SIGNAL(toggled(bool)), this, SLOT(ifUseProxyEnabled(bool)));

	addTab(box5, i18n("Network"));
}

void ConfigDialog::setupTab6(void) {

	QPixmap pm_buttoncolor(30,7);
	QFontDatabase fdb;
	QValueList<int> vl;

//	KIconLoader *loader = KGlobal::iconLoader();

	QVBox *box6 = new QVBox(this);
	box6->setMargin(2);

	QVGroupBox *chatprop = new QVGroupBox(box6);
	chatprop->setTitle(i18n("Chat properties"));

	QHBox *chatmybgcolor = new QHBox(chatprop);
	chatmybgcolor->setSpacing(5);

	QLabel *l_chatmybgcolor = new QLabel(chatmybgcolor);
	l_chatmybgcolor->setText(i18n("Your background color"));

	e_chatmybgcolor = new QLineEdit(chatmybgcolor);
	e_chatmybgcolor->setText(config.colors.chatMyBgColor);

	pm_buttoncolor.fill(QColor(config.colors.chatMyBgColor));

	pb_chatmybgcolor = new QPushButton(chatmybgcolor);
	pb_chatmybgcolor->setPixmap(pm_buttoncolor);
	connect(pb_chatmybgcolor, SIGNAL(clicked()), this, SLOT(chooseChatMyBgColorGet()));

	QHBox *chatusrbgcolor = new QHBox(chatprop);
	chatusrbgcolor->setSpacing(5);

	QLabel *l_chatusrbgcolor = new QLabel(chatusrbgcolor);
	l_chatusrbgcolor->setText(i18n("User background color"));

	e_chatusrbgcolor = new QLineEdit(chatusrbgcolor);
	e_chatusrbgcolor->setText(config.colors.chatUsrBgColor);

	pm_buttoncolor.fill(QColor(config.colors.chatUsrBgColor));

	pb_chatusrbgcolor = new QPushButton(chatusrbgcolor);
	pb_chatusrbgcolor->setPixmap(pm_buttoncolor);
	connect(pb_chatusrbgcolor, SIGNAL(clicked()), this, SLOT(chooseChatUsrBgColorGet()));

	QHBox *chatmyfontcolor = new QHBox(chatprop);
	chatmyfontcolor->setSpacing(5);

	QLabel *l_chatmyfontcolor = new QLabel(chatmyfontcolor);
	l_chatmyfontcolor->setText(i18n("Your font color"));

	e_chatmyfontcolor = new QLineEdit(chatmyfontcolor);
	e_chatmyfontcolor->setText(config.colors.chatMyFontColor);

	pm_buttoncolor.fill(QColor(config.colors.chatMyFontColor));

	pb_chatmyfontcolor = new QPushButton(chatmyfontcolor);
	pb_chatmyfontcolor->setPixmap(pm_buttoncolor);
	connect(pb_chatmyfontcolor, SIGNAL(clicked()), this, SLOT(chooseChatMyFontColorGet()));

	QHBox *chatusrfontcolor = new QHBox(chatprop);
	chatusrfontcolor->setSpacing(5);

	QLabel *l_chatusrfontcolor = new QLabel(chatusrfontcolor);
	l_chatusrfontcolor->setText(i18n("User font color"));

	e_chatusrfontcolor = new QLineEdit(chatusrfontcolor);
	e_chatusrfontcolor->setText(config.colors.chatUsrFontColor);

	pm_buttoncolor.fill(QColor(config.colors.chatUsrFontColor));

	pb_chatusrfontcolor = new QPushButton(chatusrfontcolor);
	pb_chatusrfontcolor->setPixmap(pm_buttoncolor);
	connect(pb_chatusrfontcolor, SIGNAL(clicked()), this, SLOT(chooseChatUsrFontColorGet()));

	QHBox *chatfont = new QHBox(chatprop);
	chatfont->setSpacing(5);

	QLabel *l_chatfont = new QLabel(chatfont);
	l_chatfont->setText(i18n("Font"));

	cb_chatfont = new QComboBox(chatfont);
	cb_chatfont->insertStringList(fdb.families());
	cb_chatfont->setCurrentText(config.fonts.chatFont);
	connect(cb_chatfont, SIGNAL(activated(int)), this, SLOT(chooseChatFontGet(int)));

	QHBox *chatfontsize = new QHBox(chatprop);
	chatfontsize->setSpacing(5);

	QLabel *l_chatfontsize = new QLabel(chatfontsize);
	l_chatfontsize->setText(i18n("Font size"));

	cb_chatfontsize = new QComboBox(chatfontsize);

	vl = fdb.pointSizes(config.fonts.chatFont,"Normal");
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
	cb_chatfontsize->insertItem(QString::number(*points));
	cb_chatfontsize->setCurrentText(QString::number(config.fonts.chatFontSize));

	QVGroupBox *userboxprop = new QVGroupBox(box6);
	userboxprop->setTitle(i18n("Userbox properties"));

	QHBox *userboxbgcolor = new QHBox(userboxprop);
	userboxbgcolor->setSpacing(5);

	QLabel *l_userboxbgcolor = new QLabel(userboxbgcolor);
	l_userboxbgcolor->setText(i18n("Userbox background color"));

	e_userboxbgcolor = new QLineEdit(userboxbgcolor);
	e_userboxbgcolor->setText(config.colors.userboxBgColor);

	pm_buttoncolor.fill(QColor(config.colors.userboxBgColor));

	pb_userboxbgcolor = new QPushButton(userboxbgcolor);
	pb_userboxbgcolor->setPixmap(pm_buttoncolor);
	connect(pb_userboxbgcolor, SIGNAL(clicked()), this, SLOT(chooseUserboxBgColorGet()));
	
	QHBox *userboxfgcolor = new QHBox(userboxprop);
	userboxfgcolor->setSpacing(5);

	QLabel *l_userboxfgcolor = new QLabel(userboxfgcolor);
	l_userboxfgcolor->setText(i18n("Userbox foreground color"));

	e_userboxfgcolor = new QLineEdit(userboxfgcolor);
	e_userboxfgcolor->setText(config.colors.userboxFgColor);

	pm_buttoncolor.fill(QColor(config.colors.userboxFgColor));

	pb_userboxfgcolor = new QPushButton(userboxfgcolor);
	pb_userboxfgcolor->setPixmap(pm_buttoncolor);
	connect(pb_userboxfgcolor, SIGNAL(clicked()), this, SLOT(chooseUserboxFgColorGet()));

	QHBox *userboxfont = new QHBox(userboxprop);
	userboxfont->setSpacing(5);

	QLabel *l_userboxfont = new QLabel(userboxfont);
	l_userboxfont->setText(i18n("Font"));

	cb_userboxfont = new QComboBox(userboxfont);
	cb_userboxfont->insertStringList(fdb.families());
	cb_userboxfont->setCurrentText(config.fonts.userboxFont);
	connect(cb_userboxfont, SIGNAL(activated(int)), this, SLOT(chooseUserboxFontGet(int)));

	QHBox *userboxfontsize = new QHBox(userboxprop);
	userboxfontsize->setSpacing(5);

	QLabel *l_userboxfontsize = new QLabel(userboxfontsize);
	l_userboxfontsize->setText(i18n("Font size"));

	cb_userboxfontsize = new QComboBox(userboxfontsize);

	vl = fdb.pointSizes(config.fonts.userboxFont,"Normal");
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
	cb_userboxfontsize->insertItem(QString::number(*points));
	cb_userboxfontsize->setCurrentText(QString::number(config.fonts.userboxFontSize));

	addTab(box6, i18n("Look"));
};


void ConfigDialog::onSmsBuildInCheckToogle(bool toggled)
{
	smshbox1->setEnabled(!toggled);
	smshbox2->setEnabled(!toggled);
};

void ConfigDialog::onDefWebBrowserToogle(bool toggled)
{
	webhbox1->setEnabled(!toggled);
};

void ConfigDialog::ifDccEnabled(bool toggled) {
	b_dccip->setEnabled(toggled);
	b_dccfwd->setEnabled(toggled);
	if (!toggled) {
		g_dccip->setEnabled(false);
		g_fwdprop->setEnabled(false);
		}
	else	{
		if (!b_dccip->isChecked())
			g_dccip->setEnabled(toggled);
		if (b_dccfwd->isChecked())
			g_fwdprop->setEnabled(toggled);
		}
}

void ConfigDialog::ifNotifyGlobal(bool toggled) {
	b_notifyall->setEnabled(toggled);
	panebox->setEnabled(toggled && !b_notifyall->isChecked());
	notifybox->setEnabled(toggled);
}

void ConfigDialog::ifNotifyAll(bool toggled) {
	panebox->setEnabled(!toggled);
}

void ConfigDialog::ifDccIpEnabled(bool toggled) {
	g_dccip->setEnabled(!toggled);
}

void ConfigDialog::ifDefServerEnabled(bool toggled) {
	g_server->setEnabled(!toggled);
}

void ConfigDialog::ifUseProxyEnabled(bool toggled) {
	g_proxy->setEnabled(toggled);
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
	if (s.length())
		e_msgfile->setText(s);
}

void ConfigDialog::chooseNotifyFile(void) {
	QString s(QFileDialog::getOpenFileName( QString::null, "Audio Files (*.wav *.au *.raw)", this));
	if (s.length())
		e_soundnotify->setText(s);
}

void ConfigDialog::chooseEmoticonsPath(void) {
	QString s(QFileDialog::getExistingDirectory( QString::null, this));
	if (s.length())
		e_emoticonspath->setText(s);
}

void ConfigDialog::chooseChatFile(void) {
	QString s(QFileDialog::getOpenFileName( QString::null, "Audio Files (*.wav *.au *.raw)", this));
	if (s.length())
		e_chatfile->setText(s);
}

void ConfigDialog::choosePlayerFile(void) {
	QString s(QFileDialog::getOpenFileName( QString::null, "All Files (*)", this));
	if (s.length())
		e_soundprog->setText(s);
}

void ConfigDialog::chooseChatMyBgColorGet(void) {
	QColor color = QColorDialog::getColor(QColor(config.colors.chatMyBgColor), this, i18n("Color dialog"));
	if ( color.isValid() ) {
		e_chatmybgcolor->setText(color.name());
		QPixmap pm(30,7);
		pm.fill(QColor(color.name()));
		pb_chatmybgcolor->setPixmap(pm);
		}
}

void ConfigDialog::chooseChatUsrBgColorGet(void) {
	QColor color = QColorDialog::getColor(QColor(config.colors.chatUsrBgColor), this, i18n("Color dialog"));
	if ( color.isValid() ) {
		e_chatusrbgcolor->setText(color.name());
		QPixmap pm(30,7);
		pm.fill(QColor(color.name()));
		pb_chatusrbgcolor->setPixmap(pm);
		}
}

void ConfigDialog::chooseUserboxBgColorGet(void) {
	QColor color = QColorDialog::getColor(QColor(e_userboxbgcolor->text()), this, i18n("Color dialog"));
	if ( color.isValid() ) {
		e_userboxbgcolor->setText(color.name());
		QPixmap pm(30,7);
		pm.fill(QColor(color.name()));
		pb_userboxbgcolor->setPixmap(pm);
		}
}

void ConfigDialog::chooseUserboxFgColorGet(void) {
	QColor color = QColorDialog::getColor(QColor(e_userboxfgcolor->text()), this, i18n("Color dialog"));
	if ( color.isValid() ) {
		e_userboxfgcolor->setText(color.name());
		QPixmap pm(30,7);
		pm.fill(QColor(color.name()));
		pb_userboxfgcolor->setPixmap(pm);
		}
}

void ConfigDialog::chooseChatMyFontColorGet(void) {
	QColor color = QColorDialog::getColor(QColor(e_chatmyfontcolor->text()), this, i18n("Color dialog"));
	if ( color.isValid() ) {
		e_chatmyfontcolor->setText(color.name());
		QPixmap pm(30,7);
		pm.fill(QColor(color.name()));
		pb_chatmyfontcolor->setPixmap(pm);
		}
}

void ConfigDialog::chooseChatUsrFontColorGet(void) {
	QColor color = QColorDialog::getColor(QColor(e_chatusrfontcolor->text()), this, i18n("Color dialog"));
	if ( color.isValid() ) {
		e_chatusrfontcolor->setText(color.name());
		QPixmap pm(30,7);
		pm.fill(QColor(color.name()));
		pb_chatusrfontcolor->setPixmap(pm);
		}
}

void ConfigDialog::chooseChatFontGet(int index) {
	QFontDatabase fdb;
	QValueList<int> vl;
	vl = fdb.pointSizes(cb_chatfont->text(index),"Normal");
	cb_chatfontsize->clear();
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
		cb_chatfontsize->insertItem(QString::number( *points));
}

void ConfigDialog::chooseUserboxFontGet(int index) {
	QFontDatabase fdb;
	QValueList<int> vl;
	vl = fdb.pointSizes(cb_userboxfont->text(index),"Normal");
	cb_userboxfontsize->clear();
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
		cb_userboxfontsize->insertItem(QString::number( *points));
}

void ConfigDialog::chooseMsgTest(void) {
	playSound(config.soundmsg);
}

void ConfigDialog::chooseChatTest(void) {
	playSound(config.soundchat);
}

void ConfigDialog::generateMyKeys(void) {
#ifdef HAVE_OPENSSL
	QString keyfile_path;
	
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(config.uin));
	keyfile_path.append(".pem");
	
	QFileInfo keyfile(keyfile_path);
	
	if (keyfile.permission(QFileInfo::WriteUser)) {
		switch(QMessageBox::warning(this, "Kadu", i18n("Keys exist. Do you want to overwrite them ?"), i18n("Yes"), i18n("No"), QString::null, 0, 1)) {
			case 1: // No
				return;
		}
	}
	
	char fname[PATH_MAX];

	QCString tmp;

	tmp = ggPath("keys").local8Bit();
	mkdir(tmp.data(), 0700);

	fprintf(stderr,"KK Generating my keys, len: %d\n", atoi(cb_keyslen->currentText()));
	if (sim_key_generate(config.uin) < 0) {
		QMessageBox::critical(this, "Kadu", i18n("B..d przy generowaniu klucza"), i18n("OK"), QString::null, 0);
		return;
	}


	QMessageBox::information(this, "Kadu", i18n("Keys have been generated and written"), i18n("OK"), QString::null, 0);

	return;
#endif
}

void ConfigDialog::updateConfig(void) {
	QString tmp;
	int i;//, j;

	config.uin = atoi(e_uin->text().latin1());
	config.password = e_password->text();
	if (e_nick->text().length())
		config.nick = e_nick->text();
	else
		config.nick = i18n("Me");
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
	if (config.autoaway)
		AutoAwayTimer::on();
	else
		AutoAwayTimer::off();
	config.dock = b_dock->isChecked();
	config.defaultstatus = gg_statuses[cb_defstatus->currentItem()];

	bool prevprivatestatus = config.privatestatus;
	config.privatestatus = b_private->isChecked();
	if (!statusppm->isItemChecked(6) && !statusppm->isItemChecked(7)
		&& prevprivatestatus != config.privatestatus) {
		statusppm->setItemChecked(8, config.privatestatus);
		kadu->setStatus(sess->status & (~GG_STATUS_FRIENDS_MASK));
		}
	config.rundocked = b_rdocked->isChecked();
	config.grouptabs = b_grptabs->isChecked();
	config.checkupdates = b_checkupdates->isChecked();
	if (!config.addtodescription && b_addtodescription->isChecked())
		kadu->autostatus_timer->start(1000,TRUE);
	config.addtodescription = b_addtodescription->isChecked();
	if (!config.addtodescription)
		kadu->autostatus_timer->stop();
		
	if (!b_showhint->isChecked() && config.showhint);
		tip = NULL;
	config.showhint = b_showhint->isChecked();

	config.smsbuildin = b_smsbuildin->isChecked();
	config.smsapp = strdup(e_smsapp->text().latin1());
	config.smsconf = strdup(e_smsconf->text().latin1());
	config.smscustomconf = b_smscustomconf->isChecked();
	config.emoticons = b_emoticons->isChecked();
	emoticons.setEmoticonsTheme(cb_emoticons_theme->currentText());
	config.autosend = b_autosend->isChecked();
	config.scrolldown = b_scrolldown->isChecked();
	config.chatprune = b_chatprune->isChecked();
	config.chatprunelen = atoi(e_chatprunelen->text().latin1());
	config.msgacks = b_msgacks->isChecked();
	config.blinkchattitle = b_blinkchattitle->isChecked();
	config.ignoreanonusers = b_ignoreanonusers->isChecked();
	config.defaultwebbrowser = b_defwebbrowser->isChecked();
	config.webbrowser = e_webbrowser->text();
#ifdef HAVE_OPENSSL
	config.encryption = b_encryption->isChecked();
	config.keyslen = atoi(cb_keyslen->currentText());
#endif

	config.colors.chatMyBgColor = e_chatmybgcolor->text();
	config.colors.chatUsrBgColor = e_chatusrbgcolor->text();
	config.colors.chatMyFontColor = e_chatmyfontcolor->text();
	config.colors.chatUsrFontColor = e_chatusrfontcolor->text();
	config.colors.userboxBgColor = e_userboxbgcolor->text();
	config.colors.userboxFgColor = e_userboxfgcolor->text();
	config.fonts.chatFont = cb_chatfont->currentText();
	config.fonts.chatFontSize = atoi(cb_chatfontsize->currentText().latin1());
	config.fonts.userboxFont = cb_userboxfont->currentText();
	config.fonts.userboxFontSize = atoi(cb_userboxfontsize->currentText().latin1());
	free(config.soundnotify);
	config.soundnotify = strdup(e_soundnotify->text().latin1());
	config.notifyglobal = b_notifyglobal->isChecked();
	config.notifyall = b_notifyall->isChecked();
	config.notifysound = b_notifysound->isChecked();
	config.notifydialog = b_notifydialog->isChecked();

	for (i = 0; i < e_notifies->count(); i++) {
		tmp = e_notifies->text(i);
		userlist.byAltNick(tmp).notify = true;
		}
	for (i = 0; i < e_availusers->count(); i++) {
		tmp = e_availusers->text(i);
		userlist.byAltNick(tmp).notify = false;
		}

	delete config.dccip;
	delete config.extip;
	config.allowdcc = b_dccenabled->isChecked();
	if (config.allowdcc && !b_dccip->isChecked() && inet_addr(e_dccip->text().latin1()) != INADDR_NONE)
		config.dccip = strdup(e_dccip->text().latin1());
	else
		config.dccip = strdup("0.0.0.0");
	if (config.allowdcc && b_dccfwd->isChecked() && inet_addr(e_extip->text().latin1()) != INADDR_NONE
		&& atoi(e_extport->text().latin1()) > 1023) {
		config.extip = strdup(e_extip->text().latin1());
		config.extport = atoi(e_extport->text().latin1());
		}
	else {
		config.extip = strdup("0.0.0.0");
		config.extport = 0;
		}

	QStringList tmpservers;
	tmpservers = QStringList::split(";", e_server->text());
	for (i = 0; i < tmpservers.count(); i++)
		if (inet_addr(tmpservers[i].latin1()) == INADDR_NONE)
			break;
	if (!b_defserver->isChecked() && i == tmpservers.count())
		config.servers = QStringList::split(";", e_server->text());
	else
		config.servers = "";
	server_nr = 0;

	config.useproxy = b_useproxy->isChecked() && inet_addr(e_proxyserver->text().latin1()) != INADDR_NONE
		&& atoi(e_proxyport->text().latin1()) > 1023;
	if (config.useproxy) {
		config.proxyaddr = e_proxyserver->text();
		config.proxyport = (unsigned short)atoi(e_proxyport->text().latin1());
		config.proxyuser = e_proxyuser->text();
		if (config.proxyuser.length())
			config.proxypassword = e_proxypassword->text();
		else
			config.proxypassword.truncate(0);
		}
	else {
		config.proxyaddr.truncate(0);
		config.proxyport = 0;
		config.proxyuser.truncate(0);
		config.proxypassword.truncate(0);
		}

	/* and now, save it */
	saveKaduConfig();
	userlist.writeToFile();

	/* I odswiez okno Kadu */
	kadu->refreshGroupTabBar();
}

#include "config.moc"
