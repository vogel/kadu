/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qstylesheet.h>

#include "hint_manager.h"
#include "hint_manager_slots.h"
#include "debug.h"
#include "config_file.h"
#include "config_dialog.h"
#include "../notify/notify.h"
#include "chat.h"
#include "userlist.h"
#include "misc.h"
#include "kadu.h"

#define FRAME_WIDTH 1

HintManager::HintManager()
	: QFrame(0, "HintManager", WStyle_NoBorder | WStyle_StaysOnTop | WStyle_Tool | WX11BypassWM | WWinOwnDC)
{
	kdebugf();

	setFrameStyle(QFrame::Box | QFrame::Plain);
	setLineWidth(FRAME_WIDTH);

	grid = new QGridLayout(this, 0, 0, 1, 0, "QGridLayout");
	grid->setResizeMode(QLayout::Fixed);

	hints.setAutoDelete(true);

	hint_timer = new QTimer(this);
	connect(hint_timer, SIGNAL(timeout()), this, SLOT(oneSecond()));

/* Zak³adka konfiguracyjna */
	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Hints"));
	
	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "New chat / new message"));
		ConfigDialog::addCheckBox("Hints", "New chat / new message", QT_TRANSLATE_NOOP("@default", "Show message content in hint"), "ShowContentMessage", false);
		ConfigDialog::addSpinBox("Hints", "New chat / new message", QT_TRANSLATE_NOOP("@default", "Number of quoted characters"), "CiteSign", 10, 1000, 1, 50);
	
	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "Status change"));
		ConfigDialog::addCheckBox("Hints", "Status change", QT_TRANSLATE_NOOP("@default", "Add description to hint if exists"), "NotifyHintDescription", false);
		ConfigDialog::addCheckBox("Hints", "Status change", QT_TRANSLATE_NOOP("@default", "Use custom syntax"), "NotifyHintUseSyntax", false);
		ConfigDialog::addLineEdit("Hints", "Status change", QT_TRANSLATE_NOOP("@default", "Hint syntax"), "NotifyHintSyntax", "");

	QStringList options;
	QStringList values;
	options<<tr("Nothing")<<tr("Open chat")<<tr("Delete hint")<<tr("Delete all hints");
	values<<"0"<<"1"<<"2"<<"3";
	
	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "Mouse buttons"));
		ConfigDialog::addComboBox("Hints", "Mouse buttons", QT_TRANSLATE_NOOP("@default", "Left button"), "LeftButton", options, values, "1");
		ConfigDialog::addComboBox("Hints", "Mouse buttons", QT_TRANSLATE_NOOP("@default", "Middle button"), "MiddleButton", options, values, "3");
		ConfigDialog::addComboBox("Hints", "Mouse buttons", QT_TRANSLATE_NOOP("@default", "Right button"), "RightButton", options, values, "2");
	
	ConfigDialog::addHBox("Hints", "Hints", "hints-hbox");
		QStringList options2;
		QStringList values2;
		options2<<tr("Auto")<<tr("On top")<<tr("On bottom");
		values2<<"0"<<"1"<<"2";
		ConfigDialog::addVRadioGroup("Hints", "hints-hbox", QT_TRANSLATE_NOOP("@default", "New hints go"), "NewHintUnder", options2, values2, "0");

		ConfigDialog::addVGroupBox("Hints", "hints-hbox", QT_TRANSLATE_NOOP("@default","Hints position"));
			ConfigDialog::addCheckBox("Hints", "Hints position", QT_TRANSLATE_NOOP("@default", "Own hints position"), "UseUserPosition", false);
			ConfigDialog::addHBox("Hints", "Hints position", "coords");
				ConfigDialog::addSpinBox("Hints", "coords", "x=", "HintsPositionX", -2048, 2048, 1, 100);
				ConfigDialog::addLabel("Hints", "coords", "", "stretcher");
				ConfigDialog::addSpinBox("Hints", "coords", "y=", "HintsPositionY", -2048, 2048, 1, 100);
	
	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "Parameters"));
		ConfigDialog::addHBox("Hints", "Parameters", "top");
			ConfigDialog::addCheckBox("Hints", "top", QT_TRANSLATE_NOOP("@default", "Set for all"), "SetAll", false);
			ConfigDialog::addLabel("Hints", "top", QT_TRANSLATE_NOOP("@default", "<b>Text</b> preview"));
			
		ConfigDialog::addHBox("Hints", "Parameters", "center");
			QStringList options3;
			QStringList values3;
			options3<<tr("Online")<<tr("Online (d.)")<<tr("Busy")<<tr("Busy (d.)")<<
					tr("Invisible")<<tr("Invisible (d.)")<<tr("Offline")<<tr("Offline (d.)")<<
					tr("Blocking")<<tr("New chat")<<tr("New message in chat")<<tr("Error");
			values3<<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11";
			ConfigDialog::addVRadioGroup("Hints", "center", QT_TRANSLATE_NOOP("@default", "Hint type"), "LastSelected", options3, values3, "0");

			ConfigDialog::addVBox("Hints", "center", "bottom");
				ConfigDialog::addLabel("Hints", "bottom", "", "stretcher2");
				ConfigDialog::addLabel("Hints", "bottom", "", "stretcher3");

				ConfigDialog::addSpinBox("Hints", "bottom", "Hint timeout", "LastTimeout", -2048, 2048, 1, 10);
				ConfigDialog::addPushButton("Hints", "bottom", QT_TRANSLATE_NOOP("@default", "Change font color"));
				ConfigDialog::addPushButton("Hints", "bottom", QT_TRANSLATE_NOOP("@default", "Change background color"));
				ConfigDialog::addPushButton("Hints", "bottom", QT_TRANSLATE_NOOP("@default", "Change font"));

	hint_manager_slots = new HintManagerSlots(); 
	ConfigDialog::registerSlotOnCreate(hint_manager_slots, SLOT(onCreateConfigDialog()));
	ConfigDialog::registerSlotOnApply(hint_manager_slots, SLOT(onApplyConfigDialog()));
	ConfigDialog::registerSlotOnClose(hint_manager_slots, SLOT(onCloseConfigDialog()));

	config_file.addVariable("Notify", "NewChat_Window", true);
	config_file.addVariable("Notify", "NewMessage_Window", true);
	config_file.addVariable("Notify", "ConnError_Window", true);
	config_file.addVariable("Notify", "ChangingStatus_Window", false);
	config_file.addVariable("Notify", "toAvailable_Window", true);
	config_file.addVariable("Notify", "toBusy_Window", true);
	config_file.addVariable("Notify", "toNotAvailable_Window", false);
	config_file.addVariable("Notify", "Message_Window", true);
	
	connect(this, SIGNAL(searchingForTrayPosition(QPoint &)), kadu, SIGNAL(searchingForTrayPosition(QPoint &)));

	QMap<QString, QString> s;
	s["NewChat"]=SLOT(newChat(UinsList, const QString &, time_t, bool &));
	s["NewMessage"]=SLOT(newMessage(UinsList, const QString &, time_t));
	s["ConnError"]=SLOT(connectionError(const QString &));
	s["ChangingStatus"]=SLOT(userChangingStatus(const UinType, const unsigned int, const unsigned int));
	s["toAvailable"]=SLOT(userChangedStatusToAvailable(const UserListElement &));
	s["toBusy"]=SLOT(userChangedStatusToBusy(const UserListElement &));
	s["toNotAvailable"]=SLOT(userChangedStatusToNotAvailable(const UserListElement &));
	s["Message"]=SLOT(message(const QString &, const QString &, const QString &, const UserListElement *));
	notify->registerNotifier("Hints", this, s);

	kdebugf2();
}

