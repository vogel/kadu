/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef UINS_LIST_H
#define UINS_LIST_H

#include <QtCore/QList>

#include "gadu_exports.h"

#include "gadu.h"

class GADUAPI UinsList : public QList<UinType>
{
public:
	/**
		konstruuje obiekt UinsList
	**/
	UinsList();

	/**
		konstruuje obiekt UinsList, inicjuj�c go uinem
	**/
	UinsList(UinType uin);

	/**
		konstruuje obiekt UinsList na podstawie �a�cucha "uins" sk�adaj�cego
		si� z oddzielonych przecinkami Uin�w
	**/
	UinsList(const QString &uins);

	/**
	konstruuje obiekt UinsList na podstawie "list"
		przekszta�acaj�c ka�dy element do typy UinType
	**/
	UinsList(const QStringList &list);

	bool equals(const UinsList &uins) const;
	void sort();
	QStringList toStringList() const;

	bool operator < (const UinsList &) const;

};

#endif // UINS_LIST_H
