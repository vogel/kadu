/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

#include "accounts/account-manager.h"
#include "gui/widgets/identities-combo-box.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "os/generic/url-opener.h"
#include "protocols/protocols-manager.h"

#include "gadu-account-details.h"
#include "gadu-id-validator.h"
#include "gadu-protocol-factory.h"

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

	auto registerAccountLabel = new QLabel(QString("<a href='register'>%1</a>").arg(tr("Register Account")));
	registerAccountLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	layout->addRow(0, registerAccountLabel);
	connect(registerAccountLabel, SIGNAL(linkActivated(QString)), this, SLOT(registerAccount()));

	auto remindUinLabel = new QLabel(QString("<a href='change'>%1</a>").arg(tr("Remind GG number")));
	remindUinLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	layout->addRow(0, remindUinLabel);
	connect(remindUinLabel, SIGNAL(linkActivated(QString)), this, SLOT(remindUin()));

	auto remindPasswordLabel = new QLabel(QString("<a href='change'>%1</a>").arg(tr("Remind Password")));
	remindPasswordLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
	layout->addRow(0, remindPasswordLabel);
	connect(remindPasswordLabel, SIGNAL(linkActivated(QString)), this, SLOT(remindPassword()));

	Identity = new IdentitiesComboBox(this);
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
	IdentityManager::instance()->removeUnused();
	Identity->setCurrentIndex(0);

	dataChanged();
}

void GaduAddAccountWidget::apply()
{
	Account gaduAccount = Account::create("gadu");

	gaduAccount.setId(AccountId->text());
	gaduAccount.setPassword(AccountPassword->text());
	gaduAccount.setHasPassword(!AccountPassword->text().isEmpty());
	gaduAccount.setRememberPassword(RememberPassword->isChecked());
	// bad code: order of calls is important here
	// we have to set identity after password
	// so in cache of identity status container it already knows password and can do status change without asking user for it
	gaduAccount.setAccountIdentity(Identity->currentIdentity());

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
	bool valid = !AccountId->text().isEmpty()
			&& !AccountPassword->text().isEmpty()
			&& !AccountManager::instance()->byId("gadu", AccountId->text())
			&& Identity->currentIdentity();

	AddAccountButton->setEnabled(valid);

	if (AccountId->text().isEmpty()
			&& AccountPassword->text().isEmpty()
			&& RememberPassword->isChecked()
			&& 0 == Identity->currentIndex())
		simpleStateNotifier()->setState(StateNotChanged);
	else
		simpleStateNotifier()->setState(valid ? StateChangedDataValid : StateChangedDataInvalid);
}

void GaduAddAccountWidget::registerAccount()
{
	UrlOpener::openUrl("https://login.gg.pl/rejestracja-gg/");
}

void GaduAddAccountWidget::remindUin()
{
	UrlOpener::openUrl("https://login.gg.pl/account/remindGG_email/");
}

void GaduAddAccountWidget::remindPassword()
{
	UrlOpener::openUrl("https://login.gg.pl/account/remindPassword/");
}

#include "moc_gadu-add-account-widget.cpp"
