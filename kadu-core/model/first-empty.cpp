 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "first-empty.h"

FirstEmpty::FirstEmpty(const QString &emptyString, QObject *parent) :
		QAbstractListModel(parent), EmptyString(emptyString), EmptyCount(emptyString.isEmpty() ? 0 : 1)
{
}

FirstEmpty::FirstEmpty(QObject *parent) :
		QAbstractListModel(parent), EmptyCount(0)
{
}

FirstEmpty::~FirstEmpty()
{
}

QVariant FirstEmpty::data(const QModelIndex &index, int role) const
{
	if (0 == EmptyCount || Qt::DisplayRole != role || index.row() >= EmptyCount)
		return QVariant();

	return EmptyString;
}
