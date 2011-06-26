/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
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

#ifndef KEYS_MANAGER_H
#define KEYS_MANAGER_H

#include <QtCore/QObject>

#include "key.h"

#include "storage/simple-manager.h"
#include "../encryption_exports.h"

class ENCRYPTIONAPI KeysManager : public QObject, public SimpleManager<Key>
{
	Q_OBJECT
	Q_DISABLE_COPY(KeysManager)

	static KeysManager *Instance;

	KeysManager();
	virtual ~KeysManager();

private slots:
	void keyDataUpdated();

protected:
	virtual void itemAboutToBeAdded(Key item);
	virtual void itemAdded(Key item);
	virtual void itemAboutToBeRemoved(Key item);
	virtual void itemRemoved(Key item);

public:
	static KeysManager * instance();

	static void destroyInstance();

	virtual QString storageNodeName() { return QLatin1String("Keys"); }
	virtual QString storageNodeItemName() { return QLatin1String("Key"); }

	Key byContactAndType(Contact contact, const QString &keyType, NotFoundAction action);

signals:
	void keyAboutToBeAdded(Key key);
	void keyAdded(Key key);
	void keyAboutToBeRemoved(Key key);
	void keyRemoved(Key key);

	void keyUpdated(Key key);

};

#endif // KEYS_MANAGER_H
