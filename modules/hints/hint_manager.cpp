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
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

#include "contacts/account-data/contact-account-data.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/tool-tip-class-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "misc/misc.h"
#include "notify/chat-notification.h"
#include "notify/notification-manager.h"
#include "parser/parser.h"

#include "activate.h"
#include "debug.h"
#include "icons-manager.h"
#include "hints_configuration_widget.h"
#include "hint-over-user-configuration-window.h"

#include "hint_manager.h"


/**
 * @ingroup hints
 * @{
 */
#define FRAME_WIDTH 1
#define BORDER_RADIUS 0

HintManager::HintManager(QWidget *parent)
	: Notifier("Hint", "Hints", IconsManager::instance()->loadIcon("OpenChat"), parent), AbstractToolTip(),
	hint_timer(new QTimer(this)),
	hints(), tipFrame(0), overUserConfigurationWindow(0)
{
	kdebugf();

	import_0_6_5_configuration();
	createDefaultConfiguration();

#ifdef Q_OS_MAC
	frame = new QFrame(parent, Qt::FramelessWindowHint | Qt::SplashScreen | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#else
	frame = new QFrame(parent, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#endif
	frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QString style = QString("QFrame {border-width: %1px; border-style: solid; border-color: %2; border-radius: %3px;}")
			.arg(config_file.readNumEntry("Hints", "AllEvents_borderWidth", FRAME_WIDTH))
			.arg(config_file.readColorEntry("Hints", "AllEvents_bdcolor").name())
			.arg(BORDER_RADIUS);
	frame->setStyleSheet(style);

	layout = new QVBoxLayout(frame);
	layout->setSpacing(0);
	layout->setMargin(FRAME_WIDTH);
	layout->setSizeConstraint(QLayout::SetFixedSize);

	opacity = config_file.readNumEntry("Hints", "AllEvents_transparency", 0);
	opacity = 1 - opacity/100;

	connect(hint_timer, SIGNAL(timeout()), this, SLOT(oneSecond()));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)));

	const QString default_hints_syntax(QT_TRANSLATE_NOOP("HintManager", "[<i>%s</i><br/>][<br/><b>Description:</b><br/>%d<br/><br/>][<i>Mobile:</i> <b>%m</b><br/>]"));
	if (config_file.readEntry("Hints", "MouseOverUserSyntax") == default_hints_syntax || config_file.readEntry("Hints", "MouseOverUserSyntax").isEmpty())
		config_file.writeEntry("Hints", "MouseOverUserSyntax", tr(default_hints_syntax.toAscii()));

	connect(this, SIGNAL(searchingForTrayPosition(QPoint &)), Core::instance(), SIGNAL(searchingForTrayPosition(QPoint &)));

	NotificationManager::instance()->registerNotifier(this);
	ToolTipClassManager::instance()->registerToolTipClass(QT_TRANSLATE_NOOP("@default", "Hints"), this);

	kdebugf2();
}

HintManager::~HintManager()
{
	kdebugf();

	ToolTipClassManager::instance()->unregisterToolTipClass("Hints");
	NotificationManager::instance()->unregisterNotifier(this);

	disconnect(this, SIGNAL(searchingForTrayPosition(QPoint &)), Core::instance(), SIGNAL(searchingForTrayPosition(QPoint &)));

	delete tipFrame;
	tipFrame = 0;

	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)));
	disconnect(hint_timer, SIGNAL(timeout()), this, SLOT(oneSecond()));
	delete hint_timer;
	hint_timer = 0;

	hints.clear();

	delete frame;
	frame = 0;

	kdebugf2();
}

void HintManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
    	connect(mainConfigurationWindow, SIGNAL(destroyed(QObject *)), this, SLOT(mainConfigurationWindowDestroyed()));

	connect(mainConfigurationWindow->widget()->widgetById("hints/showContent"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("hints/showContentCount"), SLOT(setEnabled(bool)));

	QWidget *ownPosition = mainConfigurationWindow->widget()->widgetById("hints/ownPosition");
	connect(ownPosition, SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("hints/ownPositionX"), SLOT(setEnabled(bool)));
	connect(ownPosition, SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("hints/ownPositionY"), SLOT(setEnabled(bool)));
	connect(ownPosition, SIGNAL(toggled(bool)), mainConfigurationWindow->widget()->widgetById("hints/ownPositionCorner"), SLOT(setEnabled(bool)));

	minimumWidth = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("hints/minimumWidth"));
	maximumWidth = dynamic_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("hints/maximumWidth"));
	connect(minimumWidth, SIGNAL(valueChanged(int)), this, SLOT(minimumWidthChanged(int)));
	connect(maximumWidth, SIGNAL(valueChanged(int)), this, SLOT(maximumWidthChanged(int)));

	connect(mainConfigurationWindow->widget()->widgetById("toolTipClasses"), SIGNAL(currentIndexChanged(const QString &)),
		this, SLOT(toolTipClassesHighlighted(const QString &)));

	ConfigGroupBox *toolTipBox = mainConfigurationWindow->widget()->configGroupBox("Look", "Userbox", "General");

	QWidget *configureHint = new QWidget(toolTipBox->widget());
	overUserConfigurationPreview = new QFrame(configureHint);
	QHBoxLayout *lay = new QHBoxLayout(overUserConfigurationPreview);
	lay->setMargin(10);
	lay->setSizeConstraint(QLayout::SetFixedSize);

	overUserConfigurationIconLabel = new QLabel(overUserConfigurationPreview);
	overUserConfigurationIconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	overUserConfigurationTipLabel = new QLabel(overUserConfigurationPreview);
	overUserConfigurationTipLabel->setTextFormat(Qt::RichText);
	overUserConfigurationTipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	overUserConfigurationTipLabel->setContentsMargins(10, 0, 0, 0);

	lay->addWidget(overUserConfigurationIconLabel, Qt::AlignTop);
	lay->addWidget(overUserConfigurationTipLabel);

    	configureOverUserHint = new QPushButton(tr("Configure"));
	connect(configureOverUserHint, SIGNAL(clicked()), this, SLOT(showOverUserConfigurationWindow()));

	Contact example = Contact::dummy();

	if (!example.isNull())
		prepareOverUserHint(overUserConfigurationPreview, overUserConfigurationIconLabel,
				    overUserConfigurationTipLabel, example);

	lay = new QHBoxLayout(configureHint);
	lay->addWidget(overUserConfigurationPreview);
	lay->addWidget(configureOverUserHint);

	toolTipBox->addWidgets(new QLabel("Hint over userlist: "), configureHint);
}

void HintManager::toolTipClassesHighlighted(const QString &value)
{
	configureOverUserHint->setEnabled(value == qApp->translate("@default", "Hints"));
	overUserConfigurationPreview->setEnabled(value == qApp->translate("@default", "Hints"));
}

NotifierConfigurationWidget *HintManager::createConfigurationWidget(QWidget *parent)
{
	configurationWidget = new HintsConfigurationWidget(parent);
	return configurationWidget;
}

void HintManager::showOverUserConfigurationWindow()
{
	if (overUserConfigurationWindow)
		_activateWindow(overUserConfigurationWindow);
	else
	{
		overUserConfigurationWindow = new HintOverUserConfigurationWindow(Contact::dummy());
		connect(overUserConfigurationWindow, SIGNAL(configurationSaved()), this, SLOT(updateOverUserPreview()));
		connect(overUserConfigurationWindow, SIGNAL(destroyed()), this, SLOT(hintOverUserConfigurationWindowDestroyed()));
		overUserConfigurationWindow->show();
	}
}

void HintManager::updateOverUserPreview()
{
	if (!overUserConfigurationPreview)
		return;

	Contact example = Contact::dummy();

	if (!example.isNull())
		prepareOverUserHint(overUserConfigurationPreview, overUserConfigurationIconLabel, overUserConfigurationTipLabel, example);
}

void HintManager::mainConfigurationWindowDestroyed()
{
	overUserConfigurationPreview = 0;
}

void HintManager::hintOverUserConfigurationWindowDestroyed()
{
	overUserConfigurationWindow = 0;
}

void HintManager::minimumWidthChanged(int value)
{
	if (value > maximumWidth->value())
		maximumWidth->setValue(value);
}

void HintManager::maximumWidthChanged(int value)
{
	if (value < minimumWidth->value())
		minimumWidth->setValue(value);
}

void HintManager::hintUpdated()
{
	setHint();
}

