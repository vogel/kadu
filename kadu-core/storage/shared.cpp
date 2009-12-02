/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "shared.h"

Shared::Shared(QUuid uuid) :
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
}

Shared::~Shared()
{
}

void Shared::load()
{
	if (!isValidStorage())
		return;

	UuidStorableObject::load();
	Uuid = QUuid(loadAttribute<QString>("uuid"));
}

void Shared::store()
{
	if (!isValidStorage())
		return;

	storeValue("uuid", Uuid.toString(), true);
}

void Shared::aboutToBeRemoved()
{
}

void Shared::blockUpdatedSignal()
{
	if (0 == BlockUpdatedSignalCount)
		Updated = false;
	BlockUpdatedSignalCount++;
}

void Shared::unblockUpdatedSignal()
{
	BlockUpdatedSignalCount--;
	if (0 == BlockUpdatedSignalCount)
		emitUpdated();
}

void Shared::dataUpdated()
{
	Updated = true;
	doEmitUpdated();
}

void Shared::doEmitUpdated()
{
	if (0 == BlockUpdatedSignalCount && Updated)
	{
		emitUpdated();
		Updated = false;
	}
}

void Shared::emitUpdated()
{
}
