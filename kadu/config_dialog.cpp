/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qvaluelist.h>
#include <qcolordialog.h>
#include <qfontdatabase.h>
#include <qfiledialog.h>
#include <math.h>
#include <qvbox.h>
#include <qhgroupbox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qtimer.h>
#include <qgrid.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <qrect.h>
//
#include "kadu.h"
#include "misc.h"
#include "chat.h"
#include "emoticons.h"
#include "config_dialog.h"
#include "dock_widget.h"
#include "config_file.h"
#include "debug.h"
#include "sound.h"
#ifdef HAVE_OPENSSL
extern "C"
{
#include "simlite.h"
};
#endif
//

void loadKaduConfig(void) {  	
	/* first read our own config file... */
	kdebug("loadKaduConfig(): Reading config file...\n");
	ConfigFile * konf;
	konf = new ConfigFile(ggPath(QString("kadu.conf")));

	konf->setGroup("Global");
	config.uin = konf->readNumEntry("UIN",0);
	config.password = pwHash(konf->readEntry("Password",""));
	kdebug("Read user data: uin %d password :-P\n", config.uin);
	config.soundprog = konf->readEntry("SoundPlayer","");
	config.soundmsg = konf->readEntry("Message_sound","");
	config.soundvolctrl = konf->readBoolEntry("VolumeControl",false);
	config.soundvol = konf->readDoubleNumEntry("SoundVolume",1.0);
	config.soundchat = konf->readEntry("Chat_sound","");
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
	config.dccip = konf->readEntry("DccIP", "0.0.0.0");
	config.extip = strdup(konf->readEntry("ExternalIP", "0.0.0.0"));
	config.extport = konf->readNumEntry("ExternalPort", 0);
	config.servers = QStringList::split(";", konf->readEntry("Server", ""));
	config.default_servers = konf->readBoolEntry("isDefServers",true);
	config.default_port = konf->readNumEntry("DefaultPort", 8074);
	server_nr = 0;

	config.dock = konf->readBoolEntry("UseDocking",true);
	config.rundocked = konf->readBoolEntry("RunDocked", false);

	config.raise = konf->readBoolEntry("AutoRaise",false);
	config.privatestatus = konf->readBoolEntry("PrivateStatus", false);
	config.grouptabs = konf->readBoolEntry("DisplayGroupTabs", true);
	config.checkupdates = konf->readBoolEntry("CheckUpdates", true);
	config.addtodescription = konf->readBoolEntry("AddToDescription", false);
	config.trayhint = konf->readBoolEntry("TrayHint",true);
	config.hinterror = konf->readBoolEntry("HintError",true);
	config.hinttime = konf->readNumEntry("TimeoutHint",5);
	QRect def_rect(0,0,145,465);
	config.geometry = konf->readRectEntry("Geometry",&def_rect);
	QSize def_size(340,60);
	config.splitsize = konf->readSizeEntry("SplitSize",&def_size);
	config.showdesc = konf->readBoolEntry("ShowDesc",true);
	
	konf->setGroup("WWW");
	config.defaultwebbrowser = konf->readBoolEntry("DefaultWebBrowser", true);
	config.webbrowser = konf->readEntry("WebBrowser", "");
	
	konf->setGroup("SMS");
	config.smsbuildin = konf->readBoolEntry("BuiltInApp",true);
	config.smsapp = strdup(konf->readEntry("SmsApp",""));
	config.smscustomconf = konf->readBoolEntry("UseCustomString",false);
	config.smsconf = strdup(konf->readEntry("SmsString",""));

	konf->setGroup("Other");
	config.emoticons_style = (EmoticonsStyle)konf->readNumEntry("EmoticonsStyle",EMOTS_ANIMATED);
	emoticons.setEmoticonsTheme(konf->readEntry("EmoticonsTheme",""));
	config.autosend = konf->readBoolEntry("AutoSend",false);
	config.scrolldown = konf->readBoolEntry("ScrollDown",true);
	config.chatprune = konf->readBoolEntry("ChatPrune",false);
	config.chatprunelen = konf->readNumEntry("ChatPruneLen",20);
	config.chathistorycitation = konf->readNumEntry("ChatHistoryCitation", 10);
	config.msgacks = konf->readBoolEntry("MessageAcks", true);
	config.blinkchattitle = konf->readBoolEntry("BlinkChatTitle", true);
	config.hintalert = konf->readBoolEntry("HintAlert", false);
	config.ignoreanonusers = konf->readBoolEntry("IgnoreAnonymousUsers", false);
#ifdef HAVE_OPENSSL
	config.encryption = konf->readBoolEntry("Encryption", false);
//	config.keyslen = konf->readNumEntry("KeysLength", 1024);
#endif
	config.panelcontents = konf->readEntry("PanelContents", "");
	config.chatcontents = konf->readEntry("ChatContents", "");
	config.conferenceprefix = konf->readEntry("ConferencePrefix", "");
	config.conferencecontents = konf->readEntry("ConferenceContents", "");

	konf->setGroup("Notify");
	config.soundnotify = strdup(konf->readEntry("NotifySound", ""));
	config.notifyglobal = konf->readBoolEntry("NotifyStatusChange", false);
	config.notifyall = konf->readBoolEntry("NotifyAboutAll", false);
	config.notifydialog = konf->readBoolEntry("NotifyWithDialogBox", false);
	config.notifysound = konf->readBoolEntry("NotifyWithSound", false);
	config.notifyhint = konf->readBoolEntry("NotifyWithHint",true);
//	config.notifies = konf->readListEntry("NotifyUsers");

	konf->setGroup("Proxy");
	config.useproxy = konf->readBoolEntry("UseProxy", false);
	config.proxyaddr = konf->readEntry("ProxyHost", "");
	config.proxyport = konf->readNumEntry("ProxyPort", 0);
	config.proxyuser = pwHash(konf->readEntry("ProxyUser", ""));
	config.proxypassword = pwHash(konf->readEntry("ProxyPassword", ""));

	konf->setGroup("Colors");
	
	QColor def_color("#FFFFFF");
	config.colors.userboxBg = konf->readColorEntry("UserboxBgColor",&def_color);
	
	def_color.setNamedColor("#000000");
	config.colors.userboxFg = konf->readColorEntry("UserboxFgColor",&def_color);
	
	def_color.setNamedColor("#E0E0E0");
	config.colors.mychatBg = konf->readColorEntry("ChatMyBgColor",&def_color);
	
	def_color.setNamedColor("#F0F0F0");
	config.colors.usrchatBg = konf->readColorEntry("ChatUsrBgColor",&def_color);
	
	def_color.setNamedColor("#000000");
	config.colors.mychatText = konf->readColorEntry("ChatMyFontColor",&def_color);
	
	def_color.setNamedColor("#000000");
	config.colors.usrchatText = konf->readColorEntry("ChatUsrFontColor",&def_color);
	
	def_color.setNamedColor("#C0C0C0");
	config.colors.userboxDescBg = konf->readColorEntry("UserboxDescBgColor",&def_color);
	
	def_color.setNamedColor("#000000");
	config.colors.userboxDescText = konf->readColorEntry("UserboxDescTextColor",&def_color);

	def_color.setNamedColor("#F0F0F0");
	config.colors.trayhintBg = konf->readColorEntry("TrayHintBgColor",&def_color);

	def_color.setNamedColor("#000000");
	config.colors.trayhintText = konf->readColorEntry("TrayHintTextColor",&def_color);

	konf->setGroup("Fonts");
	QFontInfo info(a->font());
	QFont def_font(info.family(),info.pointSize());
	config.fonts.userbox = konf->readFontEntry("UserboxFont", &def_font);
	config.fonts.chat = konf->readFontEntry("ChatFont", &def_font);
	config.fonts.userboxDesc = konf->readFontEntry("UserboxDescFont", &def_font);
	config.fonts.trayhint = konf->readFontEntry("TrayHintFont", &def_font);

	/* no need for it anymore */
	delete konf;
}

