/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qhostaddress.h>
#include <qregexp.h>
#include <qvbox.h>
#include <qspinbox.h>
#include <sys/stat.h>

//
#include "kadu.h"
#include "emoticons.h"
#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"

#ifdef HAVE_OPENSSL
extern "C"
{
#include "simlite.h"
};
#endif
//

void loadKaduConfig(void) {  	
	QStringList servers;
	QString temp;
	/* first read our own config file... */
	kdebug("loadKaduConfig(): Reading config file...\n");
	
	defaultdescriptions = QStringList::split(QRegExp("<-->"),
		config_file.readEntry("Global", "DefaultDescription", QT_TR_NOOP("I am busy.")), true);

	temp = config_file.readEntry("Global","VolumeControl");
	if (temp == "")
	config_file.writeEntry("Global", "VolumeControl", false);
	temp = config_file.readEntry("Global", "SoundVolume");
	if (temp == "")
	config_file.writeEntry("Global", "SoundVolume", 1.0);
	
	
	temp=config_file.readEntry("Global","Logging");
	if (temp=="")
	config_file.writeEntry("Global","Logging",true);	
	
	temp=config_file.readEntry("Global","Message_sound");
	if (temp=="")
	config_file.writeEntry("Global","Message_sound","");

	temp=config_file.readEntry("Global","Chat_sound");
	if (temp=="")
	config_file.writeEntry("Global","Chat_sound","");

	temp=config_file.readEntry("Global","SoundPlayer");
	if (temp=="")
	config_file.writeEntry("Global","SoundPlayer","");

	
	temp=config_file.readEntry("Global","SaveGeometry");
	if (temp=="")
	config_file.writeEntry("Global","SaveGeometry",true);
	
	temp=config_file.readEntry("Global","PlaySoundChat");
	if (temp=="")
	config_file.writeEntry("Global","PlaySoundChat",true);
	
	temp=config_file.readEntry("Global","PlaySoundChatInvisible");
	if (temp=="")
	config_file.writeEntry("Global","PlaySoundChatInvisible",true);
	
	temp=config_file.readEntry("Global","PlaySound");
	if (temp=="")
	config_file.writeEntry("Global","PlaySound",false);
	
	temp=config_file.readEntry("Global","PlaySoundArtsDsp");
	if (temp=="")
	config_file.writeEntry("Global","PlaySoundArtsDsp",false);
	
	temp=config_file.readEntry("Global","SystemMsgIndex");
	if (temp=="")
	config_file.writeEntry("Global","SystemMsgIndex",0);
	
	temp=config_file.readEntry("Global","AutoAway");
	if (temp=="")
	config_file.writeEntry("Global","AutoAway", false);
	
	temp=config_file.readEntry("Global","AutoAwayTime");
	if (temp=="")
	config_file.writeEntry("Global","AutoAwayTime", 300);
	
	temp=config_file.readEntry("Global","AllowDCC");
	if (temp=="")
	config_file.writeEntry("Global","AllowDCC",false);
	
	if (!config_dccip.setAddress(config_file.readEntry("Global","DccIP", "")))
		config_dccip.setAddress((unsigned int)0);
		
	if (!config_extip.setAddress(config_file.readEntry("Global","ExternalIP", "")))
	config_extip.setAddress((unsigned int)0);	

	temp=config_file.readEntry("Global","ExternalPort");
	if (temp=="")
	config_file.writeEntry("Global","ExternalPort", 0);
	
	
        servers = QStringList::split(";", config_file.readEntry("Global","Server", ""));
	config_servers.clear();
	QHostAddress ip;
	    for (int i = 0; i < servers.count(); i++) 
			{
		    if (ip.setAddress(servers[i]))
				config_servers.append(ip);
			}
						


	temp=config_file.readEntry("Global","isDefServers");
	if (temp=="")
	config_file.writeEntry("Global","isDefServers",true);
	
#ifdef HAVE_OPENSSL
	temp=config_file.readEntry("Global","UseTLS");
	if (temp=="")
	config_file.writeEntry("Global","UseTLS", 0);
#else
	config_file.writeEntry("Global","UseTLS", 0);	
#endif

	temp=config_file.readEntry("Global","DefaultPort");
	if (temp=="")
	config_file.writeEntry("Global","DefaultPort", 8074);
	
	server_nr = 0;	
	
	temp=config_file.readEntry("Global","UseDocking");
	if (temp=="")
	config_file.writeEntry("Global","UseDocking",true);	
	
	temp=config_file.readEntry("Global","RunDocked");
	if (temp=="")
	config_file.writeEntry("Global","RunDocked", false);
	
	temp=config_file.readEntry("Global","AutoRaise");
	if (temp=="")
	config_file.readBoolEntry("Global","AutoRaise",false);
	
	temp=config_file.readEntry("Global","PrivateStatus");
	if (temp=="")
	config_file.writeEntry("Global","PrivateStatus", false);
	
	temp=config_file.readEntry("Global","DisplayGroupTabs");
	if (temp=="")
	config_file.writeEntry("Global","DisplayGroupTabs", true);
	
	temp=config_file.readEntry("Global","CheckUpdates");
	if (temp=="")
	config_file.writeEntry("Global","CheckUpdates", true);
	
	temp=config_file.readEntry("Global","AddToDescription");
	if (temp=="")
	config_file.writeEntry("Global","AddToDescription", false);
	
	temp=config_file.readEntry("Global","TrayHint");
	if (temp=="")
	config_file.writeEntry("Global","TrayHint",true);
	
	temp=config_file.readEntry("Global","HintError");
	if (temp=="")
	config_file.writeEntry("Global","HintError",true);
	
	temp=config_file.readEntry("Global","TimeoutHint");
	if (temp=="")
	config_file.writeEntry("Global","TimeoutHint",5);
	
	QRect def_rect(0, 0, 145, 465);
	temp=config_file.readEntry("Global","Geometry");
	if (temp=="")
	config_file.writeEntry("Global","Geometry", def_rect);
	
	temp=config_file.readEntry("Global","DockWindows");
	if (temp=="")
	config_file.writeEntry("Global","DockWindows", QString::null);
	
	QSize def_size(340, 60);
	temp=config_file.readEntry("Global","SplitSize");
	if (temp=="")
	config_file.writeEntry("Global","SplitSize", def_size);
	
	temp=config_file.readEntry("Global","ShowDesc");
	if (temp=="")
	config_file.writeEntry("Global","ShowDesc", true);
	
	temp=config_file.readEntry("Global","MultiColumnUserbox");
	if (temp=="")
	config_file.writeEntry("Global","MultiColumnUserbox", true);

//	
	
	temp=config_file.readEntry("WWW","DefaultWebBrowser");
	if (temp=="")
	config_file.writeEntry("WWW","DefaultWebBrowser", true);
	
	temp=config_file.readEntry("WWW","WebBrowser", "");
	if (temp=="")
	config_file.writeEntry("WWW","WebBrowser", "");

//	
	
	temp=config_file.readEntry("SMS","BuiltInApp");
	if (temp=="")
	config_file.writeEntry("SMS","BuiltInApp",true);
	
	temp=config_file.readEntry("SMS","SmsApp");
	if (temp=="")
	config_file.writeEntry("SMS","SmsApp","");

	temp=config_file.readEntry("SMS","SmsString");
	if (temp=="")
	config_file.writeEntry("SMS","SmsString","");


	temp=config_file.readEntry("SMS","UseCustomString");
	if (temp=="")
	config_file.writeEntry("SMS","UseCustomString",false);

		    	
//
	
	temp=config_file.readEntry("Other","EmoticonsStyle");
	if (temp=="")
	config_file.writeEntry("Other","EmoticonsStyle",EMOTS_ANIMATED);
	
	emoticons.setEmoticonsTheme(config_file.readEntry("Other","EmoticonsTheme",""));
	
	temp=config_file.readEntry("Other","AutoSend");
	if (temp=="")
	config_file.writeEntry("Other","AutoSend",false);
	
	temp=config_file.readEntry("Other","ScrollDown");
	if (temp=="")
	config_file.writeEntry("Other","ScrollDown",true);
	
	temp=config_file.readEntry("Other","ChatPrune");
	if (temp=="")
	config_file.writeEntry("Other","ChatPrune",false);
	
	temp=config_file.readEntry("Other","ChatPruneLen");
	if (temp=="")
	config_file.writeEntry("Other","ChatPruneLen",20);
	
	temp=config_file.readEntry("Other","ChatHistoryCitation");
	if (temp=="")
	config_file.writeEntry("Other","ChatHistoryCitation", 10);
	
	temp=config_file.readEntry("Other","ChatHistoryQuotationTime");
	if (temp=="")
	config_file.writeEntry("Other","ChatHistoryQuotationTime", 336);
	
	temp=config_file.readEntry("Other","MessageAcks");
	if (temp=="")
	config_file.writeEntry("Other","MessageAcks",true);
	
	temp=config_file.readEntry("Other","BlinkChatTitle");
	if (temp=="")
	config_file.writeEntry("Other","BlinkChatTitle",true);
	
	temp=config_file.readEntry("Other","HintAlert");
	if (temp=="")
	config_file.writeEntry("Other","HintAlert", false);
	
	temp=config_file.readEntry("Other","IgnoreAnonymousUsers");
	if (temp=="")
	config_file.writeEntry("Other","IgnoreAnonymousUsers", false);
	
#ifdef HAVE_OPENSSL
	temp=config_file.readEntry("Other","Encryption");
	if (temp=="")
	config_file.writeEntry("Other","Encryption", false);
#endif

	temp=config_file.readEntry("Other","PanelContents");
	if (temp=="")
	config_file.writeEntry("Other","PanelContents", "[#%u][, %f] %r [- %d] [ (%i)]");
	
	temp=config_file.readEntry("Other","ChatContents");
	if (temp=="")
	config_file.writeEntry("Other","ChatContents", "");

	temp=config_file.readEntry("Other","ConferencePrefix");
	if (temp=="")
	config_file.writeEntry("Other","ConferencePrefix", "");
    
    	
	temp=config_file.readEntry("Other","ConferenceContents");
	if (temp=="")
	config_file.writeEntry("Other","ConferenceContents", "%a (%s[: %d])");	

	temp=config_file.readEntry("Other","DisconnectWithDescription");
	if (temp=="")
	config_file.writeEntry("Other","DisconnectWithDescription", false);
	
//
	temp=config_file.readEntry("Notify","NotifySound");
	if (temp=="")	
	config_file.writeEntry("Notify","NotifySound", "");
	
	temp=config_file.readEntry("Notify","NotifyStatusChange");
	if (temp=="")
	config_file.writeEntry("Notify","NotifyStatusChange", false);
	
	temp=config_file.readEntry("Notify","NotifyAboutAll");
	if (temp=="")
	config_file.writeEntry("Notify","NotifyAboutAll", false);
	
	temp=config_file.readEntry("Notify","NotifyWithDialogBox");
	if (temp=="")
	config_file.writeEntry("Notify","NotifyWithDialogBox", false);
	
	temp=config_file.readEntry("Notify","NotifyWithSound");
	if (temp=="")
	config_file.writeEntry("Notify","NotifyWithSound", false);
	
	temp=config_file.readEntry("Notify","NotifyWithHint");
	if (temp=="")
	config_file.writeEntry("Notify","NotifyWithHint",true);	

//	    	
	
	temp=config_file.readEntry("Proxy","UseProxy");
	if (temp=="")
	config_file.writeEntry("Proxy","UseProxy", false);

	temp=config_file.readEntry("Proxy","ProxyHost");
	if (temp=="")
	config_file.writeEntry("Proxy","ProxyHost", "");


	    if (!config_proxyaddr.setAddress(config_file.readEntry("Proxy","ProxyHost", "")))
		    config_proxyaddr.setAddress((unsigned int)0);

	temp=config_file.readEntry("Proxy","ProxyPort");
	if (temp=="")
	config_file.writeEntry("Proxy","ProxyPort", 0);

	temp=config_file.readEntry("Proxy","ProxyUser");
	if (temp=="")
	config_file.writeEntry("Proxy","ProxyUser", "");

	temp=config_file.readEntry("Proxy","ProxyPassword");
	if (temp=="")
	config_file.writeEntry("Proxy","ProxyPassword", "");
				
//
	
	QColor def_color("#FFFFFF");

	temp=config_file.readEntry("Colors","UserboxBgColor");
	if (temp=="")
	config_file.writeEntry("Colors","UserboxBgColor",def_color);
	
	def_color.setNamedColor("#000000");
	temp=config_file.readEntry("Colors","UserboxFgColor");
	if (temp=="")
	config_file.writeEntry("Colors","UserboxFgColor",def_color);
		
	def_color.setNamedColor("#E0E0E0");
	temp=config_file.readEntry("Colors","ChatMyBgColor");
	if (temp=="")
	config_file.writeEntry("Colors","ChatMyBgColor",def_color);
			
	def_color.setNamedColor("#F0F0F0");
	temp=config_file.readEntry("Colors","ChatUsrBgColor");
	if (temp=="")
	config_file.writeEntry("Colors","ChatUsrBgColor",def_color);	
					
	def_color.setNamedColor("#000000");
	temp=config_file.readEntry("Colors","ChatMyFontColor");
	if (temp=="")
	config_file.writeEntry("Colors","ChatMyFontColor",def_color);
					
	def_color.setNamedColor("#000000");
	temp=config_file.readEntry("Colors","ChatUsrFontColor");
	if (temp=="")
	config_file.writeEntry("Colors","ChatUsrFontColor",def_color);
							
	def_color.setNamedColor("#C0C0C0");
	temp=config_file.readEntry("Colors","UserboxDescBgColor");
	if (temp=="")
	config_file.writeEntry("Colors","UserboxDescBgColor",def_color);
								
	def_color.setNamedColor("#000000");
	temp=config_file.readEntry("Colors","UserboxDescTextColor");
	if (temp=="")
	config_file.writeEntry("Colors","UserboxDescTextColor",def_color);
						    
	def_color.setNamedColor("#F0F0F0");
	temp=config_file.readEntry("Colors","TrayHintBgColor");
	if (temp=="")
	config_file.writeEntry("Colors","TrayHintBgColor",def_color);
								    
	def_color.setNamedColor("#000000");
	temp=config_file.readEntry("Colors","TrayHintTextColor");
	if (temp=="")
	config_file.writeEntry("Colors","TrayHintTextColor",def_color);

//
	QFontInfo info(a->font());
	
	QFont def_font(info.family(),info.pointSize());
	
	temp=config_file.readEntry("Fonts","UserboxFont");
	if (temp=="")
	config_file.writeEntry("Fonts","UserboxFont", def_font);
	
	temp=config_file.readEntry("Fonts","ChatFont");
	if (temp=="")
	config_file.writeEntry("Fonts","ChatFont", def_font);
	
	temp=config_file.readEntry("Fonts","UserboxDescFont");
	if (temp=="")
	config_file.writeEntry("Fonts","UserboxDescFont", def_font);
	
	temp=config_file.readEntry("Fonts","TrayHintFont");
	if (temp=="")
	config_file.writeEntry("Fonts","TrayHintFont", def_font);
			    


	
}

