/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright end%
 *
 * Copyright 2011 Adam "Vertex" Makświej (vertexbz@gmail.com)
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

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QTextDocument>

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/status-menu.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/main-configuration-window.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "message/message.h"
#include "message/unread-message-repository.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "services/notification-service.h"
#include "status/status-changer.h"
#include "status/status-container-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "activate.h"
#include "debug.h"

#ifdef Q_OS_MAC
#include "services/notification-service.h"
#include "mac_docking_helper.h"
extern void qt_mac_set_dock_menu(QMenu *);
#endif

#include "docker.h"

#include <gui/status-icon.h>
#include "docking.h"

DockingManager * DockingManager::Instance = 0;

void DockingManager::createInstance()
{
	if (!Instance)
		Instance = new DockingManager();
#ifdef Q_OS_MAC
	MacDockingHelper::instance();
#endif
}

void DockingManager::destroyInstance()
{
#ifdef Q_OS_MAC
	MacDockingHelper::destroyInstance();
#endif
	delete Instance;
	Instance = 0;
}

DockingManager * DockingManager::instance()
{
	return Instance;
}

DockingManager::DockingManager() :
		CurrentDocker(0), DockMenuNeedsUpdate(true), AllAccountsMenu(0),
		newMessageIcon(StaticEnvelope), icon_timer(new QTimer(this)), blink(false)
{
	kdebugf();

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	KaduWindowLastTimeVisible = true;
#endif

	createDefaultConfiguration();

	Icon = new StatusIcon(StatusContainerManager::instance(), this);
	connect(Icon, SIGNAL(iconUpdated(KaduIcon)), this, SLOT(statusIconChanged(KaduIcon)));

	connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	connect(Core::instance()->unreadMessageRepository(), SIGNAL(unreadMessageAdded(Message)),
	        this, SLOT(unreadMessageAdded()));
	connect(Core::instance()->unreadMessageRepository(), SIGNAL(unreadMessageRemoved(Message)),
	        this, SLOT(unreadMessageRemoved()));

	connect(Core::instance(), SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(searchingForTrayPosition(QPoint&)));

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));

	DockMenu = new QMenu();
	DockMenu->setSeparatorsCollapsible(true);
	connect(DockMenu, SIGNAL(aboutToShow()), this, SLOT(contextMenuAboutToBeShown()));

#ifdef Q_OS_MAC
	MacDockMenu = new QMenu();
	qt_mac_set_dock_menu(MacDockMenu);
	MacDockMenu->setSeparatorsCollapsible(true);
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	ShowKaduAction = new QAction(tr("&Restore"), this);
	connect(ShowKaduAction, SIGNAL(triggered()), this, SLOT(showKaduWindow()));

	HideKaduAction = new QAction(tr("&Minimize"), this);
	connect(HideKaduAction, SIGNAL(triggered()), this, SLOT(hideKaduWindow()));
#endif

	CloseKaduAction = new QAction(KaduIcon("application-exit").icon(), tr("&Exit Kadu"), this);
	connect(CloseKaduAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	configurationUpdated();

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	/* Fill context menu for the first time. It seems that Ubuntu panels, when at bottom
	 * of the screen, set maximum height for menus to what their height was when they
	 * were clicked for the first time. And normally we fill the menu in response to
	 * click. See Kadu bug #2627. */
	doUpdateContextMenu();
#endif

	kdebugf2();
}

DockingManager::~DockingManager()
{
	kdebugf();

	icon_timer->stop();

	delete DockMenu;
	DockMenu = 0;
#ifdef Q_OS_MAC
	delete MacDockMenu;
	MacDockMenu = 0;
#endif
}

