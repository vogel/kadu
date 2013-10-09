/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GADU_IMPORTER
#define GADU_IMPORTER

#include <QtCore/QObject>
#include <QtCore/QVariant>

#include "../gadu-exports.h"

class QXmlItem;
class QXmlQuery;

class Account;
class Buddy;
class Contact;

class GADUAPI GaduImporter : public QObject
{
	Q_OBJECT

	static GaduImporter *Instance;

	static const QString EntryQuery;
	static const QString ContactsQuery;

	GaduImporter() {}

	bool alreadyImported();
	void markImported();

	static Contact importGaduContact(Account account, Buddy buddy);
	void importIgnored();

	static QVariant readEntry(QXmlQuery &xmlQuery, const QString &groupName, const QString &entryName, const QVariant &defaultValue = QVariant());

private slots:
	void buddyAdded(const Buddy &buddy);

public:
	static GaduImporter * instance() { return Instance; }
	static void createInstance();
	static void destroyInstance();

	static Account import065Account(QXmlQuery &xmlQuery);
	static QList<Buddy> import065Buddies(Account account, QXmlQuery &xmlQuery);

	void importAccounts();
	void importContacts();

};

#endif // GADU_IMPORTER
