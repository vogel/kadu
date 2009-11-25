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

#include "storage/uuid-storable-object.h"

#define KaduShared_PropertyRead(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; }
#define KaduShared_PropertyWrite(type, name, capitalized_name) \
	void set##capitalized_name(type name) { ensureLoaded(); capitalized_name = name; dataUpdated(); }
#define KaduShared_Property(type, name, capitalized_name) \
	KaduShared_PropertyRead(type, name, capitalized_name) \
	KaduShared_PropertyWrite(type, name, capitalized_name)

#define KaduShared_PropertyBoolRead(capitalized_name) \
	bool is##capitalized_name() { ensureLoaded(); return capitalized_name; }
#define KaduShared_PropertyBoolWrite(capitalized_name) \
	void set##capitalized_name(bool name) { ensureLoaded(); capitalized_name = name; dataUpdated(); }
#define KaduShared_PropertyBool(capitalized_name) \
	KaduShared_PropertyBoolRead(capitalized_name) \
	KaduShared_PropertyBoolWrite(capitalized_name)

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
