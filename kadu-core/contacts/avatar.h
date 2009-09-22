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

#include "exports.h"

class ContactAccountData;

class KADUAPI Avatar : public StorableObject
{
	ContactAccountData *MyContactAccountData;
	QDateTime LastUpdated;
	QDateTime NextUpdate;
	QString FileName;
	QPixmap Pixmap;
	QString FilePath;

public:
	Avatar(ContactAccountData *contactAccountData, bool loadFromConfiguration = true);
	virtual ~Avatar();

	virtual void load();
	virtual void store();

	ContactAccountData * contactAccountData();

	QDateTime lastUpdated();
	void setLastUpdated(const QDateTime &lastUpdated);

	QDateTime nextUpdate();
	void setNextUpdate(const QDateTime &nextUpdate);

	QString fileName();
	void setFileName(const QString &fileName);

	QString filePath();

	QPixmap pixmap();
	void setPixmap(const QPixmap &pixmap);
};

#endif // AVATAR_H