void HintManager::configurationUpdated()
{
    	QString style = QString("QFrame {border-width: %1px; border-style: solid; border-color: %2; border-radius: %3px;}")
			.arg(config_file.readNumEntry("Hints", "AllEvents_borderWidth", FRAME_WIDTH))
			.arg(config_file.readColorEntry("Hints", "AllEvents_bdcolor").name())
			.arg(BORDER_RADIUS);
	frame->setStyleSheet(style);

    	opacity = config_file.readNumEntry("Hints", "AllEvents_transparency", 0);
	opacity = 1 - opacity/100;

	setHint();
}

void HintManager::setHint()
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
	QSize desktopSize = QApplication::desktop()->screenGeometry(frame).size();

	emit searchingForTrayPosition(trayPosition);
	if (config_file.readBoolEntry("Hints", "UseUserPosition") || trayPosition.isNull())
	{
		newPosition = QPoint(config_file.readNumEntry("Hints", "HintsPositionX"), config_file.readNumEntry("Hints", "HintsPositionY"));

//		kdebugm(KDEBUG_INFO, "%d %d %d\n", config_file.readNumEntry("Hints", "Corner"), preferredSize.width(), preferredSize.height());
		switch(config_file.readNumEntry("Hints", "Corner"))
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

	frame->setWindowOpacity(opacity);

	kdebugf2();
}

void HintManager::deleteHint(Hint *hint)
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

void HintManager::deleteHintAndUpdate(Hint *hint)
{
	deleteHint(hint);
	setHint();
}

void HintManager::oneSecond(void)
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

void HintManager::processButtonPress(const QString &buttonName, Hint *hint)
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", buttonName.toAscii().data());

	switch (config_file.readNumEntry("Hints", buttonName))
	{
		case 1:
			openChat(hint);
			hint->acceptNotification();
			break;

		case 2:
			if (hint->chat() && config_file.readBoolEntry("Hints", "DeletePendingMsgWhenHintDeleted"))
				ChatWidgetManager::instance()->deletePendingMsgs(hint->chat());

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

	if (!hint->chat())
		return;

	if (!config_file.readBoolEntry("Hints", "OpenChatOnEveryNotification"))
		if ((hint->getNotification()->type() != "NewChat") && (hint->getNotification()->type() != "NewMessage"))
			return;

	ChatWidgetManager::instance()->openPendingMsgs(hint->chat(), true);

	deleteHintAndUpdate(hint);

	kdebugf2();
}

void HintManager::chatWidgetActivated(ChatWidget *chatWidget)
{
	QPair<Chat *, QString> newChat = qMakePair(chatWidget->chat(), QString("NewChat"));
	QPair<Chat *, QString> newMessage = qMakePair(chatWidget->chat(), QString("NewMessage"));

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

	foreach (Hint *h, hints)
	{
		if (h->chat() == (chatWidget->chat()) && !h->requireManualClosing())
			deleteHint(h);
	}

	setHint();
}