void saveKaduConfig(void) {
	kdebug("saveKaduConfig(): Writing config files...\n");
	ConfigFile * konf;
	konf = new ConfigFile(ggPath(QString("kadu.conf")));

	konf->setGroup("Global");
	konf->writeEntry("UIN", int(config.uin));
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
	konf->writeEntry("isDefServers",config.default_servers);
	konf->writeEntry("Server", config.servers.join(";"));
	konf->writeEntry("DefaultPort",config.default_port);

	konf->writeEntry("UseDocking",config.dock);
	konf->writeEntry("RunDocked",config.rundocked);	

	konf->writeEntry("AutoRaise",config.raise);
	konf->writeEntry("PrivateStatus",config.privatestatus);
	konf->writeEntry("CheckUpdates", config.checkupdates);
	konf->writeEntry("DisplayGroupTabs",config.grouptabs);
	konf->writeEntry("AddToDescription",config.addtodescription);
	konf->writeEntry("TrayHint",config.trayhint);
	konf->writeEntry("HintError",config.hinterror);
	konf->writeEntry("TimeoutHint",config.hinttime);
	konf->writeEntry("ShowDesc",config.showdesc);

	if (config.savegeometry) {
		konf->writeEntry("SplitSize",config.splitsize);
		konf->writeEntry("Geometry",config.geometry);
	}

	konf->setGroup("WWW");
	konf->writeEntry("DefaultWebBrowser", config.defaultwebbrowser);	
	konf->writeEntry("WebBrowser", config.webbrowser);

	konf->setGroup("SMS");
	konf->writeEntry("BuiltInApp",config.smsbuildin);	
	konf->writeEntry("SmsApp",config.smsapp);
	konf->writeEntry("SmsString",config.smsconf);
	konf->writeEntry("UseCustomString",config.smscustomconf);

	konf->setGroup("Other");
	konf->writeEntry("EmoticonsStyle",config.emoticons_style);
	konf->writeEntry("EmoticonsTheme",config.emoticons_theme);
	konf->writeEntry("AutoSend",config.autosend);
	konf->writeEntry("ScrollDown",config.scrolldown);
	konf->writeEntry("ChatPrune",config.chatprune);
	konf->writeEntry("ChatPruneLen",config.chatprunelen);
	konf->writeEntry("ChatHistoryCitation", config.chathistorycitation);
	konf->writeEntry("MessageAcks", config.msgacks);
	konf->writeEntry("BlinkChatTitle", config.blinkchattitle);
	konf->writeEntry("HintAlert", config.hintalert);
	konf->writeEntry("IgnoreAnonymousUsers", config.ignoreanonusers);
#ifdef HAVE_OPENSSL
        konf->writeEntry("Encryption", config.encryption);
        //konf->writeEntry("KeysLength", config.keyslen);
#endif
	konf->writeEntry("PanelContents", config.panelcontents);
	konf->writeEntry("ChatContents", config.chatcontents);
	konf->writeEntry("ConferencePrefix", config.conferenceprefix);
	konf->writeEntry("ConferenceContents", config.conferencecontents);

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
	konf->writeEntry("NotifyWithHint", config.notifyhint);

	konf->setGroup("Colors");
	konf->writeEntry("UserboxBgColor", config.colors.userboxBg);
	konf->writeEntry("UserboxFgColor", config.colors.userboxFg);
	konf->writeEntry("ChatMyBgColor", config.colors.mychatBg);
	konf->writeEntry("ChatUsrBgColor", config.colors.usrchatBg);
	konf->writeEntry("ChatMyFontColor", config.colors.mychatText);
	konf->writeEntry("ChatUsrFontColor", config.colors.usrchatText);
	konf->writeEntry("UserboxDescBgColor", config.colors.userboxDescBg);
	konf->writeEntry("UserboxDescTextColor", config.colors.userboxDescText);
	konf->writeEntry("TrayHintBgColor", config.colors.trayhintBg);
	konf->writeEntry("TrayHintTextColor", config.colors.trayhintText);

	konf->setGroup("Fonts");
	konf->writeEntry("UserboxFont", config.fonts.userbox);
	konf->writeEntry("ChatFont", config.fonts.chat);
	konf->writeEntry("UserboxDescFont", config.fonts.userboxDesc);
	konf->writeEntry("TrayHintFont", config.fonts.trayhint);
	
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
	resize(480,500);
}

