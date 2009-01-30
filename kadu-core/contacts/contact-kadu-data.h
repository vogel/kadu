/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_KADU_DATA_H
#define CONTACT_KADU_DATA_H

#include <QtCore/QRect>

#include "contact-module-data.h"

#define Property(type, name, capitalized_name) \
	type name() const { return capitalized_name; } \
	void set##capitalized_name(const type &name) { capitalized_name = name; }

class ContactKaduData : public ContactModuleData
{
	QRect ChatGeometry;

public:
	static QString key() { return "kadu"; }

	ContactKaduData(StoragePoint *storage);

	virtual void storeConfiguration() const;

	Property(QRect, chatGeometry, ChatGeometry)

};

#undef Property

#endif // CONTACT_KADU_DATA_H