void DockingManager::changeIcon()
{
	kdebugf();
	if (!Core::instance()->unreadMessageRepository()->hasUnreadMessages() && !icon_timer->isActive())
		return;

	switch (newMessageIcon)
	{
		case AnimatedEnvelope:
			if (CurrentDocker)
				CurrentDocker->changeTrayMovie(KaduIcon("protocols/common/message_anim", "16x16").fullPath());
			break;
		case StaticEnvelope:
			if (CurrentDocker)
				CurrentDocker->changeTrayIcon(KaduIcon("protocols/common/message"));
			break;
		case BlinkingEnvelope:
			if (!blink)
			{
				if (CurrentDocker)
					CurrentDocker->changeTrayIcon(KaduIcon("protocols/common/message"));
				icon_timer->setSingleShot(true);
				icon_timer->start(500);
				blink = true;
			}
			else
			{
				if (CurrentDocker)
					CurrentDocker->changeTrayIcon(StatusContainerManager::instance()->statusIcon());

				icon_timer->setSingleShot(true);
				icon_timer->start(500);
				blink = false;
			}
			break;
	}
}

void DockingManager::unreadMessageAdded()
{
	changeIcon();
#ifdef Q_OS_MAC
	MacDockingHelper::instance()->overlay(MessageManager::instance()->unreadMessagesCount());
	if (!Core::instance()->notificationService()->silentMode())
		MacDockingHelper::instance()->startBounce();
#endif
}

void DockingManager::unreadMessageRemoved()
{
#ifdef Q_OS_MAC
	MacDockingHelper::instance()->overlay(MessageManager::instance()->unreadMessagesCount());
	MacDockingHelper::instance()->stopBounce();
#endif
	if (!Core::instance()->unreadMessageRepository()->hasUnreadMessages())
		if (CurrentDocker)
			CurrentDocker->changeTrayIcon(defaultIcon());
}

QList<StatusPair> DockingManager::getStatuses() const
{
	QList<StatusPair> statusesList;

	QSet<StatusType> alltypes;

	foreach (StatusContainer *container, StatusContainerManager::instance()->subStatusContainers())
	{
		QList<StatusType> statuses;
		foreach (StatusContainer *innerContainer, container->subStatusContainers())
		{
			StatusType type = innerContainer->status().type();
			if (!statuses.contains(type))
				statuses.append(type);
			alltypes.insert(type);
		}
		statusesList.append(qMakePair(container->statusContainerName(), statuses));
	}

	if (alltypes.count() == 1)
	{
		statusesList.clear();
		statusesList.append(qMakePair(QString(), QList<StatusType>() << *alltypes.begin()));
	}

	return statusesList;
}

QList<DescriptionPair> DockingManager::getDescriptions() const
{
	QList<DescriptionPair> descriptionsList;

	QHash<QString,QStringList> descriptionsContainers;
	QList<QString> orderedDescriptions;

	foreach (StatusContainer *container, StatusContainerManager::instance()->subStatusContainers())
	{
		QString description = container->status().description();
		descriptionsContainers[description].append(container->statusContainerName());
		if (!orderedDescriptions.contains(description))
			orderedDescriptions.append(description);
	}

	foreach(QString description, orderedDescriptions)
		descriptionsList.append(qMakePair(descriptionsContainers[description], description));

	return descriptionsList;
}

QString DockingManager::prepareDescription(const QString &description) const
{
	QColor color = qApp->palette().windowText().color();
	color.setAlpha(128);
	QString colorString = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());

	QString html = Qt::escape(description);
	html.replace('\n', QString(QLatin1String("<span style='color:%1;'> ") + QChar(0x21B5) + QLatin1String("</span><br />")).arg(colorString));

	return html;
}