void ConfigDialog::setupTab1(void) {
	int i;

	QVBox *box = new QVBox(this);
	box->setMargin(5);

	QHGroupBox *userinfo = new QHGroupBox(box);
	userinfo->setTitle(i18n("User data"));

	QLabel *l_uin = new QLabel(i18n("Uin"),userinfo);

	e_uin = new QLineEdit(QString::number(config.uin),userinfo);

	QLabel *l_password = new QLabel(i18n("Password"),userinfo);

	e_password = new QLineEdit(config.password,userinfo);
	e_password->setEchoMode(QLineEdit::Password);

	QLabel *l_nick = new QLabel(i18n("Nick"),userinfo);
	
	e_nick = new QLineEdit(config.nick,userinfo);
	
	/* SMS begin */
	QVGroupBox *smsvgrp = new QVGroupBox(box);
	smsvgrp->setTitle(i18n("SMS options"));

	b_smsbuildin = new QCheckBox(i18n("Use built-in SMS application"),smsvgrp);

	smshbox1 = new QHBox(smsvgrp);
	smshbox1->setSpacing(5);
	QLabel *l_smsapp = new QLabel(i18n("Custom SMS application"),smshbox1);
	e_smsapp = new QLineEdit(config.smsapp,smshbox1);

	smshbox2 = new QHBox(smsvgrp);
	smshbox2->setSpacing(5);
	b_smscustomconf = new QCheckBox(i18n("SMS custom string"),smshbox2);
	QToolTip::add(b_smscustomconf,i18n("Check this box if your sms application doesn't understand arguments: number \"message\"\nArguments should be separated with spaces. %n argument is converted to number, %m to message"));

	e_smsconf = new QLineEdit(config.smsconf,smshbox2);

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

	b_autoaway = new QCheckBox(i18n("Enable autoaway"),box);

	QHGroupBox *awygrp = new QHGroupBox(box);
	QLabel *l_autoaway = new QLabel(i18n("Set status to away after "),awygrp);
	e_autoawaytime = new QLineEdit(QString::number(config.autoawaytime),awygrp);
	QLabel *l_autoaway2 = new QLabel(i18n(" seconds"),awygrp);

	if (config.autoaway)
		b_autoaway->setChecked(true);
	else
		awygrp->setEnabled(false);

	QObject::connect(b_autoaway, SIGNAL(toggled(bool)), awygrp, SLOT(setEnabled(bool)));

	b_trayhint = new QCheckBox(i18n("Enable tray hints"),box);

	QVGroupBox *hintgrp = new QVGroupBox(box);
	QHBox *box_time = new QHBox(hintgrp);
	QLabel *l_trayhint = new QLabel(i18n("Tray hints timeout "),box_time);

	e_hinttime = new QLineEdit(QString::number(config.hinttime),box_time);
	QLabel *l_trayhint2 = new QLabel(i18n(" seconds"),box_time);

	b_hinterror = new QCheckBox(i18n("Show connection errors in tray hints"),hintgrp);
	b_hinterror->setChecked(config.hinterror);

	if (config.trayhint)
		b_trayhint->setChecked(true);
	else
		hintgrp->setEnabled(false);

	QObject::connect(b_trayhint, SIGNAL(toggled(bool)), hintgrp, SLOT(setEnabled(bool)));

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

	b_logging = new QCheckBox(i18n("Log messages"),grid);
	if (config.logmessages)
		b_logging->setChecked(true);

	b_geometry = new QCheckBox(i18n("Restore window geometry"),grid);
	if (config.savegeometry)
		b_geometry->setChecked(true);

	b_dock = new QCheckBox(i18n("Enable dock icon"),grid);
	if (config.dock)
		b_dock->setChecked(true);
	QObject::connect(b_dock, SIGNAL(toggled(bool)), this, SLOT(ifDockEnabled(bool)));

	b_private = new QCheckBox(i18n("Private status"),grid);
	if (config.privatestatus)
		b_private->setChecked(true);

	b_rdocked = new QCheckBox(i18n("Start docked"),grid);
	if (config.rundocked)
		b_rdocked->setChecked(true);

	b_grptabs = new QCheckBox(i18n("Display group tabs"),grid);
	if (config.grouptabs)
		b_grptabs->setChecked(true);

	b_checkupdates = new QCheckBox(i18n("Check for updates"),grid);
	if (config.checkupdates)
		b_checkupdates->setChecked(true);
		
	b_addtodescription = new QCheckBox(i18n("Add to description"),grid);
	if (config.addtodescription)
		b_addtodescription->setChecked(true);		
	QToolTip::add(b_addtodescription,i18n("If a file description in gg settings directory is present, its contents will be added\nto the status description and then the file will be deleted."));
	
	b_showdesc = new QCheckBox(i18n("Show userbox-desc."),grid);
	b_showdesc->setChecked(config.showdesc);

	addTab(box, i18n("General"));
}

void ConfigDialog::setupTab2(void) {
	QVBox *box2 = new QVBox(this);
	box2->setMargin(5);

	b_playsound = new QCheckBox(i18n("Play sounds"),box2);

	b_playartsdsp = new QCheckBox(i18n("Play sounds using aRts! server"),box2);
	if (config.playartsdsp)
		b_playartsdsp->setChecked(true);

	if (config.playsound)
		b_playsound->setChecked(true);
	else
		b_playartsdsp->setEnabled(false);

	QObject::connect(b_playsound, SIGNAL(toggled(bool)), b_playartsdsp, SLOT(setEnabled(bool)));

	QHGroupBox *sndgroup = new QHGroupBox(box2);
	sndgroup->setTitle(i18n("Sound player"));
	e_soundprog = new QLineEdit(config.soundprog,sndgroup);

	QPushButton *f_soundprog = new QPushButton(sndgroup);
	f_soundprog->setPixmap(loadIcon("fileopen.png"));
	connect(f_soundprog, SIGNAL(clicked()), this, SLOT(choosePlayerFile()));

	b_soundvolctrl = new QCheckBox(i18n("Enable volume control (player must support it)"),box2);

	QHBox *volbox = new QHBox(box2);
	volbox->setSpacing(5);
	QLabel *l_vol = new QLabel(i18n("Volume"),volbox);
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

	e_msgfile = new QLineEdit(config.soundmsg,msggroup);

	QPixmap icon = loadIcon("fileopen.png");

	QPushButton *msgsnd = new QPushButton(msggroup);
	msgsnd->setPixmap(icon);
	connect(msgsnd, SIGNAL(clicked()), this, SLOT(chooseMsgFile()));

	QPushButton *testsoundmsg = new QPushButton(i18n("Test"),msggroup);
	connect(testsoundmsg, SIGNAL(clicked()), this, SLOT(chooseMsgTest()));

	b_playchat = new QCheckBox(i18n("Play sounds from a person whilst chatting"),box2);
	b_playchatinvisible = new QCheckBox(i18n("Play chat sounds only when window is invisible"),box2);
	QObject::connect(b_playchat, SIGNAL(toggled(bool)), b_playchatinvisible, SLOT(setEnabled(bool)));

	if (config.playsoundchat)
		b_playchat->setChecked(true);
	else
		b_playchatinvisible->setEnabled(false);

	if (config.playsoundchatinvisible)
		b_playchatinvisible->setChecked(true);

	QHGroupBox *chatgroup = new QHGroupBox(box2);
	chatgroup->setTitle(i18n("Chat sound"));

	e_chatfile = new QLineEdit(config.soundchat,chatgroup);

	QPushButton *chatsnd = new QPushButton(chatgroup);
	chatsnd->setPixmap(icon);
	connect(chatsnd, SIGNAL(clicked()), this, SLOT(chooseChatFile()));

	QPushButton *testsoundchat = new QPushButton(i18n("Test"),chatgroup);
	connect(testsoundchat, SIGNAL(clicked()), this, SLOT(chooseChatTest()));

	QObject::connect(b_playsound, SIGNAL(toggled(bool)), testsoundmsg, SLOT(setEnabled(bool)));
	QObject::connect(b_playsound, SIGNAL(toggled(bool)), testsoundchat, SLOT(setEnabled(bool)));

	addTab(box2, i18n("Sounds"));
}

