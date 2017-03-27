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

#include "facebook-add-account-widget.h"

#include "accounts/account-manager.h"
#include "accounts/account-storage.h"
#include "identities/identity-manager.h"
#include "plugin/plugin-injected-factory.h"
#include "widgets/identities-combo-box.h"
#include "widgets/simple-configuration-value-state-notifier.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

FacebookAddAccountWidget::FacebookAddAccountWidget(bool showButtons, QWidget *parent) :
		AccountAddWidget{parent},
		m_showButtons{showButtons}
{
}

FacebookAddAccountWidget::~FacebookAddAccountWidget()
{
}

void FacebookAddAccountWidget::setAccountManager(AccountManager *accountManager)
{
	m_accountManager = accountManager;
}

void FacebookAddAccountWidget::setAccountStorage(AccountStorage *accountStorage)
{
	m_accountStorage = accountStorage;
}

void FacebookAddAccountWidget::setIdentityManager(IdentityManager *identityManager)
{
	m_identityManager = identityManager;
}

void FacebookAddAccountWidget::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void FacebookAddAccountWidget::init()
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(m_accountManager, SIGNAL(accountAdded(Account)), this, SLOT(dataChanged()));

	createGui(m_showButtons);
	resetGui();
}

void FacebookAddAccountWidget::createGui(bool showButtons)
{
	auto mainLayout = make_owned<QVBoxLayout>(this);

	auto formWidget = make_owned<QWidget>(this);
	mainLayout->addWidget(formWidget);

	auto layout = make_owned<QFormLayout>(formWidget);

	m_username = make_owned<QLineEdit>(this);
	connect(m_username, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Username") + ':', m_username);

	m_password = make_owned<QLineEdit>(this);
	connect(m_password, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	m_password->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ':', m_password);

	m_rememberPassword = make_owned<QCheckBox>(tr("Remember Password"), this);
	layout->addRow(0, m_rememberPassword);

	m_identity = m_pluginInjectedFactory->makeInjected<IdentitiesComboBox>(this);
	connect(m_identity, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ':', m_identity);

	auto infoLabel = make_owned<QLabel>(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout->addRow(0, infoLabel);

	mainLayout->addStretch(100);

	auto buttons = make_owned<QDialogButtonBox>(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	m_addAccountButton = make_owned<QPushButton>(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Add Account"), this);
	auto cancelButton = make_owned<QPushButton>(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	buttons->addButton(m_addAccountButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::DestructiveRole);

	connect(m_addAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	if (!showButtons)
		buttons->hide();
}

void FacebookAddAccountWidget::dataChanged()
{
	auto valid = !m_username->text().isEmpty()
			&& !m_password->text().isEmpty()
			&& !m_accountManager->byId("facebook", m_username->text())
			&& m_identity->currentIdentity();

	m_addAccountButton->setEnabled(valid);

	if (m_username->text().isEmpty()
			&& m_password->text().isEmpty()
			&& m_rememberPassword->isChecked()
			&& 0 == m_identity->currentIndex())
		simpleStateNotifier()->setState(StateNotChanged);
	else
		simpleStateNotifier()->setState(valid ? StateChangedDataValid : StateChangedDataInvalid);
}

void FacebookAddAccountWidget::apply()
{
	auto facebookAccount = m_accountStorage->create("facebook");

	facebookAccount.setId(m_username->text());
	facebookAccount.setPassword(m_password->text());
	facebookAccount.setHasPassword(!m_password->text().isEmpty());
	facebookAccount.setRememberPassword(m_rememberPassword->isChecked());
	// bad code: order of calls is important here
	// we have to set identity after password
	// so in cache of identity status container it already knows password and can do status change without asking user for it
	facebookAccount.setAccountIdentity(m_identity->currentIdentity());

	resetGui();

	emit accountCreated(facebookAccount);
}

void FacebookAddAccountWidget::cancel()
{
	resetGui();
}

void FacebookAddAccountWidget::resetGui()
{
	m_username->clear();
	m_password->clear();
	m_rememberPassword->setChecked(true);
	m_identityManager->removeUnused();
	m_identity->setCurrentIndex(0);

	simpleStateNotifier()->setState(StateNotChanged);
}

#include "facebook-add-account-widget.moc"