void HintManager::deleteAllHints()
{
	kdebugf();
	hint_timer->stop();

	foreach (Hint *h, hints)
	{
		if (!h->requireManualClosing())
			deleteHint(h);
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
	layout->addWidget(static_cast<QWidget *>(hint));

	connect(hint, SIGNAL(leftButtonClicked(Hint *)), this, SLOT(leftButtonSlot(Hint *)));
	connect(hint, SIGNAL(rightButtonClicked(Hint *)), this, SLOT(rightButtonSlot(Hint *)));
	connect(hint, SIGNAL(midButtonClicked(Hint *)), this, SLOT(midButtonSlot(Hint *)));
	connect(hint, SIGNAL(closing(Hint *)), this, SLOT(deleteHintAndUpdate(Hint *)));
	connect(hint, SIGNAL(updated(Hint *)), this, SLOT(hintUpdated()));
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
	QSize desktopSize = QApplication::desktop()->screenGeometry(frame).size();
	emit searchingForTrayPosition(trayPosition);
	switch (config_file.readNumEntry("Hints", "NewHintUnder"))
	{
		case 0:
			if (trayPosition.isNull() || config_file.readBoolEntry("Hints","UseUserPosition"))
			{
				if (config_file.readNumEntry("Hints","HintsPositionY") < desktopSize.height()/2)
					layout->setDirection(QBoxLayout::Down);
				else
					layout->setDirection(QBoxLayout::Up);
			}
			else
			{
				if (trayPosition.y() < desktopSize.height()/2)
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

void HintManager::prepareOverUserHint(QFrame *tipFrame, QLabel *iconLabel, QLabel *tipLabel, Contact contact)
{\
	QString text = Parser::parse(config_file.readEntry("Hints", "MouseOverUserSyntax"), contact.prefferedAccount(), contact);

	/* Dorr: the file:// in img tag doesn't generate the image on hint.
	 * for compatibility with other syntaxes we're allowing to put the file://
	 * so we have to remove it here */
	text = text.replace("file://", "");

	while (text.endsWith("<br/>"))
		text.resize(text.length() - 5 /* 5 == QString("<br/>").length()*/);
	while (text.startsWith("<br/>"))
		text = text.right(text.length() - 5 /* 5 == QString("<br/>").length()*/);

//TODO 0.6.6: icon:
	iconLabel->setPixmap(contact.prefferedAccount()->statusPixmap(contact.accountData(contact.prefferedAccount())->status()));
	tipLabel->setFont(config_file.readFontEntry("Hints", "HintOverUser_font"));
	tipLabel->setText(text);

	tipFrame->setObjectName("tip_frame");
	QString style = QString("QFrame#tip_frame {border-width: %1px; border-style: solid; border-color: %2;"
				"border-radius: %3px; background-color: %4} QFrame { color: %5}")
			.arg(config_file.readNumEntry("Hints", "HintOverUser_borderWidth", FRAME_WIDTH))
			.arg(config_file.readColorEntry("Hints", "HintOverUser_bdcolor").name())
			.arg(BORDER_RADIUS)
			.arg(config_file.readColorEntry("Hints", "HintOverUser_bgcolor").name())
			.arg(config_file.readColorEntry("Hints", "HintOverUser_fgcolor").name());

	tipFrame->setStyleSheet(style);

	tipFrame->setFixedSize(tipLabel->sizeHint() + QSize(2 * FRAME_WIDTH, 2 * FRAME_WIDTH));
}

void HintManager::showToolTip(const QPoint &point, Contact contact)
{
	kdebugf();

	if (tipFrame)
		delete tipFrame;

    #ifdef Q_OS_MAC
	tipFrame = new QFrame(0, Qt::FramelessWindowHint | Qt::SplashScreen | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
    #else
	tipFrame = new QFrame(0, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
    #endif

	QHBoxLayout *lay = new QHBoxLayout(tipFrame);
	lay->setMargin(10);
	lay->setSizeConstraint(QLayout::SetFixedSize);

	QLabel *icon = new QLabel(tipFrame);
	icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QLabel *tipLabel = new QLabel(tipFrame);
	tipLabel->setTextFormat(Qt::RichText);
	tipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	tipLabel->setContentsMargins(10, 0, 0, 0);

	lay->addWidget(icon, Qt::AlignTop);
	lay->addWidget(tipLabel);

	prepareOverUserHint(tipFrame, icon, tipLabel, contact);

    	double opacity = config_file.readNumEntry("Hints", "HintOverUser_transparency", 0);
	opacity = 1 - opacity/100;
	tipFrame->setWindowOpacity(opacity);

	QPoint pos(point + QPoint(5, 5));

	QSize preferredSize = tipFrame->sizeHint();
	QSize desktopSize = QApplication::desktop()->screenGeometry(frame).size();
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

	ChatNotification *chatNotification = dynamic_cast<ChatNotification *>(notification);
	if (!chatNotification)
	{
		addHint(notification);

		kdebugf2();
		return;
	}

	if (linkedHints.count(qMakePair(chatNotification->chat(), notification->type())))
	{
		Hint *linkedHint = linkedHints[qMakePair(chatNotification->chat(), notification->type())];
		linkedHint->addDetail(notification->details());
	}
	else
	{
		Hint *linkedHint = addHint(notification);
		linkedHints[qMakePair(chatNotification->chat(), notification->type())] = linkedHint;
	}

	kdebugf2();
}

void HintManager::notificationClosed(Notification *notification)
{
	ChatNotification *chatNotification = dynamic_cast<ChatNotification *>(notification);
	if (!chatNotification)
		return;

	if (linkedHints.count(qMakePair(chatNotification->chat(), notification->type())))
		linkedHints.remove(qMakePair(chatNotification->chat(), notification->type()));
}

void HintManager::copyConfiguration(const QString &fromEvent, const QString &toEvent)
{
}

void HintManager::realCopyConfiguration(const QString &fromCategory, const QString &fromHint, const QString &toHint)
{
	config_file.writeEntry("Hints", toHint + "_font", config_file.readFontEntry(fromCategory, fromHint + "_font", &qApp->font()));
	config_file.writeEntry("Hints", toHint + "_fgcolor", config_file.readColorEntry(fromCategory, fromHint + "_fgcolor", &qApp->palette().foreground().color()));
	config_file.writeEntry("Hints", toHint + "_bgcolor", config_file.readColorEntry(fromCategory, fromHint + "_bgcolor", &qApp->palette().background().color()));
	config_file.writeEntry("Hints", toHint + "_timeout", (int) config_file.readUnsignedNumEntry(fromCategory,  fromHint + "_timeout", 10));
}

void HintManager::import_0_6_5_configuration()
{
	config_file.addVariable("Hints", "AllEvents_transparency", 1 - config_file.readNumEntry("OSDHints", "Opacity", 100)/100);
	config_file.addVariable("Hints", "AllEvents_iconSize", config_file.readNumEntry("OSDHints", "IconSize", 32));
	config_file.addVariable("Hints", "AllEvents_borderWidth",config_file.readNumEntry("OSDHints", "SetAll_borderWidth", FRAME_WIDTH));
	config_file.addVariable("Hints", "AllEvents_bdcolor", config_file.readColorEntry("OSDHints", "SetAll_bdcolor").name());

	config_file.addVariable("Hints", "HintOverUser_transparency", 1 - config_file.readNumEntry("OSDHints", "Opacity", 100)/100);
	config_file.addVariable("Hints", "HintOverUser_iconSize", config_file.readNumEntry("OSDHints", "IconSize", 32));
	config_file.addVariable("Hints", "HintOverUser_borderWidth",config_file.readNumEntry("OSDHints", "SetAll_borderWidth", FRAME_WIDTH));
	config_file.addVariable("Hints", "HintOverUser_bdcolor", config_file.readColorEntry("OSDHints", "SetAll_bdcolor").name());
	config_file.addVariable("Hints", "HintOverUser_bgcolor", config_file.readColorEntry("OSDHints", "SetAll_bgcolor").name());
	config_file.addVariable("Hints", "HintOverUser_fgcolor", config_file.readColorEntry("OSDHints", "SetAll_fgcolor").name());
	config_file.addVariable("Hints", "HintOverUser_font", config_file.readFontEntry("OSDHints", "SetAll_font"));

	if (config_file.readEntry("Look", "UserboxToolTipStyle") == "OSDHints")
		config_file.writeEntry("Hints", "MouseOverUserSyntax", config_file.readEntry("OSDHints", "MouseOverUserSyntax"));

	QStringList events;
	events << "ConnectionError" << "NewChat" << "NewMessage" << "StatusChanged"
		<<"StatusChanged/ToFreeForChat" << "StatusChanged/ToOnline" << "StatusChanged/ToNotAvailable"
		<< "StatusChanged/ToDoNotDisturb" << "StatusChanged/ToOffline"
		<< "FileTransfer" << "FileTransfer/IncomingFile" << "FileTransfer/Finished";

	bool osdHintsSetAll = config_file.readBoolEntry("OSDHints", "SetAll", false);
	bool hintsSetAll = config_file.readBoolEntry("Hints", "SetAll", false);

	foreach (const QString &event, events)
	{
		if (config_file.readBoolEntry("Notify", event + "_OSDHints", false))
		{
			if (osdHintsSetAll)
				realCopyConfiguration("OSDHints", "SetAll", event);
			else
				realCopyConfiguration("OSDHints", event, event);

			config_file.writeEntry("Notify", event + "_Hints", true);
		}
		else if (hintsSetAll)
				realCopyConfiguration("Hints", "SetAll", event);

	}

	if (config_file.readBoolEntry("Notify", "StatusChanged/ToBusy_OSDHints", false))
	{
		if (osdHintsSetAll)
			realCopyConfiguration("OSDHints", "SetAll", "ToAway");
		else
			realCopyConfiguration("OSDHints", "StatusChanged/ToBusy", "StatusChanged/ToAway");

		config_file.writeEntry("Notify", "StatusChanged/ToAway_Hints", true);
	}
	else
	{
		if (osdHintsSetAll)
			realCopyConfiguration("Hints", "SetAll", "ToAway");
		else
			realCopyConfiguration("Hints", "StatusChanged/ToBusy", "StatusChanged/ToAway");
	}
}

void HintManager::createDefaultConfiguration()
{
	QWidget w;

	config_file.addVariable("Notify", "ConnectionError_Hints", true);
	config_file.addVariable("Notify", "NewChat_Hints", true);
	config_file.addVariable("Notify", "NewMessage_Hints", true);
	config_file.addVariable("Notify", "StatusChanged_Hints", true);
	config_file.addVariable("Notify", "StatusChanged/ToFreeForChat_Hints", true);
	config_file.addVariable("Notify", "StatusChanged/ToOnline_Hints", true);
	config_file.addVariable("Notify", "StatusChanged/ToAway_Hints", true);
	config_file.addVariable("Notify", "StatusChanged/ToNotAvailable_Hints", true);
	config_file.addVariable("Notify", "StatusChanged/ToDoNotDisturb_Hints", true);
	config_file.addVariable("Notify", "StatusChanged/ToOffline_Hints", true);
	config_file.addVariable("Notify", "FileTransfer_Hints", true);
	config_file.addVariable("Notify", "FileTransfer/IncomingFile_Hints", true);
	config_file.addVariable("Notify", "FileTransfer/Finished_Hints", true);

	config_file.addVariable("Hints", "CiteSign", 50);
	config_file.addVariable("Hints", "Corner", 0);
	config_file.addVariable("Hints", "DeletePendingMsgWhenHintDeleted", true);

	//TODO:
	QStringList events;
	events << "ConnectionError" << "NewChat" << "NewMessage" << "StatusChanged"
		<<"StatusChanged/ToFreeForChat" << "StatusChanged/ToOnline"  << "StatusChanged/ToAway"
		<< "StatusChanged/ToNotAvailable"<< "StatusChanged/ToDoNotDisturb" << "StatusChanged/ToOffline"
		<< "FileTransfer" << "FileTransfer/IncomingFile" << "FileTransfer/Finished";
	foreach (const QString &event, events)
	{
		config_file.addVariable("Hints", "Event_" + event + "_bgcolor", qApp->palette().background().color());
		config_file.addVariable("Hints", "Event_" + event + "_fgcolor",qApp->palette().foreground().color());
		config_file.addVariable("Hints", "Event_" + event + "_font", qApp->font());
		config_file.addVariable("Hints", "Event_" + event + "_timeout", 10);
	}

	config_file.addVariable("Hints", "HintsPositionX", 0);
	config_file.addVariable("Hints", "HintsPositionY", 0);
	config_file.addVariable("Hints", "LeftButton", 1);
	config_file.addVariable("Hints", "RightButton", 2);
	config_file.addVariable("Hints", "MaximumWidth", 500);
	config_file.addVariable("Hints", "MiddleButton", 3);
	config_file.addVariable("Hints", "MinimumWidth", 100);
	config_file.addVariable("Hints", "MouseOverUserSyntax", "");
	config_file.addVariable("Hints", "NewHintUnder", 0);
	config_file.addVariable("Hints", "ShowContentMessage", true);
	config_file.addVariable("Hints", "UseUserPosition", false);
	config_file.addVariable("Hints", "OpenChatOnEveryNotification", false);

	config_file.addVariable("Hints", "AllEvents_transparency", 0);
	config_file.addVariable("Hints", "AllEvents_iconSize", 32);
	config_file.addVariable("Hints", "AllEvents_borderWidth", FRAME_WIDTH);

	config_file.addVariable("Hints", "HintOverUser_transparency", 0);
	config_file.addVariable("Hints", "HintOverUser_iconSize", 32);
	config_file.addVariable("Hints", "HintOverUser_borderWidth", FRAME_WIDTH);
	config_file.addVariable("Hints", "HintOverUser_bdcolor", &qApp->palette().background().color());
	config_file.addVariable("Hints", "HintOverUser_bgcolor", &qApp->palette().background().color());
	config_file.addVariable("Hints", "HintOverUser_fgcolor", &qApp->palette().foreground().color());
	config_file.addVariable("Hints", "HintOverUser_font", qApp->font());
}

HintManager *hint_manager = NULL;

/** @} */

