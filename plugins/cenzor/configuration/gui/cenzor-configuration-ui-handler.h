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

#include "configuration/gui/configuration-ui-handler.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class CenzorConfiguration;
class ListEditWidget;

class CenzorConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit CenzorConfigurationUiHandler(QObject *parent = nullptr);
    virtual ~CenzorConfigurationUiHandler();

protected:
    virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
    virtual void mainConfigurationWindowDestroyed() override;
    virtual void mainConfigurationWindowApplied() override;

private:
    QPointer<CenzorConfiguration> m_cenzorConfiguration;

    ListEditWidget *m_swearwordsWidget;
    ListEditWidget *m_exclusionsWidget;

private slots:
    INJEQT_SET void setCenzorConfiguration(CenzorConfiguration *cenzorConfiguration);
};
