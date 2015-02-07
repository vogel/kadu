/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

#include "chat/chat-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/tool-tip-class-manager.h"
#include "message/message-manager.h"
#include "message/sorted-messages.h"
#include "message/unread-message-repository.h"
#include "misc/misc.h"
#include "notify/notification-manager.h"
#include "notify/notification/chat-notification.h"
#include "parser/parser.h"

#include "icons/icons-manager.h"
#include "activate.h"
#include "debug.h"
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
	connect(ChatManager::instance(), SIGNAL(chatUpdated(Chat)), this, SLOT(chatUpdated(Chat)));

	const QString default_hints_syntax(QT_TRANSLATE_NOOP("HintManager", "<table>"
"<tr>"
"<td align=\"left\" valign=\"top\">"
"<img style=\"max-width:64px; max-height:64px;\" "
"src=\"{#{avatarPath} #{avatarPath}}{~#{avatarPath} @{kadu_icons/kadu:64x64}}\""
">"
"</td>"
"<td width=\"100%\">"
"<div>[<b>%a</b>][&nbsp;<b>(%g)</b>]</div>"
"[<div><img height=\"16\" width=\"16\" src=\"#{statusIconPath}\">&nbsp;&nbsp;%u</div>]"
"[<div><img height=\"16\" width=\"16\" src=\"@{phone:16x16}\">&nbsp;&nbsp;%m</div>]"
"[<div><img height=\"16\" width=\"16\" src=\"@{mail-message-new:16x16}\">&nbsp;&nbsp;%e</div>]"
"</td>"
"</tr>"
"</table>"
"[<hr><b>%s</b>][<b>:</b><br><small>%d</small>]"));
	if (Application::instance()->configuration()->deprecatedApi()->readEntry("Hints", "MouseOverUserSyntax").isEmpty())
		Application::instance()->configuration()->deprecatedApi()->writeEntry("Hints", "MouseOverUserSyntax", default_hints_syntax);

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

	if (hint_timer)
		hint_timer->stop();

	ToolTipClassManager::instance()->unregisterToolTipClass("Hints");
	NotificationManager::instance()->unregisterNotifier(this);

	disconnect();

	if (tipFrame)
		tipFrame->deleteLater();

	if (frame)
		frame->deleteLater();

	kdebugf2();
}

void HintManager::hintUpdated()
{
	setHint();
}

void HintManager::configurationUpdated()
{
	Style = QString("Hint {border-width: %1px; border-style: solid; border-color: %2; border-radius: %3px;}")
			.arg(Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "AllEvents_borderWidth", FRAME_WIDTH))
			.arg(Application::instance()->configuration()->deprecatedApi()->readColorEntry("Hints", "AllEvents_bdcolor").name())
			.arg(BORDER_RADIUS);
	frame->setStyleSheet(Style);

	Opacity = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "AllEvents_transparency", 0);
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

	int minimumWidth = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "MinimumWidth", 285);
	int maximumWidth = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "MaximumWidth", 500);

	minimumWidth = minimumWidth >= 285 ? minimumWidth : 285;
	maximumWidth = maximumWidth >= 285 ? maximumWidth : 285;

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
	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Hints", "UseUserPosition") || trayPosition.isNull())
	{
		newPosition = QPoint(Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "HintsPositionX"), Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "HintsPositionY"));

//		kdebugm(KDEBUG_INFO, "%d %d %d\n", Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "Corner"), preferredSize.width(), preferredSize.height());
		switch(Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "Corner"))
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

	frame->setWindowOpacity(Opacity);
	// Only setFixedSize() and move() (in this order) guarantees correct
	// placement on all platforms (at least those I tested).
	frame->setFixedSize(preferredSize);
	frame->move(newPosition);

	if (frame->isVisible())
		frame->update();
	else
		frame->show();

	kdebugf2();
}

