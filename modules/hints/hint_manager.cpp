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

#include "chat.h"
#include "chat_manager.h"
#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "hint_manager.h"
#include "hint_manager_slots.h"
#include "icons_manager.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "misc.h"
#include "../notify/notify.h"
#include "userbox.h"
#include "userlist.h"

/**
 * @ingroup hints
 * @{
 */
#define FRAME_WIDTH 1

HintManager::HintManager(QWidget *parent, const char *name)	: Notifier(parent, name),
	frame(0), hint_manager_slots(0), hint_timer(new QTimer(this, "hint_timer")),
	hints(), tipFrame(0)
{
	kdebugf();
	frame = new QFrame(parent, name, WStyle_NoBorder | WStyle_StaysOnTop | WStyle_Tool | WX11BypassWM | WWinOwnDC);

	frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	frame->setFrameStyle(QFrame::Box | QFrame::Plain);
	frame->setLineWidth(FRAME_WIDTH);

	layout = new QVBoxLayout(frame, FRAME_WIDTH, 0, "grid");
	layout->setResizeMode(QLayout::Fixed);

	connect(hint_timer, SIGNAL(timeout()), this, SLOT(oneSecond()));

	ConfigDialog::addTab(QT_TRANSLATE_NOOP("@default", "Hints"), "HintsTab");

	QStringList options=toStringList(tr("Nothing"),tr("Open chat"),tr("Delete hint"),tr("Delete all hints"));
	QStringList values=toStringList("0","1","2","3");

	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "Mouse buttons"));
		ConfigDialog::addComboBox("Hints", "Mouse buttons", QT_TRANSLATE_NOOP("@default", "Left button"), "LeftButton", options, values, "1");
		ConfigDialog::addComboBox("Hints", "Mouse buttons", QT_TRANSLATE_NOOP("@default", "Middle button"), "MiddleButton", options, values, "3");
		ConfigDialog::addComboBox("Hints", "Mouse buttons", QT_TRANSLATE_NOOP("@default", "Right button"), "RightButton", options, values, "2");

	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "New chat / new message"), 0, Advanced);
		ConfigDialog::addCheckBox("Hints", "New chat / new message", QT_TRANSLATE_NOOP("@default", "Show message content in hint"), "ShowContentMessage", true);
		ConfigDialog::addCheckBox("Hints", "New chat / new message", QT_TRANSLATE_NOOP("@default", "Delete pending message when user deletes hint"), "DeletePendingMsgWhenHintDeleted", false);
		ConfigDialog::addSpinBox("Hints", "New chat / new message", QT_TRANSLATE_NOOP("@default", "Number of quoted characters"), "CiteSign", 10, 1000, 1, 50);

	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "Status change"), 0, Advanced);
		ConfigDialog::addCheckBox("Hints", "Status change", QT_TRANSLATE_NOOP("@default", "Open chat on click"), "OpenChatOnClick", false);
		ConfigDialog::addCheckBox("Hints", "Status change", QT_TRANSLATE_NOOP("@default", "Add description to hint if exists"), "NotifyHintDescription", false);
		ConfigDialog::addCheckBox("Hints", "Status change", QT_TRANSLATE_NOOP("@default", "Use custom syntax"), "NotifyHintUseSyntax", false, 0, 0, Expert);
		ConfigDialog::addLineEdit("Hints", "Status change", QT_TRANSLATE_NOOP("@default", "Hint syntax"), "NotifyHintSyntax", QString::null, Kadu::SyntaxText, 0, Expert);

	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "Parameters"), 0, Advanced);
		ConfigDialog::addHBox("Hints", "Parameters", "top");
			ConfigDialog::addCheckBox("Hints", "top", QT_TRANSLATE_NOOP("@default", "Set for all"), "SetAll", true);
			ConfigDialog::addLabel("Hints", "top", QT_TRANSLATE_NOOP("@default", "<b>Text</b> preview"));

		ConfigDialog::addHBox("Hints", "Parameters", "center");
			QStringList options2;
			QStringList values2;
			options2<<tr("Online")<<tr("Online (d.)")<<tr("Busy")<<tr("Busy (d.)")<<
					tr("Invisible")<<tr("Invisible (d.)")<<tr("Offline")<<tr("Offline (d.)")<<
					tr("Blocking")<<tr("New chat")<<tr("New message in chat")<<tr("Error")<<tr("Other message");
			values2<<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<"11"<<"12";
			ConfigDialog::addVRadioGroup("Hints", "center", QT_TRANSLATE_NOOP("@default", "Hint type"), "LastSelected", options2, values2, "0");

			ConfigDialog::addVBox("Hints", "center", "bottom");
				ConfigDialog::addLabel("Hints", "bottom", 0, "stretcher2");
				ConfigDialog::addLabel("Hints", "bottom", 0, "stretcher3");

				ConfigDialog::addSpinBox("Hints", "bottom", QT_TRANSLATE_NOOP("@default","Hint timeout"), "LastTimeout", -2048, 2048, 1, 10);
				ConfigDialog::addPushButton("Hints", "bottom", QT_TRANSLATE_NOOP("@default", "Change font color"));
				ConfigDialog::addPushButton("Hints", "bottom", QT_TRANSLATE_NOOP("@default", "Change background color"));
				ConfigDialog::addPushButton("Hints", "bottom", QT_TRANSLATE_NOOP("@default", "Change font"));

	ConfigDialog::addHBox("Hints", "Hints", "hints-hbox");
		ConfigDialog::addVRadioGroup("Hints", "hints-hbox", QT_TRANSLATE_NOOP("@default", "New hints go"), "NewHintUnder",
			toStringList(tr("Auto"),tr("On top"),tr("On bottom")),
			toStringList("0", "1", "2"), "0", 0, 0, Expert);

		ConfigDialog::addVGroupBox("Hints", "hints-hbox", QT_TRANSLATE_NOOP("@default","Hints position"), 0, Expert);
			ConfigDialog::addCheckBox("Hints", "Hints position", QT_TRANSLATE_NOOP("@default", "Own hints position"), "UseUserPosition", false);

			ConfigDialog::addVBox("Hints", "Hints position", "coords");
				ConfigDialog::addSpinBox("Hints", "coords", "x=", "HintsPositionX", -2048, 2048, 1, 100);
				ConfigDialog::addSpinBox("Hints", "coords", "y=", "HintsPositionY", -2048, 2048, 1, 100);

		ConfigDialog::addVRadioGroup("Hints", "hints-hbox", QT_TRANSLATE_NOOP("@default", "Corner"), "Corner",
			toStringList(tr("Top left"),tr("Top right"),tr("Bottom left"),tr("Bottom right")),
			toStringList("0","1","2","3"), "0", 0, 0, Expert);

	const QString default_hints_syntax(QT_TRANSLATE_NOOP("HintManager", "[<i>%s</i><br/>][<br/><b>Description:</b><br/>%d<br/><br/>][<i>Mobile:</i> <b>%m</b><br/>]"));
	if (config_file.readEntry("Hints", "MouseOverUserSyntax") == default_hints_syntax || config_file.readEntry("Hints", "MouseOverUserSyntax").isEmpty())
		config_file.writeEntry("Hints", "MouseOverUserSyntax", tr(default_hints_syntax.ascii()));
	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "Hints over userlist"), 0, Expert);
		ConfigDialog::addTextEdit("Hints", "Hints over userlist", QT_TRANSLATE_NOOP("@default", "Hints syntax:"), "MouseOverUserSyntax", QString::null, Kadu::SyntaxText, 0, Expert);

	hint_manager_slots = new HintManagerSlots(NULL, "hint_manager_slots");
	ConfigDialog::registerSlotOnCreateTab("Hints", hint_manager_slots, SLOT(onCreateTabHints()));
	ConfigDialog::registerSlotOnApplyTab("Hints", hint_manager_slots, SLOT(onApplyTabHints()));
	ConfigDialog::registerSlotOnCloseTab("Hints", hint_manager_slots, SLOT(onCloseTabHints()));

	config_file.addVariable("Notify", "NewChat_Hints", true);
	config_file.addVariable("Notify", "NewMessage_Hints", true);
	config_file.addVariable("Notify", "ConnError_Hints", true);
	config_file.addVariable("Notify", "ChangingStatus_Hints", false);
	config_file.addVariable("Notify", "toAvailable_Hints", true);
	config_file.addVariable("Notify", "toBusy_Hints", true);
	config_file.addVariable("Notify", "toInvisible_Hints", false);
	config_file.addVariable("Notify", "toNotAvailable_Hints", false);
	config_file.addVariable("Notify", "UserBoxChangeToolTip_Hints", config_file.readBoolEntry("General", "ShowTooltipOnUserbox", true));
	config_file.addVariable("Notify", "Message_Hints", true);

	connect(this, SIGNAL(searchingForTrayPosition(QPoint &)), kadu, SIGNAL(searchingForTrayPosition(QPoint &)));

	QMap<QString, QString> s;
	s["NewChat"]=SLOT(newChat(Protocol *, UserListElements, const QString &, time_t));
	s["NewMessage"]=SLOT(newMessage(Protocol *, UserListElements, const QString &, time_t, bool &));
	s["ConnError"]=SLOT(connectionError(Protocol *, const QString &));
	s["StatusChanged"] = SLOT(userStatusChanged(UserListElement, QString, const UserStatus &));
	s["toAvailable"]=SLOT(userChangedStatusToAvailable(const QString &, UserListElement));
	s["toBusy"]=SLOT(userChangedStatusToBusy(const QString &, UserListElement));
	s["toInvisible"]=SLOT(userChangedStatusToInvisible(const QString &, UserListElement));
	s["toNotAvailable"]=SLOT(userChangedStatusToNotAvailable(const QString &, UserListElement));
	s["UserBoxChangeToolTip"]=SLOT(userBoxChangeToolTip(const QPoint &, UserListElement, bool));
	s["Message"]=SLOT(message(const QString &, const QString &, const QMap<QString, QVariant> *, const UserListElement *));
	notify->registerNotifier(QT_TRANSLATE_NOOP("@default","Hints"), this, s);

	kdebugf2();
}

