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
#include <qvbox.h>
#include <qhgroupbox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qtimer.h>
#include <qgrid.h>
#include <qrect.h>
#include <qtextstream.h>
#include <qhostaddress.h>
#include <qspinbox.h>
#include <qslider.h>

#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <netinet/in.h>
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
	QHostAddress ip;
	QStringList servers;
	/* first read our own config file... */
	kdebug("loadKaduConfig(): Reading config file...\n");

	config_file.setGroup("Global");
	config.uin = config_file.readNumEntry("UIN",0);
	config.password = pwHash(config_file.readEntry("Password",""));
	kdebug("Read user data: uin %d password :-P\n", config.uin);
	config.soundprog = config_file.readEntry("SoundPlayer","");
	config.soundmsg = config_file.readEntry("Message_sound","");
	config.soundvolctrl = config_file.readBoolEntry("VolumeControl",false);
	config.soundvol = config_file.readDoubleNumEntry("SoundVolume",1.0);
	config.soundchat = config_file.readEntry("Chat_sound","");
	config.nick = config_file.readEntry("Nick", i18n("Me"));
	
	config.defaultstatus = config_file.readNumEntry("DefaultStatus", GG_STATUS_NOT_AVAIL);

	if (!config.defaultstatus)
		config.defaultstatus = GG_STATUS_NOT_AVAIL;
	config.defaultdescription = QStringList::split(QRegExp(";0;"),config_file.readEntry("DefaultDescription", i18n("I am busy.")),true);

	config.logmessages = config_file.readBoolEntry("Logging",true);
	config.savegeometry = config_file.readBoolEntry("SaveGeometry",true);
	config.playsoundchat = config_file.readBoolEntry("PlaySoundChat",true);
	config.playsoundchatinvisible = config_file.readBoolEntry("PlaySoundChatInvisible",true);
	config.playsound = config_file.readBoolEntry("PlaySound",false);
	config.playartsdsp = config_file.readBoolEntry("PlaySoundArtsDsp",false);
	config.sysmsgidx = config_file.readNumEntry("SystemMsgIndex",0);
	config.autoaway = config_file.readBoolEntry("AutoAway", false);
	config.autoawaytime = config_file.readNumEntry("AutoAwayTime", 300);
	config.allowdcc = config_file.readBoolEntry("AllowDCC",false);
	if (!config.dccip.setAddress(config_file.readEntry("DccIP", "0.0.0.0")))
		config.dccip.setAddress((unsigned int)0);
	if (!config.extip.setAddress(config_file.readEntry("ExternalIP", "0.0.0.0")))
		config.extip.setAddress((unsigned int)0);
	config.extport = config_file.readNumEntry("ExternalPort", 0);
	servers = QStringList::split(";", config_file.readEntry("Server", ""));
	config.servers.clear();
	for (int i = 0; i < servers.count(); i++) {
		if (ip.setAddress(servers[i]))
			config.servers.append(ip);
		}
	config.default_servers = config_file.readBoolEntry("isDefServers",true);

#ifdef HAVE_OPENSSL
	config.tls = config_file.readNumEntry("UseTLS", 0);
#else
	config.tls = 0;
#endif

	config.default_port = config_file.readNumEntry("DefaultPort", 8074);
	server_nr = 0;

	config.dock = config_file.readBoolEntry("UseDocking",true);
	config.rundocked = config_file.readBoolEntry("RunDocked", false);

	config.raise = config_file.readBoolEntry("AutoRaise",false);
	config.privatestatus = config_file.readBoolEntry("PrivateStatus", false);
	config.grouptabs = config_file.readBoolEntry("DisplayGroupTabs", true);
	config.checkupdates = config_file.readBoolEntry("CheckUpdates", true);
	config.addtodescription = config_file.readBoolEntry("AddToDescription", false);
	config.trayhint = config_file.readBoolEntry("TrayHint",true);
	config.hinterror = config_file.readBoolEntry("HintError",true);
	config.hinttime = config_file.readNumEntry("TimeoutHint",5);
	QRect def_rect(0, 0, 145, 465);
	config.geometry = config_file.readRectEntry("Geometry", &def_rect);
	config.dockwindows = config_file.readEntry("DockWindows", QString::null);
	QSize def_size(340, 60);
	config.splitsize = config_file.readSizeEntry("SplitSize", &def_size);
	config.showdesc = config_file.readBoolEntry("ShowDesc", true);
	config.multicoluserbox = config_file.readBoolEntry("MultiColumnUserbox", true);
	
	config_file.setGroup("WWW");
	config.defaultwebbrowser = config_file.readBoolEntry("DefaultWebBrowser", true);
	config.webbrowser = config_file.readEntry("WebBrowser", "");
	
	config_file.setGroup("SMS");
	config.smsbuildin = config_file.readBoolEntry("BuiltInApp",true);
	config.smsapp = strdup(config_file.readEntry("SmsApp",""));
	config.smscustomconf = config_file.readBoolEntry("UseCustomString",false);
	config.smsconf = strdup(config_file.readEntry("SmsString",""));

	config_file.setGroup("Other");
	config.emoticons_style = (EmoticonsStyle)config_file.readNumEntry("EmoticonsStyle",EMOTS_ANIMATED);
	emoticons.setEmoticonsTheme(config_file.readEntry("EmoticonsTheme",""));
	config.autosend = config_file.readBoolEntry("AutoSend",false);
	config.scrolldown = config_file.readBoolEntry("ScrollDown",true);
	config.chatprune = config_file.readBoolEntry("ChatPrune",false);
	config.chatprunelen = config_file.readNumEntry("ChatPruneLen",20);
	config.chathistorycitation = config_file.readNumEntry("ChatHistoryCitation", 10);
	config.chathistorycitation = config_file.readNumEntry("ChatHistoryQuotation", config.chathistorycitation);
	config.chathistorycitationtime = config_file.readNumEntry("ChatHistoryQuotationTime", 336);
	config.msgacks = config_file.readBoolEntry("MessageAcks", true);
	config.blinkchattitle = config_file.readBoolEntry("BlinkChatTitle", true);
	config.hintalert = config_file.readBoolEntry("HintAlert", false);
	config.ignoreanonusers = config_file.readBoolEntry("IgnoreAnonymousUsers", false);
