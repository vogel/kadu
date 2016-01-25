/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QPoint>
#include <injeqt/injeqt.h>

#include "exports.h"

class QAction;
class QActionGroup;
class QMenu;

class InjectedFactory;
class StatusActions;
class StatusContainer;
class StatusSetter;
class StatusTypeManager;
class StatusWindowService;
class WindowManager;

class KADUAPI StatusMenu : public QObject
{
	Q_OBJECT

	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<StatusSetter> m_statusSetter;
	QPointer<StatusWindowService> m_statusWindowService;
	QPointer<StatusTypeManager> m_statusTypeManager;
	QPointer<WindowManager> m_windowManager;

	QMenu *Menu;
	StatusActions *Actions;

	QPoint MousePositionBeforeMenuHide;

	StatusContainer *Container;
	bool m_includePrefix;

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setStatusSetter(StatusSetter *statusSetter);
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
	INJEQT_SET void setStatusWindowService(StatusWindowService *statusWindowService);
	INJEQT_SET void setWindowManager(WindowManager *windowManager);
	INJEQT_INIT void init();

	void addStatusActions();
	void aboutToHide();
	void changeStatus(QAction *action);
	void changeDescription();

public:
	explicit StatusMenu(StatusContainer *statusContainer, bool includePrefix, QMenu *menu);
	virtual ~StatusMenu();

signals:
	void menuRecreated();

};
