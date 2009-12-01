/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DETAILS_H
#define DETAILS_H

#include "storage/storable-object.h"

template<typename T>
class Details : public StorableObject
{
	T *MainData;

public:
	explicit Details(T *mainData) :
			MainData(mainData)
	{
		setStorage(mainData->storage());
		setState(StateNotLoaded);
	}

	virtual ~Details() {}

	T * mainData() { return MainData; }

	virtual StorableObject * storageParent()
	{
		return MainData
				? MainData->storageParent()
				: 0;
	}

	virtual QString storageNodeName()
	{
		return MainData
				? MainData->storageNodeName()
				: QString::null;
	}

};

#endif // DETAILS_H
