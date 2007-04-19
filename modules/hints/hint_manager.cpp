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

HintManager::HintManager(QWidget *parent, const char *name)	: Notifier(parent, name), ToolTipClass(),
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
		ConfigDialog::addCheckBox("Hints", "Status change", QT_TRANSLATE_NOOP("@default", "Add description to hint if exists"), "NotifyHintDescription", false);
		ConfigDialog::addLineEdit("Hints", "Status change", QT_TRANSLATE_NOOP("@default", "Hint syntax"), "NotifyHintSyntax", QString::null, Kadu::SyntaxText, 0, Expert);

	ConfigDialog::addVGroupBox("Hints", "Hints", QT_TRANSLATE_NOOP("@default", "Parameters"), 0, Advanced);
		ConfigDialog::addHBox("Hints", "Parameters", "top");
			ConfigDialog::addCheckBox("Hints", "top", QT_TRANSLATE_NOOP("@default", "Set for all"), "SetAll", true);
			ConfigDialog::addLabel("Hints", "top", QT_TRANSLATE_NOOP("@default", "<b>Text</b> preview"));

		ConfigDialog::addHBox("Hints", "Parameters", "center");
			QStringList empty;

			ConfigDialog::addComboBox("Hints", "center", QT_TRANSLATE_NOOP("@default", "Hint type"), "LastSelected", empty, empty, "0");

			ConfigDialog::addVBox("Hints", "center", "bottom");
				ConfigDialog::addLabel("Hints", "bottom", 0, "stretcher2");
				ConfigDialog::addLabel("Hints", "bottom", 0, "stretcher3");

				ConfigDialog::addSpinBox("Hints", "bottom", QT_TRANSLATE_NOOP("@default","Hint timeout"), "LastTimeout", 0, 2048, 1, 10);

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

	connect(this, SIGNAL(searchingForTrayPosition(QPoint &)), kadu, SIGNAL(searchingForTrayPosition(QPoint &)));

	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "Hints"), this);
	tool_tip_class_manager->registerToolTipClass("Hints", this);

	import_0_5_0_Configuration();

	config_file.addVariable("Notify", "ConnectionError_Hints", true);
	config_file.addVariable("Notify", "NewChat_Hints", true);
	config_file.addVariable("Notify", "NewMessage_Hints", true);
	config_file.addVariable("Notify", "StatusChanged/ToOnline_Hints", true);
	config_file.addVariable("Notify", "StatusChanged/ToBusy_Hints", true);
	config_file.addVariable("Notify", "StatusChanged/ToInvisible_Hints", true);
	config_file.addVariable("Notify", "StatusChanged/ToOffline_Hints", true);
	config_file.addVariable("Notify", "FileTransfer/IncomingFile_Hints", true);
	config_file.addVariable("Notify", "FileTransfer/Finished_Hints", true);

	kdebugf2();
}

HintManager::~HintManager()
{
	kdebugf();

	tool_tip_class_manager->unregisterToolTipClass("Hints");
	notification_manager->unregisterNotifier("Hints");

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
	ConfigDialog::removeControl("Hints", "Add description to hint if exists");
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

	hints.remove(hint);
	layout->remove(hint);
	hint->deleteLater();

	kdebugf2();
}

void HintManager::deleteHintAndUpdate(Hint *hint)
{
	deleteHint(hint);
	setHint();
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
			hint->acceptNotification();
			break;

		case 2:
			if (hint->hasUsers() && config_file.readBoolEntry("Hints", "DeletePendingMsgWhenHintDeleted"))
				chat_manager->deletePendingMsgs(hint->getUsers());

			hint->discardNotification();
			deleteHintAndUpdate(hint);
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

	if (!hint->hasUsers())
		return;

	const UserListElements & senders = hint->getUsers();
	if (!senders.isEmpty())
		chat_manager->openPendingMsgs(senders);
	deleteHintAndUpdate(hint);

	kdebugf2();
}

void HintManager::deleteAllHints()
{
	kdebugf();
	hint_timer->stop();

	Hint *toDelete = hints.first();
	while (toDelete)
	{
		if (!toDelete->requireManualClosing())
		{
			deleteHint(toDelete);
			toDelete = hints.current();
		}
		else
			toDelete = hints.next();
	}

	if (hints.isEmpty())
		frame->hide();

	kdebugf2();
}

