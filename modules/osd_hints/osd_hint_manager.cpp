/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>

#include "chat_widget.h"
#include "chat_manager.h"
#include "config_file.h"
#include "debug.h"
#include "osd_hint_manager.h"
#include "osd_hints_configuration_widget.h"
#include "icons_manager.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "misc.h"
#include "userbox.h"
#include "userlist.h"

#include "../notify/notify.h"

/**
 * @ingroup hints
 * @{
 */
#define FRAME_WIDTH 1
#define BORDER_RADIUS 10

static QRegion roundedRect(const QRect& rect, int r)
{
	r -= FRAME_WIDTH;
	QRegion region;
        // middle and borders
	region += rect.adjusted(r, 0, -r, 0);
	region += rect.adjusted(0, r, 0, -r);
	// top left
	QRect corner(rect.topLeft(), QSize(r * 2, r * 2));
	region += QRegion(corner, QRegion::Ellipse);
	// top right
	corner.moveTopRight(rect.topRight());
	corner.translate(-1,0);
	region += QRegion(corner, QRegion::Ellipse);
	// bottom left
	corner.moveBottomLeft(rect.bottomLeft());
	corner.translate(0,-1);
	region += QRegion(corner, QRegion::Ellipse);
	// bottom right
	corner.moveBottomRight(rect.bottomRight());
	corner.translate(-1,-1);
	region += QRegion(corner, QRegion::Ellipse);
	return region;
}

OSDHintManager::OSDHintManager(QWidget *parent, const char *name)	: Notifier(parent, name), ToolTipClass(),
	hint_timer(new QTimer(this, "hint_timer")),
	hints(), tipFrame(0)
{
	kdebugf();
#ifdef Q_OS_MAC
	frame = new QFrame(parent, name, Qt::FramelessWindowHint | Qt::SplashScreen | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#else
	frame = new QFrame(parent, name, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#endif
	frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	frame->setFrameStyle(QFrame::Box | QFrame::Plain);
	frame->setLineWidth(FRAME_WIDTH);

	QString style = QString("QFrame {border-width: %1px; border-style: solid; border-color: %2; border-radius: %3px;}").arg(config_file.readNumEntry("OSDHints", "SetAll_borderWidth", FRAME_WIDTH)).arg(config_file.readColorEntry("OSDHints", "SetAll_bdcolor").name()).arg(BORDER_RADIUS);
	frame->setStyleSheet(style);

	layout = new QVBoxLayout(frame, FRAME_WIDTH, 0, "grid");
	layout->setResizeMode(QLayout::Fixed);
	
	opacity = config_file.readNumEntry("OSDHints", "Opacity", 100);
	opacity /= 100;

	connect(hint_timer, SIGNAL(timeout()), this, SLOT(oneSecond()));
	connect(chat_manager, SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)));

	const QString default_hints_syntax(QT_TRANSLATE_NOOP("OSDHintManager", "[<i>%s</i><br/>][<br/><b>Description:</b><br/>%d<br/><br/>][<i>Mobile:</i> <b>%m</b><br/>]"));
	if (config_file.readEntry("OSDHints", "MouseOverUserSyntax") == default_hints_syntax || config_file.readEntry("OSDHints", "MouseOverUserSyntax").isEmpty())
		config_file.writeEntry("OSDHints", "MouseOverUserSyntax", tr(default_hints_syntax.ascii()));

	connect(this, SIGNAL(searchingForTrayPosition(QPoint &)), kadu, SIGNAL(searchingForTrayPosition(QPoint &)));

	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "OSDHints"), this);
	tool_tip_class_manager->registerToolTipClass(QT_TRANSLATE_NOOP("@default", "OSDHints"), this);

	createDefaultConfiguration();

	kdebugf2();
}