HintManager::~HintManager()
{
	kdebugf();
	notify->unregisterNotifier("Hints");
	disconnect(this, SIGNAL(searchingForTrayPosition(QPoint &)), kadu, SIGNAL(searchingForTrayPosition(QPoint &)));
	
	ConfigDialog::unregisterSlotOnCreate(hint_manager_slots, SLOT(onCreateConfigDialog()));
	ConfigDialog::unregisterSlotOnApply(hint_manager_slots, SLOT(onApplyConfigDialog()));
	ConfigDialog::unregisterSlotOnClose(hint_manager_slots, SLOT(onCloseConfigDialog()));

	delete hint_manager_slots;
	hint_manager_slots=NULL;

	ConfigDialog::removeControl("Hints", "Change font");
	ConfigDialog::removeControl("Hints", "Change background color");
	ConfigDialog::removeControl("Hints", "Change font color");
	ConfigDialog::removeControl("Hints", "Hint timeout");
	ConfigDialog::removeControl("Hints", "", "stretcher3");
	ConfigDialog::removeControl("Hints", "", "stretcher2");
	ConfigDialog::removeControl("Hints", "bottom");
	ConfigDialog::removeControl("Hints", "Hint type");
	ConfigDialog::removeControl("Hints", "center");
	ConfigDialog::removeControl("Hints", "<b>Text</b> preview");
	ConfigDialog::removeControl("Hints", "Set for all");
	ConfigDialog::removeControl("Hints", "top");
	ConfigDialog::removeControl("Hints", "Parameters");
	ConfigDialog::removeControl("Hints", "y=");
	ConfigDialog::removeControl("Hints", "", "stretcher");
	ConfigDialog::removeControl("Hints", "x=");
	ConfigDialog::removeControl("Hints", "coords");
	ConfigDialog::removeControl("Hints", "Own hints position");
	ConfigDialog::removeControl("Hints", "Hints position");
	ConfigDialog::removeControl("Hints", "New hints go");
	ConfigDialog::removeControl("Hints", "hints-hbox");
	ConfigDialog::removeControl("Hints", "Right button");
	ConfigDialog::removeControl("Hints", "Middle button");
	ConfigDialog::removeControl("Hints", "Left button");
	ConfigDialog::removeControl("Hints", "Mouse buttons");
	ConfigDialog::removeControl("Hints", "Hint syntax");
	ConfigDialog::removeControl("Hints", "Use custom syntax");
	ConfigDialog::removeControl("Hints", "Add description to hint if exists");
	ConfigDialog::removeControl("Hints", "Status change");
	ConfigDialog::removeControl("Hints", "Number of quoted characters");
	ConfigDialog::removeControl("Hints", "Show message content in hint");
	ConfigDialog::removeControl("Hints", "New chat / new message");
	ConfigDialog::removeTab("Hints");
	
	kdebugf2();
}

