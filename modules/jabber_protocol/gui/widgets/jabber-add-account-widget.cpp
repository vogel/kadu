/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "gui/widgets/choose-identity-widget.h"
#include "gui/windows/message-dialog.h"
#include "icons-manager.h"
#include "protocols/protocols-manager.h"
#include "server/jabber-server-register-account.h"
#include "jabber-account-details.h"
#include "jabber-protocol-factory.h"

#include "jabber-add-account-widget.h"

JabberAddAccountWidget::JabberAddAccountWidget(QWidget *parent) :
		AccountAddWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

JabberAddAccountWidget::~JabberAddAccountWidget()
{
}

void JabberAddAccountWidget::createGui()
{
  	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	AccountName = new QLineEdit(this);
	connect(AccountName, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Name") + ":", AccountName);

	QWidget *jidWidget = new QWidget(this);
	QHBoxLayout *jidLayout = new QHBoxLayout(jidWidget);
	jidLayout->setSpacing(0);
	jidLayout->setMargin(0);

	Username = new QLineEdit(this);
	connect(Username, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	jidLayout->addWidget(Username, 20);
	
	QLabel *atLabel = new QLabel("@", this);
	jidLayout->addWidget(atLabel);
	
	Domain = new QComboBox();
	Domain->setEditable(true);
	jidLayout->addWidget(Domain, 10);
	jidLayout->addStretch(100);
	
	layout->addRow(tr("Username") + ":", jidWidget);

	AccountPassword = new QLineEdit(this);
	connect(AccountPassword, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	AccountPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ":", AccountPassword);
	
	RememberPassword = new QCheckBox(tr("Remember Password"), this);
	RememberPassword->setChecked(true);
	layout->addRow(0, RememberPassword);

	RemindPassword = new QLabel(QString("<a href='remind'>%1</a>").arg(tr("Remind password")));
	RemindPassword->setTextInteractionFlags(Qt::TextBrowserInteraction);
	layout->addRow(tr("Forgot Your Password?"), RemindPassword);

	Identity = new IdentitiesComboBox(this);
	connect(Identity, SIGNAL( identityChanged()), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ":", Identity);

	layout->addWidget(new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.<i></font>"), this));

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	AddAccountButton = new QPushButton(IconsManager::instance()->iconByPath("kadu/dialog-apply.png"), tr("Add Account"), this);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->iconByPath("kadu/dialog-cancel.png"), tr("Cancel"), this);

	buttons->addButton(AddAccountButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::DestructiveRole);

	connect(AddAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	dataChanged();
}

void JabberAddAccountWidget::dataChanged()
{
	RemindPassword->setEnabled(!Username->text().isEmpty());
	AddAccountButton->setEnabled(!Username->text().isEmpty() && !AccountPassword->text().isEmpty()
				   && !Domain->currentText().isEmpty() && !Identity->currentIndex() != -1);
}

void JabberAddAccountWidget::apply()
{
	Account jabberAccount = Account::create();
	// TODO: 0.6.6 set protocol after details because of crash
	//jabberAccount.setProtocolName("jabber");
	JabberAccountDetails *details = new JabberAccountDetails(jabberAccount);
	details->setState(StorableObject::StateNew);
	details->setResource("Kadu");
	details->setPriority(5);
	jabberAccount.setDetails(details);
	jabberAccount.setProtocolName("jabber");
	jabberAccount.setName(AccountName->text());
	jabberAccount.setId(Username->text() + "@" + Domain->currentText());
	jabberAccount.setPassword(AccountPassword->text());
	jabberAccount.setHasPassword(!AccountPassword->text().isEmpty());
	jabberAccount.setRememberPassword(RememberPassword->isChecked());

	emit accountCreated(jabberAccount);
}

void JabberAddAccountWidget::cancel()
{
	resetGui();
}

void JabberAddAccountWidget::resetGui()
{
	AccountName->setText("");
	AccountPassword->setText("");
	Username->setText("");
	Domain->setCurrentIndex(-1);
	RememberPassword->setChecked(true);
	Identity->setCurrentIdentity(Identity::null);
}
