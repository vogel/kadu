/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ActionDescription;
class Actions;
class Action;
class InjectedFactory;

class QAction;

class SearchWindowActions : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SearchWindowActions(QObject *parent = nullptr);
	virtual ~SearchWindowActions();

private:
	friend class SearchWindow;

	QPointer<Actions> m_actions;
	QPointer<InjectedFactory> m_injectedFactory;

	ActionDescription *FirstSearch;
	ActionDescription *NextResults;
	ActionDescription *StopSearch;
	ActionDescription *ClearResults;
	ActionDescription *AddFound;
	ActionDescription *ChatFound;

private slots:
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void firstSearchActionCreated(Action *action);
	void nextResultsActionCreated(Action *action);
	void stopSearchActionCreated(Action *action);
	void clearResultsActionCreated(Action *action);
	void actionsFoundActionCreated(Action *action);

	void firstSearchActionActivated(QAction *sender);
	void nextResultsActionActivated(QAction *sender);
	void stopSearchActionActivated(QAction *sender);
	void clearResultsActionActivated(QAction *sender);
	void addFoundActionActivated(QAction *sender);
	void chatFoundActionActivated(QAction *sender);

};
