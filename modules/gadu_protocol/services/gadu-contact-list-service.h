/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CONTACT_LIST_SERVICE_H
#define GADU_CONTACT_LIST_SERVICE_H

#include "protocols/services/contact-list-service.h"

class GaduProtocol;

class GaduContactListService : public ContactListService
{
	Q_OBJECT

	GaduProtocol *Protocol;
	QString ImportReply;

private slots:
	void contactListReplyReceived(char type, char *content);

public:
	GaduContactListService(GaduProtocol *protocol);

	virtual void importContactList();
	virtual void exportContactList();
	virtual void exportContactList(ContactList contacts);

};

#endif // GADU_CONTACT_LIST_SERVICE_H
