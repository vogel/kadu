/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-aware-object.h"
#include "exports.h"
#include "injeqt-type-roles.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AggregatedContactAvatarService;
class Avatars;
class Configuration;
class ContactManager;
class Contact;
class SerialDelayedTaskExecutor;
struct AvatarId;
struct ContactAvatarGlobalId;
struct ContactGlobalId;

class KADUAPI ContactAvatarDownloader : public QObject, ConfigurationAwareObject
{
    Q_OBJECT
    INJEQT_TYPE_ROLE(SERVICE)

public:
    Q_INVOKABLE explicit ContactAvatarDownloader(QObject *parent = nullptr);
    virtual ~ContactAvatarDownloader();

protected:
    virtual void configurationUpdated() override;

private:
    QPointer<AggregatedContactAvatarService> m_aggregatedContactAvatarService;
    QPointer<Avatars> m_avatars;
    QPointer<Configuration> m_configuration;
    QPointer<ContactManager> m_contactManager;
    QPointer<SerialDelayedTaskExecutor> m_serialDelayedTaskExecutor;
    bool m_downloadAvatars{false};

    void downloadIfNeeded(const ContactAvatarGlobalId &id) const;
    bool downloadRequired(const ContactAvatarGlobalId &id) const;
    void store(const ContactAvatarGlobalId &id, const QByteArray &content);
    void remove(const ContactGlobalId &id);
    void removeFor(const Contact &contact);

private slots:
    INJEQT_SET void setAggregatedContactAvatarService(AggregatedContactAvatarService *aggregatedContactAvatarService);
    INJEQT_SET void setAvatars(Avatars *avatars);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setContactManager(ContactManager *contactManager);
    INJEQT_SET void setSerialDelayedTaskExecutor(SerialDelayedTaskExecutor *serialDelayedTaskExecutor);
    INJEQT_INIT void init();
};
