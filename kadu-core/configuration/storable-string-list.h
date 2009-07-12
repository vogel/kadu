/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STORABLE_STRING_LIST_H
#define STORABLE_STRING_LIST_H

#include <QtCore/QStringList>

#include "configuration/storable-object.h"

class StorableStringList : public StorableObject
{
	QString ContentNodeName;
	QStringList Content;

public:
	StorableStringList(const QString &nodeName, const QString conrentNodeName, StorableObject *parent);

	virtual void load();
	virtual void store();

	QStringList & content();

};

#endif // STORABLE_STRING_LIST_H
