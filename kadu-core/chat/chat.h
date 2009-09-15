/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_H
#define CHAT_H

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtGui/QPixmap>

#include "accounts/account.h"
#include "configuration/uuid-storable-object.h"
#include "contacts/contact-set.h"

class Chat : public QObject, public UuidStorableObject
{
	Q_OBJECT

	Account *CurrentAccount;
	QUuid Uuid;
	QString Title;
	QPixmap Icon;

public:
	static Chat * loadFromStorage(StoragePoint *conferenceStoragePoint);

	explicit Chat(StoragePoint *storage);
	explicit Chat(Account *parentAccount, QUuid uuid = QUuid());
	virtual ~Chat();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }

	virtual ContactSet contacts() const = 0;
	Account * account() { return CurrentAccount; }
	void setTitle(const QString &newTitle);
	QString title() { return Title; }
	QPixmap icon() { return Icon; }

public slots:
	void refreshTitle();

signals:
	void titleChanged(Chat * chat, const QString &newTitle);

};

#endif // CHAT_H