void DockingManager::defaultToolTip()
{
	if (!CurrentDocker)
		return;

	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
	{
		QString tiptext("");

		// TODO: support KDE4 tray tooltip

#ifdef Q_OS_WIN

		Status status = StatusContainerManager::instance()->status();

		tiptext += QString("%1:\n%1").arg(tr("Status"), status.displayName());

		if (!status.description().isEmpty())
			tiptext += QString("\n\n%1:\n%2").arg(tr("Description"), status.description());

#else

		tiptext += "<table>";

		tiptext += QString("<tr><td align='center' style='white-space:nowrap; font-weight:bold;'><big>Kadu</big></td></tr>");

		QList<StatusPair> statuses = getStatuses();
		if (statuses.count() == 1)
		{
			QStringList displays;
			foreach (StatusType type, statuses.first().second)
				displays.append(StatusTypeManager::instance()->statusTypeData(type).displayName());
			QString status = displays.join(", ");
			tiptext += QString("<tr><td style='white-space:nowrap; font-weight:bold;'>%1:</td></tr>").arg(tr("Status"));
			tiptext += QString("<tr><td style='white-space:nowrap; padding:0 1.5em;'>%1</td></tr>").arg(status);
		}
		else if (statuses.count() > 1)
		{
			tiptext += QString("<tr><td style='white-space:nowrap; font-weight:bold;'>%1:</td></tr>").arg(tr("Statuses"));
			QString table = "<table>";
			foreach (StatusPair pair, statuses)
			{
				QString id = pair.first;
				QList<StatusType> list = pair.second;
				QStringList displays;
				foreach (StatusType type, list)
					displays.append(StatusTypeManager::instance()->statusTypeData(type).displayName());
				QString status = displays.join(", ");
				table += QString("<tr><td align='right' style='white-space:nowrap;'>%1:</td><td style='white-space:nowrap; padding-left:0.4em; font-style:italic;'>%2</td></tr>")
					.arg(id, status);
			}
			table += "</table>";
			tiptext += QString("<tr><td style='padding:0 1.5em;'>%1</td></tr>").arg(table);
		}

		QList<DescriptionPair> descriptions = getDescriptions();
		if (descriptions.count() == 1)
		{
			QString description = prepareDescription(descriptions.first().second);
			tiptext += QString("<tr><td style='white-space:nowrap; font-weight:bold;'>%1:</td></tr>").arg(tr("Description"));
			tiptext += QString("<tr><td style='padding:0 1.5em;'>%1</td></tr>").arg(description);
		}
		else if (descriptions.count() > 1)
		{
			tiptext += QString("<tr><td style='white-space:nowrap; font-weight:bold;'>%1:</td></tr>").arg(tr("Descriptions"));
 			QString table = "<table>";
			foreach (DescriptionPair pair, descriptions)
			{
				QStringList ids = pair.first;
				QString description = prepareDescription(pair.second);
				int k = 0;
				foreach (QString id, ids) // Qt has problems with calculating width of table cell containing <br>s
				{
					QString separator = (k < ids.count() - 1 ? "," : ":");
					if (k == 0)
						table += QString("<tr><td align='right' style='white-space:nowrap;'>%1%2</td><td rowspan='%3' style='padding-left:0.4em; font-style:italic;'>%4</td></tr>")
							.arg(id, separator, QString::number(ids.count()), description);
					else
						table += QString("<tr><td align='right' style='white-space:nowrap;'>%1%2</td></tr>")
							.arg(id, separator);
					k++;
				}
			}
			table += "</table>";
			tiptext += QString("<tr><td style='padding:0 1.5em;'>%1</td></tr>").arg(table);
		}

		tiptext += "</table>";
#endif

		CurrentDocker->changeTrayTooltip(tiptext);
	}
}

void DockingManager::showKaduWindow()
{
	_activateWindow(Core::instance()->kaduWindow());
}

void DockingManager::hideKaduWindow()
{
	KaduWindow *kaduWindow = Core::instance()->kaduWindow();
	if (kaduWindow->docked())
		kaduWindow->window()->hide();
}

void DockingManager::openUnreadMessages()
{
	const Message &message = Core::instance()->unreadMessageRepository()->unreadMessage();
	Core::instance()->chatWidgetManager()->openChat(message.messageChat(), OpenChatActivation::Activate);
}

void DockingManager::trayMousePressEvent(QMouseEvent * e)
{
	kdebugf();
	if (e->button() == Qt::MidButton)
	{
		emit mousePressMidButton();
		openUnreadMessages();
		return;
	}

	if (e->button() == Qt::LeftButton)
	{
		QWidget *kadu = Core::instance()->kaduWindow()->window();

		emit mousePressLeftButton();
		kdebugm(KDEBUG_INFO, "minimized: %d, visible: %d\n", kadu->isMinimized(), kadu->isVisible());

		if (Core::instance()->unreadMessageRepository()->hasUnreadMessages() && (e->modifiers() != Qt::ControlModifier))
		{
			openUnreadMessages();
			return;
		}

		if (kadu->isMinimized() || !kadu->isVisible()
#ifndef Q_OS_WIN32
				// NOTE: It won't work as expected on Windows since when you click on tray icon,
				// the tray will become active and any other window will loose focus.
				// See bug #1915.
				|| !_isActiveWindow(kadu)
#endif
				)
			showKaduWindow();
		else
			hideKaduWindow();

		return;
	}

	if (e->button() == Qt::RightButton)
	{
		emit mousePressRightButton();
		//showPopupMenu(dockMenu);
		return;
	}
	kdebugf2();
}

