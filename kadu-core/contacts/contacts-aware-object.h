/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACTS_AWARE_OBJECT
#define CONTACTS_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class Contact;

class KADUAPI ContactsAwareObject : public AwareObject<ContactsAwareObject>
{

protected:
	virtual void contactAdded(Contact contact) = 0;
	virtual void contactRemoved(Contact contact) = 0;

public:
	static void notifyContactAdded(Contact contact);
	static void notifyContactRemoved(Contact contact);

	void triggerAllContactsAdded();
	void triggerAllContactsRemoved();

};

#endif // CONTACTS_AWARE_OBJECT
