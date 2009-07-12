/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DESCRIPTION_MANAGER_H
#define DESCRIPTION_MANAGER_H

#include "configuration/storable-string-list.h"

class KADUAPI DescriptionManager : public QObject, public StorableStringList
{
	Q_OBJECT
	Q_DISABLE_COPY(DescriptionManager)

	static DescriptionManager *Instance;

	DescriptionManager();
	virtual ~DescriptionManager();

	void import();

public:
	static DescriptionManager * instance();

	void addDescription(const QString &description);
	void removeDescription(const QString &description);

signals:
	void descriptionAboutToBeAdded(const QString &description);
	void descriptionAdded(const QString &description);
	void descriptionAboutToBeRemoved(const QString &description);
	void descriptionRemoved(const QString &description);

};

#endif // DESCRIPTION_MANAGER_H