void saveKaduConfig(void) {
	QStringList servers;

	kdebug("saveKaduConfig(): Writing config files...\n");
	config_file.writeEntry("Global","Geometry",kadu->geometry());
	
	config_file.writeEntry("Global","DefaultDescription", defaultdescriptions.join("<-->"));


    for (int i = 0; i < config_servers.count(); i++)
	    servers.append(config_servers[i].toString());
		config_file.writeEntry("Global","Server", servers.join(";"));

		

	
//#ifdef HAVE_OPENSSL
//        config_file.writeEntry("Other","Encryption");
//#endif
		

	QString dockwindows=config_file.readEntry("Global","DockWindows");
	QTextStream stream(&dockwindows, IO_WriteOnly);
	stream << *kadu;
	dockwindows.replace(QRegExp("\\n"), "\\n");
	config_file.writeEntry("Global","DockWindows", dockwindows);

	config_file.sync();
}

QString ConfigDialog::acttab = QT_TR_NOOP("General");
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
			case CONFIG_CHECKBOX:
			{
				QCheckBox* check=new QCheckBox((*i).caption,parent,(*i).name);
				check->setChecked(config_file.readBoolEntry((*i).group,(*i).entry,(*i).defaultS));
				(*i).widget=check;
				QToolTip::add((*i).widget,(*i).tip);
				break;
			}
			case CONFIG_COMBOBOX:
			{
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel((*i).caption,hbox);
				QComboBox* combo=new QComboBox(hbox,(*i).name);
				(*i).widget=combo;
				QToolTip::add((*i).widget,(*i).tip);
				break;
			}
			case CONFIG_GRID:
				{
				QGrid* grid=new QGrid((*i).defaultS.toInt(),parent,(*i).caption);
				(*i).widget=grid;
				break;
			}
			case CONFIG_HGROUPBOX:
			{
				QHGroupBox* box = new QHGroupBox((*i).caption,parent,(*i).name);
				(*i).widget=box;
				break;
			}
			case CONFIG_HOTKEYEDIT:
			{
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel((*i).caption,hbox);
				HotKey* hotkey=new HotKey(hbox,(*i).name);
				hotkey->setText(config_file.readEntry((*i).group,(*i).entry,(*i).defaultS));
				(*i).widget=hotkey;
				QToolTip::add((*i).widget,(*i).tip);
				break;
			}
			case CONFIG_LABEL:
			{
				QLabel* label=new QLabel((*i).caption,parent,(*i).name);
				(*i).widget=label;
				break;
			}	
			case CONFIG_LINEEDIT:
			{
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel((*i).caption,hbox);
				QLineEdit* line=new QLineEdit(hbox,(*i).name);
				line->setText(config_file.readEntry((*i).group,(*i).entry,(*i).defaultS));
				(*i).widget=line;
				QToolTip::add((*i).widget,(*i).tip);
				break;
			}
			case CONFIG_LISTBOX:
			{	
				QListBox* listbox= new QListBox(parent,(*i).caption);
				(*i).widget=listbox;
				QToolTip::add((*i).widget,(*i).tip);
				break;
			}
			case CONFIG_PUSHBUTTON:
			{
				QPushButton *button =new QPushButton((*i).caption,parent,(*i).name);
				button->setIconSet(loadIcon((*i).defaultS));
				(*i).widget=button;
				QToolTip::add((*i).widget,(*i).tip);
				break;
			}
			case CONFIG_SLIDER:
			{
				int minVal;
				int maxVal;
				int pageStep;
				int value;
				QStringList values= QStringList::split(",",(*i).defaultS);

				minVal=values[0].toInt();
				maxVal=values[1].toInt();
				pageStep=values[2].toInt();
				value=values[3].toInt();
				QSlider *slider=new QSlider(minVal,maxVal,pageStep,value,Qt::Horizontal,parent,(*i).caption);
				slider->setValue(-config_file.readNumEntry((*i).group,(*i).entry));
				slider->setTickmarks(QSlider::Below);
				(*i).widget=slider;
				QToolTip::add((*i).widget,(*i).tip);
				break;
			}
			case CONFIG_SPINBOX:
			{	
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel((*i).caption,hbox);			
				QStringList values= QStringList::split(",",(*i).defaultS);
				int minVal=values[0].toInt();
				int maxVal=values[1].toInt();
				int step=values[2].toInt();
				int value=values[3].toInt();
				QSpinBox *spinbox=new QSpinBox(minVal,maxVal,step,hbox);
				int val=config_file.readNumEntry((*i).group,(*i).entry);
				if (val) value=val;
				spinbox->setValue(value);
				(*i).widget=spinbox;
				QToolTip::add((*i).widget,(*i).tip);
				break;
			}
			case CONFIG_TAB:
			{
				QVBox* box = new QVBox(this,(*i).caption);
				box->setMargin(2);
				(*i).widget=box;
				addTab(box,(*i).caption);
		    		   if ((*i).caption==acttab) 
				   {
				   showPage(box);
				   kdebug("activeated Tab: "+(*i).caption+"\n");
				   }
				break;
			}
			case CONFIG_VGROUPBOX:
			{
				QVGroupBox* box = new QVGroupBox((*i).caption,parent,(*i).name);
				(*i).widget=box;
				break;
			}
			
		};			

			for(QValueList<ElementConnections>::iterator k=(*i).ConnectedSlots.begin(); k!=(*i).ConnectedSlots.end(); k++)
			{
				connect((*i).widget,(*k).signal,(*k).receiver,(*k).slot);
			}
	};
	//

		for(QValueList<ElementConnections>::iterator a=SlotsOnCreate.begin(); a!=SlotsOnCreate.end(); a++)
			connect(this,SIGNAL(create()),(*a).receiver,(*a).slot);

	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(updateConfig()));
	setCancelButton(tr("Cancel"));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close()));
	setCaption(tr("Kadu configuration"));
	resize(480,500);

	configdialog = this;
	emit create();
}

