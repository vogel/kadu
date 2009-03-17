/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CORE_H
#define CORE_H

#include <QtCore/QObject>

#include "accounts/accounts_aware_object.h"
#include "contacts/contact.h"
#include "contacts/contact-list.h"

class Core : public QObject, private AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Core)

	static Core *Instance;

	Contact Myself;

	Core();
	virtual ~Core();

protected:
	virtual void accountRegistered(Account* account);
	virtual void accountUnregistered(Account* account);

public:
	static Core * instance();

	Contact myself() { return Myself; }

	void createGui();

signals:
	void connecting();
	void connected();
	void disconnected();

	void messageReceived(Account *account, Contact sender, ContactList receipients, const QString &message, time_t time);

};

#endif // CORE_H