void HintManager::deleteHint(Hint *hint)
{
	kdebugf();

	Notification *notification = hint->getNotification();

	DisplayedNotifications.removeAll(hint->getNotification()->identifier());
	hints.removeAll(hint);

	layout->removeWidget(hint);

	hint->deleteLater();
	notification->release(this);

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

	switch (Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", buttonName))
	{
		case 1:
			hint->acceptNotification();
			break;

		case 2:
			if (hint->chat() && Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Hints", "DeletePendingMsgWhenHintDeleted"))
			{
				auto unreadMessages = Core::instance()->unreadMessageRepository()->unreadMessagesForChat(hint->chat());
				for (auto const &message : unreadMessages)
					Core::instance()->unreadMessageRepository()->removeUnreadMessage(message);
			}

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

	if (!Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Hints", "OpenChatOnEveryNotification"))
		if ((hint->getNotification()->type() != "NewChat") && (hint->getNotification()->type() != "NewMessage"))
			return;

	Core::instance()->chatWidgetManager()->openChat(hint->chat(), OpenChatActivation::Activate);

	deleteHintAndUpdate(hint);

	kdebugf2();
}

void HintManager::chatUpdated(const Chat &chat)
{
	if (chat.unreadMessagesCount() > 0)
		return;

	foreach (Hint *h, hints)
	{
		if (h->chat() == chat && !h->requireManualClosing())
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

	if (DisplayedNotifications.contains(notification->identifier()))
	{
		for (auto hint : hints)
			if (hint->getNotification()->identifier() == notification->identifier())
			{
				//hope this refreshes this hint
				hint->notificationUpdated();
				return hint;
			}
	}

	notification->acquire(this);

	connect(notification, SIGNAL(closed(Notification *)), this, SLOT(notificationClosed(Notification *)));

	auto hint = new Hint(frame, notification);
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

	DisplayedNotifications.append(notification->identifier());

	return hint;
}

void HintManager::setLayoutDirection()
{
	kdebugf();
	QPoint trayPosition;
	QSize desktopSize = QApplication::desktop()->screenGeometry(frame).size();
	emit searchingForTrayPosition(trayPosition);
	switch (Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "NewHintUnder"))
	{
		case 0:
			if (trayPosition.isNull() || Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Hints","UseUserPosition"))
			{
				if (Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints","HintsPositionY") < desktopSize.height()/2)
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

void HintManager::prepareOverUserHint(QFrame *tipFrame, QLabel *tipLabel, Talkable talkable)
{
	auto syntax = Application::instance()->configuration()->deprecatedApi()->readEntry("Hints", "MouseOverUserSyntax");
	// file:/// is added by parser where required
	syntax = syntax.remove("file:///");

	auto text = Parser::parse(syntax, talkable, ParserEscape::HtmlEscape);
	while (text.endsWith(QLatin1String("<br/>")))
		text.resize(text.length() - 5 /* 5 == QString("<br/>").length()*/);
	while (text.startsWith(QLatin1String("<br/>")))
		text = text.right(text.length() - 5 /* 5 == QString("<br/>").length()*/);

#ifdef Q_OS_UNIX
	text = text.remove("file://");
#endif

	tipLabel->setFont(Application::instance()->configuration()->deprecatedApi()->readFontEntry("Hints", "HintOverUser_font"));
	tipLabel->setText(text);

	tipFrame->setObjectName("tip_frame");
	QString style = QString("QFrame#tip_frame {border-width: %1px; border-style: solid; border-color: %2;"
				"border-radius: %3px; background-color: %4} QFrame { color: %5}")
			.arg(Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "HintOverUser_borderWidth", FRAME_WIDTH))
			.arg(Application::instance()->configuration()->deprecatedApi()->readColorEntry("Hints", "HintOverUser_bdcolor").name())
			.arg(BORDER_RADIUS)
			.arg(Application::instance()->configuration()->deprecatedApi()->readColorEntry("Hints", "HintOverUser_bgcolor").name())
			.arg(Application::instance()->configuration()->deprecatedApi()->readColorEntry("Hints", "HintOverUser_fgcolor").name());

	tipFrame->setStyleSheet(style);

	tipFrame->setFixedSize(tipLabel->sizeHint() + QSize(2 * FRAME_WIDTH, 2 * FRAME_WIDTH));
}

void HintManager::showToolTip(const QPoint &point, Talkable talkable)
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
	tipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	tipLabel->setContentsMargins(10, 10, 10, 10);
	tipLabel->setTextFormat(Qt::RichText);
	tipLabel->setWordWrap(true);

	lay->addWidget(tipLabel);

	prepareOverUserHint(tipFrame, tipLabel, talkable);

	double opacity = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "HintOverUser_transparency", 0);
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

	addHint(notification);

	kdebugf2();
}

void HintManager::notificationClosed(Notification *notification)
{
	Q_UNUSED(notification)
}

void HintManager::createDefaultConfiguration()
{
	// TODO: this should be more like: if (plugins.loaded(freedesktop_notify) && this_is_first_time_we_are_loaded_or_whatever)
#if !defined(Q_OS_UNIX) || defined(Q_OS_MAC)
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "ConnectionError_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "NewChat_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "NewMessage_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "StatusChanged_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "StatusChanged/ToFreeForChat_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOnline_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "StatusChanged/ToAway_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "StatusChanged/ToNotAvailable_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "StatusChanged/ToDoNotDisturb_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "StatusChanged/ToOffline_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "FileTransfer_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "FileTransfer/IncomingFile_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "FileTransfer/Finished_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "multilogon_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "multilogon/sessionConnected_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "multilogon/sessionDisconnected_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "Roster/ImportFailed_UseCustomSettings", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "Roster/ImportFailed_Hints", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "Roster/ExportFailed_UseCustomSettings", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Notify", "Roster/ExportFailed_Hints", true);
#endif

	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "CiteSign", 50);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "Corner", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "DeletePendingMsgWhenHintDeleted", true);

	//TODO:
	QStringList events;
	events << "ConnectionError" << "NewChat" << "NewMessage" << "StatusChanged"
		<<"StatusChanged/ToFreeForChat" << "StatusChanged/ToOnline"  << "StatusChanged/ToAway"
		<< "StatusChanged/ToNotAvailable"<< "StatusChanged/ToDoNotDisturb" << "StatusChanged/ToOffline"
		<< "FileTransfer" << "FileTransfer/IncomingFile" << "FileTransfer/Finished" << "InvalidPassword";
	foreach (const QString &event, events)
	{
		Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "Event_" + event + "_bgcolor", qApp->palette().window().color());
		Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "Event_" + event + "_fgcolor",qApp->palette().windowText().color());
		Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "Event_" + event + "_font", qApp->font());
		Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "Event_" + event + "_timeout", 10);
	}

	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintsPositionX", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintsPositionY", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "LeftButton", 1);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "RightButton", 2);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "MaximumWidth", 500);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "MiddleButton", 3);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "MinimumWidth", 285);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "MouseOverUserSyntax", QString());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "NewHintUnder", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "ShowContentMessage", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "UseUserPosition", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "OpenChatOnEveryNotification", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "MarginSize", 2);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "AllEvents_transparency", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "AllEvents_iconSize", 32);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "AllEvents_borderWidth", FRAME_WIDTH);

	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintOverUser_transparency", 0);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintOverUser_iconSize", 32);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintOverUser_borderWidth", FRAME_WIDTH);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintOverUser_bdcolor", qApp->palette().window().color());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintOverUser_bgcolor", qApp->palette().window().color());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintOverUser_fgcolor", qApp->palette().windowText().color());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintOverUser_font", qApp->font());
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintOverUser_Geometry", "50, 50, 640, 610");
	Application::instance()->configuration()->deprecatedApi()->addVariable("Hints", "HintEventConfiguration_Geometry", "50, 50, 520, 345");
}

HintManager *hint_manager = NULL;

#include "moc_hint_manager.cpp"
