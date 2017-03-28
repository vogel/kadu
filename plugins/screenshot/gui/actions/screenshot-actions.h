/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "misc/memory.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Actions;
class PluginInjectedFactory;
class ScreenshotAction;
class ScreenShotConfiguration;

class ScreenshotActions : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit ScreenshotActions(QObject *parent = nullptr);
    virtual ~ScreenshotActions();

private:
    QPointer<Actions> m_actions;
    QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
    QPointer<ScreenShotConfiguration> m_screenShotConfiguration;

    owned_qptr<ScreenshotAction> m_screenShotActionDescription;

private slots:
    INJEQT_SET void setActions(Actions *actions);
    INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
    INJEQT_SET void setScreenShotConfiguration(ScreenShotConfiguration *screenShotConfiguration);
    INJEQT_INIT void init();
    INJEQT_DONE void done();
};
