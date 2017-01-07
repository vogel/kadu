/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "facebook-edit-account-widget.h"

#include "accounts/account-manager.h"
#include "configuration/configuration-manager.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "plugin/plugin-injected-factory.h"
#include "widgets/account-configuration-widget-tab-adapter.h"
#include "widgets/identities-combo-box.h"
#include "widgets/proxy-combo-box.h"
#include "widgets/simple-configuration-value-state-notifier.h"
#include "windows/message-dialog.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>

FacebookEditAccountWidget::FacebookEditAccountWidget(Account account, QWidget *parent) :
		AccountEditWidget{account, parent}
{
}

FacebookEditAccountWidget::~FacebookEditAccountWidget()
{
}

void FacebookEditAccountWidget::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void FacebookEditAccountWidget::setConfigurationManager(ConfigurationManager *configurationManager)
{
	m_configurationManager = configurationManager;
}

void FacebookEditAccountWidget::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void FacebookEditAccountWidget::setIdentityManager(IdentityManager *identityManager)
{
	m_identityManager = identityManager;
}

void FacebookEditAccountWidget::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void FacebookEditAccountWidget::init()
{
	createGui();
	loadAccountData();
	simpleStateNotifier()->setState(StateNotChanged);
	stateChangedSlot(stateNotifier()->state());
}