HintManager::~HintManager()
{
	kdebugf();
	notify->unregisterNotifier("Hints");
	disconnect(this, SIGNAL(searchingForTrayPosition(QPoint &)), kadu, SIGNAL(searchingForTrayPosition(QPoint &)));

	ConfigDialog::unregisterSlotOnCreateTab("Hints", hint_manager_slots, SLOT(onCreateTabHints()));
	ConfigDialog::unregisterSlotOnApplyTab("Hints", hint_manager_slots, SLOT(onApplyTabHints()));
	ConfigDialog::unregisterSlotOnCloseTab("Hints", hint_manager_slots, SLOT(onCloseTabHints()));

	delete tipFrame;
	tipFrame = 0;

	disconnect(hint_timer, SIGNAL(timeout()), this, SLOT(oneSecond()));
	delete hint_timer;
	hint_timer = 0;

	hints.clear();

	delete frame;
	frame = 0;

	delete hint_manager_slots;
	hint_manager_slots = 0;

	ConfigDialog::removeControl("Hints", "Change font");
	ConfigDialog::removeControl("Hints", "Change background color");
	ConfigDialog::removeControl("Hints", "Change font color");
	ConfigDialog::removeControl("Hints", "Hint timeout");
	ConfigDialog::removeControl("Hints", 0, "stretcher3");
	ConfigDialog::removeControl("Hints", 0, "stretcher2");
	ConfigDialog::removeControl("Hints", "bottom");
	ConfigDialog::removeControl("Hints", "Hint type");
	ConfigDialog::removeControl("Hints", "center");
	ConfigDialog::removeControl("Hints", "<b>Text</b> preview");
	ConfigDialog::removeControl("Hints", "Set for all");
	ConfigDialog::removeControl("Hints", "top");
	ConfigDialog::removeControl("Hints", "Parameters");
	ConfigDialog::removeControl("Hints", "Hints syntax:");
	ConfigDialog::removeControl("Hints", "Hints over userlist");
	ConfigDialog::removeControl("Hints", "Corner");
	ConfigDialog::removeControl("Hints", "y=");
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
	ConfigDialog::removeControl("Hints", "Open chat on click");
	ConfigDialog::removeControl("Hints", "Status change");
	ConfigDialog::removeControl("Hints", "Number of quoted characters");
	ConfigDialog::removeControl("Hints", "Delete pending message when user deletes hint");
	ConfigDialog::removeControl("Hints", "Show message content in hint");
	ConfigDialog::removeControl("Hints", "New chat / new message");
	ConfigDialog::removeTab("Hints");

	kdebugf2();
}