void HintManager::setHint(void)
{
	kdebugf();
	QPoint newPosition;
	QPoint trayPosition;
	QSize preferredSize = sizeHint();
	QSize desktopSize = QApplication::desktop()->size();

	emit searchingForTrayPosition(trayPosition);
	if (config_file.readBoolEntry("Hints", "UseUserPosition") || trayPosition.isNull())
	{
		newPosition=QPoint(config_file.readNumEntry("Hints","HintsPositionX"),config_file.readNumEntry("Hints","HintsPositionY"));
		if (newPosition.x()+preferredSize.width()>=desktopSize.width()) //gdy hinty wyje¿d¿aj± poza ekran w prawo
			newPosition.setX(desktopSize.width()-preferredSize.width());
		if (newPosition.y()+preferredSize.height()>=desktopSize.height()) //gdy hinty wyje¿d¿aj± poza ekran u do³u
			newPosition.setY(desktopSize.height()-preferredSize.height());
	}
	else
	{
		if (trayPosition.x() < desktopSize.width()/2) // gdy tray jest z lewej
			newPosition.setX(trayPosition.x()+32);
		else //gdy tray jest z prawej
			newPosition.setX(trayPosition.x()-preferredSize.width());
		
		if (trayPosition.y() < desktopSize.height()/2) //gdy tray jest u góry
			newPosition.setY(trayPosition.y()+32);
		else //gdy tray jest u do³u
			newPosition.setY(trayPosition.y()-preferredSize.height());
	}
	move (newPosition);
	kdebugf2();
}

void HintManager::deleteHint(unsigned int id)
{
	kdebugm(KDEBUG_FUNCTION_START, "HintManager::deleteHint() id=%d\n", id);
#if QT_VERSION >= 0x030100
	grid->removeItem(hints.at(id));
#endif
	hints.remove(id);
	if (!hints.count())
	{
		hint_timer->stop();
		hide();
		return;
	}
	for (unsigned int i = id; i < hints.count(); i++)
		hints.at(i)->setId(i);
	setHint();
	kdebugf2();
}

void HintManager::oneSecond(void)
{
	kdebugf();
	for (unsigned int i = 0; i < hints.count(); i++)
		if (!(hints.at(i)->nextSecond()))
			deleteHint(i--);
	kdebugf2();
}

void HintManager::leftButtonSlot(unsigned int id)
{
	kdebugm(KDEBUG_FUNCTION_START, "HintManager::leftButtonSlot() %d\n", id);
	switch(config_file.readNumEntry("Hints", "LeftButton"))
	{
		case 1:
			openChat(id);
			break;
		case 2:
			deleteHint(id);
			break;
		case 3:
			deleteAllHints();
			break;
	}
	kdebugf2();
}

