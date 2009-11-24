/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef JABBER_CONTACT_DATA_MANAGER_H
#define JABBER_CONTACT_DATA_MANAGER_H

#include "configuration/configuration-contact-data-manager.h"

class JabberContactDetails;

class JabberContactDataManager : public ConfigurationContactDataManager
{
	JabberContactDetails *Data;

protected:
	JabberContactDetails * data() { return Data; }

public:
	explicit JabberContactDataManager(Contact data, QObject *parent = 0);

	virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
	virtual QVariant readEntry(const QString &section, const QString &name);

};

#endif // JABBER_CONTACT_DATA_MANAGER_H
