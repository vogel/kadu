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

#ifndef MULTILOGON_SERVICE_H
#define MULTILOGON_SERVICE_H

#include <QtCore/QObject>

#include "account-service.h"

#include "exports.h"

class MultilogonSession;

class KADUAPI MultilogonService : public AccountService
{
	Q_OBJECT

public:
	explicit MultilogonService(Account account, QObject *parent) : AccountService(account, parent) {}

	virtual const QList<MultilogonSession *> & sessions() const = 0;
	virtual void killSession(MultilogonSession *session) = 0;

signals:
	void multilogonSessionAboutToBeConnected(MultilogonSession *session);
	void multilogonSessionConnected(MultilogonSession *session);
	void multilogonSessionAboutToBeDisconnected(MultilogonSession *session);
	void multilogonSessionDisconnected(MultilogonSession *session);

};

#endif // MULTILOGON_SERVICE_H
