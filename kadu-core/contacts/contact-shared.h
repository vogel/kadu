/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_SHARED_H
#define CONTACT_SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>

#include "configuration/uuid-storable-object.h"

#undef Property
#define Property(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(type name) { ensureLoaded(); capitalized_name = name; dataUpdated(); }

class KADUAPI ContactShared : public QObject, public UuidStorableObject, public QSharedData
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactShared)

public:
	enum ContactType
	{
		TypeNull = 0,
		TypeNormal = 1
	};

private:
	QUuid Uuid;
	ContactType Type;

	int BlockUpdatedSignalCount;
	bool Updated;

	void dataUpdated();
	void emitUpdated();

public:
	static ContactShared * loadFromStorage(StoragePoint *contactStoragePoint);

	explicit ContactShared(ContactType type, QUuid uuid = QUuid());
	virtual ~ContactShared();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }
	void setUuid(const QUuid uuid) { Uuid = uuid; }

	// contact type
	bool isNull() const { return TypeNull == Type; }

signals:
	void updated();

};


#endif // CONTACT_SHARED_H
