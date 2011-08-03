/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2009, 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QDesktopWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

#include "contacts/contact.h"
#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/tool-tip-class-manager.h"
#include "gui/widgets/chat-widget.h"
#include "misc/misc.h"
#include "notify/chat-notification.h"
#include "notify/notification-manager.h"
#include "parser/parser.h"

#include "activate.h"
#include "debug.h"
#include "icons/icons-manager.h"
#include "hints-configuration-ui-handler.h"
#include "hints_configuration_widget.h"

#include "hint_manager.h"


/**
 * @ingroup hints
 * @{
 */
#define FRAME_WIDTH 1
#define BORDER_RADIUS 0

HintManager::HintManager(QObject *parent) :
		Notifier("Hints", "Hints", KaduIcon("kadu_icons/notify-hints"), parent), AbstractToolTip(),
		hint_timer(new QTimer(this)),
		tipFrame(0), hints()
{
	kdebugf();

	import_0_6_5_configuration();
	createDefaultConfiguration();

#ifdef Q_OS_MAC
	frame = new QFrame(0, Qt::FramelessWindowHint | Qt::SplashScreen | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#else
	frame = new QFrame(0, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#endif
	frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	layout = new QVBoxLayout(frame);
	layout->setSpacing(0);
	layout->setMargin(0);

	connect(hint_timer, SIGNAL(timeout()), this, SLOT(oneSecond()));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)));

	const QString default_hints_syntax(QT_TRANSLATE_NOOP("HintManager", "<table>"
"<tr>"
"<td align=\"left\" valign=\"top\">"
"<img style=\"max-width:64px; max-height:64px;\" "
"src=\"{#{avatarPath} #{avatarPath}}{~#{avatarPath} @{kadu_icons/kadu:64x64}}\""
">"
"</td>"
"<td width=\"100%\">"
"<div>[<b>%a</b>][&nbsp;<b>(%g)</b>]</div>"
"[<div><img height=\"16\" width=\"16\" src=\"file:///#{statusIconPath}\">&nbsp;&nbsp;%u</div>]"
"[<div><img height=\"16\" width=\"16\" src=\"file:///@{phone:16x16}\">&nbsp;&nbsp;%m</div>]"
"[<div><img height=\"16\" width=\"16\" src=\"file:///@{mail-message-new:16x16}\">&nbsp;&nbsp;%e</div>]"
"</td>"
"</tr>"
"</table>"
"[<hr><b>%s</b>][<b>:</b><br><small>%d</small>]"));
	if (config_file.readEntry("Hints", "MouseOverUserSyntax").isEmpty())
		config_file.writeEntry("Hints", "MouseOverUserSyntax", default_hints_syntax);

	connect(this, SIGNAL(searchingForTrayPosition(QPoint &)), Core::instance(), SIGNAL(searchingForTrayPosition(QPoint &)));

	NotificationManager::instance()->registerNotifier(this);
	ToolTipClassManager::instance()->registerToolTipClass(QT_TRANSLATE_NOOP("@default", "Hints"), this);

	configurationUpdated();

	// remember to call it after setting `Style' member
	UiHandler = new HintsConfigurationUiHandler(Style, this);

	kdebugf2();
}

HintManager::~HintManager()
{
	kdebugf();

	hint_timer->stop();

	ToolTipClassManager::instance()->unregisterToolTipClass("Hints");
	NotificationManager::instance()->unregisterNotifier(this);

	disconnect(this, SIGNAL(searchingForTrayPosition(QPoint &)), Core::instance(), SIGNAL(searchingForTrayPosition(QPoint &)));
	disconnect(ChatWidgetManager::instance(), SIGNAL(chatWidgetActivated(ChatWidget *)), this, SLOT(chatWidgetActivated(ChatWidget *)));

	delete tipFrame;
	tipFrame = 0;

	delete frame;
	frame = 0;

	kdebugf2();
}

void HintManager::hintUpdated()
{
	setHint();
}

