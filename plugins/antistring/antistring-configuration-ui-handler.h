/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

class AntistringConfiguration;

class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QSpinBox;

class AntistringConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
    Q_OBJECT

public:
    Q_INVOKABLE AntistringConfigurationUiHandler(QObject *parent = nullptr);
    virtual ~AntistringConfigurationUiHandler();

    QListWidget *ConditionListWidget;
    QLineEdit *ConditionWidget;
    QSpinBox *FactorWidget;

    void updateConditionList();

protected:
    virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
    virtual void mainConfigurationWindowDestroyed() override;
    virtual void mainConfigurationWindowApplied() override;

private:
    QPointer<AntistringConfiguration> m_antistringConfiguration;

private slots:
    INJEQT_SET void setAntistringConfiguration(AntistringConfiguration *antistringConfiguration);

    void addCondition();
    void changeCondition();
    void deleteCondition();

    void applyConfiguration();

    void wordSelected(QListWidgetItem *item);
};
