/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADUROOTOBJECT_H
#define KADUROOTOBJECT_H

#include <QObject>
#include <QMap>
#include <QVariantMap>

class Account;
class Status;

class KaduRootObject : public QObject
{
	QMap<Account,Status> AccountStatus;

	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "im.kadu")

public:
	KaduRootObject(QObject *parent = 0);
	~KaduRootObject();

public slots:
	// print version
	QString Version();
	// store status and disconnect
	void Disconnect();
	// connect and restore status
	void Reconnect();

	void Show();
	void Hide();

	// kadu quit
	void Quit();
};

#endif
