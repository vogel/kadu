/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QObject>

class QAction;
class QActionGroup;

class StatusContainer;
class StatusType;

class StatusActions : public QObject
{
	Q_OBJECT

	StatusContainer *MyStatusContainer;
	bool CommonStatusIcons;

	QActionGroup *ChangeStatusActionGroup;
	QAction *ChangeDescription;
	QList<QAction *> Actions;
	QList<StatusType *> MyStatusTypes;

	void createActions();
	void createBasicActions();
	QAction * createSeparator();
	QAction * createStatusAction(StatusType *statusType);

	void cleanUpActions();

private slots:
	void statusChanged();
	void iconThemeChanged();

public:
	StatusActions(StatusContainer *statusContainer, QObject *parent, bool commonStatusIcons = false);
	virtual ~StatusActions();

	const QList<QAction *> & actions() const { return Actions; }

signals:
	void statusActionsRecreated();

	void statusActionTriggered(QAction *);
	void changeDescriptionActionTriggered(bool);

};

#endif // STATUS_ACTIONS_H
