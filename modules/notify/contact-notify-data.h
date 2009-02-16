/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_NOTIFY_DATA_H
#define CONTACT_NOTIFY_DATA_H

#include "contacts/contact-module-data.h"

#define Property(type, name, capitalized_name) \
	type name() const { return capitalized_name; } \
	void set##capitalized_name(const type &name) { capitalized_name = name; }

class ContactNotifyData : public ContactModuleData
{
	bool Notify;

public:
	static QString key() { return "notify"; }

	ContactNotifyData(StoragePoint *storage);

	virtual void storeConfiguration() const;

	Property(bool, notify, Notify)

};

#undef Property

#endif // CONTACT_NOTIFY_DATA_H