void FacebookEditAccountWidget::createGui()
{
	auto mainLayout = make_owned<QVBoxLayout>(this);

	auto tabWidget = make_owned<QTabWidget>(this);
	mainLayout->addWidget(tabWidget);

	createGeneralTab(tabWidget);

	make_owned<AccountConfigurationWidgetTabAdapter>(this, tabWidget, this);

	auto buttons = make_owned<QDialogButtonBox>(Qt::Horizontal, this);

	m_apply = make_owned<QPushButton>(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Apply"), this);
	connect(m_apply, &QPushButton::clicked, this, &FacebookEditAccountWidget::apply);

	m_cancel = make_owned<QPushButton>(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	connect(m_cancel, &QPushButton::clicked, this, &FacebookEditAccountWidget::cancel);

	auto removeAccount = make_owned<QPushButton>(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Delete account"), this);
	connect(removeAccount, &QPushButton::clicked, this, &FacebookEditAccountWidget::removeAccount);

	buttons->addButton(m_apply, QDialogButtonBox::ApplyRole);
	buttons->addButton(m_cancel, QDialogButtonBox::RejectRole);
	buttons->addButton(removeAccount, QDialogButtonBox::DestructiveRole);

	mainLayout->addWidget(buttons);

	connect(stateNotifier(), &ConfigurationValueStateNotifier::stateChanged, this, &FacebookEditAccountWidget::stateChangedSlot);
}

void FacebookEditAccountWidget::createGeneralTab(QTabWidget *tabWidget)
{
	auto generalTab = make_owned<QWidget>(this);

	auto layout = make_owned<QGridLayout>(generalTab);
	auto form = make_owned<QWidget>(generalTab);
	layout->addWidget(form, 0, 0);

	auto formLayout = make_owned<QFormLayout>(form);

	m_accountId = make_owned<QLineEdit>(this);
	connect(m_accountId, &QLineEdit::textEdited, this, &FacebookEditAccountWidget::dataChanged);
	formLayout->addRow(tr("Username") + ':', m_accountId);

	m_password = make_owned<QLineEdit>(this);
	m_password->setEchoMode(QLineEdit::Password);
	connect(m_password, &QLineEdit::textEdited, this, &FacebookEditAccountWidget::dataChanged);
	formLayout->addRow(tr("Password") + ':', m_password);

	m_rememberPassword = make_owned<QCheckBox>(tr("Remember password"), this);
	m_rememberPassword->setChecked(true);
	connect(m_rememberPassword, &QCheckBox::clicked, this, &FacebookEditAccountWidget::dataChanged);
	formLayout->addRow(0, m_rememberPassword);

	m_identities = m_pluginInjectedFactory->makeInjected<IdentitiesComboBox>(this);
	connect(m_identities, static_cast<void(IdentitiesComboBox::*)(int)>(&IdentitiesComboBox::currentIndexChanged),
			this, &FacebookEditAccountWidget::dataChanged);
	formLayout->addRow(tr("Account Identity") + ':', m_identities);

	m_proxies = m_pluginInjectedFactory->makeOwned<ProxyComboBox>(this);
	m_proxies->enableDefaultProxyAction();
	connect(m_proxies, static_cast<void(ProxyComboBox::*)(int)>(&IdentitiesComboBox::currentIndexChanged),
			this, &FacebookEditAccountWidget::dataChanged);
	formLayout->addRow(tr("Proxy") + ':', m_proxies);

	tabWidget->addTab(generalTab, tr("General"));
}

void FacebookEditAccountWidget::stateChangedSlot(ConfigurationValueState state)
{
	m_apply->setEnabled(state == StateChangedDataValid);
	m_cancel->setEnabled(state != StateNotChanged);
}

void FacebookEditAccountWidget::dataChanged()
{
	auto widgetsState = stateNotifier()->state();

	if (account().accountIdentity() == m_identities->currentIdentity()
		&& account().id() == m_accountId->text()
		&& account().rememberPassword() == m_rememberPassword->isChecked()
		&& account().password() == m_password->text()
		&& account().useDefaultProxy() == m_proxies->isDefaultProxySelected()
		&& account().proxy() == m_proxies->currentProxy())
	{
		simpleStateNotifier()->setState(StateNotChanged);
		return;
	}

	auto sameIdExists = m_accountManager->byId(account().protocolName(), m_accountId->text())
			&& m_accountManager->byId(account().protocolName(), m_accountId->text()) != account();

	if (m_accountId->text().isEmpty()
		|| sameIdExists
		|| StateChangedDataInvalid == widgetsState)
		simpleStateNotifier()->setState(StateChangedDataInvalid);
	else
		simpleStateNotifier()->setState(StateChangedDataValid);
}

void FacebookEditAccountWidget::loadAccountData()
{
	m_identities->setCurrentIdentity(account().accountIdentity());
	m_accountId->setText(account().id());
	m_rememberPassword->setChecked(account().rememberPassword());
	m_password->setText(account().password());
	if (account().useDefaultProxy())
		m_proxies->selectDefaultProxy();
	else
		m_proxies->setCurrentProxy(account().proxy());
}

void FacebookEditAccountWidget::apply()
{
	applyAccountConfigurationWidgets();

	account().setId(m_accountId->text());
	account().setRememberPassword(m_rememberPassword->isChecked());
	account().setPassword(m_password->text());
	account().setHasPassword(!m_password->text().isEmpty());
	account().setUseDefaultProxy(m_proxies->isDefaultProxySelected());
	account().setProxy(m_proxies->currentProxy());
	// bad code: order of calls is important here
	// we have to set identity after password
	// so in cache of identity status container it already knows password and can do status change without asking user for it
	account().setAccountIdentity(m_identities->currentIdentity());

	m_identityManager->removeUnused();
	m_configurationManager->flush();

	simpleStateNotifier()->setState(StateNotChanged);
}

void FacebookEditAccountWidget::cancel()
{
	cancelAccountConfigurationWidgets();

	loadAccountData();

	m_identityManager->removeUnused();

	simpleStateNotifier()->setState(StateNotChanged);
}

void FacebookEditAccountWidget::removeAccount()
{
	auto dialog = MessageDialog::create(m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Confrim Account Removal"),
	                        tr("Are you sure do you want to remove account %1 (%2)?")
				.arg(account().accountIdentity().name())
				.arg(account().id()));
	dialog->addButton(QMessageBox::Yes, tr("Remove account"));
	dialog->addButton(QMessageBox::Cancel, tr("Cancel"));
	dialog->setDefaultButton(QMessageBox::Cancel);
	auto decision = dialog->exec();

	if (decision == QMessageBox::Yes)
	{
		m_accountManager->removeAccountAndBuddies(account());
		deleteLater();
	}
}

#include "facebook-edit-account-widget.moc"