void ConfigDialog::setupTab3(void) {
	QVBox *box3 = new QVBox(this);
	box3->setMargin(5);	

	QVGroupBox *emogroup = new QVGroupBox(box3);
	emogroup->setTitle(i18n("Emoticons"));

	QHBox* emostyle_box = new QHBox(emogroup);
	QLabel* l_emoticons_style=new QLabel(i18n("Emoticons"),emostyle_box);
	cb_emoticons_style=new QComboBox(emostyle_box);
	cb_emoticons_style->insertItem(i18n("None"));
	cb_emoticons_style->insertItem(i18n("Static"));
	cb_emoticons_style->insertItem(i18n("Animated"));
	cb_emoticons_style->setCurrentItem(config.emoticons_style);

	emotheme_box = new QHBox(emogroup);
	QLabel* l_emoticons_theme=new QLabel(i18n("Emoticons theme"),emotheme_box);
	cb_emoticons_theme=new QComboBox(emotheme_box);
	cb_emoticons_theme->insertStringList(emoticons.themes());
	cb_emoticons_theme->setCurrentText(config.emoticons_theme);

	if (config.emoticons_style==EMOTS_NONE)
		emotheme_box->setEnabled(false);
	QObject::connect(cb_emoticons_style,SIGNAL(activated(int)), this, SLOT(chooseEmoticonsStyle(int)));

	/* WWW begin */
	QVGroupBox *webvgrp = new QVGroupBox(box3);
	webvgrp->setTitle(i18n("WWW options"));

	b_defwebbrowser = new QCheckBox(i18n("Use default Web browser"),webvgrp);

	webhbox1 = new QHBox(webvgrp);
	webhbox1->setSpacing(5);
	QLabel *l_webbrowser = new QLabel(i18n("Custom Web browser"),webhbox1);
	e_webbrowser = new QLineEdit(webhbox1);
	e_webbrowser->setText(config.webbrowser);

	if (config.defaultwebbrowser) {
		b_defwebbrowser->setChecked(true);
		webhbox1->setEnabled(false);
		}
		
	QObject::connect(b_defwebbrowser, SIGNAL(toggled(bool)), this, SLOT(onDefWebBrowserToogle(bool)));
	/* WWW end */

	b_chatprune = new QCheckBox(i18n("Automatically prune chat messages"),box3);

	QHGroupBox *prunebox = new QHGroupBox(box3);
	prunebox->setTitle(i18n("Message pruning"));

	QLabel *l_chatprunedsc1 = new QLabel(i18n("Reduce the number of visible messages to"),prunebox);

	e_chatprunelen = new QLineEdit(QString::number(config.chatprunelen),prunebox);

	if (config.chatprune)
		b_chatprune->setChecked(true);
	else
		prunebox->setEnabled(false);

	QObject::connect(b_chatprune, SIGNAL(toggled(bool)), prunebox, SLOT(setEnabled(bool)));

#ifdef HAVE_OPENSSL
	const char* keyslens[] = { "128", "256", "512", "768", "1024", 0 };

	b_encryption = new QCheckBox(i18n("Use encryption"),box3);

	QHGroupBox *encryptbox = new QHGroupBox(box3);
	encryptbox->setTitle(i18n("Encryption properties"));
	QLabel *l_lenencrypt = new QLabel(i18n("Keys length"),encryptbox);

	cb_keyslen = new QComboBox(encryptbox);
	cb_keyslen->insertStrList(keyslens);
	//cb_keyslen->setCurrentText(QString::number(config.keyslen));
	// Protokó³ nie obs³uguje narazie innych d³ugo¶ci klucza
	cb_keyslen->setCurrentText("1024");
	cb_keyslen->setEnabled(false);

	QPushButton *pb_genkeys = new QPushButton(i18n("Generate keys"),encryptbox);

        if (config.encryption) {
		b_encryption->setChecked(true);
	} else {
		encryptbox->setEnabled(false);
	}

	QObject::connect(b_encryption, SIGNAL(toggled(bool)), encryptbox, SLOT(setEnabled(bool)));
	QObject::connect(pb_genkeys, SIGNAL(clicked()), this, SLOT(generateMyKeys()));
#endif

	b_scrolldown = new QCheckBox(i18n("Scroll chat window downward, not upward"),box3);
	if (config.scrolldown)
		b_scrolldown->setChecked(true);

	b_autosend = new QCheckBox(i18n("\"Enter\" key in chat sends message by default"),box3);
	if (config.autosend)
		b_autosend->setChecked(true);
	
	b_msgacks = new QCheckBox(i18n("Message acknowledgements (wait for delivery)"),box3);
	b_msgacks->setChecked(config.msgacks);

	b_blinkchattitle = new QCheckBox(i18n("Flash chat title on new message"),box3);
	b_blinkchattitle->setChecked(config.blinkchattitle);

	b_ignoreanonusers = new QCheckBox(i18n("Ignore messages from anonymous users"),box3);
	b_ignoreanonusers->setChecked(config.ignoreanonusers);

	b_hintalert = new QCheckBox(i18n("Show tray hint on new message"),box3);
	if (config.trayhint)
		b_hintalert->setChecked(config.hintalert);
	else {
		b_hintalert->setChecked(false);
		b_hintalert->setEnabled(false);
		}


	QObject::connect(b_trayhint, SIGNAL(toggled(bool)), b_hintalert, SLOT(setEnabled(bool)));

	addTab(box3, i18n("Chat"));
}

void ConfigDialog::setupTab4(void) {
	int i;
//	uin_t uin;

	QVBox *box4 = new QVBox(this);
	box4->setMargin(2);	
	
	b_notifyglobal = new QCheckBox(i18n("Notify when users become available"),box4);

	b_notifyall = new QCheckBox(i18n("Notify about all users"),box4);

	/* two nice panes */
	panebox = new QHBox(box4);

	QVBox *vbox1 = new QVBox(panebox);
	QLabel *_l1 = new QLabel(i18n("Available"),vbox1);
	e_availusers = new QListBox(vbox1);

	QVBox *vbox2 = new QVBox(panebox);
	QPushButton *_goRight = new QPushButton (vbox2);
	_goRight->setPixmap(loadIcon("forward.png"));

	QPushButton *_goLeft = new QPushButton (vbox2);
	_goLeft->setPixmap(loadIcon("back.png"));

	QVBox *vbox3 = new QVBox(panebox);
	QLabel *_l2 = new QLabel(i18n("Tracked"),vbox3);
	e_notifies = new QListBox(vbox3);

	i = 0;
	while (i < userlist.count()) {
		if (userlist[i].uin)
			if (!userlist[i].notify)
				e_availusers->insertItem(userlist[i].altnick);
			else
				e_notifies->insertItem(userlist[i].altnick);
		i++;
		}

	e_availusers->sort();
	e_notifies->sort();

	QObject::connect(_goRight, SIGNAL(clicked()), this, SLOT(_Right()));
	QObject::connect(_goLeft, SIGNAL(clicked()), this, SLOT(_Left()));

	// end of two panes

	notifybox = new QVGroupBox(box4);
	notifybox->setTitle(i18n("Notify options"));
	notifybox->setMargin(2);

	b_notifysound = new QCheckBox(i18n("Notify by sound"),notifybox);

	QHGroupBox *soundbox = new QHGroupBox(notifybox);
	soundbox->setTitle(i18n("Notify sound"));

	e_soundnotify = new QLineEdit(config.soundnotify,soundbox);

	QPushButton *nsndget = new QPushButton(soundbox);
	nsndget->setPixmap(loadIcon("fileopen.png"));
	connect(nsndget, SIGNAL(clicked()), this, SLOT(chooseNotifyFile()));

	QPushButton *testsoundnotify = new QPushButton(i18n("Test"),soundbox);
	connect(testsoundnotify, SIGNAL(clicked()), this, SLOT(chooseNotifyTest()));

	b_notifydialog = new QCheckBox(i18n("Notify by dialog box"),notifybox);

	b_notifyhint = new QCheckBox(i18n("Notify by hint"),notifybox);
	b_notifyhint->setChecked(config.notifyhint);

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

	QObject::connect(b_trayhint, SIGNAL(toggled(bool)), b_notifyhint, SLOT(setEnabled(bool)));

	addTab(box4, i18n("Users"));
}