#ifdef HAVE_OPENSSL
	config.encryption = config_file.readBoolEntry("Encryption", false);
//	config.keyslen = config_file.readNumEntry("KeysLength", 1024);
#endif
	config.panelsyntax = config_file.readEntry("PanelContents", "[#%u][, %f] %r [- %d] [ (%i)]");
	config.chatsyntax = config_file.readEntry("ChatContents", "");
	config.conferenceprefix = config_file.readEntry("ConferencePrefix", "");
	config.conferencesyntax = config_file.readEntry("ConferenceContents", "%a (%s[: %d])");
	config.disconnectwithdesc = config_file.readBoolEntry("DisconnectWithDescription", false);
	config.disconnectdesc = config_file.readEntry("DisconnectDescription","");

	config_file.setGroup("Notify");
	config.soundnotify = strdup(config_file.readEntry("NotifySound", ""));
	config.notifyglobal = config_file.readBoolEntry("NotifyStatusChange", false);
	config.notifyall = config_file.readBoolEntry("NotifyAboutAll", false);
	config.notifydialog = config_file.readBoolEntry("NotifyWithDialogBox", false);
	config.notifysound = config_file.readBoolEntry("NotifyWithSound", false);
	config.notifyhint = config_file.readBoolEntry("NotifyWithHint",true);
//	config.notifies = config_file.readListEntry("NotifyUsers");

	config_file.setGroup("Proxy");
	config.useproxy = config_file.readBoolEntry("UseProxy", false);
	if (!config.proxyaddr.setAddress(config_file.readEntry("ProxyHost", "")))
		config.proxyaddr.setAddress((unsigned int)0);
	config.proxyport = config_file.readNumEntry("ProxyPort", 0);
	config.proxyuser = pwHash(config_file.readEntry("ProxyUser", ""));
	config.proxypassword = pwHash(config_file.readEntry("ProxyPassword", ""));

	config_file.setGroup("Colors");
	
	QColor def_color("#FFFFFF");
	config.colors.userboxBg = config_file.readColorEntry("UserboxBgColor",&def_color);
	
	def_color.setNamedColor("#000000");
	config.colors.userboxFg = config_file.readColorEntry("UserboxFgColor",&def_color);
	
	def_color.setNamedColor("#E0E0E0");
	config.colors.mychatBg = config_file.readColorEntry("ChatMyBgColor",&def_color);
	
	def_color.setNamedColor("#F0F0F0");
	config.colors.usrchatBg = config_file.readColorEntry("ChatUsrBgColor",&def_color);
	
	def_color.setNamedColor("#000000");
	config.colors.mychatText = config_file.readColorEntry("ChatMyFontColor",&def_color);
	
	def_color.setNamedColor("#000000");
	config.colors.usrchatText = config_file.readColorEntry("ChatUsrFontColor",&def_color);
	
	def_color.setNamedColor("#C0C0C0");
	config.colors.userboxDescBg = config_file.readColorEntry("UserboxDescBgColor",&def_color);
	
	def_color.setNamedColor("#000000");
	config.colors.userboxDescText = config_file.readColorEntry("UserboxDescTextColor",&def_color);

	def_color.setNamedColor("#F0F0F0");
	config.colors.trayhintBg = config_file.readColorEntry("TrayHintBgColor",&def_color);

	def_color.setNamedColor("#000000");
	config.colors.trayhintText = config_file.readColorEntry("TrayHintTextColor",&def_color);

	config_file.setGroup("Fonts");
	QFontInfo info(a->font());
	QFont def_font(info.family(),info.pointSize());
	config.fonts.userbox = config_file.readFontEntry("UserboxFont", &def_font);
	config.fonts.chat = config_file.readFontEntry("ChatFont", &def_font);
	config.fonts.userboxDesc = config_file.readFontEntry("UserboxDescFont", &def_font);
	config.fonts.trayhint = config_file.readFontEntry("TrayHintFont", &def_font);
}

