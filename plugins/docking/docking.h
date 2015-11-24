/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef DOCKING_H
#define DOCKING_H

#include <QtCore/QMap>
#include <QtWidgets/QLabel>

#include "configuration/configuration-aware-object.h"
#include "status/status-container-aware-object.h"
#include "status/status-type.h"

#include "docking_exports.h"

class QAction;
class QMenu;

class Docker;
class KaduIcon;
class StatusContainer;
class StatusIcon;
class StatusMenu;

typedef QPair<QString,QList<StatusType> > StatusPair;
typedef QPair<QStringList,QString> DescriptionPair;

class StatusNotifierItem;

class DOCKINGAPI DockingManager : public QObject, ConfigurationAwareObject, StatusContainerAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(DockingManager)

	static DockingManager *Instance;

	StatusNotifierItem *m_statusNotifierItem;

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	bool KaduWindowLastTimeVisible;
#endif
	bool DockMenuNeedsUpdate;
	QMenu *DockMenu;
#ifdef Q_OS_MAC
	QMenu *MacDockMenu;
#endif

	StatusMenu *AllAccountsMenu;

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	QAction *ShowKaduAction;
	QAction *HideKaduAction;
#endif
	QAction *SilentModeAction;
	QAction *CloseKaduAction;
	QAction *containersSeparator;

	StatusIcon *Icon;

	QList<QAction *> ModulesActions;

	QMap<StatusContainer *, QAction *> StatusContainerMenus;

	enum IconType {BlinkingEnvelope = 0, StaticEnvelope = 1, AnimatedEnvelope = 2} newMessageIcon;
	QTimer *icon_timer;
	bool blink;

	QList<StatusPair> getStatuses() const;
	QList<DescriptionPair> getDescriptions() const;
	QString prepareDescription(const QString &description) const;
	void defaultToolTip();

	void createDefaultConfiguration();

	DockingManager();
	void init();

	virtual ~DockingManager();

	void doUpdateContextMenu();
	void openUnreadMessages();

private slots:
	void statusIconChanged(const KaduIcon &icon);
	void changeIcon();
	void unreadMessageAdded();
	void unreadMessageRemoved();
	void searchingForTrayPosition(QPoint &point);
	void iconThemeChanged();

	void showKaduWindow();
	void hideKaduWindow();
	void silentModeToggled(bool enabled);

	void contextMenuAboutToBeShown();
	void updateContextMenu();

	void containerStatusChanged(StatusContainer *container);

protected:
	virtual void configurationUpdated();
	virtual void statusContainerRegistered(StatusContainer *statusContainer);
	virtual void statusContainerUnregistered(StatusContainer *statusContainer);

public:
	static void createInstance();
	static void destroyInstance();
	static DockingManager * instance();

	void trayMousePressEvent(QMouseEvent * e);
	KaduIcon defaultIcon();
	QMenu * dockMenu() { return DockMenu; }

#ifdef Q_OS_MAC
	void showMinimizedChats();
	void dockIconClicked();
#endif

	void registerModuleAction(QAction *action);
	void unregisterModuleAction(QAction *action);

signals:
	void messageClicked();
	void mousePressMidButton();
	void mousePressLeftButton();
	void mousePressRightButton();

};

#endif // DOCKING_H
