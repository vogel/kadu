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

#include "storage/module-data.h"

#undef Property
#define Property(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(const type &name) { ensureLoaded(); capitalized_name = name; }

class BuddyKaduData : public ModuleData
{
	QRect ChatGeometry;
	bool HideDescription;

protected:
	virtual void load();

public:
	BuddyKaduData(StorableObject *parent);
	virtual ~BuddyKaduData();

	virtual void store();
	virtual QString name() const;

	Property(QRect, chatGeometry, ChatGeometry)
	Property(bool, hideDescription, HideDescription)

};

#endif // BUDDY_KADU_DATA_H
