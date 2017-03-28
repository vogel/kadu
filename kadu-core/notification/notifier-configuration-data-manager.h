/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "configuration/configuration-window-data-manager.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigurationWindow;
class Configuration;
class InjectedFactory;
class PluginInjectedFactory;

class KADUAPI NotifierConfigurationDataManager : public ConfigurationWindowDataManager
{
    Q_OBJECT

    QPointer<Configuration> m_configuration;
    QPointer<InjectedFactory> m_injectedFactory;

    QString EventName;
    int UsageCount;

    static QMap<QString, NotifierConfigurationDataManager *> DataManagers;
    static void dataManagerDestroyed(const QString &eventName);

private slots:
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);

    void configurationWindowDestroyed();

public:
    explicit NotifierConfigurationDataManager(const QString &eventName, QObject *parent = nullptr);

    virtual void writeEntry(const QString &section, const QString &name, const QVariant &value);
    virtual QVariant readEntry(const QString &section, const QString &name);

    static NotifierConfigurationDataManager *
    dataManagerForEvent(PluginInjectedFactory *pluginInjectedFactory, const QString &eventName);

    void configurationWindowCreated(ConfigurationWindow *window);
};
