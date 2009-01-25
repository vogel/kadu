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

#include "contact-module-data.h"

class ContactKaduData : public ContactModuleData
{

public:
	static QString key() { return "kadu"; }

	ContactKaduData(StoragePoint *storage);

	virtual void storeConfiguration() const;

};

#endif // CONTACT_KADU_DATA_H
