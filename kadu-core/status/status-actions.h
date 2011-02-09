/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_ACTIONS_H
#define STATUS_ACTIONS_H

#include <QtCore/QMap>
#include <QtCore/QObject>

class QAction;
class QActionGroup;

class StatusContainer;
class StatusType;

class StatusActions : public QObject
{
	Q_OBJECT

	StatusContainer *MyStatusContainer;

	QActionGroup *ChangeStatusActionGroup;
	QAction *ChangeDescription;
	QMap<StatusType *, QAction *> StatusTypeActions;
	QList<QAction *> Actions;

	void createActions();
	void createBasicActions();
	void createStatusActions();
	QAction * createSeparator();
	QAction * createStatusAction(StatusType *statusType);

	bool CommonStatusIcons;

private slots:
	void statusChanged();
	void iconThemeChanged();

public:
	explicit StatusActions(StatusContainer *statusContainer, QObject *parent, bool commonStatusIcons = false);
	virtual ~StatusActions();

	const QList<QAction *> & actions() const { return Actions; }

signals:
	void statusActionTriggered(QAction *);
	void changeDescriptionActionTriggered(bool);

};

#endif // STATUS_ACTIONS_H
