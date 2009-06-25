/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_REMOVE_PREDICATE_OBJECT
#define CONTACT_REMOVE_PREDICATE_OBJECT

#include "predicate-object.h"

class Contact;

class KADUAPI ContactRemovePredicateObject : public PredicateObject<ContactRemovePredicateObject>
{

protected:
    	virtual bool removeContactFromStorage(Contact contact) = 0;

public:
	static bool inquireAll(Contact contact);

};

#endif // CONTACT_REMOVE_PREDICATE_OBJECT
