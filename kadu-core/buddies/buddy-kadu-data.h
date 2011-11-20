/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef BUDDY_KADU_DATA_H
#define BUDDY_KADU_DATA_H

#include "storage/module-data.h"

#undef Property
#define Property(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(const type &name) { ensureLoaded(); capitalized_name = name; }

class BuddyKaduData : public ModuleData
{
	Q_OBJECT

	bool HideDescription;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	BuddyKaduData(const QString &moduleName, StorableObject *parent, QObject *qobjectParent);
	virtual ~BuddyKaduData();

	virtual QString name() const;

	Property(bool, hideDescription, HideDescription)

};

#endif // BUDDY_KADU_DATA_H