ConfigDialog::~ConfigDialog() {
	configdialog = NULL;
	acttab = QString(currentPage()->name());
	kdebug("Active tab: "+acttab+"\n");
		for(QValueList<ElementConnections>::iterator a=SlotsOnDestroy.begin(); a!=SlotsOnDestroy.end(); a++)
			connect(this,SIGNAL(destroy()),(*a).receiver,(*a).slot);

	emit destroy();
};

void ConfigDialog::registerCheckBox(
			const QString& parent,const QString& caption,
			const QString& group,const QString& entry,const bool defaultS,const QString& tip,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_CHECKBOX;
	c.parent=parent;
	c.name=name;
	c.caption=caption;
	c.group=group;
	c.entry=entry;
	c.defaultS=QString::number(defaultS);
	c.tip=tip;
	if (config_file.readEntry(group,entry)=="")
	{		
		config_file.writeEntry(group,entry,defaultS);
	}
	RegisteredControls.append(c);
};



void ConfigDialog::registerComboBox(
			const QString& parent,const QString& caption,
			const QString& group,const QString& entry,const QString& tip,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_COMBOBOX;
	c.parent=parent;
	c.name=name;
	c.caption=caption;
	c.group=group;
	c.entry=entry;
	c.tip=tip;
	
	if (config_file.readEntry(group,entry)=="")
	config_file.writeEntry(group,entry,c.defaultS);
	
	RegisteredControls.append(c);

}