void HintManager::setHint(void)
{
	kdebugf();

	if (hints.isEmpty())
	{
		hint_timer->stop();
		frame->hide();
		return;
	}

	QPoint newPosition;
	QPoint trayPosition;

	QSize preferredSize = frame->sizeHint();
	QSize desktopSize = QApplication::desktop()->size();

	emit searchingForTrayPosition(trayPosition);
	if (config_file.readBoolEntry("Hints", "UseUserPosition") || trayPosition.isNull())
	{
		newPosition=QPoint(config_file.readNumEntry("Hints","HintsPositionX"),config_file.readNumEntry("Hints","HintsPositionY"));

//		kdebugm(KDEBUG_INFO, "%d %d %d\n", config_file.readNumEntry("Hints", "Corner"), preferredSize.width(), preferredSize.height());
		switch(config_file.readNumEntry("Hints", "Corner"))
		{
			case 1: // "TopRight"
				newPosition-=QPoint(preferredSize.width(), 0);
				break;
			case 2: // "BottomLeft"
				newPosition-=QPoint(0, preferredSize.height());
				break;
			case 3: // "BottomRight"
				newPosition-=QPoint(preferredSize.width(), preferredSize.height());
				break;
			case 0: // "TopLeft"
				break;
		};

		if (newPosition.x() < 0) // when hints go out of the screen (on left)
			newPosition.setX(0);
		if (newPosition.y() < 0) //when hints go out of the screen (on top)
			newPosition.setY(0);

		if (newPosition.x() + preferredSize.width() >= desktopSize.width()) //when hints go out of the screen (on right)
			newPosition.setX(desktopSize.width() - preferredSize.width());
		if (newPosition.y() + preferredSize.height() >= desktopSize.height()) //when hints go out of the screen (on bottom)
			newPosition.setY(desktopSize.height() - preferredSize.height());
	}
	else
	{
		// those "strange" cases happens when "automatic panel hiding" is in use
		if (trayPosition.x() < 0)
			trayPosition.setX(0);
		else if (trayPosition.x() > desktopSize.width())
			trayPosition.setX(desktopSize.width() - 2);
		if (trayPosition.y() < 0)
			trayPosition.setY(0);
		else if (trayPosition.y() > desktopSize.height())
			trayPosition.setY(desktopSize.height() - 2);


		if (trayPosition.x() < desktopSize.width() / 2) // tray is on left
			newPosition.setX(trayPosition.x() + 32);
		else // tray is on right
			newPosition.setX(trayPosition.x() - preferredSize.width());

		if (trayPosition.y() < desktopSize.height() / 2) // tray is on top
			newPosition.setY(trayPosition.y() + 32);
		else // tray is on bottom
			newPosition.setY(trayPosition.y() - preferredSize.height());
	}

	frame->setGeometry(newPosition.x(), newPosition.y(), preferredSize.width(), preferredSize.height());

	kdebugf2();
}