OSDHintManager::~OSDHintManager()
{
	kdebugf();

	tool_tip_class_manager->unregisterToolTipClass("OSDHints");
	notification_manager->unregisterNotifier("OSDHints");

	disconnect(this, SIGNAL(searchingForTrayPosition(QPoint &)), kadu, SIGNAL(searchingForTrayPosition(QPoint &)));

	delete tipFrame;
	tipFrame = 0;

	disconnect(chat_manager, SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)));
	disconnect(hint_timer, SIGNAL(timeout()), this, SLOT(oneSecond()));
	delete hint_timer;
	hint_timer = 0;

	hints.clear();

	delete frame;
	frame = 0;

	kdebugf2();
}

void OSDHintManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widgetById("osdhints/showContent"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("osdhints/showContentCount"), SLOT(setEnabled(bool)));

	QWidget *ownPosition = mainConfigurationWindow->widgetById("osdhints/ownPosition");
	connect(ownPosition, SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("osdhints/ownPositionX"), SLOT(setEnabled(bool)));
	connect(ownPosition, SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("osdhints/ownPositionY"), SLOT(setEnabled(bool)));
	connect(ownPosition, SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("osdhints/ownPositionCorner"), SLOT(setEnabled(bool)));

	QCheckBox *setAll = dynamic_cast<QCheckBox *>(mainConfigurationWindow->widgetById("osdhints/setAll"));
	connect(setAll, SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("osdhints/setAllPreview"), SLOT(setEnabled(bool)));
	connect(setAll, SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("osdhints/setAll_timeout"), SLOT(setEnabled(bool)));
	connect(setAll, SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("osdhints/setAll_fgcolor"), SLOT(setEnabled(bool)));
	connect(setAll, SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("osdhints/setAll_bgcolor"), SLOT(setEnabled(bool)));
	connect(setAll, SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("osdhints/setAll_font"), SLOT(setEnabled(bool)));
	connect(setAll, SIGNAL(toggled(bool)), configurationWidget, SLOT(setAllEnabled(bool)));
	configurationWidget->setAllEnabled(setAll->isChecked());

	(dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("osdhints/setAll_timeout")))->setSpecialValueText(tr("Dont hide"));

	minimumWidth = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("osdhints/minimumWidth"));
	maximumWidth = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widgetById("osdhints/maximumWidth"));
	connect(minimumWidth, SIGNAL(valueChanged(int)), this, SLOT(minimumWidthChanged(int)));
	connect(maximumWidth, SIGNAL(valueChanged(int)), this, SLOT(maximumWidthChanged(int)));

	overUserSyntax = mainConfigurationWindow->widgetById("osdhints/overUserSyntax");
	overUserSyntax->setToolTip(qApp->translate("@default", Kadu::SyntaxText));

	connect(mainConfigurationWindow->widgetById("toolTipClasses"), SIGNAL(currentIndexChanged(const QString &)),
		this, SLOT(toolTipClassesHighlighted(const QString &)));
}

void OSDHintManager::toolTipClassesHighlighted(const QString &value)
{
	overUserSyntax->setEnabled(value == qApp->translate("@default", "OSDHints"));
}

NotifierConfigurationWidget *OSDHintManager::createConfigurationWidget(QWidget *parent, char *name)
{
	configurationWidget = new HintsConfigurationWidget(parent, name);
	return configurationWidget;
}

void OSDHintManager::minimumWidthChanged(int value)
{
	if (value > maximumWidth->value())
		maximumWidth->setValue(value);
}

void OSDHintManager::maximumWidthChanged(int value)
{
	if (value < minimumWidth->value())
		minimumWidth->setValue(value);
}

void OSDHintManager::hintUpdated()
{
	setHint();
}

void OSDHintManager::configurationUpdated()
{
	opacity = config_file.readNumEntry("OSDHints", "Opacity", 100);
	opacity /= 100;

	setHint();
}

