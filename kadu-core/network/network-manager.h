/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QtCore/QObject>

#include "exports.h"

class KADUAPI NetworkManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(NetworkManager)

	static NetworkManager *Instance;

protected slots:
	void onlineStateChanged(bool isOnline);

protected:
	NetworkManager();
	virtual ~NetworkManager();

public:
	static NetworkManager * instance();
	virtual bool isOnline() = 0;

signals:
	void online();
	void offline();

};

#endif // NETWORK_MANAGER_H