void HintManager::deleteHint(Hint *hint)
{
	kdebugf();

	layout->remove(hint);
	hints.remove(hint);
	hint->deleteLater();

	kdebugf2();
}

void HintManager::oneSecond(void)
{
	kdebugf();

	bool removed = false;
	for (unsigned int i = 0; i < hints.count(); ++i)
	{
		hints.at(i)->nextSecond();

		if (hints.at(i)->isDeprecated())
		{
			deleteHint(hints.at(i));
			removed = true;
		}
	}

	if (removed)
		setHint();

	kdebugf2();
}

void HintManager::processButtonPress(const QString &buttonName, Hint *hint)
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", buttonName.ascii());

	switch(config_file.readNumEntry("Hints", buttonName))
	{
		case 1:
			openChat(hint);
			break;

		case 2:
			if (config_file.readBoolEntry("Hints", "DeletePendingMsgWhenHintDeleted"))
				chat_manager->deletePendingMsgs(hint->getUsers());
			deleteHint(hint);
			setHint();
			break;

		case 3:
			deleteAllHints();
			setHint();
			break;
	}

	kdebugf2();
}

void HintManager::leftButtonSlot(Hint *hint)
{
	processButtonPress("LeftButton", hint);
}

void HintManager::rightButtonSlot(Hint *hint)
{
	processButtonPress("RightButton", hint);
}

void HintManager::midButtonSlot(Hint *hint)
{
	processButtonPress("MiddleButton", hint);
}

void HintManager::openChat(Hint *hint)
{
	kdebugf();
	UserListElements senders = hint->getUsers();
	if (!senders.isEmpty())
		chat_manager->openPendingMsgs(senders);
	deleteHint(hint);
	setHint();
	kdebugf2();
}

void HintManager::deleteAllHints()
{
	kdebugf();
	hint_timer->stop();

	Hint *toDelete = hints.first();
	while (toDelete)
	{
		deleteHint(toDelete);
		toDelete = hints.current();
	}

	frame->hide();

	kdebugf2();
}

void HintManager::addHint(const QString& text, const QPixmap& pixmap, const QFont &font, const QColor &color, const QColor &bgcolor, unsigned int timeout, const UserListElements &senders)
{
 	kdebugf();

	Hint *hint = new Hint(frame, text, pixmap, timeout);
	hint->set(font, color, bgcolor);

	hints.append(hint);

	setLayoutDirection();
	layout->addWidget(hint);

	if (!senders.isEmpty())
		hint->setUsers(senders);

	connect(hint, SIGNAL(leftButtonClicked(Hint *)), this, SLOT(leftButtonSlot(Hint *)));
	connect(hint, SIGNAL(rightButtonClicked(Hint *)), this, SLOT(rightButtonSlot(Hint *)));
	connect(hint, SIGNAL(midButtonClicked(Hint *)), this, SLOT(midButtonSlot(Hint *)));
	setHint();

	if (!hint_timer->isActive())
		hint_timer->start(1000);
	if (frame->isHidden())
		frame->show();
	kdebugf2();
}


