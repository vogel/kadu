/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_IMPORTER
#define GADU_IMPORTER

#include <QtCore/QObject>

class Buddy;

class GaduImporter : public QObject
{
	Q_OBJECT

	static GaduImporter *Instance;

	GaduImporter() {}

	void importContact(Buddy &buddy);
	void importGaduContact(Buddy &buddy);
	void importIgnored();

private slots:
	void buddyAdded(Buddy &buddy);

public:
	static GaduImporter * instance();

	void importAccounts();
	void importContacts();

};

#endif // GADU_IMPORTER