void ConfigDialog::setupTab5(void) {
//	int i;

	QVBox *box5 = new QVBox(this);
	box5->setMargin(2);	

	b_dccenabled = new QCheckBox(i18n("DCC enabled"),box5);
	b_dccenabled->setChecked(config.allowdcc);

	b_dccip = new QCheckBox(i18n("DCC IP autodetection"),box5);
	b_dccip->setChecked(config.dccip == "0.0.0.0");

	g_dccip = new QVGroupBox(box5);
	g_dccip->setTitle(i18n("DCC IP"));
	g_dccip->setEnabled(!b_dccip->isChecked());

	QHBox *dccipbox = new QHBox(g_dccip);
	dccipbox->setSpacing(5);
	QLabel *l4 = new QLabel(i18n("IP address:"),dccipbox);
	e_dccip = new QLineEdit(dccipbox);
	if (g_dccip->isEnabled())
		e_dccip->setText(config.dccip);

	b_dccfwd = new QCheckBox(i18n("DCC forwarding enabled"),box5);
	b_dccfwd->setEnabled(config.allowdcc);

	g_fwdprop = new QVGroupBox(box5);
	g_fwdprop->setTitle(i18n("DCC forwarding properties"));
	g_fwdprop->setMargin(2);

	QHBox *extipbox = new QHBox(g_fwdprop);
	extipbox->setSpacing(5);
	QLabel *l1 = new QLabel(i18n("External IP address:"),extipbox);
	e_extip = new QLineEdit(extipbox);

	QHBox *extportbox = new QHBox(g_fwdprop);
	extportbox->setSpacing(5);
	QLabel *l2 = new QLabel(i18n("External TCP port:"),extportbox);
	e_extport = new QLineEdit(extportbox);

	QVGroupBox *g_server = new QVGroupBox(box5);
	g_server->setTitle(i18n("Servers properties"));
	g_server->setMargin(2);
//	g_server->setEnabled(!config.default_servers && config.servers.count() && inet_addr(config.servers[0].latin1()) != INADDR_NONE);

	b_defserver = new QCheckBox(i18n("Use default servers"),g_server);
	b_defserver->setChecked(config.default_servers);
	       
	serverbox = new QHBox(g_server);
	serverbox->setSpacing(5);
	serverbox->setEnabled(!config.default_servers && config.servers.count() && inet_addr(config.servers[0].latin1()) != INADDR_NONE);
	
	QLabel *l3 = new QLabel(i18n("IP addresses:"),serverbox);
	e_server = new QLineEdit(serverbox);
	e_server->setText(config.servers.join(";"));
	
	QHBox *portserverbox = new QHBox(g_server);
	serverbox->setSpacing(5);
	QLabel *lserverport = new QLabel(i18n("Default port to connect servers"),portserverbox);
	
	cb_portselect = new QComboBox(portserverbox);
        cb_portselect->insertItem("8074");
	cb_portselect->insertItem("443");
        cb_portselect->setCurrentText(QString::number(config.default_port));
						
	b_useproxy = new QCheckBox(i18n("Use proxy server"),box5);

	g_proxy = new QVGroupBox(box5);
	g_proxy->setTitle(i18n("Proxy server"));

	QHBox *proxyserverbox = new QHBox(g_proxy);
	proxyserverbox->setSpacing(5);
	QLabel *l5 = new QLabel(i18n("IP address:"),proxyserverbox);
	e_proxyserver = new QLineEdit(proxyserverbox);
	QLabel *l6 = new QLabel(i18n("Port:"),proxyserverbox);
	e_proxyport = new QLineEdit(proxyserverbox);

	QHBox *proxyuserbox = new QHBox(g_proxy);
	proxyuserbox->setSpacing(5);
	QLabel *l7 = new QLabel(i18n("Username:"),proxyuserbox);
	e_proxyuser = new QLineEdit(proxyuserbox);
	QLabel *l8 = new QLabel(i18n("Password:"),proxyuserbox);
	e_proxypassword = new QLineEdit(proxyuserbox);
	e_proxypassword->setEchoMode(QLineEdit::Password);

	g_fwdprop->setEnabled(inet_addr(config.extip) && config.extport > 1023);    
	if (g_fwdprop->isEnabled()) {
		b_dccfwd->setChecked(true);
		e_extip->setText(config.extip);
		e_extport->setText(QString::number(config.extport));	
		}
//	g_server->setEnabled(!config.default_servers && config.servers.count() && inet_addr(config.servers[0].latin1()) != INADDR_NONE);
//	if (!g_server->isEnabled())
//		b_defserver->setChecked(true);
//	else
//		e_server->setText(config.servers.join(";"));
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

	QPixmap pm_buttoncolor(35,10);
	QFontDatabase fdb;
	QValueList<int> vl;
	
//Combobox chata
	vl_chatcolor.append(config.colors.mychatBg);
	vl_chatcolor.append(config.colors.mychatText);
	vl_chatcolor.append(config.colors.usrchatBg);
	vl_chatcolor.append(config.colors.usrchatText);
//Combobox userboxu
	vl_userboxcolor.append(config.colors.userboxBg);
	vl_userboxcolor.append(config.colors.userboxDescBg);
	vl_userboxcolor.append(config.colors.userboxFg);
	vl_userboxcolor.append(config.colors.userboxDescText);
//Combobox fontow chata i userboxu
	vl_chatfont.append(config.fonts.chat);
	vl_userboxfont.append(config.fonts.userbox);
	vl_userboxfont.append(config.fonts.userboxDesc);
//Comboboxy innych - kolory
	vl_othercolor.append(config.colors.trayhintBg);
	vl_othercolor.append(config.colors.trayhintText);
//Comboboxy innych - fonty
	vl_otherfont.append(config.fonts.trayhint);

	QVBox *box6 = new QVBox(this);
	box6->setMargin(2);

	QVGroupBox *chatprop = new QVGroupBox(box6);
	chatprop->setTitle(i18n("Chat properties"));

	QHBox *chatselectcolor = new QHBox(chatprop);
	chatselectcolor->setSpacing(5);
	
	cb_chatselect = new QComboBox(chatselectcolor);
	cb_chatselect->insertItem(i18n("Your background color"));
	cb_chatselect->insertItem(i18n("Your font color"));
	cb_chatselect->insertItem(i18n("User background color"));
	cb_chatselect->insertItem(i18n("User font color"));
	cb_chatselect->insertItem(i18n("Font in chat window"));
	cb_chatselect->setCurrentItem(0);
	
	connect(cb_chatselect, SIGNAL(activated(int)), this, SLOT(chooseChatSelect(int)));
	
	e_chatcolor = new QLineEdit(vl_chatcolor[0].name(),chatselectcolor);
	
	connect(e_chatcolor, SIGNAL(textChanged(const QString&)), this, SLOT(chooseChatLine(const QString&)));
	
	pb_chatcolor = new QPushButton(chatselectcolor);
	pm_buttoncolor.fill(vl_chatcolor[0]);
	pb_chatcolor->setPixmap(pm_buttoncolor);
	connect(pb_chatcolor, SIGNAL(clicked()), this, SLOT(chooseChatColorGet()));
	
	chatselectfont = new QHBox(chatprop);
	chatselectfont->setSpacing(5);
	
	QLabel *l_chatfont = new QLabel(i18n("Font"),chatselectfont);
	
	cb_chatfont = new QComboBox(chatselectfont);
	cb_chatfont->insertStringList(fdb.families());
	cb_chatfont->setCurrentText(vl_chatfont[0].family());
	connect(cb_chatfont, SIGNAL(activated(int)), this, SLOT(chooseChatFontGet(int)));
	
	QLabel *l_chatfontsize = new QLabel(i18n("Font size"),chatselectfont);

	cb_chatfontsize = new QComboBox(chatselectfont);

	vl = fdb.pointSizes(vl_chatfont[0].family(),"Normal");
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
	cb_chatfontsize->insertItem(QString::number(*points));
	cb_chatfontsize->setCurrentText(QString::number(vl_chatfont[0].pointSize()));

	connect(cb_chatfontsize, SIGNAL(activated(int)), this, SLOT(chooseChatFontSizeGet(int)));

	chatselectfont->hide();
	
	QVGroupBox *userboxprop = new QVGroupBox(box6);
	userboxprop->setTitle(i18n("Userbox properties"));

	QHBox *userboxselectcolor = new QHBox(userboxprop);
	userboxselectcolor->setSpacing(5);

	cb_userboxselect = new QComboBox(userboxselectcolor);
	cb_userboxselect->insertItem(i18n("Userbox background color"));
	cb_userboxselect->insertItem(i18n("Desc. background color"));
	cb_userboxselect->insertItem(i18n("Font in userbox window"));
	cb_userboxselect->insertItem(i18n("Font in desc."));
	cb_userboxselect->setCurrentItem(0);
	
	connect(cb_userboxselect, SIGNAL(activated(int)), this, SLOT(chooseUserboxSelect(int)));

	e_userboxcolor = new QLineEdit(vl_userboxcolor[0].name(),userboxselectcolor);

	connect(e_userboxcolor, SIGNAL(textChanged(const QString&)), this, SLOT(chooseUserboxLine(const QString&)));

	pb_userboxcolor = new QPushButton(userboxselectcolor);
	pm_buttoncolor.fill(vl_userboxcolor[0]);
	pb_userboxcolor->setPixmap(pm_buttoncolor);
	connect(pb_userboxcolor, SIGNAL(clicked()), this, SLOT(chooseUserboxColorGet()));
	
	userboxselectfont = new QHBox(userboxprop);
	userboxselectfont->setSpacing(5);

	QLabel *l_userboxfont = new QLabel(i18n("Font"),userboxselectfont);

	cb_userboxfont = new QComboBox(userboxselectfont);
	cb_userboxfont->insertStringList(fdb.families());
	cb_userboxfont->setCurrentText(vl_userboxfont[0].family());
	connect(cb_userboxfont, SIGNAL(activated(int)), this, SLOT(chooseUserboxFontGet(int)));

	QLabel *l_userboxfontsize = new QLabel(i18n("Font size"),userboxselectfont);

	cb_userboxfontsize = new QComboBox(userboxselectfont);

	vl = fdb.pointSizes(vl_userboxfont[0].family(),"Normal");
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
	cb_userboxfontsize->insertItem(QString::number(*points));
	cb_userboxfontsize->setCurrentText(QString::number(vl_userboxfont[0].pointSize()));

	connect(cb_userboxfontsize, SIGNAL(activated(int)), this, SLOT(chooseUserboxFontSizeGet(int)));

	userboxselectfont->hide();

	QVGroupBox *otherprop = new QVGroupBox(box6);
	otherprop->setTitle(i18n("Other properties"));

	QHBox *otherselectcolor = new QHBox(otherprop);
	otherselectcolor->setSpacing(5);

	cb_otherselect = new QComboBox(otherselectcolor);
	cb_otherselect->insertItem(i18n("Tray Hint background color"));
	cb_otherselect->insertItem(i18n("Font in Tray Hint"));
	cb_otherselect->setCurrentItem(0);
	
	connect(cb_otherselect, SIGNAL(activated(int)), this, SLOT(chooseOtherSelect(int)));

	e_othercolor = new QLineEdit(vl_othercolor[0].name(),otherselectcolor);

	connect(e_othercolor, SIGNAL(textChanged(const QString&)), this, SLOT(chooseOtherLine(const QString&)));

	pb_othercolor = new QPushButton(otherselectcolor);
	pm_buttoncolor.fill(vl_othercolor[0]);
	pb_othercolor->setPixmap(pm_buttoncolor);
	connect(pb_othercolor, SIGNAL(clicked()), this, SLOT(chooseOtherColorGet()));
	
	otherselectfont = new QHBox(otherprop);
	otherselectfont->setSpacing(5);

	QLabel *l_otherfont = new QLabel(i18n("Font"),otherselectfont);

	cb_otherfont = new QComboBox(otherselectfont);
	cb_otherfont->insertStringList(fdb.families());
	cb_otherfont->setCurrentText(vl_otherfont[0].family());
	connect(cb_otherfont, SIGNAL(activated(int)), this, SLOT(chooseOtherFontGet(int)));

	QLabel *l_otherfontsize = new QLabel(i18n("Font size"),otherselectfont);

	cb_otherfontsize = new QComboBox(otherselectfont);

	vl = fdb.pointSizes(vl_otherfont[0].family(),"Normal");
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
		cb_otherfontsize->insertItem(QString::number(*points));
	cb_otherfontsize->setCurrentText(QString::number(vl_otherfont[0].pointSize()));

	connect(cb_otherfontsize, SIGNAL(activated(int)), this, SLOT(chooseOtherFontSizeGet(int)));
	otherselectfont->hide();

	QVGroupBox *contentsprop = new QVGroupBox(box6);
	QToolTip::add(contentsprop,i18n("%s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - frist name\n%r = surname, %m - mobile, %u - uin, %g - group, %o - If person doesn't have us in userlist\nIf is empty - default setting"));

	QHBox *panelbox = new QHBox(contentsprop);
	panelbox->setSpacing(5);
	
	QLabel *l_panel = new QLabel(i18n("Panel information contents"), panelbox);
	e_panelcontents = new QLineEdit(config.panelcontents, panelbox);

	QHBox *chatconbox = new QHBox(contentsprop);
	chatconbox->setSpacing(5);

	QLabel *l_chatcon = new QLabel(i18n("Title chat window contents"), chatconbox);
	e_chatcontents = new QLineEdit(config.chatcontents, chatconbox);

	QHBox *confprefixbox = new QHBox(contentsprop);
	confprefixbox->setSpacing(5);

	QLabel *l_confprefixcon = new QLabel(i18n("Title conference window prefix contents"), confprefixbox);
	e_conferenceprefix = new QLineEdit(config.conferenceprefix, confprefixbox);

	QHBox *confconbox = new QHBox(contentsprop);
	confconbox->setSpacing(5);

	QLabel *l_confcon = new QLabel(i18n("Title conference window contents"), confconbox);
	e_conferencecontents = new QLineEdit(config.conferencecontents, confconbox);

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

void ConfigDialog::ifDockEnabled(bool toggled) {
	if (!toggled) {
		b_trayhint->setChecked(false);
		b_hinterror->setChecked(false);
		b_notifyhint->setChecked(false);
		b_notifyhint->setEnabled(false);
		b_trayhint->setEnabled(false);
		}
	else
		b_trayhint->setEnabled(true);
		b_notifyhint->setEnabled(true);
}

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
	serverbox->setEnabled(!toggled);
}