void ConfigDialog::registerGrid(
			const QString& parent,const QString& caption,const int nrColumns,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_GRID;
	c.parent=parent;
	c.name=name;
	c.caption=caption;
	c.defaultS=QString::number(nrColumns);
	RegisteredControls.append(c);

}

void ConfigDialog::registerHGroupBox(
	const QString& parent,const QString& caption,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_HGROUPBOX;
	c.parent=parent;
	c.name=name;
	c.caption=caption;
	RegisteredControls.append(c);
};

void ConfigDialog::registerHotKeyEdit(
			const QString& parent,const QString& caption,
			const QString& group,const QString& entry,const QString& defaultS,const QString& tip,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_HOTKEYEDIT;
	c.parent=parent;
	c.name=name;
	c.caption=caption;
	c.group=group;
	c.entry=entry;
	c.defaultS=defaultS;
	c.tip=tip;
	RegisteredControls.append(c);
	// zapisujemy warto¶æ domy¶ln±, aby ju¿ wiêcej nie musieæ
	// jej podawaæ przy czytaniu z pliku conf
	if (config_file.readEntry(group,entry)=="")
	config_file.writeEntry(group,entry,defaultS);
};

void ConfigDialog::registerLineEdit(
			const QString& parent,const QString& caption,
			const QString& group,const QString& entry,const QString& defaultS,const QString& tip,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_LINEEDIT;
	c.parent=parent;
	c.name=name;
	c.caption=caption;
	c.group=group;
	c.entry=entry;
	c.defaultS=defaultS;
	c.tip=tip;
	if (config_file.readEntry(group,entry)=="")
	config_file.writeEntry(group,entry,defaultS);
	RegisteredControls.append(c);
};