void DockingManager::statusIconChanged(const KaduIcon &icon)
{
	kdebugf();

	if (Core::instance()->unreadMessageRepository()->hasUnreadMessages() || icon_timer->isActive())
		return;

	if (CurrentDocker)
		CurrentDocker->changeTrayIcon(icon);

	defaultToolTip();
#ifdef Q_OS_MAC
	qApp->setWindowIcon(icon.icon());
#endif
}

void DockingManager::searchingForTrayPosition(QPoint &point)
{
	if (CurrentDocker)
		point = CurrentDocker->trayPosition();
}

KaduIcon DockingManager::defaultIcon()
{
	return StatusContainerManager::instance()->statusIcon();
}

void DockingManager::setDocker(Docker *docker)
{
	CurrentDocker = docker;

	if (CurrentDocker)
	{
		changeIcon();
		defaultToolTip();
#ifndef Q_OS_MAC
		if (config_file.readBoolEntry("General", "RunDocked"))
			Core::instance()->setShowMainWindowOnStart(false);
		Core::instance()->kaduWindow()->setDocked(true);
	}
	else
	{
 		if (!Core::instance()->isClosing())
			Core::instance()->kaduWindow()->window()->show();
		Core::instance()->kaduWindow()->setDocked(false);
#endif
	}
}

void DockingManager::contextMenuAboutToBeShown()
{
	if (DockMenuNeedsUpdate
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
			|| Core::instance()->kaduWindow()->window()->isVisible() != KaduWindowLastTimeVisible
#endif
			)
		doUpdateContextMenu();
}

void DockingManager::updateContextMenu()
{
	DockMenuNeedsUpdate = true;
}

void DockingManager::doUpdateContextMenu()
{
	if (AllAccountsMenu)
	{
		AllAccountsMenu->deleteLater();
		AllAccountsMenu = 0;
	}

	DockMenu->clear();
#ifdef Q_OS_MAC
	MacDockMenu->clear();
#endif

	qDeleteAll(StatusContainerMenus.values());
	StatusContainerMenus.clear();

	int statusContainersCount = StatusContainerManager::instance()->statusContainers().count();

	if (1 == statusContainersCount)
	{
		AllAccountsMenu = new StatusMenu(StatusContainerManager::instance(), false, DockMenu);
#ifdef Q_OS_MAC
		AllAccountsMenu = new StatusMenu(StatusContainerManager::instance(), false, MacDockMenu);
#endif
		connect(AllAccountsMenu, SIGNAL(menuRecreated()), this, SLOT(updateContextMenu()));
	}
	else
	{
		foreach (StatusContainer *container, StatusContainerManager::instance()->statusContainers())
		{
			QMenu *menu = new QMenu(container->statusContainerName(), DockMenu);
			menu->setIcon(container->statusIcon().icon());
			new StatusMenu(container, false, menu);
			StatusContainerMenus[container] = DockMenu->addMenu(menu);
			connect(container, SIGNAL(statusUpdated(StatusContainer *)), this, SLOT(containerStatusChanged(StatusContainer *)));
		}

		if (statusContainersCount > 1)
			containersSeparator = DockMenu->addSeparator();

		if (statusContainersCount > 0)
		{
			AllAccountsMenu = new StatusMenu(StatusContainerManager::instance(), true, DockMenu);
#ifdef Q_OS_MAC
			AllAccountsMenu = new StatusMenu(StatusContainerManager::instance(), true, MacDockMenu);
#endif
			connect(AllAccountsMenu, SIGNAL(menuRecreated()), this, SLOT(updateContextMenu()));
		}
	}

	if (!ModulesActions.isEmpty())
	{
		DockMenu->addSeparator();

		foreach (QAction *action, ModulesActions)
			DockMenu->addAction(action);
	}

	DockMenu->addSeparator();

	SilentModeAction = new QAction(KaduIcon("kadu_icons/enable-notifications").icon(), tr("Silent mode"), this);
	SilentModeAction->setCheckable(true);
	SilentModeAction->setChecked(Core::instance()->notificationService()->silentMode());
	connect(SilentModeAction, SIGNAL(triggered(bool)), this, SLOT(silentModeToggled(bool)));
	connect(Core::instance()->notificationService(), SIGNAL(silentModeToggled(bool)), SilentModeAction, SLOT(setChecked(bool)));
	DockMenu->addAction(SilentModeAction);

	DockMenu->addSeparator();

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	KaduWindowLastTimeVisible = Core::instance()->kaduWindow()->window()->isVisible();
	DockMenu->addAction(KaduWindowLastTimeVisible ? HideKaduAction : ShowKaduAction);
#endif
	DockMenu->addAction(CloseKaduAction);

	DockMenuNeedsUpdate = false;
}