void HintManager::setLayoutDirection()
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
					layout->setDirection(QBoxLayout::Down);
				else
					layout->setDirection(QBoxLayout::Up);
			}
			else
			{
				if (trayPosition.y() < QApplication::desktop()->size().height()/2)
					layout->setDirection(QBoxLayout::Down);
				else
					layout->setDirection(QBoxLayout::Up);
			}
			break;
		case 1:
			layout->setDirection(QBoxLayout::Up);
			break;
		case 2:
			layout->setDirection(QBoxLayout::Down);
			break;
	}
	kdebugf2();
}

void HintManager::newChat(Protocol * /*protocol*/, UserListElements senders, const QString &msg, time_t /*t*/)
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
		addHint(narg(tr("Chat with <b>%1</b><br/> <small>%2</small>"),
			QStyleSheet::escape(senders[0].altNick()), cite),
			icons_manager->loadIcon("Message"),
			config_file.readFontEntry("Hints", "HintNewChat_font"),
			config_file.readColorEntry("Hints", "HintNewChat_fgcolor"),
			config_file.readColorEntry("Hints", "HintNewChat_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintNewChat_timeout"),
			senders);
	}
	else
		addHint(tr("Chat with <b>%1</b>").arg(QStyleSheet::escape(senders[0].altNick())),
			icons_manager->loadIcon("Message"),
			config_file.readFontEntry("Hints", "HintNewChat_font"),
			config_file.readColorEntry("Hints", "HintNewChat_fgcolor"),
			config_file.readColorEntry("Hints", "HintNewChat_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintNewChat_timeout"),
			senders);
	kdebugf2();
}

void HintManager::newMessage(Protocol * /*protocol*/, UserListElements senders, const QString &msg, time_t /*t*/, bool &/*grab*/)
{
	kdebugf();
	Chat* chat=chat_manager->findChat(senders);
	if (chat==NULL)
	{
		kdebugmf(KDEBUG_ERROR|KDEBUG_FUNCTION_END, "end: chat==NULL!\n");
		return;
	}

	if (!chat->isActiveWindow())
	{
		if (config_file.readBoolEntry("Hints", "ShowContentMessage"))
		{
			unsigned int citeSign=config_file.readUnsignedNumEntry("Hints","CiteSign");
			QString cite;
			if (msg.length() <= citeSign)
				cite = msg;
			else
				cite = msg.left(citeSign)+"...";
			addHint(narg(tr("New message from <b>%1</b><br/> <small>%2</small>"),
				QStyleSheet::escape(senders[0].altNick()), cite),
				icons_manager->loadIcon("Message"),
				config_file.readFontEntry("Hints", "HintNewMessage_font"),
				config_file.readColorEntry("Hints", "HintNewMessage_fgcolor"),
				config_file.readColorEntry("Hints", "HintNewMessage_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintNewMessage_timeout"));
		}
		else
			addHint(tr("New message from <b>%1</b>")
				.arg(QStyleSheet::escape(senders[0].altNick())),
				icons_manager->loadIcon("Message"),
				config_file.readFontEntry("Hints", "HintNewMessage_font"),
				config_file.readColorEntry("Hints", "HintNewMessage_fgcolor"),
				config_file.readColorEntry("Hints", "HintNewMessage_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintNewMessage_timeout"));
	}

	kdebugf2();
}

void HintManager::connectionError(Protocol *, const QString &message)
{
	kdebugf();
	addHint(tr("<b>Error:</b> %1").arg(message), icons_manager->loadIcon("Blocking"),
		config_file.readFontEntry("Hints", "HintError_font"),
		config_file.readColorEntry("Hints", "HintError_fgcolor"),
		config_file.readColorEntry("Hints", "HintError_bgcolor"),
		config_file.readUnsignedNumEntry("Hints", "HintError_timeout"));
	kdebugf2();
}

