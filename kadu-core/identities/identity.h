/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IDENTITY_H
#define IDENTITY_H

#include <QtCore/QUuid>
#include <QtCore/QStringList>

#include "identities/identity-shared.h"
#include "status/base-status-container.h"

#include "exports.h"

#undef PropertyRead
#define PropertyRead(type, name, capitalized_name, default) \
	type name() const\
	{\
		return !Data\
			? default\
			: Data->name();\
	}

#undef PropertyWrite
#define PropertyWrite(type, name, capitalized_name, default) \
	void set##capitalized_name(type name) const\
	{\
		if (Data)\
			Data->set##capitalized_name(name);\
	}

#undef Property
#define Property(type, name, capitalized_name, default) \
	PropertyRead(type, name, capitalized_name, default) \
	PropertyWrite(type, name, capitalized_name, default)

class QPixmap;
class Status;

class KADUAPI Identity : public QObject
{
	Q_OBJECT

	QExplicitlySharedDataPointer<IdentityShared> Data;

public:
	explicit Identity(IdentityShared *data);
	virtual ~Identity() {};
	
	IdentityShared * data() const { return Data.data(); }

	bool hasAccount(Account account) const;
	void addAccount(Account account);
	
	Property(QString, name, Name, QString::null)
	Property(QUuid, uuid, Uuid, QUuid())
	Property(QList<Account>, accounts, Accounts, QList<Account>())
	Property(QStringList, accountsUuids, AccountsUuids, QStringList())


public slots:
	void removeAccount(Account account);

};

#endif // IDENTITY_H
