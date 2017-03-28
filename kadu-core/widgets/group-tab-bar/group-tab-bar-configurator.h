/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Application;
class Configuration;
class GroupFilter;
class GroupManager;
class GroupTabBar;
class GroupTabBarConfiguration;

class QDomElement;

class GroupTabBarConfigurator : public QObject, private ConfigurationAwareObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit GroupTabBarConfigurator(QObject *parent = nullptr);
    virtual ~GroupTabBarConfigurator();

    void setGroupTabBar(GroupTabBar *groupTabBar);

    GroupTabBarConfiguration loadConfiguration() const;
    void storeConfiguration();

protected:
    virtual void configurationUpdated();

private:
    QPointer<Application> m_application;
    QPointer<Configuration> m_configuration;
    QPointer<GroupManager> m_groupManager;
    QPointer<GroupTabBar> ConfigurableGroupTabBar;

    void createDefaultConfiguration();

    QVector<GroupFilter> loadGroupFilters(bool showGroupTabEverybody) const;
    GroupFilter loadGroupFilter(QDomElement element) const;
    QVector<GroupFilter> import_0_12_groupFilters(bool showGroupTabEverybody) const;

    void storeGroupFilters(const QVector<GroupFilter> &groupFilters);
    void storeGroupFilter(QDomElement parentElement, const GroupFilter &groupFilter);

private slots:
    INJEQT_SET void setApplication(Application *application);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setGroupManager(GroupManager *groupManager);
    INJEQT_INIT void init();
};
