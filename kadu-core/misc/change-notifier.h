/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

/**
 * @addtogroup Misc
 * @{
 */

/**
 * @enum ChangeNotifier
 * @author Rafał 'Vogel' Malinowski
 * @short Notify about change in object properties.
 *
 * This class can be used to notify about changes in other object properties. Each time a property in given object changes
 * a notify() method should be called. Then changed() signal is emited.
 *
 * It is possible to block emitting of changed() signal when changing a lot of properties at once. After last change and unblocking
 * changed() signal is emited - reducing number of calls from many to one. It is also possible to stack blocking - when object has
 * been blocked n times then it must be unblocked n times again to emit signal.
 */

class ChangeNotifier : public QObject
{
	Q_OBJECT

	quint16 Blocked;
	bool Changed;

public:
	explicit ChangeNotifier(QObject *parent = 0);
	virtual ~ChangeNotifier();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Block emiting of changed() signal.
	 *
	 * This method can be called multiple times. After that unblock() must be called the same amount of times to allow emiting changed()
	 * signal again.
	 */
	void block();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Unblock emiting of changed() signal.
	 *
	 * This method must be called the same amount of times as block() before it to allow emiting changed() signal again.
	 * If this method is called one time too many an assertion is issued.
	 */
	void unblock();

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Notify about change.
	 *
	 * Changed object should call this method after every change. After that changed() signal is emitted if not blocked. In other case
	 * it will be emitted after unblocking.
	 */
	void notify();

signals:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Signal emitted after every change.
	 *
	 * Signal emited after every change. If blocking is used then it will be emitted after last change, when called unblocks it.
	 */
	void changed();

};

/**
 * @}
 */

#endif // CHANGE_NOTIFIER_H