void HintManager::userStatusChanged(UserListElement ule, QString protocolName, const UserStatus &/*oldStatus*/)
{
	kdebugf();

	UserListElements ulist;
	if (config_file.readBoolEntry("Hints", "OpenChatOnClick", false))
		ulist.append(ule);

	QString stat;
	switch (ule.status(protocolName).status())
	{
		case Online: stat = "Online"; break;
		case Busy: stat = "Busy"; break;
		default: stat = "Offline";
	}

	if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
		addHint(KaduParser::parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true),
			ule.status(protocolName).pixmap(),
			config_file.readFontEntry("Hints", "Hint"+stat+"_font"),
			config_file.readColorEntry("Hints", "Hint"+stat+"_fgcolor"),
			config_file.readColorEntry("Hints", "Hint"+stat+"_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "Hint"+stat+"_timeout"), ulist);
	else
		if (ule.status(protocolName).hasDescription() && config_file.readBoolEntry("Hints","NotifyHintDescription"))
			addHint(narg(tr("<b>%1</b> changed status to <i>%2</i><br/> <small>%3</small>"),
				QStyleSheet::escape(ule.altNick()),
				qApp->translate("@default", ule.status("Gadu").name().ascii()),
				QStyleSheet::escape(ule.status("Gadu").description())),
				ule.status(protocolName).pixmap(),
				config_file.readFontEntry("Hints", "Hint"+stat+"D_font"),
				config_file.readColorEntry("Hints", "Hint"+stat+"D_fgcolor"),
				config_file.readColorEntry("Hints", "Hint"+stat+"D_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "Hint"+stat+"D_timeout"), ulist);
		else
			addHint(narg(tr("<b>%1</b> changed status to <i>%2</i>"),
				QStyleSheet::escape(ule.altNick()),
				qApp->translate("@default", ule.status(protocolName).name().ascii())),
				ule.status(protocolName).pixmap(),
				config_file.readFontEntry("Hints", "Hint"+stat+"_font"),
				config_file.readColorEntry("Hints", "Hint"+stat+"_fgcolor"),
				config_file.readColorEntry("Hints", "Hint"+stat+"_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "Hint"+stat+"_timeout"), ulist);
	kdebugf2();
}

void HintManager::userChangedStatusToAvailable(const QString &protocolName, UserListElement ule)
{
	kdebugf();

	UserListElements ulist;
	if (config_file.readBoolEntry("Hints", "OpenChatOnClick", false))
		ulist.append(ule);

	if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
		addHint(KaduParser::parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true),
			ule.status(protocolName).pixmap(),
			config_file.readFontEntry("Hints", "HintOnline_font"),
			config_file.readColorEntry("Hints", "HintOnline_fgcolor"),
			config_file.readColorEntry("Hints", "HintOnline_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintOnline_timeout"), ulist);
	else
		if (ule.status(protocolName).hasDescription() && config_file.readBoolEntry("Hints","NotifyHintDescription"))
			addHint(narg(tr("<b>%1</b> is available<br/> <small>%2</small>"),
				QStyleSheet::escape(ule.altNick()),
				QStyleSheet::escape(ule.status(protocolName).description())),
				ule.status(protocolName).pixmap(),
				config_file.readFontEntry("Hints", "HintOnlineD_font"),
				config_file.readColorEntry("Hints", "HintOnlineD_fgcolor"),
				config_file.readColorEntry("Hints", "HintOnlineD_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintOnlineD_timeout"), ulist);
		else
			addHint(tr("<b>%1</b> is available")
				.arg(QStyleSheet::escape(ule.altNick())),
				ule.status(protocolName).pixmap(),
				config_file.readFontEntry("Hints", "HintOnline_font"),
				config_file.readColorEntry("Hints", "HintOnline_fgcolor"),
				config_file.readColorEntry("Hints", "HintOnline_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintOnline_timeout"), ulist);
	kdebugf2();
}

void HintManager::userChangedStatusToBusy(const QString &protocolName, UserListElement ule)
{
	kdebugf();

	UserListElements ulist;
	if (config_file.readBoolEntry("Hints", "OpenChatOnClick", false))
		ulist.append(ule);

	if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
		addHint(KaduParser::parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true),
			ule.status(protocolName).pixmap(),
			config_file.readFontEntry("Hints", "HintBusy_font"),
			config_file.readColorEntry("Hints", "HintBusy_fgcolor"),
			config_file.readColorEntry("Hints", "HintBusy_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintBusy_timeout"), ulist);
	else
		if (ule.status(protocolName).hasDescription() && config_file.readBoolEntry("Hints","NotifyHintDescription"))
			addHint(narg(tr("<b>%1</b> is busy<br/> <small>%2</small>"),
				QStyleSheet::escape(ule.altNick()),
				QStyleSheet::escape(ule.status(protocolName).description())),
				ule.status(protocolName).pixmap(),
				config_file.readFontEntry("Hints", "HintBusyD_font"),
				config_file.readColorEntry("Hints", "HintBusyD_fgcolor"),
				config_file.readColorEntry("Hints", "HintBusyD_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintBusyD_timeout"), ulist);
		else
			addHint(tr("<b>%1</b> is busy")
				.arg(QStyleSheet::escape(ule.altNick())),
				ule.status(protocolName).pixmap(),
				config_file.readFontEntry("Hints", "HintBusy_font"),
				config_file.readColorEntry("Hints", "HintBusy_fgcolor"),
				config_file.readColorEntry("Hints", "HintBusy_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintBusy_timeout"), ulist);
	kdebugf2();
}

void HintManager::userChangedStatusToInvisible(const QString &protocolName, UserListElement ule)
{
	kdebugf();

	UserListElements ulist;
	if (config_file.readBoolEntry("Hints", "OpenChatOnClick", false))
		ulist.append(ule);

	if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
		addHint(KaduParser::parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true),
			ule.status(protocolName).pixmap(),
			config_file.readFontEntry("Hints", "HintInvisible_font"),
			config_file.readColorEntry("Hints", "HintInvisible_fgcolor"),
			config_file.readColorEntry("Hints", "HintInvisible_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintInvisible_timeout"), ulist);
	else
		if (ule.status(protocolName).hasDescription() && config_file.readBoolEntry("Hints","NotifyHintDescription"))
			addHint(narg(tr("<b>%1</b> is invisible<br/> <small>%2</small>"),
				QStyleSheet::escape(ule.altNick()),
				QStyleSheet::escape(ule.status("Gadu").description())),
				ule.status(protocolName).pixmap(),
				config_file.readFontEntry("Hints", "HintInvisibleD_font"),
				config_file.readColorEntry("Hints", "HintInvisibleD_fgcolor"),
				config_file.readColorEntry("Hints", "HintInvisibleD_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintInvisibleD_timeout"), ulist);
		else
			addHint(tr("<b>%1</b> is invisible")
				.arg(QStyleSheet::escape(ule.altNick())),
				ule.status(protocolName).pixmap(),
				config_file.readFontEntry("Hints", "HintInvisible_font"),
				config_file.readColorEntry("Hints", "HintInvisible_fgcolor"),
				config_file.readColorEntry("Hints", "HintInvisible_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintInvisible_timeout"), ulist);
	kdebugf2();
}


void HintManager::userChangedStatusToNotAvailable(const QString &protocolName, UserListElement ule)
{
	kdebugf();

	UserListElements ulist;
	if (config_file.readBoolEntry("Hints", "OpenChatOnClick", false))
		ulist.append(ule);

	if (config_file.readBoolEntry("Hints","NotifyHintUseSyntax"))
		addHint(KaduParser::parse(config_file.readEntry("Hints","NotifyHintSyntax"), ule, true),
			ule.status(protocolName).pixmap(),
			config_file.readFontEntry("Hints", "HintOffline_font"),
			config_file.readColorEntry("Hints", "HintOffline_fgcolor"),
			config_file.readColorEntry("Hints", "HintOffline_bgcolor"),
			config_file.readUnsignedNumEntry("Hints", "HintOffline_timeout"), ulist);
	else
		if (ule.status(protocolName).hasDescription() && config_file.readBoolEntry("Hints","NotifyHintDescription"))
			addHint(narg(tr("<b>%1</b> is not available<br/> <small>%2</small>"),
				QStyleSheet::escape(ule.altNick()),
				QStyleSheet::escape(ule.status(protocolName).description())),
				ule.status(protocolName).pixmap(),
				config_file.readFontEntry("Hints", "HintOfflineD_font"),
				config_file.readColorEntry("Hints", "HintOfflineD_fgcolor"),
				config_file.readColorEntry("Hints", "HintOfflineD_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintOfflineD_timeout"), ulist);
		else
			addHint(tr("<b>%1</b> is not available")
				.arg(QStyleSheet::escape(ule.altNick())),
				ule.status(protocolName).pixmap(),
				config_file.readFontEntry("Hints", "HintOffline_font"),
				config_file.readColorEntry("Hints", "HintOffline_fgcolor"),
				config_file.readColorEntry("Hints", "HintOffline_bgcolor"),
				config_file.readUnsignedNumEntry("Hints", "HintOffline_timeout"), ulist);
	kdebugf2();
}

void HintManager::userBoxChangeToolTip(const QPoint &point, UserListElement user, bool show)
{
//	kdebugf();
	if (show)
	{
		kdebugm(KDEBUG_INFO, "user: '%s', show: %d, x:%d, y:%d\n", user.altNick().local8Bit().data(), show, point.x(), point.y());
		QString text = KaduParser::parse(config_file.readEntry("Hints", "MouseOverUserSyntax"), user);

		while (text.endsWith("<br/>"))
			text.setLength(text.length() - 5 /* 5 == QString("<br/>").length()*/);
		while (text.startsWith("<br/>"))
			text = text.right(text.length() - 5 /* 5 == QString("<br/>").length()*/);

		if (tipFrame)
			delete tipFrame;
		tipFrame = new QFrame(0, "tip_frame", WStyle_NoBorder | WStyle_StaysOnTop | WStyle_Tool | WX11BypassWM | WWinOwnDC);
		tipFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
		tipFrame->setLineWidth(FRAME_WIDTH);

		QVBoxLayout *lay = new QVBoxLayout(tipFrame);
		lay->setMargin(FRAME_WIDTH);

		QLabel *tipLabel = new QLabel(text, tipFrame);
		tipLabel->setTextFormat(Qt::RichText);
		tipLabel->setAlignment(AlignVCenter | AlignLeft);

		lay->addWidget(tipLabel);

		tipFrame->setFixedSize(tipLabel->sizeHint() + QSize(2 * FRAME_WIDTH, 2 * FRAME_WIDTH));

		QPoint pos(kadu->userbox()->mapToGlobal(point) + QPoint(5, 5));
//		kdebugm(KDEBUG_INFO, "%d %d\n", pos.x(), pos.y());

		QSize preferredSize = tipFrame->sizeHint();
		QSize desktopSize = QApplication::desktop()->size();
		if (pos.x() + preferredSize.width() > desktopSize.width())
			pos.setX(pos.x() - preferredSize.width() - 10);
		if (pos.y() + preferredSize.height() > desktopSize.height())
			pos.setY(pos.y() - preferredSize.height() - 10);

//		kdebugm(KDEBUG_INFO, "%d %d\n", pos.x(), pos.y());
		tipFrame->move(pos);
		tipFrame->show();
	}
	else
	{
		kdebugm(KDEBUG_INFO, "hiding\n");
		tipFrame->hide();
		tipFrame->deleteLater();
		tipFrame = 0;
	}
}

void HintManager::message(const QString &from, const QString &msg, const QMap<QString, QVariant> *parameters, const UserListElement *ule)
{
	kdebugf();
	UserListElements uinslist;
	if (ule != NULL)
		uinslist.append(*ule);

	QString msg2;
	QPixmap pixmap;
	QFont font;
	QColor fgcolor,bgcolor;
	unsigned int timeout=0;
	bool ok;
	bool showSource=true;
	if (parameters!=NULL)
	{
		QMap<QString, QVariant>::const_iterator end=(*parameters).end();

		pixmap=(*parameters)["Pixmap"].toPixmap();
		font=(*parameters)["Font"].toFont();
		fgcolor=(*parameters)["Foreground color"].toColor();
		bgcolor=(*parameters)["Background color"].toColor();
		timeout=(*parameters)["Timeout"].toUInt(&ok);

		QMap<QString, QVariant>::const_iterator sit=(*parameters).find("ShowSource");
		if (sit!=end)
			showSource=sit.data().toBool();
	}
	if (pixmap.isNull())
		pixmap=icons_manager->loadIcon("Message");
	if (font==qApp->font())
		font=config_file.readFontEntry("Hints", "HintMessage_font");
	if (!fgcolor.isValid())
		fgcolor=config_file.readColorEntry("Hints", "HintMessage_fgcolor");
	if (!bgcolor.isValid())
		bgcolor=config_file.readColorEntry("Hints", "HintMessage_bgcolor");
	if (timeout==0 || !ok)
		timeout=config_file.readUnsignedNumEntry("Hints", "HintMessage_timeout");
	if (!from.isEmpty() && showSource)
		msg2=narg(tr("From <b>%1</b>: %2"), from, msg);
	else
		msg2=msg;

	addHint(msg2, pixmap, font, fgcolor, bgcolor, timeout, uinslist);

	kdebugf2();
}

void HintManager::externalEvent(Notification *notification)
{
	kdebugf();

	UserListElements ules = notification->userListElements();
	QString msg = notification->text();

	if (ules.count() > 0)
		message("", msg, 0, &ules[0]);
	else
		message("", msg, 0, 0);

	kdebugf2();
}

HintManager *hint_manager=NULL;

/** @} */

