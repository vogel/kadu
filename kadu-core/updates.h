/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#ifndef KADU_UPDATES_H
#define KADU_UPDATES_H

#include <QtCore/QDateTime>
#include <QtNetwork/QHttpResponseHeader>

#include "accounts/accounts-aware-object.h"
#include "protocols/protocol.h"

class QHttp;

class Updates : public QObject, AccountsAwareObject
{
	Q_OBJECT

	bool UpdateChecked;
	QDateTime LastUpdateCheck;

	QString Query;
	QHttp *HttpClient;

	void buildQuery();

	bool isNewerVersion(const QString &newestversion);
	QString stripVersion(const QString stripversion);

private slots:
	void gotUpdatesInfo(const QHttpResponseHeader &responseHeader);
	void run();

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

public:
	explicit Updates(QObject *parent = 0);
	virtual ~Updates();

};

#endif
