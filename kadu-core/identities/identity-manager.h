/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "exports.h"
#include "identities/identity.h"
#include "storage/manager.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QUuid>
#include <injeqt/injeqt.h>

class Account;
class ConfigurationManager;
class IdentityStorage;
class Status;

class KADUAPI IdentityManager : public Manager<Identity>
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit IdentityManager(QObject *parent = nullptr);
    virtual ~IdentityManager();

    virtual QString storageNodeName()
    {
        return QStringLiteral("Identities");
    }
    virtual QString storageNodeItemName()
    {
        return QStringLiteral("Identity");
    }

    Identity byName(const QString &name, bool create = true);
    Identity identityForAcccount(Account account);

    void removeUnused();

signals:
    void identityAboutToBeAdded(Identity);
    void identityAdded(Identity);
    void identityAboutToBeRemoved(Identity);
    void identityRemoved(Identity);

protected:
    virtual void itemAboutToBeAdded(Identity item);
    virtual void itemAdded(Identity item);
    virtual void itemAboutToBeRemoved(Identity item);
    virtual void itemRemoved(Identity item);

    virtual void load();
    virtual Identity loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint);

private:
    QPointer<ConfigurationManager> m_configurationManager;
    QPointer<IdentityStorage> m_identityStorage;

    void addDefaultIdentities();

private slots:
    INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
    INJEQT_SET void setIdentityStorage(IdentityStorage *identityStorage);
    INJEQT_INIT void init();
    INJEQT_DONE void done();
};