void saveKaduConfig(void) {
	QStringList servers;

	kdebug("saveKaduConfig(): Writing config files...\n");
	config_file.setGroup("Global");
	config_file.writeEntry("UIN", int(config.uin));
	config_file.writeEntry("Password", pwHash(config.password));
	config_file.writeEntry("Nick", config.nick);
	config_file.writeEntry("Geometry",kadu->geometry());
	config_file.writeEntry("Message_sound",config.soundmsg);
	config_file.writeEntry("Chat_sound",config.soundchat);
	config_file.writeEntry("Logging",config.logmessages);
	config_file.writeEntry("DefaultStatus",config.defaultstatus);
	config_file.writeEntry("DefaultDescription", (config.defaultdescription).join(";0;"));
	config_file.writeEntry("SystemMsgIndex",config.sysmsgidx);
	config_file.writeEntry("SaveGeometry",config.savegeometry);
	config_file.writeEntry("PlaySoundChat",config.playsoundchat);
	config_file.writeEntry("PlaySoundChatInvisible",config.playsoundchatinvisible);
	config_file.writeEntry("SoundPlayer",config.soundprog);
	config_file.writeEntry("PlaySound",config.playsound);
	config_file.writeEntry("PlaySoundArtsDsp",config.playartsdsp);
	config_file.writeEntry("SoundVolume",config.soundvol);
	config_file.writeEntry("VolumeControl",config.soundvolctrl);
	config_file.writeEntry("AutoAway",config.autoaway);
	config_file.writeEntry("AutoAwayTime",config.autoawaytime);
	config_file.writeEntry("AllowDCC",config.allowdcc);
	config_file.writeEntry("DccIP", config.dccip.toString());
	config_file.writeEntry("ExternalIP", config.extip.toString());
	config_file.writeEntry("ExternalPort", config.extport);
	config_file.writeEntry("isDefServers",config.default_servers);
	for (int i = 0; i < config.servers.count(); i++)
		servers.append(config.servers[i].toString());
	config_file.writeEntry("Server", servers.join(";"));
	config_file.writeEntry("UseTLS", config.tls);
	config_file.writeEntry("DefaultPort",config.default_port);

	config_file.writeEntry("UseDocking",config.dock);
	config_file.writeEntry("RunDocked",config.rundocked);	

	config_file.writeEntry("AutoRaise", config.raise);
	config_file.writeEntry("PrivateStatus", config.privatestatus);
	config_file.writeEntry("CheckUpdates", config.checkupdates);
	config_file.writeEntry("DisplayGroupTabs", config.grouptabs);
	config_file.writeEntry("AddToDescription", config.addtodescription);
	config_file.writeEntry("TrayHint", config.trayhint);
	config_file.writeEntry("HintError", config.hinterror);
	config_file.writeEntry("TimeoutHint", config.hinttime);
	config_file.writeEntry("ShowDesc", config.showdesc);
	config_file.writeEntry("MultiColumnUserbox", config.multicoluserbox);

	if (config.savegeometry) {
		config_file.writeEntry("SplitSize", config.splitsize);
		config_file.writeEntry("Geometry", config.geometry);
		}

	QTextStream stream(&config.dockwindows, IO_WriteOnly);
	stream << *kadu;
	config.dockwindows.replace(QRegExp("\\n"), "\\n");
	config_file.writeEntry("DockWindows", config.dockwindows);

	config_file.setGroup("WWW");
	config_file.writeEntry("DefaultWebBrowser", config.defaultwebbrowser);	
	config_file.writeEntry("WebBrowser", config.webbrowser);

	config_file.setGroup("SMS");
	config_file.writeEntry("BuiltInApp",config.smsbuildin);	
	config_file.writeEntry("SmsApp",config.smsapp);
	config_file.writeEntry("SmsString",config.smsconf);
	config_file.writeEntry("UseCustomString",config.smscustomconf);

	config_file.setGroup("Other");
	config_file.writeEntry("EmoticonsStyle",config.emoticons_style);
	config_file.writeEntry("EmoticonsTheme",config.emoticons_theme);
	config_file.writeEntry("AutoSend",config.autosend);
	config_file.writeEntry("ScrollDown",config.scrolldown);
	config_file.writeEntry("ChatPrune",config.chatprune);
	config_file.writeEntry("ChatPruneLen",config.chatprunelen);
	config_file.writeEntry("ChatHistoryQuotation", config.chathistorycitation);
	config_file.writeEntry("ChatHistoryQuotationTime", config.chathistorycitationtime);
	config_file.writeEntry("MessageAcks", config.msgacks);
	config_file.writeEntry("BlinkChatTitle", config.blinkchattitle);
	config_file.writeEntry("HintAlert", config.hintalert);
	config_file.writeEntry("IgnoreAnonymousUsers", config.ignoreanonusers);
#ifdef HAVE_OPENSSL
        config_file.writeEntry("Encryption", config.encryption);
        //config_file.writeEntry("KeysLength", config.keyslen);
#endif
	config_file.writeEntry("PanelContents", config.panelsyntax);
	config_file.writeEntry("ChatContents", config.chatsyntax);
	config_file.writeEntry("ConferencePrefix", config.conferenceprefix);
	config_file.writeEntry("ConferenceContents", config.conferencesyntax);
	config_file.writeEntry("DisconnectWithDescription", config.disconnectwithdesc);
	config_file.writeEntry("DisconnectDescription", config.disconnectdesc);

	config_file.setGroup("Proxy");
	config_file.writeEntry("UseProxy",config.useproxy);
	config_file.writeEntry("ProxyHost",config.proxyaddr.toString());
	config_file.writeEntry("ProxyPort",config.proxyport);
	config_file.writeEntry("ProxyUser", pwHash(config.proxyuser));
	config_file.writeEntry("ProxyPassword", pwHash(config.proxypassword));

	config_file.setGroup("Notify");
//	config_file.writeEntry("NotifyUsers", config.notifies);
	config_file.writeEntry("NotifySound", config.soundnotify);
	config_file.writeEntry("NotifyStatusChange", config.notifyglobal);
	config_file.writeEntry("NotifyAboutAll", config.notifyall);
	config_file.writeEntry("NotifyWithDialogBox", config.notifydialog);
	config_file.writeEntry("NotifyWithSound", config.notifysound);
	config_file.writeEntry("NotifyWithHint", config.notifyhint);

	config_file.setGroup("Colors");
	config_file.writeEntry("UserboxBgColor", config.colors.userboxBg);
	config_file.writeEntry("UserboxFgColor", config.colors.userboxFg);
	config_file.writeEntry("ChatMyBgColor", config.colors.mychatBg);
	config_file.writeEntry("ChatUsrBgColor", config.colors.usrchatBg);
	config_file.writeEntry("ChatMyFontColor", config.colors.mychatText);
	config_file.writeEntry("ChatUsrFontColor", config.colors.usrchatText);
	config_file.writeEntry("UserboxDescBgColor", config.colors.userboxDescBg);
	config_file.writeEntry("UserboxDescTextColor", config.colors.userboxDescText);
	config_file.writeEntry("TrayHintBgColor", config.colors.trayhintBg);
	config_file.writeEntry("TrayHintTextColor", config.colors.trayhintText);

	config_file.setGroup("Fonts");
	config_file.writeEntry("UserboxFont", config.fonts.userbox);
	config_file.writeEntry("ChatFont", config.fonts.chat);
	config_file.writeEntry("UserboxDescFont", config.fonts.userboxDesc);
	config_file.writeEntry("TrayHintFont", config.fonts.trayhint);
	
	config_file.sync();
}