void ConfigDialog::registerLabel(
			const QString& parent,const QString& caption,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_LABEL;
	c.parent=parent;
	c.name=name;
	c.caption=caption;
	RegisteredControls.append(c);
};


void ConfigDialog::registerListBox(
			    const QString& parent,const QString& caption,const QString& tip,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_LISTBOX;
	c.parent=parent;
	c.name=name;
	c.caption=caption;
	c.tip=tip;
	RegisteredControls.append(c);
};

void ConfigDialog::registerPushButton(
			    const QString& parent,const QString& caption,
			    const QString &iconFileName,const QString& tip,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_PUSHBUTTON;
	c.parent=parent;
	c.name=name;
	c.caption=caption;
	c.defaultS=iconFileName;
	c.tip=tip;
	RegisteredControls.append(c);
};

void ConfigDialog::registerSlider(
			    const QString& parent,const QString& caption,
			    const QString& group,const QString& entry,
			    const int minValue, const int maxValue,
			    const int pageStep,const int value,const QString& tip,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_SLIDER;
	c.parent=parent;
	c.caption=caption;
	c.name=name;
	c.group=group;
	c.entry=entry;
	c.tip=tip;
	c.defaultS=QString::number(minValue)+","+QString::number(maxValue)+","+QString::number(pageStep)+","+QString::number(value);
	RegisteredControls.append(c);

};

