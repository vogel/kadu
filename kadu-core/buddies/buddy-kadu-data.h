/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_KADU_DATA_H
#define BUDDY_KADU_DATA_H

#include <QtCore/QRect>

#include "modules/module-data.h"

#undef Property
#define Property(type, name, capitalized_name) \
	type name() const { return capitalized_name; } \
	void set##capitalized_name(const type &name) { capitalized_name = name; }

class BuddyKaduData : public ModuleData
{
	QRect ChatGeometry;
	bool HideDescription;

public:
	static QString key() { return "kadu"; }

	BuddyKaduData(StoragePoint *storage);

	virtual void loadFromStorage();

	virtual void storeConfiguration() const;

	Property(QRect, chatGeometry, ChatGeometry)
	Property(bool, hideDescription, HideDescription)

};

#endif // BUDDY_KADU_DATA_H