QString ConfigDialog::acttab = "tab1";
ConfigDialog *ConfigDialog::configdialog = NULL;

void ConfigDialog::showConfigDialog() {
	ConfigDialog *cd;
	if (configdialog)
		configdialog->setActiveWindow();
	else {
		cd = new ConfigDialog;
		cd->show();
		}
}

ConfigDialog::ConfigDialog(QWidget *parent, const char *name) : QTabDialog(parent, name) {
	int i;

	setWFlags(Qt::WDestructiveClose);

	setupTab1();
	setupTab2();
	setupTab3();
	setupTab4();
	setupTab5();
	setupTab6();
	setupTab7();

	// nowy mechanizm	
	for(QValueList<RegisteredControl>::iterator i=RegisteredControls.begin(); i!=RegisteredControls.end(); i++)
	{
		QWidget* parent=NULL;
		if((*i).type!=CONFIG_TAB)
			for(QValueList<RegisteredControl>::iterator j=RegisteredControls.begin(); j!=RegisteredControls.end(); j++)
				if((*j).caption==(*i).parent)
					parent=(*j).widget;
		switch((*i).type)
		{
			case CONFIG_TAB:
			{
				QVBox* box = new QVBox(this);
				box->setMargin(2);
				(*i).widget=box;
				addTab(box,(*i).caption);
				break;
			}
			case CONFIG_GROUPBOX:
			{
				QVGroupBox* box = new QVGroupBox((*i).caption,parent);
				(*i).widget=box;
				break;
			}
			case CONFIG_CHECKBOX:
			{
				QCheckBox* check=new QCheckBox((*i).caption,parent);
				config_file.setGroup((*i).group);
				check->setChecked(config_file.readBoolEntry((*i).entry));
				(*i).widget=check;
				break;
			}
			case CONFIG_LINEEDIT:
			{
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel((*i).caption,hbox);
				QLineEdit* line=new QLineEdit(hbox);
				config_file.setGroup((*i).group);
				line->setText(config_file.readEntry((*i).entry));
				(*i).widget=line;
				break;
			}
			case CONFIG_HOTKEYEDIT:
			{
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel((*i).caption,hbox);
				HotKey* hotkey=new HotKey(hbox);
				config_file.setGroup((*i).group);
				hotkey->setText(config_file.readEntry((*i).entry,(*i).defaultS));
				(*i).widget=hotkey;
				break;
			}			
		};			
	};
	//

	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(updateConfig()));
	setCancelButton(i18n("Cancel"));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close()));
	setCaption(i18n("Kadu configuration"));
	resize(480,500);

	configdialog = this;
	for (i = 0; i < 7; i++)
		if (QString(box[i]->name()) == acttab)
			break;
	if (i)
		showPage(box[i]);
}