void DockingManager::silentModeToggled(bool enabled)
{
	Core::instance()->notificationService()->setSilentMode(enabled);
}

void DockingManager::containerStatusChanged(StatusContainer *container)
{
	if (StatusContainerMenus[container])
		StatusContainerMenus[container]->setIcon(container->statusIcon().icon());
}

void DockingManager::iconThemeChanged()
{
	QMapIterator<StatusContainer *, QAction *> i(StatusContainerMenus);
	while (i.hasNext())
	{
		i.next();
		if (i.value() && i.key())
			i.value()->setIcon(i.key()->statusIcon().icon());
	}
}

void DockingManager::statusContainerRegistered(StatusContainer *statusContainer)
{
	Q_UNUSED(statusContainer)

	updateContextMenu();
}

void DockingManager::statusContainerUnregistered(StatusContainer *statusContainer)
{
	Q_UNUSED(statusContainer)

	updateContextMenu();
}

void DockingManager::configurationUpdated()
{
	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
		defaultToolTip();
	else
	{
		if (CurrentDocker)
			CurrentDocker->changeTrayTooltip(QString());
	}

	IconType it = (IconType)config_file.readNumEntry("Look", "NewMessageIcon");
	if (newMessageIcon != it)
	{
		newMessageIcon = it;
		changeIcon();
	}
}

void DockingManager::createDefaultConfiguration()
{
	config_file.addVariable("General", "RunDocked", false);
	config_file.addVariable("General", "ShowTooltipInTray", true);
	config_file.addVariable("Look", "NewMessageIcon", 0);
}

void DockingManager::registerModuleAction(QAction *action)
{
	if (ModulesActions.contains(action))
		return;

	ModulesActions.append(action);
	updateContextMenu();
}

void DockingManager::unregisterModuleAction(QAction *action)
{
	if (!ModulesActions.contains(action))
		return;

	ModulesActions.removeAll(action);
	updateContextMenu();
}

#ifdef Q_OS_MAC
void DockingManager::showMinimizedChats()
{
	foreach (ChatWidget *chat, Core::instance()->chatWidgetManager()->chats())
		chat->activate();
}

void DockingManager::dockIconClicked()
{
	QWidget *kadu = Core::instance()->kaduWindow()->window();

	if (Core::instance()->unreadMessageRepository()->hasUnreadMessages())
	{
		openUnreadMessages();
		return;
	}

	if (kadu->isMinimized())
	{
		kadu->setWindowState(kadu->windowState() & ~Qt::WindowMinimized);
		kadu->show();
		showMinimizedChats();
		return;
	}
	else if (kadu->isVisible())
	{
		//raczej nie bedziemy ukrywac okna klikajac ikonke w docku
		//hideKaduWindow();
	}
	else
	{
		kadu->show();
		showMinimizedChats();
	}
	return;
}
#endif

#include "moc_docking.cpp"
