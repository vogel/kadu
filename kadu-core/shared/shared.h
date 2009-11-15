/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SHARED_H
#define SHARED_H

#include <QtCore/QObject>
#include <QtCore/QSharedData>
#include <QtCore/QUuid>

#include "status/status.h"

#include "configuration/uuid-storable-object.h"

enum SharedType
{
	TypeNormal = 0,
	TypeNull = 1
};

class Shared : public UuidStorableObject, public QSharedData
{
	QUuid Uuid;

	int BlockUpdatedSignalCount;
	bool Updated;

	void doEmitUpdated();

protected:
	void dataUpdated();
	virtual void emitUpdated();

public:
	explicit Shared(const QUuid uuid, const QString &tagName, StorableObject *parent = 0);
	virtual ~Shared();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }
	void setUuid(const QUuid uuid) { Uuid = uuid; }

	void blockUpdatedSignal();
	void unblockUpdatedSignal();

};

#endif // SHARED_H
