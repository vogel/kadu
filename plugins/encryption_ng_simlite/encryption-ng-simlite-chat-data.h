/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef ENCRYPTION_NG_SIMLITE_CHAT_DATA_H
#define ENCRYPTION_NG_SIMLITE_CHAT_DATA_H

#include "storage/module-data.h"

#undef Property
#define Property(type, name, capitalized_name) \
	type name() { ensureLoaded(); return capitalized_name; } \
	void set##capitalized_name(type name) { ensureLoaded(); capitalized_name = name; }

class EncryptionNgSimliteChatData : public ModuleData
{
	Q_OBJECT

	bool SupportUtf;

protected:
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	explicit EncryptionNgSimliteChatData(const QString &moduleName, StorableObject *parent, QObject *qobjectParent);
	virtual ~EncryptionNgSimliteChatData();

	virtual QString name() const;

	Property(bool, supportUtf, SupportUtf)

};

#undef Property

#endif // ENCRYPTION_NG_SIMLITE_CHAT_DATA_H