void HintManager::rightButtonSlot(unsigned int id)
{
	kdebugm(KDEBUG_FUNCTION_START, "HintManager::rightButtonSlot() %d\n", id);
	switch(config_file.readNumEntry("Hints", "RightButton"))
	{
		case 1:
			openChat(id);
			break;
		case 2:
			deleteHint(id);
			break;
		case 3:
			deleteAllHints();
			break;
	}
	kdebugf2();
}

void HintManager::midButtonSlot(unsigned int id)
{
	kdebugm(KDEBUG_FUNCTION_START, "HintManager::midButtonSlot() %d\n", id);
	switch(config_file.readNumEntry("Hints", "MiddleButton"))
	{
		case 1:
			openChat(id);
			break;
		case 2:
			deleteHint(id);
			break;
		case 3:
			deleteAllHints();
			break;
	}
	kdebugf2();
}

void HintManager::openChat(unsigned int id)
{
	kdebugf();
	UinsList senders=hints.at(id)->getUins();
	if(senders.size()!=0)
		chat_manager->openPendingMsgs(senders);
	deleteHint(id);
	kdebugf2();
}

void HintManager::deleteAllHints()
{
	kdebugf();
	hint_timer->stop();
#if QT_VERSION >= 0x030100
	for (unsigned int i = 0; i < hints.count(); i++)
		grid->removeItem(hints.at(i));
#endif
	hints.clear();
	hide();
	kdebugf2();
}

void HintManager::addHint(const QString& text, const QPixmap& pixmap, const QFont &font, const QColor &color, const QColor &bgcolor, unsigned int timeout, UinsList *senders)
{
	kdebugf();
	hints.append(new Hint(this, text, pixmap, timeout));
	int i = hints.count()-1;
	setGridOrigin();
	grid->addLayout(hints.at(i), i, 0);
	hints.at(i)->set(font, color, bgcolor, i);

	if(senders)
		hints.at(i)->setUins(*senders);

	connect(hints.at(i), SIGNAL(leftButtonClicked(unsigned int)), this, SLOT(leftButtonSlot(unsigned int)));
	connect(hints.at(i), SIGNAL(rightButtonClicked(unsigned int)), this, SLOT(rightButtonSlot(unsigned int)));
	connect(hints.at(i), SIGNAL(midButtonClicked(unsigned int)), this, SLOT(midButtonSlot(unsigned int)));
	setHint();

	if (!hint_timer->isActive())
		hint_timer->start(1000);
	if (isHidden())
		show();
	kdebugf2();
}


void HintManager::setGridOrigin()
{
	kdebugf();
	QPoint trayPosition;
	emit searchingForTrayPosition(trayPosition);
	switch(config_file.readNumEntry("Hints", "NewHintUnder"))
	{
		case 0:
			if (trayPosition.isNull() || config_file.readBoolEntry("Hints","UseUserPosition"))
			{
				if (config_file.readNumEntry("Hints","HintsPositionY") < QApplication::desktop()->size().height()/2)
					grid->setOrigin(QGridLayout::TopLeft);
				else
					grid->setOrigin(QGridLayout::BottomLeft);
			}
			else			
			{
				if (trayPosition.y() < QApplication::desktop()->size().height()/2)
					grid->setOrigin(QGridLayout::TopLeft);
				else
					grid->setOrigin(QGridLayout::BottomLeft);
			}
			break;
		case 1:
			grid->setOrigin(QGridLayout::BottomLeft);
			break;
		case 2:
			grid->setOrigin(QGridLayout::TopLeft);
			break;
	}
	kdebugf2();
}

void HintManager::newChat(UinsList senders, const QString& msg, time_t time, bool &grab)
{
	kdebugf();
	if (config_file.readBoolEntry("Hints", "ShowContentMessage"))
	{
		unsigned int citeSign=config_file.readUnsignedNumEntry("Hints","CiteSign");
		QString cite;
		if (msg.length() <= citeSign)
			cite = msg;
		else
			cite = msg.left(citeSign)+"...";
		addHint(tr("Chat with <b>%1</b><br/> <small>%2</small>")
			.arg(userlist.byUinValue(senders[0]).altnick).arg(cite),
			icons_manager.loadIcon("Message"),
			config_file.readFontEntry("Hints", "HintNewChat_font"),
			config_file.readColorEntry("Hints", "HintNewChat_fgcolor"),
			config_file.readColorEntry("Hints", "HintNewChat_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintNewChat_timeout"),
			&senders);
	}
	else
		addHint(tr("Chat with <b>%1</b>").arg(userlist.byUinValue(senders[0]).altnick),
			icons_manager.loadIcon("Message"),
			config_file.readFontEntry("Hints", "HintNewChat_font"),
			config_file.readColorEntry("Hints", "HintNewChat_fgcolor"),
			config_file.readColorEntry("Hints", "HintNewChat_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintNewChat_timeout"),
			&senders);
	kdebugf2();
}

