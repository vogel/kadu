/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "gui/widgets/config-wizard-page.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AccountManager;
class ConfigurationManager;
class ModalConfigurationWidget;
class Myself;

class ConfigWizardSetUpAccountPage : public ConfigWizardPage
{
    Q_OBJECT

public:
    explicit ConfigWizardSetUpAccountPage(QWidget *parent = nullptr);
    virtual ~ConfigWizardSetUpAccountPage();

    virtual bool isComplete() const;

    virtual void initializePage();
    virtual void cleanupPage();
    virtual bool validatePage();

private:
    QPointer<AccountManager> m_accountManager;
    QPointer<ConfigurationManager> m_configurationManager;
    QPointer<ModalConfigurationWidget> AccountWidget;
    QPointer<Myself> m_myself;
    bool AccountSuccessfullyCreated;

    void createGui();

private slots:
    INJEQT_SET void setAccountManager(AccountManager *accountManager);
    INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
    INJEQT_SET void setMyself(Myself *myself);

    void accountCreated(Account account);
};