ConfigDialog::~ConfigDialog() {
	configdialog = NULL;
	acttab = QString(currentPage()->name());
};

void ConfigDialog::registerTab(const QString& caption)
{
	for(QValueList<RegisteredControl>::iterator i=RegisteredControls.begin(); i!=RegisteredControls.end(); i++)
		if((*i).caption==caption)
			return;
	RegisteredControl c;
	c.type=CONFIG_TAB;
	c.caption=caption;
	RegisteredControls.append(c);
};

void ConfigDialog::registerGroupBox(
	const QString& parent,const QString& caption)
{
	RegisteredControl c;
	c.type=CONFIG_GROUPBOX;
	c.parent=parent;
	c.caption=caption;
	RegisteredControls.append(c);
};

void ConfigDialog::registerCheckBox(
			const QString& parent,const QString& caption,
			const QString& group,const QString& entry)
{
	RegisteredControl c;
	c.type=CONFIG_CHECKBOX;
	c.parent=parent;
	c.caption=caption;
	c.group=group;
	c.entry=entry;
	RegisteredControls.append(c);
};

void ConfigDialog::registerLineEdit(
			const QString& parent,const QString& caption,
			const QString& group,const QString& entry,const QString& defaultS)
{
	RegisteredControl c;
	c.type=CONFIG_LINEEDIT;
	c.parent=parent;
	c.caption=caption;
	c.group=group;
	c.entry=entry;
	c.defaultS=defaultS;
	RegisteredControls.append(c);
};


void ConfigDialog::registerHotKeyEdit(
			const QString& parent,const QString& caption,
			const QString& group,const QString& entry,const QString& defaultS)
{
	RegisteredControl c;
	c.type=CONFIG_HOTKEYEDIT;
	c.parent=parent;
	c.caption=caption;
	c.group=group;
	c.entry=entry;
	c.defaultS=defaultS;
	RegisteredControls.append(c);
	// zapisujemy warto¶æ domy¶ln±, aby ju¿ wiêcej nie musieæ
	// jej podawaæ przy czytaniu z pliku conf
	config_file.setGroup(group);
	config_file.writeEntry(entry,defaultS);
};


void ConfigDialog::setupTab1(void) {
	int i;

	box[0] = new QVBox(this, "tab1");
	QVBox *box1 = box[0];
	box1->setMargin(5);

	QHGroupBox *userinfo = new QHGroupBox(box1);
	userinfo->setTitle(i18n("User data"));

	QLabel *l_uin = new QLabel(i18n("Uin"),userinfo);

	e_uin = new QLineEdit(QString::number(config.uin),userinfo);

	QLabel *l_password = new QLabel(i18n("Password"),userinfo);

	e_password = new QLineEdit(config.password,userinfo);
	e_password->setEchoMode(QLineEdit::Password);

	QLabel *l_nick = new QLabel(i18n("Nick"),userinfo);
	
	e_nick = new QLineEdit(config.nick,userinfo);
	
	/* SMS begin */
	QVGroupBox *smsvgrp = new QVGroupBox(box1);
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

	b_autoaway = new QCheckBox(i18n("Enable autoaway"),box1);

	QHGroupBox *awygrp = new QHGroupBox(box1);
	QLabel *l_autoaway = new QLabel(i18n("Set status to away after "),awygrp);
	e_autoawaytime = new QLineEdit(QString::number(config.autoawaytime),awygrp);
	QLabel *l_autoaway2 = new QLabel(i18n(" seconds"),awygrp);

	if (config.autoaway)
		b_autoaway->setChecked(true);
	else
		awygrp->setEnabled(false);

	QObject::connect(b_autoaway, SIGNAL(toggled(bool)), awygrp, SLOT(setEnabled(bool)));

	b_trayhint = new QCheckBox(i18n("Enable tray hints"),box1);

	QVGroupBox *hintgrp = new QVGroupBox(box1);
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

	QVGroupBox *gb_defstatus = new QVGroupBox(box1);
	gb_defstatus->setTitle(i18n("Default Status"));
	cb_defstatus = new QComboBox(gb_defstatus);
	for (i = 0;i < 7; i++)
		cb_defstatus->insertItem(i18n(__c2q(statustext[i])));
	i = 0;
	while (i < 7 && config.defaultstatus != gg_statuses[i])
		i++;
	cb_defstatus->setCurrentItem(i);

	b_disconnectdesc = new QCheckBox(i18n("On shutdown, set description:"),box1);
	b_disconnectdesc->setChecked(config.disconnectwithdesc);

	e_disconnectdesc = new QLineEdit(config.disconnectdesc,box1);
	e_disconnectdesc->setEnabled(config.disconnectwithdesc);
	e_disconnectdesc->setMaxLength(GG_STATUS_DESCR_MAXSIZE);

	QObject::connect(b_disconnectdesc, SIGNAL(toggled(bool)), e_disconnectdesc, SLOT(setEnabled(bool)));

	QGrid* grid = new QGrid(3, box1);

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

	b_checkupdates = new QCheckBox(i18n("Check for updates"),grid);
	if (config.checkupdates)
		b_checkupdates->setChecked(true);
		
	b_addtodescription = new QCheckBox(i18n("Add to description"),grid);
	if (config.addtodescription)
		b_addtodescription->setChecked(true);		
	QToolTip::add(b_addtodescription,i18n("If a file description in gg settings directory is present, its contents will be added\nto the status description and then the file will be deleted."));
	
	addTab(box1, i18n("General"));
}

