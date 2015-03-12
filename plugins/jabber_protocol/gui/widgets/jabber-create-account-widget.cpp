/*
 * %kadu copyright begin%
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

#include <QtGui/QIntValidator>
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

#include "accounts/account-manager.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"
#include "gui/windows/jabber-wait-for-account-register-window.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "protocols/protocols-manager.h"
#include "services/jabber-error-service.h"
#include "services/jabber-register-account-service.h"
#include "services/jabber-register-account.h"
#include "services/jabber-servers-service.h"
#include "jabber-account-details.h"
#include "jabber-protocol-factory.h"

#include "jabber-create-account-widget.h"

JabberCreateAccountWidget::JabberCreateAccountWidget(bool showButtons, QWidget *parent) :
		AccountCreateWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)), this, SLOT(dataChanged()));

	createGui(showButtons);
	resetGui();
}

JabberCreateAccountWidget::~JabberCreateAccountWidget()
{
}

void JabberCreateAccountWidget::createGui(bool showButtons)
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

	QLabel *atLabel = new QLabel("@", this);
	jidLayout->addWidget(atLabel, 0, 1);

	Domain = new QComboBox();
	Domain->setEditable(true);
	connect(Domain, SIGNAL(currentIndexChanged(QString)), this, SLOT(dataChanged()));
	connect(Domain, SIGNAL(editTextChanged(QString)), this, SLOT(dataChanged()));
	jidLayout->addWidget(Domain, 0, 2);

	layout->addRow(tr("Username") + ':', jidWidget);

	NewPassword = new QLineEdit(this);
	connect(NewPassword, SIGNAL(textEdited(const QString &)), this, SLOT(dataChanged()));
	NewPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ':', NewPassword);

	ReNewPassword = new QLineEdit(this);
	connect(ReNewPassword, SIGNAL(textEdited(const QString &)), this, SLOT(dataChanged()));
	ReNewPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Retype Password") + ':', ReNewPassword);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	layout->addWidget(RememberPassword);

	EMail = new QLineEdit{this};
	layout->addRow(tr("E-mail:"), EMail);

	QLabel *infoLabel = new QLabel(tr("<font size='-1'><i>Some servers require your e-mail address during registration.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout->addRow(0, infoLabel);

	IdentityCombo = new IdentitiesComboBox(this);
	connect(IdentityCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ':', IdentityCombo);

	infoLabel = new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout->addRow(0, infoLabel);

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	RegisterAccountButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Register Account"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	buttons->addButton(RegisterAccountButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(RegisterAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	if (!showButtons)
		buttons->hide();
}

void JabberCreateAccountWidget::setJabberServersService(JabberServersService* serversService)
{
	for (auto &&server : serversService->knownNoXDataServers())
		Domain->addItem(server);

	Domain->setCurrentText(QString{});
}

void JabberCreateAccountWidget::dataChanged()
{
	bool valid = !Domain->currentText().isEmpty()
			&& !Username->text().isEmpty()
			&& !NewPassword->text().isEmpty()
			&& !ReNewPassword->text().isEmpty()
			&& !AccountManager::instance()->byId("jabber", Username->text() + '@' + Domain->currentText())
			&& IdentityCombo->currentIdentity();

	RegisterAccountButton->setEnabled(valid);

	if (Domain->currentText().isEmpty()
			&& Username->text().isEmpty()
			&& NewPassword->text().isEmpty()
			&& ReNewPassword->text().isEmpty()
			&& RememberPassword->isChecked()
			&& 0 == IdentityCombo->currentIndex())
		simpleStateNotifier()->setState(StateNotChanged);
	else
		simpleStateNotifier()->setState(valid ? StateChangedDataValid : StateChangedDataInvalid);
}

void JabberCreateAccountWidget::apply()
{
	if (NewPassword->text() != ReNewPassword->text())
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Invalid data entered in required fields.\n\n"
			"Password entered in both fields (\"New password\" and \"Retype password\") "
			"must be the same!"), QMessageBox::Ok, this);
		return;
	}

	auto errorService = new JabberErrorService{this};
	auto registerAccountService = new JabberRegisterAccountService{this};
	registerAccountService->setErrorService(errorService);

	auto jid = Jid{Username->text(), Domain->currentText(), QString{}};
	auto registerAccount = registerAccountService->registerAccount(jid, NewPassword->text(), EMail->text());

	auto window = new JabberWaitForAccountRegisterWindow(registerAccount);
	connect(window, SIGNAL(jidRegistered(Jid)), this, SLOT(jidRegistered(Jid)));
	window->exec();
}

void JabberCreateAccountWidget::cancel()
{
	resetGui();
}

void JabberCreateAccountWidget::resetGui()
{
	Username->clear();
	Domain->setCurrentIndex(-1);
	NewPassword->clear();
	ReNewPassword->clear();
	RememberPassword->setChecked(true);
	IdentityManager::instance()->removeUnused();
	IdentityCombo->setCurrentIndex(0);
	RegisterAccountButton->setEnabled(false);

	simpleStateNotifier()->setState(StateNotChanged);
}

void JabberCreateAccountWidget::jidRegistered(const Jid &jid)
{
	if (jid.isEmpty())
	{
		emit accountCreated(Account());
		return;
	}

	Account jabberAccount = Account::create("jabber");
	jabberAccount.setId(jid.bare());
	jabberAccount.setHasPassword(true);
	jabberAccount.setPassword(NewPassword->text());
	jabberAccount.setRememberPassword(RememberPassword->isChecked());
	// bad code: order of calls is important here
	// we have to set identity after password
	// so in cache of identity status container it already knows password and can do status change without asking user for it
	jabberAccount.setAccountIdentity(IdentityCombo->currentIdentity());

	JabberAccountDetails *details = dynamic_cast<JabberAccountDetails *>(jabberAccount.details());
	if (details)
		details->setState(StorableObject::StateNew);

	resetGui();

	emit accountCreated(jabberAccount);
}

#include "moc_jabber-create-account-widget.cpp"
