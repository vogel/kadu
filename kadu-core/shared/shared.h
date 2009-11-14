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

#include "status/base-status-container.h"

enum SharedType
{
	TypeNormal = 0,
	TypeNull = 1
};

template<typename C>
class Shared : public BaseStatusContainer, public QSharedData
{
	QUuid Uuid;

	int BlockUpdatedSignalCount;
	bool Updated;

	void emitUpdated();

protected:
	void dataUpdated();

public:
	static Shared<C> * loadFromStorage(StoragePoint *storagePoint);

	explicit Shared<C>(const QString &tagName, StorableObject *parent = 0);
	virtual ~Shared<C>();

	virtual void load();
	virtual void store();

	virtual QUuid uuid() const { return Uuid; }
	void setUuid(const QUuid uuid) { Uuid = uuid; }

signals:
	void updated();

};

#endif // SHARED_H