void HintManager::configurationUpdated()
{
	Style = QString("Hint {border-width: %1px; border-style: solid; border-color: %2; border-radius: %3px;}")
			.arg(config_file.readNumEntry("Hints", "AllEvents_borderWidth", FRAME_WIDTH))
			.arg(config_file.readColorEntry("Hints", "AllEvents_bdcolor").name())
			.arg(BORDER_RADIUS);
	frame->setStyleSheet(Style);

	Opacity = config_file.readNumEntry("Hints", "AllEvents_transparency", 0);
	Opacity = 1 - Opacity/100;

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

	int minimumWidth = config_file.readNumEntry("Hints", "MinimumWidth", 100);
	int maximumWidth = config_file.readNumEntry("Hints", "MaximumWidth", 500);

	QPoint newPosition;
	QPoint trayPosition;

	frame->adjustSize();
	QSize preferredSize = frame->sizeHint();
	if (preferredSize.width() < minimumWidth)
		preferredSize.setWidth(minimumWidth);
	if (preferredSize.width() > maximumWidth)
		preferredSize.setWidth(maximumWidth);
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
	frame->setFixedWidth(preferredSize.width());
	frame->setWindowOpacity(Opacity);

	if (frame->isVisible())
		frame->update();
	else
		frame->show();

	kdebugf2();
}

