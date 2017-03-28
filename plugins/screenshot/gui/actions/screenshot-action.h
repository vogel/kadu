/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QPointer>
#include <QtWidgets/QAction>

#include "actions/action-description.h"

class ChatWidget;
class PluginInjectedFactory;
class ScreenShotConfiguration;

class ScreenshotAction : public ActionDescription
{
    Q_OBJECT

    QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
    QPointer<ScreenShotConfiguration> m_screenShotConfiguration;

    ChatWidget *findChatWidget(QObject *obejct);

private slots:
    INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);

    void takeStandardShotSlot(ChatWidget *chatWidget = 0);
    void takeShotWithChatWindowHiddenSlot();
    void takeWindowShotSlot();

protected:
    virtual void actionInstanceCreated(Action *action);
    virtual void actionTriggered(QAction *sender, bool toggled);
    virtual void updateActionState(Action *action);

public:
    explicit ScreenshotAction(ScreenShotConfiguration *screenShotConfiguration, QObject *parent);
    virtual ~ScreenshotAction();
};
