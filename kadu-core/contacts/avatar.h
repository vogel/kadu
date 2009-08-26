/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#ifndef AVATAR_H
#define AVATAR_H

#include <QtCore/QDateTime>
#include <QtGui/QPixmap>

#include "configuration/storable-object.h"

class ContactAccountData;

class Avatar : public StorableObject
{
	ContactAccountData *MyContactAccountData;
	QDateTime LastUpdated;
	QString FileName;
	QPixmap Pixmap;

public:
	Avatar(ContactAccountData *contactAccountData);
	virtual ~Avatar();

	virtual void load();
	virtual void store();

	ContactAccountData * contactAccountData();

	QDateTime lastUpdated();
	void setLastUpdated(const QDateTime &lastUpdated);

	QString fileName();
	void setFileName(const QString &fileName);

	QPixmap pixmap();
	void setPixmap(const QPixmap &pixmap);
};

#endif // AVATAR_H
