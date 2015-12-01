/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "docking-exports.h"

#include <QtCore/QObject>

class QAction;

class DOCKINGAPI DockingMenuActionRepository final : public QObject
{
	Q_OBJECT

public:
	explicit DockingMenuActionRepository(QObject *parent = nullptr);
	virtual ~DockingMenuActionRepository();

	void addAction(QAction *action);
	void removeAction(QAction *action);

	QList<QAction *> actions() const;

signals:
	void actionAdded(QAction *action);
	void actionRemoved(QAction *action);

private:
	QList<QAction *> m_actions;

private slots:
	void actionDestroyed(QObject *action);

};