void ConfigDialog::registerSpinBox(
			    const QString& parent,const QString& caption,
			    const QString& group,const QString& entry,
			    const int minValue, const int maxValue,const int step,const int value,const QString& tip,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_SPINBOX;
	c.parent=parent;
	c.caption=caption;
	c.group=group;
	c.entry=entry;
	c.tip=tip;
	c.defaultS=QString::number(minValue)+","+QString::number(maxValue)+","+QString::number(step)+","+QString::number(value);
	RegisteredControls.append(c);

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

void ConfigDialog::registerVGroupBox(
	const QString& parent,const QString& caption,const QString& name)
{
	RegisteredControl c;
	c.type=CONFIG_VGROUPBOX;
	c.parent=parent;
	c.name=name;
	c.caption=caption;
	RegisteredControls.append(c);
};

void ConfigDialog::connectSlot(const QString& name,const char* signal,const QObject* receiver,const char* slot)
{
		for(QValueList<RegisteredControl>::iterator j=RegisteredControls.begin(); j!=RegisteredControls.end(); j++)
				if((*j).caption==name)
			{
	ElementConnections c;
	c.signal=signal;
	c.receiver=(QObject *)receiver;
	c.slot=slot;
	(*j).ConnectedSlots.append(c);
	kdebug("Slot connected::\n");
		    break;
			}
}

void ConfigDialog::registerSlotOnCreate(const QObject* receiver,const char* name)
{
	ElementConnections c;
	c.receiver=(QObject *)receiver;
	c.slot=name;
	SlotsOnCreate.append(c);

};

void ConfigDialog::registerSlotOnDestroy(const QObject* receiver,const char* name)
{
	ElementConnections c;
	c.receiver=(QObject *)receiver;
	c.slot=name;
	SlotsOnDestroy.append(c);

};



QWidget* ConfigDialog::getWidget(const QString& parent,const QString& caption,const QString& name)
{

	for(QValueList<RegisteredControl>::iterator j=RegisteredControls.begin(); j!=RegisteredControls.end(); j++)
				if  (((*j).parent==parent)&& ((*j).caption==caption)&& ((*j).name=name))
				{
					return ((*j).widget);
				}

};


void ConfigDialog::generateMyKeys(void) {
#ifdef HAVE_OPENSSL
	QString keyfile_path;

	
	keyfile_path.append(ggPath("keys/"));
	keyfile_path.append(QString::number(config_file.readNumEntry("Global","UIN")));
	keyfile_path.append(".pem");
	
	QFileInfo keyfile(keyfile_path);
	
	if (keyfile.permission(QFileInfo::WriteUser))
		if(QMessageBox::warning(this, "Kadu",
			tr("Keys exist. Do you want to overwrite them?"),
			tr("Yes"), tr("No"),QString::null, 0, 1)==1)
				return;
	
	QCString tmp=ggPath("keys").local8Bit();
	mkdir(tmp.data(), 0700);

//	kdebug("Generating my keys, len: %d\n", atoi(cb_keyslen->currentText()));
	if (sim_key_generate(config_file.readNumEntry("Global","UIN")) < 0) {
		QMessageBox::critical(this, "Kadu", tr("Error generating keys"), tr("OK"), QString::null, 0);
		return;
	}

	QMessageBox::information(this, "Kadu", tr("Keys have been generated and written"), tr("OK"), QString::null, 0);
#endif
}

void ConfigDialog::updateConfig(void) {

	/* and now, save it */
	saveKaduConfig();
	userlist.writeToFile();

	// nowy mechanizm
	for(QValueList<RegisteredControl>::iterator i=RegisteredControls.begin(); i!=RegisteredControls.end(); i++)
	{
		switch((*i).type)
		{
			case CONFIG_CHECKBOX:
			{
				config_file.writeEntry((*i).group,(*i).entry,((QCheckBox*)((*i).widget))->isChecked());
				break;
			}
			case CONFIG_HOTKEYEDIT:
			{
				config_file.writeEntry((*i).group,(*i).entry,((HotKey*)((*i).widget))->text());
				break;
			}
			case CONFIG_LINEEDIT:
			{
				config_file.writeEntry((*i).group,(*i).entry,((QLineEdit*)((*i).widget))->text());
				break;
			}
			case CONFIG_LISTBOX:
			{
				break;
			}
			case CONFIG_SLIDER:
			{
				config_file.writeEntry((*i).group,(*i).entry,((QSlider*)((*i).widget))->value());
				break;
			}
			case CONFIG_SPINBOX:
			{
				config_file.writeEntry((*i).group,(*i).entry,((QSpinBox*)((*i).widget))->value());
				break;
			}
		};
	};
	
	config_file.sync();
	//

	/* I odswiez okno Kadu */
	int i;	
	kadu->changeAppearance();
	for (i = 0; i < chats.count(); i++)
		chats[i].ptr->changeAppearance();
	kadu->refreshGroupTabBar();
}

void ConfigDialog::initModule()
{

// zakladka "ogolne"
	
	ConfigDialog::registerGrid(tr("General"),"grid",3);
	ConfigDialog::registerCheckBox("grid",tr("Log messages"),"Global","Logging",true);
	ConfigDialog::registerCheckBox("grid",tr("Restore window geometry"),"Global","SaveGeometry",true);
	ConfigDialog::registerCheckBox("grid",tr("Enable dock icon"),"Global","UseDocking",true);
	ConfigDialog::registerCheckBox("grid",tr("Private status"),"Global","PrivateStaus",false);
	ConfigDialog::registerCheckBox("grid",tr("Start docked"),"Global","RunDocked",false);
	ConfigDialog::registerCheckBox("grid",tr("Check for updates"),"Global","CheckUpdates",true);
	ConfigDialog::registerCheckBox("grid",tr("Add to description"),"Global","AddToDescription",false);

/*

// zakladka "dzwieki"

	ConfigDialog::registerTab(tr("Sounds"));
	ConfigDialog::registerCheckBox(tr("Sounds"),tr("Play sounds"),"Global","PlaySound",false);
	ConfigDialog::registerCheckBox(tr("Sounds"),tr("Play sounds using aRts! server"),"Global","PlaySoundArtsDsp",false);
	
	ConfigDialog::registerHGroupBox(tr("Sounds"),tr("Sound player"));
	ConfigDialog::registerLineEdit(tr("Sound player"),tr("Path:"),"Global","SoundPlayer","");
	ConfigDialog::registerPushButton(tr("Sound player"),"","fileopen.png");
	
	ConfigDialog::registerCheckBox(tr("Sounds"),tr("Enable volume control (player must support it)"),"Global","VolumeControl",false);
	ConfigDialog::registerGrid(tr("Sounds"),"volume",2);
	ConfigDialog::registerLabel("volume",tr("Volume"));
	ConfigDialog::registerSlider("volume","slider","Global","SoundVolume");

	ConfigDialog::registerHGroupBox(tr("Sounds"),tr("Message sound"));
	ConfigDialog::registerLineEdit(tr("Message sound"),tr("Path:"),"Global","Message_sound","");
	ConfigDialog::registerPushButton(tr("Message sound"),"","fileopen.png");
	ConfigDialog::registerPushButton(tr("Message sound"),tr("Test"));

	ConfigDialog::registerCheckBox(tr("Sounds"),tr("Play sounds from a person whilst chatting"),"Global","PlaySoundChat",true);
	ConfigDialog::registerCheckBox(tr("Sounds"),tr("Play chat sounds only when window is invisible"),"Global","PlaySoundChatInvisible",true);
	
	ConfigDialog::registerHGroupBox(tr("Sounds"),tr("Chat sound"));
	ConfigDialog::registerLineEdit(tr("Chat sound"),tr("Path:"),"Global","Chat_sound","");
	ConfigDialog::registerPushButton(tr("Chat sound"),"","fileopen.png");
	ConfigDialog::registerPushButton(tr("Chat sound"),tr("Test"));

// zakladka "rozmowa"

	ConfigDialog::registerTab(tr("Chat"));
	ConfigDialog::registerVGroupBox(tr("Chat"),tr("Emoticons"));
// wczytac wartosci do comboboxa		
	values.clear();
	values.append(tr("None"));
	values.append(tr("Static"));
	values.append(tr("Animated"));

	ConfigDialog::registerComboBox(tr("Emoticons"),tr("Emoticons:"),"Other","EmoticonsTypes",values);
	ConfigDialog::registerComboBox(tr("Emoticons"),tr("Emoticons theme"),"Other","EmoticonsTheme",QStringList(""));

	ConfigDialog::registerVGroupBox(tr("Chat"),tr("WWW options"));
	ConfigDialog::registerCheckBox(tr("WWW options"),tr("Use default Web browser"),"WWW","DefaultWebBrowser",true);
	ConfigDialog::registerLineEdit(tr("WWW options"),tr("Custom Web browser"),"WWW","WebBrowser");
	
	ConfigDialog::registerCheckBox(tr("Chat"),tr("Automatically prune chat messages"),"Other","ChatPrune",false);
	
	ConfigDialog::registerHGroupBox(tr("Chat"),tr("Message pruning"));
	ConfigDialog::registerLineEdit(tr("Message pruning"),tr("Reduce the number of visible messages to"),"Other","ChatPruneLen","20");
	
	ConfigDialog::registerCheckBox(tr("Chat"),tr("Use encryption"),"Other","Encryption",false);
	
	ConfigDialog::registerHGroupBox(tr("Chat"),tr("Encryption properties"));
	values.clear();
	values.append("128");
	values.append("256");
	values.append("512");
	values.append("1024");	
	ConfigDialog::registerComboBox(tr("Encryption properties"),tr("Keys length"),"Other","KeyLength",values);
	ConfigDialog::registerPushButton(tr("Encryption properties"),tr("Generate keys"));
	
	ConfigDialog::registerCheckBox(tr("Chat"),tr("Scroll chat window downward, not upward"),"Other","ScrollDown",true);
	ConfigDialog::registerCheckBox(tr("Chat"),tr("\"Enter\" key in chat sends message by default"),"Other","AutoSend",true);
	ConfigDialog::registerCheckBox(tr("Chat"),tr("Message acknowledgements (wait for delivery)"),"Other","MessageAcks",true);
	ConfigDialog::registerCheckBox(tr("Chat"),tr("Flash chat title on new message"),"Other","BlinkChatTitle",true);
	ConfigDialog::registerCheckBox(tr("Chat"),tr("Ignore messages from anonymous users"),"Other","IgnoreAnonymousUsers",false);
	ConfigDialog::registerCheckBox(tr("Chat"),tr("Show tray hint on new message"),"Other","HintAlert",false);

// zakladka "powiadom"
	ConfigDialog::registerTab(tr("Users"));
	ConfigDialog::registerCheckBox(tr("Users"),tr("Notify when users become available"),"Notify","NotifyStatuschange",false);
	ConfigDialog::registerCheckBox(tr("Users"),tr("Notify about all users"),"Notify","NotifyAboutAll",false);
	ConfigDialog::registerGrid(tr("Users"),"listboxy",3);
	
	ConfigDialog::registerGrid("listboxy","listbox1",1);
	ConfigDialog::registerLabel("listbox1",tr("Available"));
	ConfigDialog::registerListBox("listbox1","available");
	
	ConfigDialog::registerGrid("listboxy","listbox2",1);
	ConfigDialog::registerPushButton("listbox2","","forward.png");
	ConfigDialog::registerPushButton("listbox2","","back.png");
	
	ConfigDialog::registerGrid("listboxy","listbox3",1);
	ConfigDialog::registerLabel("listbox3",tr("Tracked"));
	ConfigDialog::registerListBox("listbox3","tracked");
	
	ConfigDialog::registerVGroupBox(tr("Users"),tr("Notify options"));
	ConfigDialog::registerCheckBox(tr("Notify options"),tr("Notify by sound"),"Notify","NotifyWithSound",false);
	
	ConfigDialog::registerHGroupBox(tr("Notify options"),tr("Notify sound"));
	ConfigDialog::registerLineEdit(tr("Notify sound"),tr("Path:"),"Notify","NotifySound");
	ConfigDialog::registerPushButton(tr("Notify sound"),"","fileopen.png");
	ConfigDialog::registerPushButton(tr("Notify sound"),tr("Test"));
	ConfigDialog::registerCheckBox(tr("Notify options"),tr("Notify by dialog box"),"Notify","NotifyWithSound",false);
	ConfigDialog::registerCheckBox(tr("Notify options"),tr("Notify by hint"),"Notify","NotifyWithHint",false);
	
*/

//zakladka "siec"
	ConfigSlots *configslots= new ConfigSlots();
	
	ConfigDialog::registerTab(tr("Network"));
	ConfigDialog::registerCheckBox(tr("Network"),tr("DCC enabled"),"Global","AllowDCC",false);
	ConfigDialog::registerCheckBox(tr("Network"),tr("DCC IP autodetection"),"Global","DccIpDetect",false);
	
	ConfigDialog::registerVGroupBox(tr("Network"),tr("DCC IP"));
	ConfigDialog::registerLineEdit(tr("DCC IP"),tr("IP address:"),"Global","DccIp","");
	ConfigDialog::registerCheckBox(tr("Network"),tr("DCC forwarding enabled"),"Global","DccForwarding",false);
	
	ConfigDialog::registerVGroupBox(tr("Network"),tr("DCC forwarding properties"));
	ConfigDialog::registerLineEdit(tr("DCC forwarding properties"),tr("External IP address:"),"Global","ExternalIP","");
	ConfigDialog::registerLineEdit(tr("DCC forwarding properties"),tr("External TCP port:"),"Global","ExternalPort","0");

	ConfigDialog::registerVGroupBox(tr("Network"),tr("Servers properties"));
	ConfigDialog::registerGrid(tr("Servers properties"),"servergrid",2);
	ConfigDialog::registerCheckBox("servergrid",tr("Use default servers"),"Global","isDefServers",true);
	ConfigDialog::registerCheckBox("servergrid",tr("Use TLSv1"),"Global","UseTLS",false);
	ConfigDialog::registerLineEdit(tr("Servers properties"),tr("IP addresses:"),"Global","Server","");


	ConfigDialog::registerComboBox(tr("Servers properties"),tr("Default port to connect to servers"),"Global","DefaultPort", "");
	ConfigDialog::registerCheckBox(tr("Network"),tr("Use proxy server"),"Proxy","UseProxy",false);

	ConfigDialog::registerVGroupBox(tr("Network"),tr("Proxy server"));
	ConfigDialog::registerGrid(tr("Proxy server"),"proxygrid",2);
	ConfigDialog::registerLineEdit("proxygrid",tr("IP addresses:"),"Proxy","ProxyHost","");
	ConfigDialog::registerLineEdit("proxygrid",tr("Port:"),"Proxy","ProxyPort","0");
	ConfigDialog::registerLineEdit("proxygrid",tr("Username:"),"Proxy","ProxyUser","");
	ConfigDialog::registerLineEdit("proxygrid",tr("Password:"),"Proxy","ProxyPassword","");
	
	ConfigDialog::registerSlotOnCreate(configslots,SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnDestroy(configslots,SLOT(onDestroyConfigDialog()));
	
	ConfigDialog::connectSlot(tr("DCC enabled"),SIGNAL(toggled(bool)),configslots,SLOT(ifDccEnabled(bool)));
	ConfigDialog::connectSlot(tr("DCC IP autodetection"),SIGNAL(toggled(bool)),configslots,SLOT(ifDccIpEnabled(bool)));
	ConfigDialog::connectSlot(tr("Use default servers"),SIGNAL(toggled(bool)),configslots,SLOT(ifDefServerEnabled(bool)));
	ConfigDialog::connectSlot(tr("Use TLSv1"),SIGNAL(toggled(bool)),configslots,SLOT(useTlsEnabled(bool)));
	
	
}	

void ConfigSlots::onCreateConfigDialog()
{
	kdebug("ConfigSlots::onCreateConfigDialog() \n");

	QCheckBox *b_dccenabled = (QCheckBox*)(ConfigDialog::getWidget(tr("Network"),tr("DCC enabled")));
	QCheckBox *b_dccip= (QCheckBox*)(ConfigDialog::getWidget(tr("Network"),tr("DCC IP autodetection")));
	QVGroupBox *g_dccip = (QVGroupBox*)(ConfigDialog::getWidget(tr("Network"),tr("DCC IP")));
	QVGroupBox *g_proxy = (QVGroupBox*)(ConfigDialog::getWidget(tr("Network"),tr("Proxy server")));
	QVGroupBox *g_fwdprop =(QVGroupBox*)(ConfigDialog::getWidget(tr("Network"),tr("DCC forwarding properties")));
	QCheckBox *b_dccfwd =(QCheckBox*)(ConfigDialog::getWidget(tr("Network"),tr("DCC forwarding enabled")));
	QCheckBox *b_tls=(QCheckBox*)(ConfigDialog::getWidget("servergrid",tr("Use TLSv1")));
	QCheckBox *b_useproxy=(QCheckBox*)(ConfigDialog::getWidget(tr("Network"),tr("Use proxy server")));
	QComboBox *cb_portselect=(QComboBox*)(ConfigDialog::getWidget(tr("Servers properties"),tr("Default port to connect to servers")));
	QHBox *serverbox=(QHBox*)((ConfigDialog::getWidget(tr("Servers properties"),tr("IP addresses:")))->parent());
	QCheckBox* b_defaultserver=(QCheckBox*)(ConfigDialog::getWidget("servergrid",tr("Use default servers")));
	
	b_dccip->setEnabled(b_dccenabled->isChecked());
	g_dccip->setEnabled(b_dccip->isChecked()&& b_dccenabled->isChecked());
	b_dccfwd->setEnabled(b_dccenabled->isChecked());
	g_fwdprop->setEnabled(b_dccenabled->isChecked() && b_dccfwd->isChecked());
	g_proxy->setEnabled(b_useproxy->isChecked());
	((QHBox*)cb_portselect->parent())->setEnabled(!b_tls->isChecked());
	serverbox->setEnabled(!b_defaultserver->isChecked());
	cb_portselect->insertItem("8074");
	cb_portselect->insertItem("443");
	cb_portselect->setCurrentText(config_file.readEntry("Global","DefaultPort","8074"));
	
	connect(b_dccfwd, SIGNAL(toggled(bool)), g_fwdprop, SLOT(setEnabled(bool)));
#ifdef HAVE_OPENSSL
       connect(b_tls, SIGNAL(toggled(bool)), this, SLOT(useTlsEnabled(bool)));
#endif
       connect(b_useproxy, SIGNAL(toggled(bool)), g_proxy, SLOT(setEnabled(bool)));
};

void ConfigSlots::onDestroyConfigDialog()
{
	kdebug("ConfigSlots::onDestroyConfigDialog() \n");

	QComboBox *cb_portselect=(QComboBox*)(ConfigDialog::getWidget(tr("Servers properties"),tr("Default port to connect to servers")));
	config_file.writeEntry("Global","DefaultPort",cb_portselect->currentText());
};


void ConfigSlots::ifDccEnabled(bool value)
{
	kdebug("ConfigSlots::ifDccEnabled() \n");

	QCheckBox *b_dccip= (QCheckBox*)(ConfigDialog::getWidget(tr("Network"),tr("DCC IP autodetection")));
	QVGroupBox *g_dccip = (QVGroupBox*)(ConfigDialog::getWidget(tr("Network"),tr("DCC IP")));
	QVGroupBox *g_fwdprop =(QVGroupBox*)(ConfigDialog::getWidget(tr("Network"),tr("DCC forwarding properties")));
	QCheckBox *b_dccfwd =(QCheckBox*)(ConfigDialog::getWidget(tr("Network"),tr("DCC forwarding enabled")));
	
	b_dccip->setEnabled(value);
	g_dccip->setEnabled(b_dccip->isChecked()&& value);	
	b_dccfwd->setEnabled(value);
	g_fwdprop->setEnabled(b_dccfwd->isChecked() &&value);
};

void ConfigSlots::ifDccIpEnabled(bool value)
{
	kdebug("ConfigSlots::ifDccIpEnabled() \n");
	QVGroupBox *g_dccip = (QVGroupBox*)(ConfigDialog::getWidget(tr("Network"),tr("DCC IP")));
	g_dccip->setEnabled(value);
};

void ConfigSlots::ifDefServerEnabled(bool value)
{
	kdebug("ConfigSlots::ifDefServerEnabled() \n");
	QHBox *serverbox=(QHBox*)((ConfigDialog::getWidget(tr("Servers properties"),tr("IP addresses:")))->parent());
	serverbox->setEnabled(!value);	
};

void ConfigSlots::useTlsEnabled(bool value)
{
	kdebug("ConfigSlots::useTlsEnabled() \n");
	QHBox *box_portselect=(QHBox*)(ConfigDialog::getWidget(tr("Servers properties"),tr("Default port to connect to servers"))->parent());
	box_portselect->setEnabled(!value);
};


QValueList<ConfigDialog::RegisteredControl> ConfigDialog::RegisteredControls;
QValueList<ConfigDialog::ElementConnections> ConfigDialog::SlotsOnCreate;
QValueList<ConfigDialog::ElementConnections> ConfigDialog::SlotsOnDestroy;
