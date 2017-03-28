/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QUuid>
#include <injeqt/injeqt.h>

class Application;
class Configuration;
class StorableObject;
class ToolbarConfigurationManager;

class KADUAPI ConfigurationManager : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ConfigurationManager(QObject *parent = nullptr);
    virtual ~ConfigurationManager();

    const QUuid &uuid() const
    {
        return m_uuid;
    }

    void registerStorableObject(StorableObject *object);
    void unregisterStorableObject(StorableObject *object);

    ToolbarConfigurationManager *toolbarConfigurationManager()
    {
        return m_toolbarConfiguration;
    }

public slots:
    void flush();

private:
    QPointer<Application> m_application;
    QPointer<Configuration> m_configuration;

    QUuid m_uuid;
    QList<StorableObject *> m_registeredStorableObjects;

    ToolbarConfigurationManager *m_toolbarConfiguration;

    void importConfiguration();

private slots:
    INJEQT_SET void setApplication(Application *application);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_INIT void init();
};
