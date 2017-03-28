/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "accounts/accounts-aware-object.h"
#include "exports.h"
#include "os/generic/desktop-aware-object.h"

#include <QtCore/QItemSelection>
#include <QtCore/QModelIndex>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QHBoxLayout;
class QGroupBox;
class QLabel;
class QListView;
class QStackedWidget;
class QVBoxLayout;

class AccountAddWidget;
class AccountCreateWidget;
class AccountEditWidget;
class AccountManager;
class AccountsModel;
class CanRegisterProtocolFilter;
class ConfigurationManager;
class Configuration;
class IconsManager;
class InjectedFactory;
class ModalConfigurationWidget;
class Myself;
class ProtocolFactory;
class ProtocolsComboBox;

class KADUAPI YourAccounts : public QWidget, AccountsAwareObject, DesktopAwareObject
{
    Q_OBJECT

    QPointer<AccountManager> m_accountManager;
    QPointer<ConfigurationManager> m_configurationManager;
    QPointer<Configuration> m_configuration;
    QPointer<IconsManager> m_iconsManager;
    QPointer<InjectedFactory> m_injectedFactory;
    QPointer<Myself> m_myself;

    QListView *AccountsView;
    AccountsModel *MyAccountsModel;
    QAction *AddExistingAccountAction;
    QAction *CreateNewAccountAction;

    QLabel *MainAccountLabel;
    QGroupBox *MainAccountGroupBox;

    QStackedWidget *MainStack;
    QStackedWidget *CreateAddStack;
    QStackedWidget *EditStack;
    ModalConfigurationWidget *CurrentWidget;
    bool IsCurrentWidgetEditAccount;

    /*
     * This is necessary to prevent infinite recursion when checking
     * if we can safely change widget. Blocking signals doesn't work
     * as expected, because ProtocolsComboBox really wants to have
     * signals enabled to update its CurrentValue properly.
     */
    bool ForceWidgetChange;

    ProtocolsComboBox *Protocols;
    ProtocolFactory *LastProtocol;
    QWidget *CreateAddAccountContainer;

    QMap<ProtocolFactory *, AccountCreateWidget *> CreateWidgets;
    QMap<ProtocolFactory *, AccountAddWidget *> AddWidgets;
    QMap<Account, AccountEditWidget *> EditWidgets;

    CanRegisterProtocolFilter *CanRegisterFilter;

    void createGui();
    void createAccountWidget();
    void createEditAccountWidget();

    AccountCreateWidget *getAccountCreateWidget(ProtocolFactory *protocol);
    AccountAddWidget *getAccountAddWidget(ProtocolFactory *protocol);
    AccountEditWidget *getAccountEditWidget(Account account);

    void switchToCreateMode();
    void switchToAddMode();

    void selectAccount(Account account);

    void updateCurrentWidget();

    bool canChangeWidget();

private slots:
    INJEQT_SET void setAccountManager(AccountManager *accountManager);
    INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
    INJEQT_SET void setMyself(Myself *myself);
    INJEQT_INIT void init();

    void protocolChanged();

    void accountCreated(Account account);
    void accountSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void okClicked();

    void protocolHandlerChanged(Account account);

protected:
    virtual void closeEvent(QCloseEvent *e) override;
    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void accountAdded(Account account) override;
    virtual void accountRemoved(Account account) override;

public:
    explicit YourAccounts(QWidget *parent = nullptr);
    virtual ~YourAccounts();
};