void ConfigDialog::ifUseProxyEnabled(bool toggled) {
	g_proxy->setEnabled(toggled);
}

void ConfigDialog::_Left(void) {
	if (e_notifies->currentItem() != -1) {
		e_availusers->insertItem(e_notifies->text(e_notifies->currentItem()));
		e_notifies->removeItem(e_notifies->currentItem());
		e_availusers->sort();
		}
}

void ConfigDialog::_Right(void) {
	if (e_availusers->currentItem() != -1) {
		e_notifies->insertItem(e_availusers->text(e_availusers->currentItem()));
		e_availusers->removeItem(e_availusers->currentItem());
		e_notifies->sort();
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

void ConfigDialog::chooseChatSelect(int index) {
	if (index <= 3) {
		QPixmap pm(35,10);
		chatselectfont->hide();
		e_chatcolor->setText(vl_chatcolor[index].name());
		pm.fill(vl_chatcolor[index]);
		pb_chatcolor->setPixmap(pm);
		e_chatcolor->setEnabled(true);
		pb_chatcolor->setEnabled(true);
		}
	else {
		QFontDatabase fdb;
		QValueList<int> vl;
		chatselectfont->show();
		cb_chatfont->setCurrentText(vl_chatfont[index-4].family());
		vl = fdb.pointSizes(cb_chatfont->currentText(),"Normal");
		cb_chatfontsize->clear();
		for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
			cb_chatfontsize->insertItem(QString::number(*points));
		cb_chatfontsize->setCurrentText(QString::number(vl_chatfont[index-4].pointSize()));
		e_chatcolor->setEnabled(false);
		pb_chatcolor->setEnabled(false);
	}
}

void ConfigDialog::chooseUserboxSelect(int index) {
	QPixmap pm(35,10);

	e_userboxcolor->setText(vl_userboxcolor[index].name());
	pm.fill(vl_userboxcolor[index]);
	pb_userboxcolor->setPixmap(pm);

	if (index >= 2) {
		QFontDatabase fdb;
		QValueList<int> vl;
		userboxselectfont->show();
		cb_userboxfont->setCurrentText(vl_userboxfont[index-2].family());
		vl = fdb.pointSizes(cb_userboxfont->currentText(),"Normal");
		cb_userboxfontsize->clear();
		for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
			cb_userboxfontsize->insertItem(QString::number(*points));
		cb_userboxfontsize->setCurrentText(QString::number(vl_userboxfont[index-2].pointSize()));
		}
	else
		userboxselectfont->hide();
}

void ConfigDialog::chooseOtherSelect(int index) {
	QPixmap pm(35,10);

	e_othercolor->setText(vl_othercolor[index].name());
	pm.fill(vl_othercolor[index]);
	pb_othercolor->setPixmap(pm);

	if (index >= 1) {
		QFontDatabase fdb;
		QValueList<int> vl;
		otherselectfont->show();
		cb_otherfont->setCurrentText(vl_otherfont[index-1].family());
		vl = fdb.pointSizes(cb_otherfont->currentText(),"Normal");
		cb_otherfontsize->clear();
		for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
			cb_otherfontsize->insertItem(QString::number(*points));
		cb_otherfontsize->setCurrentText(QString::number(vl_otherfont[index-1].pointSize()));
		}
	else
		otherselectfont->hide();
}

void ConfigDialog::chooseChatColorGet(void) {
	QColor color = QColorDialog::getColor(QColor(e_chatcolor->text()), this, i18n("Color dialog"));
	if ( color.isValid() ) {
		e_chatcolor->setText(color.name());
		QPixmap pm(35,10);
		pm.fill(QColor(color.name()));
		pb_chatcolor->setPixmap(pm);
		vl_chatcolor[cb_chatselect->currentItem()] = color;
		}
}

void ConfigDialog::chooseUserboxColorGet(void) {
	QColor color = QColorDialog::getColor(QColor(e_userboxcolor->text()), this, i18n("Color dialog"));
	if ( color.isValid() ) {
		e_userboxcolor->setText(color.name());
		QPixmap pm(35,10);
		pm.fill(QColor(color.name()));
		pb_userboxcolor->setPixmap(pm);
		vl_userboxcolor[cb_userboxselect->currentItem()] = color;
		}
}

void ConfigDialog::chooseOtherColorGet(void) {
	QColor color = QColorDialog::getColor(QColor(e_othercolor->text()), this, i18n("Color dialog"));
	if ( color.isValid() ) {
		e_othercolor->setText(color.name());
		QPixmap pm(35,10);
		pm.fill(QColor(color.name()));
		pb_othercolor->setPixmap(pm);
		vl_othercolor[cb_otherselect->currentItem()] = color;
		}
}

void ConfigDialog::chooseChatFontGet(int index) {
	QFontDatabase fdb;
	QValueList<int> vl;
	vl = fdb.pointSizes(cb_chatfont->text(index),"Normal");
	cb_chatfontsize->clear();
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
		cb_chatfontsize->insertItem(QString::number(*points));
	if (cb_chatfontsize->count() > 0) {
		cb_chatfontsize->setCurrentItem(0);
		vl_chatfont[cb_chatselect->currentItem()-4] = QFont(cb_chatfont->text(index),atoi(cb_chatfontsize->currentText()));
	}
}

void ConfigDialog::chooseUserboxFontGet(int index) {
	QFontDatabase fdb;
	QValueList<int> vl;
	vl = fdb.pointSizes(cb_userboxfont->text(index),"Normal");
	cb_userboxfontsize->clear();
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
		cb_userboxfontsize->insertItem(QString::number(*points));
	if (cb_userboxfontsize->count() > 0) {
		cb_userboxfontsize->setCurrentItem(0);
		vl_userboxfont[cb_userboxselect->currentItem()-2] = QFont(cb_userboxfont->text(index),atoi(cb_userboxfontsize->currentText()));
	}
}

void ConfigDialog::chooseOtherFontGet(int index) {
	QFontDatabase fdb;
	QValueList<int> vl;
	vl = fdb.pointSizes(cb_otherfont->text(index),"Normal");
	cb_otherfontsize->clear();
	for (QValueList<int>::Iterator points = vl.begin(); points != vl.end(); ++points)
		cb_otherfontsize->insertItem(QString::number(*points));
	if (cb_otherfontsize->count() > 0) {
		cb_otherfontsize->setCurrentItem(0);
		vl_otherfont[cb_otherselect->currentItem()-1] = QFont(cb_otherfont->text(index),atoi(cb_otherfontsize->currentText()));
	}
}

void ConfigDialog::chooseChatFontSizeGet(int index) {
	vl_chatfont[cb_chatselect->currentItem()-4] = QFont(cb_chatfont->currentText(),atoi(cb_chatfontsize->text(index)));
}

void ConfigDialog::chooseUserboxFontSizeGet(int index) {
	vl_userboxfont[cb_userboxselect->currentItem()-2] = QFont(cb_userboxfont->currentText(),atoi(cb_userboxfontsize->text(index)));
}

void ConfigDialog::chooseOtherFontSizeGet(int index) {
	vl_otherfont[cb_otherselect->currentItem()-1] = QFont(cb_otherfont->currentText(),atoi(cb_otherfontsize->text(index)));
}

void ConfigDialog::chooseChatLine(const QString& text) {
	if (e_chatcolor->text().left(1) == '#' && e_chatcolor->text().length() == 7 ) {
		QPixmap pm(35,10);
		pm.fill(QColor(text));
		pb_chatcolor->setPixmap(pm);
		vl_chatcolor[cb_chatselect->currentItem()].setNamedColor(e_chatcolor->text());
	}
}

void ConfigDialog::chooseUserboxLine(const QString& text) {
	if (e_userboxcolor->text()[0] == '#' && e_userboxcolor->text().length() == 7 ) {
		QPixmap pm(35,10);
		pm.fill(QColor(text));
		pb_userboxcolor->setPixmap(pm);
		vl_userboxcolor[cb_userboxselect->currentItem()].setNamedColor(e_userboxcolor->text());
	}
}

void ConfigDialog::chooseOtherLine(const QString& text) {
	if (e_othercolor->text()[0] == '#' && e_othercolor->text().length() == 7 ) {
		QPixmap pm(35,10);
		pm.fill(QColor(text));
		pb_othercolor->setPixmap(pm);
		vl_othercolor[cb_otherselect->currentItem()].setNamedColor(e_othercolor->text());
	}
}

void ConfigDialog::chooseMsgTest(void) {
	playSound(e_msgfile->text(), e_soundprog->text());
}

void ConfigDialog::chooseChatTest(void) {
	playSound(e_chatfile->text(), e_soundprog->text());
}

void ConfigDialog::chooseNotifyTest(void) {
	playSound(e_soundnotify->text(), e_soundprog->text());
}

void ConfigDialog::chooseEmoticonsStyle(int index) {
	if (!index)
	    emotheme_box->setEnabled(false);
	else
	    emotheme_box->setEnabled(true);
}

void ConfigDialog::generateMyKeys(void) {
#ifdef HAVE_OPENSSL
	QString keyfile_path;
	
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(config.uin));
	keyfile_path.append(".pem");
	
	QFileInfo keyfile(keyfile_path);
	
	if (keyfile.permission(QFileInfo::WriteUser))
		if(QMessageBox::warning(this, "Kadu",
			i18n("Keys exist. Do you want to overwrite them?"),
			i18n("Yes"), i18n("No"),QString::null, 0, 1)==1)
				return;
	
	QCString tmp=ggPath("keys").local8Bit();
	mkdir(tmp.data(), 0700);

	kdebug("Generating my keys, len: %d\n", atoi(cb_keyslen->currentText()));
	if (sim_key_generate(config.uin) < 0) {
		QMessageBox::critical(this, "Kadu", i18n("Error generating keys"), i18n("OK"), QString::null, 0);
		return;
	}

	QMessageBox::information(this, "Kadu", i18n("Keys have been generated and written"), i18n("OK"), QString::null, 0);
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
	config.soundmsg = e_msgfile->text();
	config.soundchat = e_chatfile->text();
	config.soundprog = e_soundprog->text();
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
	if (config.dock && !trayicon) {
		trayicon = new TrayIcon(kadu);
		trayicon->show();
		trayicon->connectSignals();
		trayicon->setType(*icons->loadIcon(gg_icons[statusGGToStatusNr(getActualStatus() & (~GG_STATUS_FRIENDS_MASK))]));
		trayicon->changeIcon();
		}
	else
		if (!config.dock && trayicon) {
			delete trayicon;
			trayicon = NULL;
			}
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

//potrzebne aby zmiany weszly w "zycie" kadu ;)
	if (trayicon) {
		config.trayhint = b_trayhint->isChecked();
		trayicon->reconfigHint();
		config.hinterror = b_hinterror->isChecked();
		config.hinttime = atoi(e_hinttime->text().latin1());
		}
	else
		config.trayhint = config.hinterror = false;	

	if (!b_showdesc->isChecked() && config.showdesc) {
		kadu->hidedesc();
		config.showdesc = b_showdesc->isChecked();
		}
	else
		if(b_showdesc->isChecked() && !config.showdesc) {
			kadu->showdesc();
			config.showdesc = b_showdesc->isChecked();
		}

	config.smsbuildin = b_smsbuildin->isChecked();
	config.smsapp = strdup(e_smsapp->text().latin1());
	config.smsconf = strdup(e_smsconf->text().latin1());
	config.smscustomconf = b_smscustomconf->isChecked();
	config.emoticons_style = (EmoticonsStyle)cb_emoticons_style->currentItem();
	emoticons.setEmoticonsTheme(cb_emoticons_theme->currentText());
	config.autosend = b_autosend->isChecked();
	config.scrolldown = b_scrolldown->isChecked();
	config.chatprune = b_chatprune->isChecked();
	config.chatprunelen = atoi(e_chatprunelen->text().latin1());
	config.msgacks = b_msgacks->isChecked();
	config.blinkchattitle = b_blinkchattitle->isChecked();
	config.hintalert = b_hintalert->isChecked();
	config.ignoreanonusers = b_ignoreanonusers->isChecked();
	config.defaultwebbrowser = b_defwebbrowser->isChecked();
	config.webbrowser = e_webbrowser->text();
#ifdef HAVE_OPENSSL
	config.encryption = b_encryption->isChecked();
	//config.keyslen = atoi(cb_keyslen->currentText());
#endif

	config.colors.mychatBg = vl_chatcolor[0];
	config.colors.usrchatBg = vl_chatcolor[2];
	config.colors.mychatText = vl_chatcolor[1];
	config.colors.usrchatText = vl_chatcolor[3];
	config.colors.userboxFg = vl_userboxcolor[2];
	config.colors.userboxBg = vl_userboxcolor[0];
	config.colors.userboxDescBg = vl_userboxcolor[1];
	config.colors.userboxDescText = vl_userboxcolor[3];
	config.colors.trayhintBg = vl_othercolor[0];
	config.colors.trayhintText = vl_othercolor[1];
	
	config.fonts.chat = vl_chatfont[0];
	config.fonts.userbox = vl_userboxfont[0];
	config.fonts.userboxDesc = vl_userboxfont[1];
	config.fonts.trayhint = vl_otherfont[0];

	config.panelcontents = e_panelcontents->text().latin1();
	config.chatcontents = e_chatcontents->text().latin1();
	config.conferencecontents = e_conferencecontents->text().latin1();
	config.conferenceprefix = e_conferenceprefix->text().latin1();

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

	delete config.extip;
	config.allowdcc = b_dccenabled->isChecked();
	if (config.allowdcc && !b_dccip->isChecked() && inet_addr(e_dccip->text().latin1()) != INADDR_NONE)
		config.dccip = e_dccip->text();
	else
		config.dccip = "0.0.0.0";
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
	config.default_servers = b_defserver->isChecked();
	server_nr = 0;

	config.default_port = atoi(cb_portselect->currentText().latin1());

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
	kadu->changeAppearance();
	for (i = 0; i < chats.count(); i++)
		chats[i].ptr->changeAppearance();
	kadu->refreshGroupTabBar();
}

struct config config;