void HintManager::newMessage(UinsList senders, const QString& msg, time_t time)
{
	kdebugf();
	if (config_file.readBoolEntry("Hints", "ShowContentMessage"))
	{
		unsigned int citeSign=config_file.readUnsignedNumEntry("Hints","CiteSign");
		QString cite;
		if (msg.length() <= citeSign)
			cite = msg;
		else
			cite = msg.left(citeSign)+"...";
		addHint(tr("New message from <b>%1</b><br/> <small>%2</small>")
			.arg(userlist.byUinValue(senders[0]).altnick).arg(cite),
			icons_manager.loadIcon("Message"),
			config_file.readFontEntry("Hints", "HintNewMessage_font"),
			config_file.readColorEntry("Hints", "HintNewMessage_fgcolor"),
			config_file.readColorEntry("Hints", "HintNewMessage_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintNewMessage_timeout"));
	}
	else
		addHint(tr("New message from <b>%1</b>").arg(userlist.byUinValue(senders[0]).altnick),
			icons_manager.loadIcon("Message"),
			config_file.readFontEntry("Hints", "HintNewMessage_font"),
			config_file.readColorEntry("Hints", "HintNewMessage_fgcolor"),
			config_file.readColorEntry("Hints", "HintNewMessage_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintNewMessage_timeout"));

	kdebugf2();
}

void HintManager::connectionError(const QString &message)
{
	kdebugf();
	addHint(tr("<b>Error:</b> %1").arg(message), icons_manager.loadIcon("Blocking"),
		config_file.readFontEntry("Hints", "HintError_font"),
		config_file.readColorEntry("Hints", "HintError_fgcolor"),
		config_file.readColorEntry("Hints", "HintError_bgcolor"),
		config_file.readUnsignedNumEntry("Hints", "HintError_timeout"));
	kdebugf2();
}

