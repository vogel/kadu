/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_SERVER_CONTACT_LIST_MANAGER_H
#define GADU_SERVER_CONTACT_LIST_MANAGER_H

#include "protocols/server-contact-list-manager.h"

class GaduProtocol;

class GaduServerContactListManager : public ServerContactListManager
{
	Q_OBJECT

	GaduProtocol *Protocol;
	QString ImportReply;

private slots:
	void contactListReplyReceived(char type, char *content);

public:
	GaduServerContactListManager(GaduProtocol *protocol);

	virtual void importContactList();
	virtual void exportContactList();
	virtual void exportContactList(ContactList contacts);

};

#endif // GADU_SERVER_CONTACT_LIST_MANAGER_H
