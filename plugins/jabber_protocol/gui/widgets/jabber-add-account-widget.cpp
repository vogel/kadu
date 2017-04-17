/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "jabber-add-account-widget.h"
#include "jabber-add-account-widget.moc"

#include "jabber-account-data.h"
#include "services/jabber-servers-service.h"

#include "accounts/account-manager.h"
#include "accounts/account-storage.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "plugin/plugin-injected-factory.h"
#include "protocols/protocols-manager.h"
#include "widgets/simple-configuration-value-state-notifier.h"
#include "windows/message-dialog.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

JabberAddAccountWidget::JabberAddAccountWidget(bool isGmail, QString defaultServer, bool showButtons, QWidget *parent)
        : AccountAddWidget{parent}, m_isGmail{isGmail}, m_defaultServer{std::move(defaultServer)},
          m_showButtons{showButtons}
{
}

JabberAddAccountWidget::~JabberAddAccountWidget()
{
}

void JabberAddAccountWidget::setAccountManager(AccountManager *accountManager)
{
    m_accountManager = accountManager;
}

void JabberAddAccountWidget::setAccountStorage(AccountStorage *accountStorage)
{
    m_accountStorage = accountStorage;
}

void JabberAddAccountWidget::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void JabberAddAccountWidget::setIdentityManager(IdentityManager *identityManager)
{
    m_identityManager = identityManager;
}

void JabberAddAccountWidget::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
    m_pluginInjectedFactory = pluginInjectedFactory;
}

void JabberAddAccountWidget::init()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(m_accountManager, SIGNAL(accountAdded(Account)), this, SLOT(dataChanged()));

    createGui(m_showButtons);
    resetGui();
}

void JabberAddAccountWidget::createGui(bool showButtons)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *formWidget = new QWidget(this);
    mainLayout->addWidget(formWidget);

    QFormLayout *layout = new QFormLayout(formWidget);

    QWidget *jidWidget = new QWidget(this);
    QGridLayout *jidLayout = new QGridLayout(jidWidget);
    jidLayout->setSpacing(0);
    jidLayout->setMargin(0);
    jidLayout->setColumnStretch(0, 2);
    jidLayout->setColumnStretch(2, 2);

    Username = new QLineEdit(this);
    connect(Username, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
    jidLayout->addWidget(Username);

    AtLabel = new QLabel("@", this);
    jidLayout->addWidget(AtLabel, 0, 1);

    Domain = new QComboBox();
    Domain->setEditable(true);
    connect(Domain, SIGNAL(currentIndexChanged(QString)), this, SLOT(dataChanged()));
    connect(Domain, SIGNAL(editTextChanged(QString)), this, SLOT(dataChanged()));
    jidLayout->addWidget(Domain, 0, 2);

    layout->addRow(tr("Username") + ':', jidWidget);

    AccountPassword = new QLineEdit(this);
    connect(AccountPassword, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
    AccountPassword->setEchoMode(QLineEdit::Password);
    layout->addRow(tr("Password") + ':', AccountPassword);

    RememberPassword = new QCheckBox(tr("Remember Password"), this);
    layout->addRow(0, RememberPassword);

    Identity = m_pluginInjectedFactory->makeInjected<IdentitiesComboBox>(this);
    connect(Identity, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
    layout->addRow(tr("Account Identity") + ':', Identity);

    QLabel *infoLabel = new QLabel(
        tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"),
        this);
    infoLabel->setWordWrap(true);
    infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout->addRow(0, infoLabel);

    mainLayout->addStretch(100);

    QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
    mainLayout->addWidget(buttons);

    AddAccountButton =
        new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Add Account"), this);
    QPushButton *cancelButton =
        new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

    buttons->addButton(AddAccountButton, QDialogButtonBox::AcceptRole);
    buttons->addButton(cancelButton, QDialogButtonBox::DestructiveRole);

    connect(AddAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

    if (!showButtons)
        buttons->hide();
}

void JabberAddAccountWidget::setJabberServersService(JabberServersService *serversService)
{
    for (auto &&server : serversService->knownServers())
        Domain->addItem(server);
    Domain->setCurrentText(m_defaultServer);
}

void JabberAddAccountWidget::dataChanged()
{
    bool valid = !Username->text().isEmpty() && !AccountPassword->text().isEmpty() &&
                 !Domain->currentText().isEmpty() &&
                 !m_accountManager->byId("jabber", Username->text() + '@' + Domain->currentText()) &&
                 Identity->currentIdentity();

    AddAccountButton->setEnabled(valid);

    if (Username->text().isEmpty() && AccountPassword->text().isEmpty() && RememberPassword->isChecked() &&
        Domain->currentText() == m_defaultServer && 0 == Identity->currentIndex())
        simpleStateNotifier()->setState(StateNotChanged);
    else
        simpleStateNotifier()->setState(valid ? StateChangedDataValid : StateChangedDataInvalid);
}

void JabberAddAccountWidget::apply()
{
    auto jabberAccount = m_accountStorage->create("jabber");

    jabberAccount.setId(Username->text() + '@' + Domain->currentText());
    jabberAccount.setPassword(AccountPassword->text());
    jabberAccount.setHasPassword(!AccountPassword->text().isEmpty());
    jabberAccount.setRememberPassword(RememberPassword->isChecked());
    // bad code: order of calls is important here
    // we have to set identity after password
    // so in cache of identity status container it already knows password and can do status change without asking user
    // for it
    jabberAccount.setAccountIdentity(Identity->currentIdentity());

    auto accountData = JabberAccountData{jabberAccount};
    accountData.setResource("Kadu");
    accountData.setPriority(5);

    bool isGoogleAppsAccount = m_isGmail && !Domain->currentText().contains("gmail");
    // Google Apps account sometimes needs custom host/port settings to work
    if (isGoogleAppsAccount)
    {
        accountData.setUseCustomHostPort(true);
        accountData.setCustomHost("talk.google.com");
        accountData.setCustomPort(5222);
    }

    resetGui();

    emit accountCreated(jabberAccount);
}

void JabberAddAccountWidget::cancel()
{
    resetGui();
}

void JabberAddAccountWidget::resetGui()
{
    AccountPassword->clear();
    Username->clear();
    Domain->setEditText(m_defaultServer);
    RememberPassword->setChecked(true);
    m_identityManager->removeUnused();
    Identity->setCurrentIndex(0);
    AddAccountButton->setDisabled(true);

    simpleStateNotifier()->setState(StateNotChanged);
}