void HintManager::userChangingStatus(const UinType uin, const unsigned int oldstatus, const unsigned int status)
{
	kdebugf();
	UserListElement &ule=userlist.byUin(uin);
	int statusnr = statusGGToStatusNr(status);

	QString stat;
	if (statusnr==0 || statusnr==1)
		stat="Online";
	else if (statusnr==2 || statusnr==3)
		stat="Busy";
	else if (statusnr>=4 && statusnr<=7)
		stat="Offline";

	if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
		addHint(parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true),
			icons_manager.loadIcon(gg_icons[statusnr]),
			config_file.readFontEntry("Hints", "Hint"+stat+"_font"),
			config_file.readColorEntry("Hints", "Hint"+stat+"_fgcolor"),
			config_file.readColorEntry("Hints", "Hint"+stat+"_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "Hint"+stat+"_timeout"));
	else
		if (ifStatusWithDescription(status) && config_file.readBoolEntry("Hints","NotifyHintDescription"))
			addHint(tr("<b>%1</b> changed status to <i>%2</i><br/> <small>%3</small>")
				.arg(ule.altnick)
				.arg(qApp->translate("@default", statustext[statusnr]))
				.arg(QStyleSheet::escape(ule.description)),
				icons_manager.loadIcon(gg_icons[statusnr]),
				config_file.readFontEntry("Hints", "Hint"+stat+"D_font"),
				config_file.readColorEntry("Hints", "Hint"+stat+"D_fgcolor"),
				config_file.readColorEntry("Hints", "Hint"+stat+"D_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "Hint"+stat+"D_timeout"));
		else
			addHint(tr("<b>%1</b> changed status to <i>%2</i>").arg(ule.altnick)
				.arg(qApp->translate("@default", statustext[statusnr])),
				icons_manager.loadIcon(gg_icons[statusnr]),
				config_file.readFontEntry("Hints", "Hint"+stat+"_font"),
				config_file.readColorEntry("Hints", "Hint"+stat+"_fgcolor"),
				config_file.readColorEntry("Hints", "Hint"+stat+"_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "Hint"+stat+"_timeout"));
	kdebugf2();
}

void HintManager::userChangedStatusToAvailable(const UserListElement &ule)
{
	kdebugf();
	int statusnr = statusGGToStatusNr(ule.status);
	if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
		addHint(parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true),
			icons_manager.loadIcon(gg_icons[statusnr]),
			config_file.readFontEntry("Hints", "HintOnline_font"),
			config_file.readColorEntry("Hints", "HintOnline_fgcolor"),
			config_file.readColorEntry("Hints", "HintOnline_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintOnline_timeout"));
	else
		if (ifStatusWithDescription(ule.status) && config_file.readBoolEntry("Hints","NotifyHintDescription"))
			addHint(tr("<b>%1</b> is available<br/> <small>%2</small>")
				.arg(ule.altnick).arg(QStyleSheet::escape(ule.description)),
				icons_manager.loadIcon(gg_icons[statusnr]),
				config_file.readFontEntry("Hints", "HintOnlineD_font"),
				config_file.readColorEntry("Hints", "HintOnlineD_fgcolor"),
				config_file.readColorEntry("Hints", "HintOnlineD_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintOnlineD_timeout"));
		else
			addHint(tr("<b>%1</b> is available").arg(ule.altnick),
				icons_manager.loadIcon(gg_icons[statusnr]),
				config_file.readFontEntry("Hints", "HintOnline_font"),
				config_file.readColorEntry("Hints", "HintOnline_fgcolor"),
				config_file.readColorEntry("Hints", "HintOnline_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintOnline_timeout"));
	kdebugf2();
}

void HintManager::userChangedStatusToBusy(const UserListElement &ule)
{
	kdebugf();
	int statusnr = statusGGToStatusNr(ule.status);
	if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
		addHint(parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true),
			icons_manager.loadIcon(gg_icons[statusnr]),
			config_file.readFontEntry("Hints", "HintBusy_font"),
			config_file.readColorEntry("Hints", "HintBusy_fgcolor"),
			config_file.readColorEntry("Hints", "HintBusy_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintBusy_timeout"));
	else
		if (ifStatusWithDescription(ule.status) && config_file.readBoolEntry("Hints","NotifyHintDescription"))
			addHint(tr("<b>%1</b> is busy<br/> <small>%2</small>")
				.arg(ule.altnick).arg(QStyleSheet::escape(ule.description)),
				icons_manager.loadIcon(gg_icons[statusnr]),
				config_file.readFontEntry("Hints", "HintBusyD_font"),
				config_file.readColorEntry("Hints", "HintBusyD_fgcolor"),
				config_file.readColorEntry("Hints", "HintBusyD_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintBusyD_timeout"));
		else
			addHint(tr("<b>%1</b> is busy").arg(ule.altnick),
				icons_manager.loadIcon(gg_icons[statusnr]),
				config_file.readFontEntry("Hints", "HintBusy_font"),
				config_file.readColorEntry("Hints", "HintBusy_fgcolor"),
				config_file.readColorEntry("Hints", "HintBusy_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintBusy_timeout"));
	kdebugf2();
}

void HintManager::userChangedStatusToNotAvailable(const UserListElement &ule)
{
	kdebugf();
	int statusnr = statusGGToStatusNr(ule.status);
	if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
		addHint(parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true),
			icons_manager.loadIcon(gg_icons[statusnr]),
			config_file.readFontEntry("Hints", "HintOffline_font"),
			config_file.readColorEntry("Hints", "HintOffline_fgcolor"),
			config_file.readColorEntry("Hints", "HintOffline_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintOffline_timeout"));
	else
		if (ifStatusWithDescription(ule.status) && config_file.readBoolEntry("Hints","NotifyHintDescription"))
			addHint(tr("<b>%1</b> is available<br/> <small>%2</small>")
				.arg(ule.altnick).arg(QStyleSheet::escape(ule.description)),
				icons_manager.loadIcon(gg_icons[statusnr]),
				config_file.readFontEntry("Hints", "HintOfflineD_font"),
				config_file.readColorEntry("Hints", "HintOfflineD_fgcolor"),
				config_file.readColorEntry("Hints", "HintOfflineD_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintOfflineD_timeout"));
		else
			addHint(tr("<b>%1</b> is available").arg(ule.altnick),
				icons_manager.loadIcon(gg_icons[statusnr]),
				config_file.readFontEntry("Hints", "HintOffline_font"),
				config_file.readColorEntry("Hints", "HintOffline_fgcolor"),
				config_file.readColorEntry("Hints", "HintOffline_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintOffline_timeout"));
	kdebugf2();
}

void HintManager::message(const QString &from, const QString &type, const QString &message, const UserListElement *ule)
{
	kdebugf();
	kdebugf2();
}

HintManager *hint_manager=NULL;
