/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_MANAGER_H
#define CONTACT_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "contacts/contact-shared.h"
#include "storage/manager.h"
#include "exports.h"

class Contact;

class KADUAPI ContactManager : public QObject, public Manager<Contact>
{
	Q_OBJECT
	Q_DISABLE_COPY(ContactManager)

	static ContactManager * Instance;

	ContactManager();
	virtual ~ContactManager();

protected:
	virtual QString configurationNodeName() { return QLatin1String("Contacts"); }
	virtual QString configurationNodeItemName() { return QLatin1String("Contact"); }

	virtual void itemAdded(Contact item);
	virtual void itemRemoved(Contact item);

	virtual void itemAboutToBeRegistered(Contact item);
	virtual void itemRegisterd(Contact item);
	virtual void itemAboutToBeUnregisterd(Contact item);
	virtual void itemUnregistered(Contact item);

public:
	static ContactManager * instance();

	// TODO: 0.6.6, hide it
	void detailsLoaded(Contact item);
	void detailsUnloaded(Contact item);

	Contact byId(Account account, const QString &id);
	QList<Contact> contacts(Account account);

signals:
	void contactAboutToBeAdded(Contact contact);
	void contactAdded(Contact contact);
	void contactAboutToBeRemoved(Contact contact);
	void contactRemoved(Contact contact);

	// TODO emit somewhere
	void contactIdChanged(Contact contact, const QString &oldId);

};

// for MOC
#include "contacts/contact.h"

#endif // CONTACT_MANAGER_H
