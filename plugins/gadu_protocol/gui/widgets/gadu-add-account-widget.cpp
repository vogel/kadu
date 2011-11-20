/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>

#include "accounts/account-manager.h"
#include "gui/widgets/identities-combo-box.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "protocols/protocols-manager.h"
#include "html_document.h"

#include "gui/windows/gadu-remind-password-window.h"
#include "server/gadu-server-register-account.h"
#include "gadu-account-details.h"
#include "gadu-id-validator.h"
#include "gadu-protocol-factory.h"
#include "token-widget.h"

#include "gadu-add-account-widget.h"

GaduAddAccountWidget::GaduAddAccountWidget(bool showButtons, QWidget *parent) :
		AccountAddWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)), this, SLOT(dataChanged()));

	createGui(showButtons);
	resetGui();
}

GaduAddAccountWidget::~GaduAddAccountWidget()
{
}

void GaduAddAccountWidget::createGui(bool showButtons)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	AccountId = new QLineEdit(this);
	AccountId->setValidator(GaduIdValidator::instance());
	connect(AccountId, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Gadu-Gadu number") + ':', AccountId);

	AccountPassword = new QLineEdit(this);
	connect(AccountPassword, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	AccountPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ':', AccountPassword);

	RememberPassword = new QCheckBox(tr("Remember Password"), this);
	layout->addRow(0, RememberPassword);

	RemindPassword = new QLabel();
	RemindPassword->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	layout->addRow(0, RemindPassword);
	connect(RemindPassword, SIGNAL(linkActivated(QString)), this, SLOT(remindPasssword()));

	Identity = new IdentitiesComboBox(true, this);
	connect(Identity, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ':', Identity);

	QLabel *infoLabel = new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout->addRow(0, infoLabel);

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	AddAccountButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Add Account"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	buttons->addButton(AddAccountButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::DestructiveRole);

	connect(AddAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	if (!showButtons)
		buttons->hide();
}

void GaduAddAccountWidget::resetGui()
{
	AccountId->clear();
	AccountPassword->clear();
	RememberPassword->setChecked(true);
	Identity->setCurrentIdentity(Identity::null);

	IdentityManager::instance()->removeUnused();
	dataChanged();
}

void GaduAddAccountWidget::apply()
{
	Account gaduAccount = Account::create();

	gaduAccount.setAccountIdentity(Identity->currentIdentity());
	gaduAccount.setProtocolName("gadu");
	gaduAccount.setId(AccountId->text());
	gaduAccount.setPassword(AccountPassword->text());
	gaduAccount.setHasPassword(!AccountPassword->text().isEmpty());
	gaduAccount.setRememberPassword(RememberPassword->isChecked());

	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(gaduAccount.details());
	if (details)
		details->setState(StorableObject::StateNew);

	resetGui();

	emit accountCreated(gaduAccount);
}

void GaduAddAccountWidget::cancel()
{
	resetGui();
}

void GaduAddAccountWidget::dataChanged()
{
	if (AccountId->text().isEmpty())
	{
		RemindPassword->setText(tr("Forgot Your Password?"));
		RemindPassword->setEnabled(false);
	}
	else
	{
		RemindPassword->setText(QString("<a href='remind'>%1</a>").arg(tr("Forgot Your Password?")));
		RemindPassword->setEnabled(true);
	}

	bool valid = !AccountId->text().isEmpty()
			&& !AccountPassword->text().isEmpty()
			&& !AccountManager::instance()->byId("gadu", AccountId->text())
			&& Identity->currentIdentity();

	AddAccountButton->setEnabled(valid);

	if (AccountId->text().isEmpty()
			&& AccountPassword->text().isEmpty()
			&& RememberPassword->isChecked()
			&& !Identity->currentIdentity())
		setState(StateNotChanged);
	else
		setState(valid ? StateChangedDataValid : StateChangedDataInvalid);
}

void GaduAddAccountWidget::remindPasssword()
{
	bool ok;
	UinType uin = AccountId->text().toUInt(&ok);
	if (ok)
		(new GaduRemindPasswordWindow(uin))->show();
}
