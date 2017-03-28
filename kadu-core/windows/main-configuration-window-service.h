/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigurationUiHandlerRepository;
class InjectedFactory;
class KaduWindowService;
class MainConfigurationWindow;

class KADUAPI MainConfigurationWindowService : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit MainConfigurationWindowService(QObject *parent = nullptr);
    virtual ~MainConfigurationWindowService();

    void registerUiFile(const QString &uiFile);
    void unregisterUiFile(const QString &uiFile);

    void show();

private:
    QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
    QPointer<InjectedFactory> m_injectedFactory;
    QPointer<KaduWindowService> m_kaduWindowService;
    QPointer<MainConfigurationWindow> m_mainConfigurationWindow;

    QList<QString> m_uiFiles;

private slots:
    INJEQT_SET void
    setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
    INJEQT_SET void setKaduWindowService(KaduWindowService *kaduWindowService);
};
