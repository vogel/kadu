/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy.h"

#include <QtCore/QObject>
#include <injeqt/injeqt.h>

class BuddyDataWindow;
class InjectedFactory;
class Myself;

class BuddyDataWindowRepository : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit BuddyDataWindowRepository(QObject *parent = nullptr);
	virtual ~BuddyDataWindowRepository();

	BuddyDataWindow * windowForBuddy(const Buddy &buddy);
	const QMap<Buddy, BuddyDataWindow *> & windows() const;

public slots:
	void showBuddyWindow(const Buddy &buddy);

private:
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<Myself> m_myself;

	QMap<Buddy, BuddyDataWindow *> Windows;

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setMyself(Myself *myself);

	void windowDestroyed(const Buddy &buddy);

};
