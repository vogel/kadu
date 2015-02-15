/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHANGE_NOTIFIER_H
#define CHANGE_NOTIFIER_H

#include <QtCore/QObject>

#include "exports.h"

/**
 * @addtogroup Misc
 * @{
 */

/**
 * @class ChangeNotifier
 * @author Rafał 'Vogel' Malinowski
 * @short Notify about change in object properties.
 *
 * This class can be used to notify about changes in other object's properties. Each time a property in given object changes,
 * the notify() method should be called. Then the changed() signal is emitted.
 *
 * It is possible to block emitting of the changed() signal, for example when changing a lot of properties at once. After the last
 * change and call to unblock(), the changed() signal will be emitted - reducing number of calls from many to one. It is also
 * possible to stack blocking - if object has been blocked n times then it must be unblocked n times again to begin emitting
 * the changed() signal again.
 */
class KADUAPI ChangeNotifier : public QObject
{
	Q_OBJECT

	quint16 Blocked;
	bool Changed;

public:
	explicit ChangeNotifier(QObject *parent = 0);
	virtual ~ChangeNotifier();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Block emitting of changed() signal.
	 *
	 * This method can be called multiple times. After that unblock() must be called the same amount of times to allow emitting changed()
	 * signal again.
	 */
	void block();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Unblock emitting of changed() signal and emit the changed() signal if a change happened when the object was blocked.
	 *
	 * This method must be called the same amount of times as block() before it will allow emitting changed() signal again.
	 * If this method is called without a correspoing block() call, an assertion is issued.
	 */
	void unblock();

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Notify about change.
	 *
	 * Changed object should call this method after every change. After that, changed() signal is emitted if not blocked. In other case
	 * it will be emitted after unblocking.
	 */
	void notify();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Forget about all changes.
	 *
	 * Forget about all changes. Call this method between block() and unblock() to forget about all changes made up to this moment. In
	 * that case no changed() signal will be emitted.
	 */
	void forget();

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emitted after every change.
	 *
	 * Signal emitted after each change. If blocking is used then it will be emitted only once after last change, after call to unblock().
	 */
	void changed();

};

/**
 * @}
 */

#endif // CHANGE_NOTIFIER_H
