/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