Hint *HintManager::addHint(Notification *notification)
{
	kdebugf();

	connect(notification, SIGNAL(closed(Notification *)), this, SLOT(notificationClosed(Notification *)));

	Hint *hint = new Hint(frame, notification);
	hints.append(hint);

	setLayoutDirection();
	layout->addWidget(hint);

	connect(hint, SIGNAL(leftButtonClicked(Hint *)), this, SLOT(leftButtonSlot(Hint *)));
	connect(hint, SIGNAL(rightButtonClicked(Hint *)), this, SLOT(rightButtonSlot(Hint *)));
	connect(hint, SIGNAL(midButtonClicked(Hint *)), this, SLOT(midButtonSlot(Hint *)));
	connect(hint, SIGNAL(closing(Hint *)), this, SLOT(deleteHintAndUpdate(Hint *)));
	setHint();

	if (!hint_timer->isActive())
		hint_timer->start(1000);
	if (frame->isHidden())
		frame->show();

	kdebugf2();

	return hint;
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

void HintManager::showToolTip(const QPoint &point, const UserListElement &user)
{
	kdebugf();

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

	QSize preferredSize = tipFrame->sizeHint();
	QSize desktopSize = QApplication::desktop()->size();
	if (pos.x() + preferredSize.width() > desktopSize.width())
		pos.setX(pos.x() - preferredSize.width() - 10);
	if (pos.y() + preferredSize.height() > desktopSize.height())
		pos.setY(pos.y() - preferredSize.height() - 10);

	tipFrame->move(pos);
	tipFrame->show();

	kdebugf2();
}

void HintManager::hideToolTip()
{
	kdebugf();

	if (tipFrame)
	{
		tipFrame->hide();
		tipFrame->deleteLater();
		tipFrame = 0;
	}

	kdebugf2();
}

void HintManager::notify(Notification *notification)
{
	kdebugf();

	if (notification->details() == "")
	{
		addHint(notification);

		kdebugf2();
		return;
	}

	const UserListElements &ules = notification->userListElements();
	if (linkedHints.count(qMakePair(ules, notification->type())))
	{
		Hint *linkedHint = linkedHints[qMakePair(ules, notification->type())];
		linkedHint->addDetail(notification->details());
	}
	else
	{
		Hint *linkedHint = addHint(notification);
		linkedHints[qMakePair(ules, notification->type())] = linkedHint;
	}

	kdebugf2();
}

void HintManager::notificationClosed(Notification *notification)
{
	const UserListElements &ules = notification->userListElements();
	if (linkedHints.count(qMakePair(ules, notification->type())))
		linkedHints.remove(qMakePair(ules, notification->type()));
}

void HintManager::copyConfiguration(const QString &fromEvent, const QString &toEvent)
{
}

void HintManager::realCopyConfiguration(const QString &fromHint, const QString &toHint)
{
	config_file.writeEntry("Hints", toHint + "_font", config_file.readFontEntry("Hints", fromHint + "_font"));
	config_file.writeEntry("Hints", toHint + "_fgcolor", config_file.readColorEntry("Hints", fromHint + "_fgcolor"));
	config_file.writeEntry("Hints", toHint + "_bgcolor", config_file.readColorEntry("Hints", fromHint + "_bgcolor"));
	config_file.writeEntry("Hints", toHint + "_timeout", (int) config_file.readUnsignedNumEntry("Hints",  fromHint + "_timeout"));
}

void HintManager::import_0_5_0_Configuration()
{
	if (config_file.readBoolEntry("Notify", "UserBoxChangeToolTip_Hints", false))
	{
		config_file.writeEntry("Look", "UserboxToolTipStyle", "Hints");
		tool_tip_class_manager->useToolTipClass("Hints");
		config_file.removeVariable("Notify", "UserBoxChangeToolTip_Hints");
	}

	QString syntax = config_file.readEntry("Hints","NotifyHintSyntax");

	import_0_5_0_Configuration_fromTo("HintError", "ConnectionError");
	import_0_5_0_Configuration_fromTo("HintOnline", "StatusChanged/ToOnline", syntax);
	import_0_5_0_Configuration_fromTo("HintBusy", "StatusChanged/ToBusy", syntax);
	import_0_5_0_Configuration_fromTo("HintInvisible", "StatusChanged/ToInvisible", syntax);
	import_0_5_0_Configuration_fromTo("HintOffline", "StatusChanged/ToOffline", syntax);
	import_0_5_0_Configuration_fromTo("HintNewChat", "NewChat");
	import_0_5_0_Configuration_fromTo("HintNewMessage", "NewMessage");
}

void HintManager::import_0_5_0_Configuration_fromTo(const QString &from, const QString &to, const QString &syntax, const QString &detailSyntax)
{
	if (config_file.readNumEntry("Hints", from + "_timeout", -1) == -1)
		return;

// TODO: fix it, dont use real QColor instances here
	QColor fgDefaultColor(0x00, 0x00, 0x00);
	QColor bgDefaultColor(0xf0, 0xf0, 0xf0);

	config_file.addVariable("Hints", QString("Event_") + to + "_font", config_file.readFontEntry("Hints", from + "_font"));
	config_file.addVariable("Hints", QString("Event_") + to + "_fgcolor", config_file.readColorEntry("Hints", from + "_fgcolor", &fgDefaultColor));
	config_file.addVariable("Hints", QString("Event_") + to + "_bgcolor", config_file.readColorEntry("Hints", from + "_bgcolor", &bgDefaultColor));
	config_file.addVariable("Hints", QString("Event_") + to + "_timeout", config_file.readNumEntry("Hints", from + "_timeout", 10));

	if (QString::null != syntax && syntax != "")
		config_file.writeEntry("Hints",  QString("Event_") + to + "_syntax", syntax);
	if (QString::null != detailSyntax && detailSyntax != "")
		config_file.writeEntry("Hints",  QString("Event_") + to + "_detailSyntax", detailSyntax);

	config_file.removeVariable("Hints", from + "_font");
	config_file.removeVariable("Hints", from + "_fgcolor");
	config_file.removeVariable("Hints", from + "_bgcolor");
	config_file.removeVariable("Hints", from + "_timeout");
}

HintManager *hint_manager=NULL;

/** @} */

