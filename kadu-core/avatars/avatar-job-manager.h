/*
 * %kadu copyright begin%
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/accounts-aware-object.h"
#include "avatars/avatar.h"
#include "exports.h"
#include "storage/manager.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <injeqt/injeqt.h>

class Configuration;
class Contact;
class InjectedFactory;

class KADUAPI AvatarJobManager : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit AvatarJobManager(QObject *parent = nullptr);
    virtual ~AvatarJobManager();

    void addJob(const Contact &contact);

    bool hasJob();
    Contact nextJob();

signals:
    void jobAvailable();

private:
    QPointer<Configuration> m_configuration;
    QPointer<InjectedFactory> m_injectedFactory;

    bool IsJobRunning;

    QSet<Contact> Jobs;

    void scheduleJob();

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);

    void runJob();
    void jobFinished();
};