void HintManager::deleteHint(Hint *hint)
{
	kdebugf();

	hints.removeAll(hint);
	for (QMap<QPair<Chat, QString>, Hint *>::iterator it = linkedHints.begin(); it != linkedHints.end(); )
	{
		if (it.value() == hint)
			it = linkedHints.erase(it);
		else
			it++;
	}

	layout->removeWidget(hint);
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

NotifierConfigurationWidget *HintManager::createConfigurationWidget(QWidget *parent)
{
	configurationWidget = new HintsConfigurationWidget(parent);
	return configurationWidget;
}

void HintManager::processButtonPress(const QString &buttonName, Hint *hint)
{
	kdebugmf(KDEBUG_FUNCTION_START, "%s\n", buttonName.toUtf8().constData());

	switch (config_file.readNumEntry("Hints", buttonName))
	{
		case 1:
			openChat(hint);
			hint->acceptNotification();
			break;

		case 2:
			if (hint->chat() && config_file.readBoolEntry("Hints", "DeletePendingMsgWhenHintDeleted"))
				ChatWidgetManager::instance()->deletePendingMessages(hint->chat());

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

	ChatWidgetManager::instance()->openPendingMessages(hint->chat(), true);

	deleteHintAndUpdate(hint);

	kdebugf2();
}

void HintManager::chatWidgetActivated(ChatWidget *chatWidget)
{
	QPair<Chat , QString> newChat = qMakePair(chatWidget->chat(), QString("NewChat"));
	QPair<Chat , QString> newMessage = qMakePair(chatWidget->chat(), QString("NewMessage"));

	if (linkedHints.contains(newChat))
	{
		Hint *linkedHint = linkedHints.take(newChat);
		linkedHint->close();
	}
	if (linkedHints.contains(newMessage))
	{
		Hint *linkedHint = linkedHints.take(newMessage);
		linkedHint->close();
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
		{
			h->discardNotification();
			deleteHint(h);
		}
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
	connect(hint, SIGNAL(updated(Hint *)), this, SLOT(hintUpdated()));
	setHint();

	if (!hint_timer->isActive())
		hint_timer->start(1000);

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

void HintManager::prepareOverUserHint(QFrame *tipFrame, QLabel *tipLabel, BuddyOrContact buddyOrContact)
{
	QString text = Parser::parse(config_file.readEntry("Hints", "MouseOverUserSyntax"), buddyOrContact);

	/* Dorr: the file:// in img tag doesn't generate the image on hint.
	 * for compatibility with other syntaxes we're allowing to put the file://
	 * so we have to remove it here */
	text = text.remove("file://");

	while (text.endsWith(QLatin1String("<br/>")))
		text.resize(text.length() - 5 /* 5 == QString("<br/>").length()*/);
	while (text.startsWith(QLatin1String("<br/>")))
		text = text.right(text.length() - 5 /* 5 == QString("<br/>").length()*/);

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

void HintManager::showToolTip(const QPoint &point, BuddyOrContact buddyOrContact)
{
	kdebugf();

	delete tipFrame;

#ifdef Q_OS_MAC
	tipFrame = new QFrame(0, Qt::FramelessWindowHint | Qt::SplashScreen | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#else
	tipFrame = new QFrame(0, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#endif

	QHBoxLayout *lay = new QHBoxLayout(tipFrame);
	lay->setMargin(10);
	lay->setSizeConstraint(QLayout::SetFixedSize);

	QLabel *tipLabel = new QLabel(tipFrame);
	tipLabel->setTextFormat(Qt::RichText);
	tipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	tipLabel->setContentsMargins(10, 10, 10, 10);

	lay->addWidget(tipLabel);

	prepareOverUserHint(tipFrame, tipLabel, buddyOrContact);

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
	if (tipFrame)
	{
		tipFrame->hide();
		tipFrame->deleteLater();
		tipFrame = 0;
	}
}

void HintManager::notify(Notification *notification)
{
	kdebugf();

	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	//TODO hack
	if (!chatNotification || notification->type().contains("StatusChanged"))
	{
		addHint(notification);

		kdebugf2();
		return;
	}

	if (linkedHints.contains(qMakePair(chatNotification->chat(), notification->type())))
	{
		Hint *linkedHint = linkedHints.value(qMakePair(chatNotification->chat(), notification->type()));
		linkedHint->addDetail(notification->details());
	}
	else
	{
		Hint *linkedHint = addHint(notification);
		linkedHints.insert(qMakePair(chatNotification->chat(), notification->type()), linkedHint);
	}

	kdebugf2();
}

void HintManager::notificationClosed(Notification *notification)
{
	ChatNotification *chatNotification = qobject_cast<ChatNotification *>(notification);
	if (!chatNotification)
		return;

	if (linkedHints.contains(qMakePair(chatNotification->chat(), notification->type())))
		linkedHints.remove(qMakePair(chatNotification->chat(), notification->type()));
}

void HintManager::realCopyConfiguration(const QString &fromCategory, const QString &fromHint, const QString &toHint)
{
	QFont font(qApp->font());
	QPalette palette(qApp->palette());

	config_file.writeEntry("Hints", toHint + "_font", config_file.readFontEntry(fromCategory, fromHint + "_font", &font));
	config_file.writeEntry("Hints", toHint + "_fgcolor", config_file.readColorEntry(fromCategory, fromHint + "_fgcolor", &palette.windowText().color()));
	config_file.writeEntry("Hints", toHint + "_bgcolor", config_file.readColorEntry(fromCategory, fromHint + "_bgcolor", &palette.window().color()));
	config_file.writeEntry("Hints", toHint + "_timeout", (int) config_file.readUnsignedNumEntry(fromCategory,  fromHint + "_timeout", 10));
}

void HintManager::import_0_6_5_configuration()
{
	config_file.addVariable("Hints", "AllEvents_transparency", 1 - config_file.readNumEntry("OSDHints", "Opacity", 100)/100);
	config_file.addVariable("Hints", "AllEvents_iconSize", config_file.readNumEntry("OSDHints", "IconSize", 32));
	config_file.addVariable("Hints", "AllEvents_borderWidth",config_file.readNumEntry("OSDHints", "SetAll_borderWidth", FRAME_WIDTH));
	config_file.addVariable("Hints", "AllEvents_bdcolor", config_file.readColorEntry("OSDHints", "SetAll_bdcolor", &qApp->palette().window().color()).name());

	config_file.addVariable("Hints", "HintOverUser_transparency", 1 - config_file.readNumEntry("OSDHints", "Opacity", 100)/100);
	config_file.addVariable("Hints", "HintOverUser_iconSize", config_file.readNumEntry("OSDHints", "IconSize", 32));
	config_file.addVariable("Hints", "HintOverUser_borderWidth",config_file.readNumEntry("OSDHints", "SetAll_borderWidth", FRAME_WIDTH));
	config_file.addVariable("Hints", "HintOverUser_bdcolor", config_file.readColorEntry("OSDHints", "SetAll_bdcolor", &qApp->palette().window().color()).name());
	config_file.addVariable("Hints", "HintOverUser_bgcolor", config_file.readColorEntry("OSDHints", "SetAll_bgcolor", &qApp->palette().window().color()).name());
	config_file.addVariable("Hints", "HintOverUser_fgcolor", config_file.readColorEntry("OSDHints", "SetAll_fgcolor", &qApp->palette().windowText().color()).name());
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
			config_file.removeVariable("Notify", event + "_OSDHints");
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
		config_file.removeVariable("Notify", "StatusChanged/ToBusy_OSDHints");
	}
	else
	{
		if (osdHintsSetAll)
			realCopyConfiguration("Hints", "SetAll", "ToAway");
		else
			realCopyConfiguration("Hints", "StatusChanged/ToBusy", "StatusChanged/ToAway");
	}
	config_file.removeVariable("OSDHints", "SetAll");
	config_file.removeVariable("Hints", "SetAll");

}

void HintManager::createDefaultConfiguration()
{
#if !defined(Q_WS_X11) || defined(Q_WS_MAEMO_5)
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
	config_file.addVariable("Notify", "multilogon_Hints", true);
	config_file.addVariable("Notify", "multilogon/sessionConnected_Hints", true);
	config_file.addVariable("Notify", "multilogon/sessionDisconnected_Hints", true);
#endif

	config_file.addVariable("Hints", "CiteSign", 50);
	config_file.addVariable("Hints", "Corner", 0);
	config_file.addVariable("Hints", "DeletePendingMsgWhenHintDeleted", true);

	//TODO:
	QStringList events;
	events << "ConnectionError" << "NewChat" << "NewMessage" << "StatusChanged"
		<<"StatusChanged/ToFreeForChat" << "StatusChanged/ToOnline"  << "StatusChanged/ToAway"
		<< "StatusChanged/ToNotAvailable"<< "StatusChanged/ToDoNotDisturb" << "StatusChanged/ToOffline"
		<< "FileTransfer" << "FileTransfer/IncomingFile" << "FileTransfer/Finished" << "InvalidPassword";
	foreach (const QString &event, events)
	{
		config_file.addVariable("Hints", "Event_" + event + "_bgcolor", qApp->palette().window().color());
		config_file.addVariable("Hints", "Event_" + event + "_fgcolor",qApp->palette().windowText().color());
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
	config_file.addVariable("Hints", "MouseOverUserSyntax", QString());
	config_file.addVariable("Hints", "NewHintUnder", 0);
	config_file.addVariable("Hints", "ShowContentMessage", true);
	config_file.addVariable("Hints", "UseUserPosition", false);
	config_file.addVariable("Hints", "OpenChatOnEveryNotification", false);
	config_file.addVariable("Hints", "MarginSize", 2);

	config_file.addVariable("Hints", "AllEvents_transparency", 0);
	config_file.addVariable("Hints", "AllEvents_iconSize", 32);
	config_file.addVariable("Hints", "AllEvents_borderWidth", FRAME_WIDTH);

	config_file.addVariable("Hints", "HintOverUser_transparency", 0);
	config_file.addVariable("Hints", "HintOverUser_iconSize", 32);
	config_file.addVariable("Hints", "HintOverUser_borderWidth", FRAME_WIDTH);
	config_file.addVariable("Hints", "HintOverUser_bdcolor", qApp->palette().window().color());
	config_file.addVariable("Hints", "HintOverUser_bgcolor", qApp->palette().window().color());
	config_file.addVariable("Hints", "HintOverUser_fgcolor", qApp->palette().windowText().color());
	config_file.addVariable("Hints", "HintOverUser_font", qApp->font());
	config_file.addVariable("Hints", "HintOverUser_Geometry", "50, 50, 640, 610");
	config_file.addVariable("Hints", "HintEventConfiguration_Geometry", "50, 50, 520, 345");
}

HintManager *hint_manager = NULL;