void OSDHintManager::setHint()
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

	frame->adjustSize();
	QSize preferredSize = frame->sizeHint();
	QSize desktopSize = QApplication::desktop()->size();

	emit searchingForTrayPosition(trayPosition);
	if (config_file.readBoolEntry("OSDHints", "UseUserPosition") || trayPosition.isNull())
	{
		newPosition = QPoint(config_file.readNumEntry("OSDHints", "HintsPositionX"), config_file.readNumEntry("OSDHints", "HintsPositionY"));

//		kdebugm(KDEBUG_INFO, "%d %d %d\n", config_file.readNumEntry("OSDHints", "Corner"), preferredSize.width(), preferredSize.height());
		switch(config_file.readNumEntry("OSDHints", "Corner"))
		{
			case 1: // "TopRight"
				newPosition -= QPoint(preferredSize.width(), 0);
				break;
			case 2: // "BottomLeft"
				newPosition -= QPoint(0, preferredSize.height());
				break;
			case 3: // "BottomRight"
				newPosition -= QPoint(preferredSize.width(), preferredSize.height());
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

	frame->resize(preferredSize.width(), preferredSize.height());
	frame->setMask(roundedRect(frame->rect(), BORDER_RADIUS));
	frame->setWindowOpacity(opacity);

	kdebugf2();
}

void OSDHintManager::deleteHint(OSDHint *hint)
{
	kdebugf();

	hints.removeAll(hint);
	layout->removeWidget(static_cast<QWidget *>(hint));
	hint->deleteLater();

	if (hints.isEmpty())
	{
		hint_timer->stop();
		frame->hide();
	}

	kdebugf2();
}

void OSDHintManager::deleteHintAndUpdate(OSDHint *hint)
{
	deleteHint(hint);
	setHint();
}

void OSDHintManager::oneSecond(void)
{
	kdebugf();

	bool removed = false;
	for (int i = 0; i < hints.count(); ++i)
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

void OSDHintManager::processButtonPress(const QString &buttonName, OSDHint *hint)
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", buttonName.ascii());

	switch(config_file.readNumEntry("OSDHints", buttonName))
	{
		case 1:
			openChat(hint);
			hint->acceptNotification();
			break;

		case 2:
			if (hint->hasUsers() && config_file.readBoolEntry("OSDHints", "DeletePendingMsgWhenHintDeleted"))
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

void OSDHintManager::leftButtonSlot(OSDHint *hint)
{
	processButtonPress("LeftButton", hint);
}

void OSDHintManager::rightButtonSlot(OSDHint *hint)
{
	processButtonPress("RightButton", hint);
}

void OSDHintManager::midButtonSlot(OSDHint *hint)
{
	processButtonPress("MiddleButton", hint);
}

void OSDHintManager::openChat(OSDHint *hint)
{
	kdebugf();

	if (!hint->hasUsers())
		return;

	if (!config_file.readBoolEntry("OSDHints", "OpenChatOnEveryNotification"))
		if ((hint->getNotification()->type() != "NewChat") && (hint->getNotification()->type() != "NewMessage"))
			return;

	const UserListElements & senders = hint->getUsers();
	if (!senders.isEmpty())
		chat_manager->openPendingMsgs(senders, true);
	deleteHintAndUpdate(hint);

	kdebugf2();
}

void OSDHintManager::chatWidgetActivated(ChatWidget *chat)
{
	QPair<UserListElements, QString> newChat = qMakePair(chat->users()->toUserListElements(), QString("NewChat"));
	QPair<UserListElements, QString> newMessage = qMakePair(chat->users()->toUserListElements(), QString("NewMessage"));

	if (linkedHints.count(newChat))
	{
		linkedHints[newChat]->close();
		linkedHints.remove(newChat);
	}

	if (linkedHints.count(newMessage))
	{
		linkedHints[newMessage]->close();
		linkedHints.remove(newMessage);
	}

	foreach (OSDHint *h, hints)
	{
		if (h->getUsers().equals(chat->users()) && !h->requireManualClosing())
			deleteHint(h);
	}

	setHint();
}

void OSDHintManager::deleteAllHints()
{
	kdebugf();
	hint_timer->stop();

	foreach(OSDHint *h, hints)
	{
		if(!h->requireManualClosing())
			deleteHint(h);
	}

	if (hints.isEmpty())
		frame->hide();

	kdebugf2();
}

OSDHint *OSDHintManager::addHint(Notification *notification)
{
	kdebugf();

	connect(notification, SIGNAL(closed(Notification *)), this, SLOT(notificationClosed(Notification *)));

	OSDHint *hint = new OSDHint(frame, notification);
	hints.append(hint);

	setLayoutDirection();
	layout->addWidget(static_cast<QWidget *>(hint));

	connect(hint, SIGNAL(leftButtonClicked(OSDHint *)), this, SLOT(leftButtonSlot(OSDHint *)));
	connect(hint, SIGNAL(rightButtonClicked(OSDHint *)), this, SLOT(rightButtonSlot(OSDHint *)));
	connect(hint, SIGNAL(midButtonClicked(OSDHint *)), this, SLOT(midButtonSlot(OSDHint *)));
	connect(hint, SIGNAL(closing(OSDHint *)), this, SLOT(deleteHintAndUpdate(OSDHint *)));
	connect(hint, SIGNAL(updated(OSDHint *)), this, SLOT(hintUpdated()));
	setHint();

	if (!hint_timer->isActive())
		hint_timer->start(1000);
	if (frame->isHidden())
		frame->show();

	kdebugf2();

	return hint;
}

void OSDHintManager::setLayoutDirection()
{
	kdebugf();
	QPoint trayPosition;
	emit searchingForTrayPosition(trayPosition);
	switch(config_file.readNumEntry("OSDHints", "NewHintUnder"))
	{
		case 0:
			if (trayPosition.isNull() || config_file.readBoolEntry("OSDHints","UseUserPosition"))
			{
				if (config_file.readNumEntry("OSDHints","HintsPositionY") < QApplication::desktop()->size().height()/2)
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

void OSDHintManager::showToolTip(const QPoint &point, const UserListElement &user)
{
	kdebugf();

	QString text = KaduParser::parse(config_file.readEntry("OSDHints", "MouseOverUserSyntax"), user);

	/* Dorr: the file:// in img tag doesn't generate the image on hint.
	 * for compatibility with other syntaxes we're allowing to put the file://
	 * so we have to remove it here */
	text = text.replace("file://", "");

	while (text.endsWith("<br/>"))
		text.setLength(text.length() - 5 /* 5 == QString("<br/>").length()*/);
	while (text.startsWith("<br/>"))
		text = text.right(text.length() - 5 /* 5 == QString("<br/>").length()*/);

	if (tipFrame)
		delete tipFrame;

#ifdef Q_OS_MAC
	tipFrame = new QFrame(0, "tip_frame", Qt::FramelessWindowHint | Qt::SplashScreen | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#else
	tipFrame = new QFrame(0, "tip_frame", Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#endif
	tipFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	tipFrame->setFrameStyle(QFrame::Box | QFrame::Plain);

	QString style = QString("QFrame#tip_frame {border-width: %1px; border-style: solid; border-color: %2; border-radius: %3px;}").arg(config_file.readNumEntry("OSDHints", "SetAll_borderWidth", FRAME_WIDTH)).arg(config_file.readColorEntry("OSDHints", "SetAll_bdcolor").name()).arg(BORDER_RADIUS);
	tipFrame->setStyleSheet(style);

	opacity = config_file.readNumEntry("OSDHints", "Opacity", 100);
	opacity /= 100;
	tipFrame->setWindowOpacity(opacity);

	QHBoxLayout *lay = new QHBoxLayout(tipFrame, FRAME_WIDTH, 0, "lay");
	lay->setMargin(10);
	lay->setResizeMode(QLayout::Fixed);

	int iconSize = config_file.readNumEntry("OSDHints", "IconSize", 32);
	QString pic = dataPath("kadu/modules/data/osd_hints/") + user.status("Gadu").pixmapName().lower() + QString::number(iconSize) + ".png";

	QPixmap pixmap = icons_manager->loadPixmap(pic);
    	if (pixmap.isNull())
		pixmap = icons_manager->loadPixmap(dataPath("kadu/modules/data/osd_hints/osd_icon.png"));

	QLabel *icon = new QLabel(tipFrame, "Icon");
	icon->setPixmap(pixmap);
	icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	lay->addWidget(icon, 0, Qt::AlignTop);

	QLabel *tipLabel = new QLabel(text, tipFrame);
	tipLabel->setTextFormat(Qt::RichText);
	tipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

	lay->addWidget(tipLabel);

	tipFrame->setFixedSize(tipLabel->sizeHint() + QSize(2 * FRAME_WIDTH, 2 * FRAME_WIDTH));

	QPoint pos(point + QPoint(5, 5));

	QSize preferredSize = tipFrame->sizeHint();
	QSize desktopSize = QApplication::desktop()->size();
	if (pos.x() + preferredSize.width() > desktopSize.width())
		pos.setX(pos.x() - preferredSize.width() - lay->margin());
	if (pos.y() + preferredSize.height() > desktopSize.height())
		pos.setY(pos.y() - preferredSize.height() - lay->margin());

	tipFrame->resize(preferredSize.width(), preferredSize.height());
	tipFrame->move(pos);
	tipFrame->setMask(roundedRect(tipFrame->rect(), BORDER_RADIUS));
	tipFrame->show();

	kdebugf2();
}

void OSDHintManager::hideToolTip()
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

void OSDHintManager::notify(Notification *notification)
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
		OSDHint *linkedHint = linkedHints[qMakePair(ules, notification->type())];
		linkedHint->addDetail(notification->details());
	}
	else
	{
		OSDHint *linkedHint = addHint(notification);
		linkedHints[qMakePair(ules, notification->type())] = linkedHint;
	}

	kdebugf2();
}

void OSDHintManager::notificationClosed(Notification *notification)
{
	const UserListElements &ules = notification->userListElements();
	if (linkedHints.count(qMakePair(ules, notification->type())))
		linkedHints.remove(qMakePair(ules, notification->type()));
}

void OSDHintManager::copyConfiguration(const QString &fromEvent, const QString &toEvent)
{
}

void OSDHintManager::createDefaultConfiguration()
{
	QWidget w;

	config_file.addVariable("OSDHints", "CiteSign", 50);
	config_file.addVariable("OSDHints", "Corner", 0);
	config_file.addVariable("OSDHints", "DeletePendingMsgWhenHintDeleted", true);
	config_file.addVariable("OSDHints", "HintsPositionX", 0);
	config_file.addVariable("OSDHints", "HintsPositionY", 0);
	config_file.addVariable("OSDHints", "LeftButton", 1);
	config_file.addVariable("OSDHints", "RightButton", 2);
	config_file.addVariable("OSDHints", "MaximumWidth", 500);
	config_file.addVariable("OSDHints", "MiddleButton", 3);
	config_file.addVariable("OSDHints", "MinimumWidth", 100);
	config_file.addVariable("OSDHints", "MouseOverUserSyntax", "");
	config_file.addVariable("OSDHints", "NewHintUnder", 0);
	config_file.addVariable("OSDHints", "SetAll", false); // TODO: fix
	config_file.addVariable("OSDHints", "SetAll_bgcolor", w.paletteBackgroundColor());
	config_file.addVariable("OSDHints", "SetAll_fgcolor", w.paletteForegroundColor());
	config_file.addVariable("OSDHints", "SetAll_bdcolor", w.paletteForegroundColor());
	config_file.addVariable("OSDHints", "SetAll_font", *defaultFont);
	config_file.addVariable("OSDHints", "SetAll_timeout", 10);
	config_file.addVariable("OSDHints", "SetAll_borderWidth", 1);
	config_file.addVariable("OSDHints", "ShowContentMessage", true);
	config_file.addVariable("OSDHints", "UseUserPosition", false);
	config_file.addVariable("OSDHints", "OpenChatOnEveryNotification", false);
	config_file.addVariable("OSDHints", "IconSize", 32);
	config_file.addVariable("OSDHints", "Opacity", 100);
}

OSDHintManager *hint_manager = NULL;

/** @} */

