/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "docking.h"

#include "docking-menu-action-repository.h"
#include "docking-menu-handler.h"
#include "status-notifier-item.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/status-icon.h"
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
#include "provider/default-provider.h"
#include "notification/notification-service.h"
#include "status/status-changer.h"
#include "status/status-container-manager.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "activate.h"
#include "attention-service.h"
#include "debug.h"

#include <QtCore/QTimer>
#include <QtGui/QMouseEvent>
#include <QtGui/QTextDocument>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenu>

Docking * Docking::Instance = 0;

void Docking::createInstance()
{
	if (!Instance)
	{
		Instance = new Docking();
		Instance->init();
	}
}

void Docking::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

Docking * Docking::instance()
{
	return Instance;
}

Docking::Docking() :
		newMessageIcon(StaticEnvelope), icon_timer(new QTimer(this)), blink(false)
{
	kdebugf();

	m_dockingMenuActionRepository = new DockingMenuActionRepository{this};

	createDefaultConfiguration();

	Icon = new StatusIcon(StatusContainerManager::instance(), this);
	connect(Icon, SIGNAL(iconUpdated(KaduIcon)), this, SLOT(statusIconChanged(KaduIcon)));

	connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	connect(Core::instance()->attentionService(), SIGNAL(needAttentionChanged(bool)),
	        this, SLOT(needAttentionChanged(bool)));

	connect(Core::instance(), SIGNAL(searchingForTrayPosition(QPoint&)), this, SLOT(searchingForTrayPosition(QPoint&)));

	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));

	configurationUpdated();

	changeIcon();
	defaultToolTip();

	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "RunDocked"))
		Core::instance()->setShowMainWindowOnStart(false);
	Core::instance()->kaduWindow()->setDocked(true);

	kdebugf2();
}

void Docking::init()
{
	m_statusNotifierItem = new StatusNotifierItem{this};
	connect(m_statusNotifierItem, SIGNAL(messageClicked()), this, SIGNAL(messageClicked()));
	m_statusNotifierItem->setAssociatedWidget(Core::instance()->mainWindowProvider()->provide());
	m_statusNotifierItem->setIcon(defaultIcon());

	m_dockingMenuHandler = new DockingMenuHandler{m_statusNotifierItem->contextMenu(), this};
	m_dockingMenuHandler->setDockingMenuActionRepository(m_dockingMenuActionRepository);
	m_dockingMenuHandler->setIconsManager(IconsManager::instance());
	m_dockingMenuHandler->setNotificationService(Core::instance()->notificationService());
	m_dockingMenuHandler->setStatusContainerManager(StatusContainerManager::instance());
}

Docking::~Docking()
{
	kdebugf();

	if (!Core::instance()->isClosing())
		Core::instance()->kaduWindow()->window()->show();
	Core::instance()->kaduWindow()->setDocked(false);

	icon_timer->stop();

#ifdef Q_OS_MAC
	delete MacDockMenu;
	MacDockMenu = 0;
#endif
}

DockingMenuActionRepository * Docking::dockingMenuActionRepository() const
{
	return m_dockingMenuActionRepository;
}

void Docking::changeIcon()
{
	kdebugf();
	if (!Core::instance()->unreadMessageRepository()->hasUnreadMessages() && !icon_timer->isActive())
		return;

	switch (newMessageIcon)
	{
		case AnimatedEnvelope:
			m_statusNotifierItem->changeTrayMovie(KaduIcon("protocols/common/message_anim", "16x16").fullPath());
			break;
		case StaticEnvelope:
			m_statusNotifierItem->setIcon(KaduIcon("protocols/common/message"));
			break;
		case BlinkingEnvelope:
			if (!blink)
			{
				m_statusNotifierItem->setIcon(KaduIcon("protocols/common/message"));
				icon_timer->setSingleShot(true);
				icon_timer->start(500);
				blink = true;
			}
			else
			{
				m_statusNotifierItem->setIcon(StatusContainerManager::instance()->statusIcon());

				icon_timer->setSingleShot(true);
				icon_timer->start(500);
				blink = false;
			}
			break;
	}
}

void Docking::needAttentionChanged(bool needAttention)
{
	m_statusNotifierItem->setNeedAttention(needAttention);
}

QList<StatusPair> Docking::getStatuses() const
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

QList<DescriptionPair> Docking::getDescriptions() const
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

QString Docking::prepareDescription(const QString &description) const
{
	QColor color = qApp->palette().windowText().color();
	color.setAlpha(128);
	QString colorString = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());

	QString html = Qt::escape(description);
	html.replace('\n', QString(QLatin1String("<span style='color:%1;'> ") + QChar(0x21B5) + QLatin1String("</span><br />")).arg(colorString));

	return html;
}

void Docking::defaultToolTip()
{
	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "ShowTooltipInTray"))
	{
		QString tiptext("");

		// TODO: support KDE4 tray tooltip

#ifdef Q_OS_WIN

		Status status = StatusContainerManager::instance()->status();

		tiptext += QString("%1: %2").arg(tr("Status"), status.displayName());

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

		m_statusNotifierItem->changeTrayTooltip(tiptext);
	}
}

void Docking::openUnreadMessages()
{
	auto message = Core::instance()->unreadMessageRepository()->unreadMessage();
	Core::instance()->chatWidgetManager()->openChat(message.messageChat(), OpenChatActivation::Activate);
}

void Docking::trayMousePressEvent(QMouseEvent * e)
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
#ifndef Q_OS_WIN
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

void Docking::statusIconChanged(const KaduIcon &icon)
{
	kdebugf();

	if (Core::instance()->unreadMessageRepository()->hasUnreadMessages() || icon_timer->isActive())
		return;

	m_statusNotifierItem->setIcon(icon);

	defaultToolTip();
#ifdef Q_OS_MAC
	qApp->setWindowIcon(icon.icon());
#endif
}

void Docking::searchingForTrayPosition(QPoint &point)
{
	point = m_statusNotifierItem->trayPosition();
}

KaduIcon Docking::defaultIcon()
{
	return StatusContainerManager::instance()->statusIcon();
}

void Docking::configurationUpdated()
{
	if (Application::instance()->configuration()->deprecatedApi()->readBoolEntry("General", "ShowTooltipInTray"))
		defaultToolTip();
	else
	{
		m_statusNotifierItem->changeTrayTooltip(QString());
	}

	IconType it = (IconType)Application::instance()->configuration()->deprecatedApi()->readNumEntry("Look", "NewMessageIcon");
	if (newMessageIcon != it)
	{
		newMessageIcon = it;
		changeIcon();
	}
}

void Docking::createDefaultConfiguration()
{
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "RunDocked", false);
	Application::instance()->configuration()->deprecatedApi()->addVariable("General", "ShowTooltipInTray", true);
	Application::instance()->configuration()->deprecatedApi()->addVariable("Look", "NewMessageIcon", 0);
}

#ifdef Q_OS_MAC
void Docking::showMinimizedChats()
{
	foreach (ChatWidget *chat, Core::instance()->chatWidgetManager()->chats())
		chat->activate();
}

void Docking::dockIconClicked()
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

void Docking::showKaduWindow()
{
	_activateWindow(Core::instance()->kaduWindow());
}

void Docking::hideKaduWindow()
{
	Core::instance()->kaduWindow()->window()->hide();
}

#include "moc_docking.cpp"