void ConfigDialog::setupTab2(void) {
	box[1] = new QVBox(this, "tab2");
	QVBox *box2 = box[1];
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
	box[2] = new QVBox(this, "tab3");
	QVBox *box3 = box[2];
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

	box[3] = new QVBox(this, "tab4");
	QVBox *box4 = box[3];
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

void ConfigDialog::ifNotifyGlobal(bool toggled) {
	b_notifyall->setEnabled(toggled);
	panebox->setEnabled(toggled && !b_notifyall->isChecked());
	notifybox->setEnabled(toggled);
}

void ConfigDialog::ifNotifyAll(bool toggled) {
	panebox->setEnabled(!toggled);
}

void ConfigDialog::setupTab5(void) {
	QHostAddress ip;

	box[4] = new QVBox(this, "tab5");
	QVBox *box5 = box[4];
	box5->setMargin(2);	

	b_dccenabled = new QCheckBox(i18n("DCC enabled"),box5);
	b_dccenabled->setChecked(config.allowdcc);

	b_dccip = new QCheckBox(i18n("DCC IP autodetection"),box5);
	b_dccip->setChecked(!config.dccip.ip4Addr());

	g_dccip = new QVGroupBox(box5);
	g_dccip->setTitle(i18n("DCC IP"));
	g_dccip->setEnabled(!b_dccip->isChecked());

	QHBox *dccipbox = new QHBox(g_dccip);
	dccipbox->setSpacing(5);
	QLabel *l4 = new QLabel(i18n("IP address:"),dccipbox);
	e_dccip = new QLineEdit(dccipbox);
	if (g_dccip->isEnabled())
		e_dccip->setText(config.dccip.toString());

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

	QHBox *servertypebox = new QHBox(g_server);
	b_defserver = new QCheckBox(i18n("Use default servers"), servertypebox);
	b_defserver->setChecked(config.default_servers);
#ifdef HAVE_OPENSSL
	b_tls = new QCheckBox(i18n("Use TLSv1"), servertypebox);
	b_tls->setChecked(config.tls);
#endif

	serverbox = new QHBox(g_server);
	serverbox->setSpacing(5);
	ip = config.servers[0];
	serverbox->setEnabled(!config.default_servers && config.servers.count() && ip.ip4Addr());

	QLabel *l3 = new QLabel(i18n("IP addresses:"),serverbox);
	e_server = new QLineEdit(serverbox);
	QStringList servers;
	for (int i = 0; i < config.servers.count(); i++)
		servers.append(config.servers[i].toString());
	e_server->setText(servers.join(";"));

	portbox = new QHBox(g_server);
	portbox->setSpacing(5);
	QLabel *lserverport = new QLabel(i18n("Default port to connect to servers"), portbox);
	
	cb_portselect = new QComboBox(portbox);
        cb_portselect->insertItem("8074");
	cb_portselect->insertItem("443");
        cb_portselect->setCurrentText(QString::number(config.default_port));

#ifdef HAVE_OPENSSL
	portbox->setEnabled(!config.tls);
#endif

	b_useproxy = new QCheckBox(i18n("Use proxy server"),box5);
	b_useproxy->setChecked(config.useproxy);

	QVGroupBox *g_proxy = new QVGroupBox(box5);
	g_proxy->setTitle(i18n("Proxy server"));
	g_proxy->setEnabled(b_useproxy->isChecked());

	QHBox *proxyserverbox = new QHBox(g_proxy);
	proxyserverbox->setSpacing(5);
	QLabel *l5 = new QLabel(i18n("IP address:"),proxyserverbox);
	e_proxyserver = new QLineEdit(config.proxyaddr.toString(),proxyserverbox);
	QLabel *l6 = new QLabel(i18n("Port:"),proxyserverbox);
	e_proxyport = new QLineEdit(QString::number(config.proxyport),proxyserverbox);

	QHBox *proxyuserbox = new QHBox(g_proxy);
	proxyuserbox->setSpacing(5);
	QLabel *l7 = new QLabel(i18n("Username:"),proxyuserbox);
	e_proxyuser = new QLineEdit(config.proxyuser,proxyuserbox);
	QLabel *l8 = new QLabel(i18n("Password:"),proxyuserbox);
	e_proxypassword = new QLineEdit(config.proxypassword,proxyuserbox);
	e_proxypassword->setEchoMode(QLineEdit::Password);

	g_fwdprop->setEnabled(config.extip.ip4Addr() && config.extport > 1023);    
	if (g_fwdprop->isEnabled()) {
		b_dccfwd->setChecked(true);
		e_extip->setText(config.extip.toString());
		e_extport->setText(QString::number(config.extport));	
		}
//	g_server->setEnabled(!config.default_servers && config.servers.count() && inet_addr(config.servers[0].latin1()) != INADDR_NONE);
//	if (!g_server->isEnabled())
//		b_defserver->setChecked(true);
//	else
//		e_server->setText(config.servers.join(";"));

	QObject::connect(b_dccenabled, SIGNAL(toggled(bool)), this, SLOT(ifDccEnabled(bool)));
	QObject::connect(b_dccip, SIGNAL(toggled(bool)), this, SLOT(ifDccIpEnabled(bool)));
	QObject::connect(b_dccfwd, SIGNAL(toggled(bool)), g_fwdprop, SLOT(setEnabled(bool)));
	QObject::connect(b_defserver, SIGNAL(toggled(bool)), this, SLOT(ifDefServerEnabled(bool)));
#ifdef HAVE_OPENSSL
	QObject::connect(b_tls, SIGNAL(toggled(bool)), this, SLOT(useTlsEnabled(bool)));
#endif
	QObject::connect(b_useproxy, SIGNAL(toggled(bool)), g_proxy, SLOT(setEnabled(bool)));

	addTab(box5, i18n("Network"));
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

void ConfigDialog::ifDccIpEnabled(bool toggled) {
	g_dccip->setEnabled(!toggled);
}

void ConfigDialog::ifDefServerEnabled(bool toggled) {
	serverbox->setEnabled(!toggled);
}

void ConfigDialog::useTlsEnabled(bool toggled) {
	portbox->setEnabled(!toggled);
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

	box[5] = new QVBox(this, "tab6");
	QVBox *box6 = box[5];
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

	QVGroupBox *syntaxprop = new QVGroupBox(box6);

	QHBox *panelbox = new QHBox(syntaxprop);
	panelbox->setSpacing(5);
	
	QLabel *l_panel = new QLabel(i18n("Information panel syntax:"), panelbox);
	e_panelsyntax = new QLineEdit(config.panelsyntax, panelbox);

	QToolTip::add(e_panelsyntax,i18n("Syntax: %s - status, %d - description, %i - ip, %n - nick, %a - altnick, %f - frist name\n%r - surname, %m - mobile, %u - uin, %g - group, %o - return _space_ if user doesn't have us in userlist\n%v - revDNS, %p - port\nIf you leave blank, default settings will be used"));

	QHBox *chatsyntaxbox = new QHBox(syntaxprop);
	chatsyntaxbox->setSpacing(5);

	QLabel *l_chatsyntax = new QLabel(i18n("Chat window title syntax:"), chatsyntaxbox);
	e_chatsyntax = new QLineEdit(chatsyntaxbox);
	if (config.chatsyntax.isEmpty())
		e_chatsyntax->setText(i18n("Chat with ")+"%a (%s[: %d])");
	else
		e_chatsyntax->setText(config.chatsyntax);

	QToolTip::add(e_chatsyntax,i18n("Syntax the same as in information panel."));

	QHBox *confsyntaxbox = new QHBox(syntaxprop);
	confsyntaxbox->setSpacing(5);

	QLabel *l_confprefixcon = new QLabel(i18n("Conference window title prefix:"), confsyntaxbox);
	e_conferenceprefix = new QLineEdit(confsyntaxbox);
	if (config.conferenceprefix.isEmpty())
		e_conferenceprefix->setText(i18n("Conference with "));
	else
		e_conferenceprefix->setText(config.conferenceprefix);

	QToolTip::add(e_conferenceprefix,i18n("This text will be before syntax.\nIf you leave blank, default settings will be used."));

	QLabel *l_confsyntax = new QLabel(i18n("syntax:"), confsyntaxbox);
	e_conferencesyntax = new QLineEdit(config.conferencesyntax, confsyntaxbox);

	QToolTip::add(e_conferencesyntax,i18n("Syntax the same as in information panel."));

	QGrid *grid = new QGrid(3, box6);

	b_showdesc = new QCheckBox(i18n("Show userbox-desc."), grid);
	b_showdesc->setChecked(config.showdesc);

	b_grptabs = new QCheckBox(i18n("Display group tabs"), grid);
	if (config.grouptabs)
		b_grptabs->setChecked(true);

	b_multicoluserbox = new QCheckBox(i18n("Multicolumn userbox"), grid);
	if (config.multicoluserbox)
		b_multicoluserbox->setChecked(true);

	addTab(box6, i18n("Look"));
};

void ConfigDialog::setupTab7() {
	box[6] = new QVBox(this, "tab7");
	QVBox *box7 = box[6];
	box7->setMargin(2);

	QVGroupBox *vbox1 = new QVGroupBox(i18n("Quoted phrases during chat open"), box7); 

	QHBox *hbox1 = new QHBox(vbox1);
	QLabel *l_qcount = new QLabel(i18n("Count:"), hbox1);
	s_qcount = new QSpinBox(0, 50, 1, hbox1);
	s_qcount->setValue(config.chathistorycitation);

	QLabel *l_qtime = new QLabel(i18n("Don't quote phrases older than:"), vbox1);
	s_qtime = new QSlider(-744, -1, 24, -336, Qt::Horizontal, vbox1);
	s_qtime->setValue(-config.chathistorycitationtime);
	s_qtime->setTickmarks(QSlider::Below);
	l_qtimeinfo = new QLabel(vbox1);
	l_qtimeinfo->setAlignment(Qt::AlignHCenter);
	updateQuoteTimeLabel(-config.chathistorycitationtime);
	connect(s_qtime, SIGNAL(valueChanged(int)), this, SLOT(updateQuoteTimeLabel(int)));

	addTab(box7, i18n("History"));
}

void ConfigDialog::updateQuoteTimeLabel(int value) {
	l_qtimeinfo->setText(QString(i18n("%1 day(s) %2 hour(s)")).arg(-value / 24).arg((-value) % 24));
}

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
	int i;
	QHostAddress ip;
	bool ipok;

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

	config.disconnectwithdesc = b_disconnectdesc->isChecked();
	config.disconnectdesc = e_disconnectdesc->text();

	config.showdesc = b_showdesc->isChecked();
	kadu->showdesc(config.showdesc);
	config.multicoluserbox = b_multicoluserbox->isChecked();
	if (config.multicoluserbox)
		kadu->userbox->setColumnMode(QListBox::FitToWidth);
	else
		kadu->userbox->setColumnMode(1);

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

	config.panelsyntax = e_panelsyntax->text();

	if (e_chatsyntax->text() == i18n("Chat with ")+"%a (%s[: %d])" || e_chatsyntax->text() == "Chat with %a (%s[: %d])")
		config.chatsyntax = "";
	else
		config.chatsyntax = e_chatsyntax->text();
	
	config.conferencesyntax = e_conferencesyntax->text();

	if (e_conferenceprefix->text() == i18n("Conference with ") || e_conferenceprefix->text() == "Conference with ")
		config.conferenceprefix = "";
	else
		config.conferenceprefix = e_conferenceprefix->text();

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

	config.allowdcc = b_dccenabled->isChecked();
	ipok = ip.setAddress(e_dccip->text());
	if (config.allowdcc && !b_dccip->isChecked() && ipok)
		config.dccip = ip;
	else
		config.dccip.setAddress((unsigned int)0);
	ipok = ip.setAddress(e_extip->text());
	if (config.allowdcc && b_dccfwd->isChecked() && ipok
		&& atoi(e_extport->text().latin1()) > 1023) {
		config.extip = ip;
		config.extport = atoi(e_extport->text().latin1());
		}
	else {
		config.extip.setAddress((unsigned int)0);
		config.extport = 0;
		}

	QStringList tmpservers;
	QValueList<QHostAddress> servers;
	tmpservers = QStringList::split(";", e_server->text());
	for (i = 0; i < tmpservers.count(); i++) {
		ipok = ip.setAddress(tmpservers[i]);
		if (!ipok)
			break;
		servers.append(ip);
		}
	if (!b_defserver->isChecked() && i == tmpservers.count())
		config.servers = servers;
	config.default_servers = b_defserver->isChecked();
	server_nr = 0;

#ifdef HAVE_OPENSSL
	config.tls = b_tls->isChecked();
#endif

	config.default_port = atoi(cb_portselect->currentText().latin1());
	config.useproxy = b_useproxy->isChecked();
	if (config.useproxy) {
		ipok = ip.setAddress(e_proxyserver->text());
		if (ipok && ip.isIp4Addr())
			config.proxyaddr = ip;
		if (atoi(e_proxyport->text().latin1()) > 1023)
			config.proxyport = (unsigned short)atoi(e_proxyport->text().latin1());
		config.proxyuser = e_proxyuser->text();
		if (config.proxyuser.length())
			config.proxypassword = e_proxypassword->text();
		else
			config.proxypassword.truncate(0);
		}

	config.chathistorycitation = s_qcount->value();
	config.chathistorycitationtime = -s_qtime->value();

	/* and now, save it */
	saveKaduConfig();
	userlist.writeToFile();

	// nowy mechanizm
	for(QValueList<RegisteredControl>::iterator i=RegisteredControls.begin(); i!=RegisteredControls.end(); i++)
	{
		config_file.setGroup((*i).group);
		switch((*i).type)
		{
			case CONFIG_CHECKBOX:
				config_file.writeEntry((*i).entry,((QCheckBox*)((*i).widget))->isChecked());
				break;
			case CONFIG_LINEEDIT:
				config_file.writeEntry((*i).entry,((QLineEdit*)((*i).widget))->text());
				break;
			case CONFIG_HOTKEYEDIT:
				config_file.writeEntry((*i).entry,((HotKey*)((*i).widget))->text());
				break;				
		};			
	};
	config_file.sync();
	//

	/* I odswiez okno Kadu */
	kadu->changeAppearance();
	for (i = 0; i < chats.count(); i++)
		chats[i].ptr->changeAppearance();
	kadu->refreshGroupTabBar();
}

struct config config;

QValueList<ConfigDialog::RegisteredControl> ConfigDialog::RegisteredControls;
