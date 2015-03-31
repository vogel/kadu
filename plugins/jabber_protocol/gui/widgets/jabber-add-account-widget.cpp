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
#include "gui/widgets/choose-identity-widget.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "protocols/protocols-manager.h"

#include "services/jabber-servers-service.h"
#include "jabber-account-details.h"
#include "jabber-protocol-factory.h"

#include "jabber-add-account-widget.h"

JabberAddAccountWidget::JabberAddAccountWidget(JabberProtocolFactory *factory, bool showButtons, QWidget *parent) :
		AccountAddWidget(parent), Factory(factory)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)), this, SLOT(dataChanged()));

	createGui(showButtons);
	resetGui();
}

JabberAddAccountWidget::~JabberAddAccountWidget()
{
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
	if (!Factory->allowChangeServer())
	{
		Domain->setVisible(false);
		AtLabel->setVisible(false);

		QString toolTip = Factory->whatIsMyUsername();
		if (!toolTip.isEmpty())
		{
			QLabel *whatIsMyUsernameLabel = new QLabel(tr("<a href='#'>What is my username?</a>"), this);
			whatIsMyUsernameLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
			jidLayout->addWidget(whatIsMyUsernameLabel, 0, 2, Qt::AlignRight);

			connect(whatIsMyUsernameLabel, SIGNAL(linkActivated(QString)), this, SLOT(showWhatIsMyUsername()));
		}
	}
	else
	{
		connect(Domain, SIGNAL(currentIndexChanged(QString)), this, SLOT(dataChanged()));
		connect(Domain, SIGNAL(editTextChanged(QString)), this, SLOT(dataChanged()));
	}
	jidLayout->addWidget(Domain, 0, 2);

	layout->addRow(tr("Username") + ':', jidWidget);

	AccountPassword = new QLineEdit(this);
	connect(AccountPassword, SIGNAL(textEdited(QString)), this, SLOT(dataChanged()));
	AccountPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ':', AccountPassword);

	RememberPassword = new QCheckBox(tr("Remember Password"), this);
	layout->addRow(0, RememberPassword);

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

void JabberAddAccountWidget::setJabberServersService(JabberServersService* serversService)
{
	for (auto &&server : serversService->knownServers())
		Domain->addItem(server);
	Domain->setCurrentText(Factory->defaultServer());
}

void JabberAddAccountWidget::dataChanged()
{
	bool valid = !Username->text().isEmpty()
			&& !AccountPassword->text().isEmpty()
			&& !Domain->currentText().isEmpty()
			&& !AccountManager::instance()->byId("jabber", Username->text() + '@' + Domain->currentText())
			&& Identity->currentIdentity();

	AddAccountButton->setEnabled(valid);

	if (Username->text().isEmpty()
			&& AccountPassword->text().isEmpty()
			&& RememberPassword->isChecked()
			&& Domain->currentText() == Factory->defaultServer()
			&& 0 == Identity->currentIndex())
		simpleStateNotifier()->setState(StateNotChanged);
	else
		simpleStateNotifier()->setState(valid ? StateChangedDataValid : StateChangedDataInvalid);
}

void JabberAddAccountWidget::apply()
{
	Account jabberAccount = Account::create("jabber");

	jabberAccount.setId(Username->text() + '@' + Domain->currentText());
	jabberAccount.setPassword(AccountPassword->text());
	jabberAccount.setHasPassword(!AccountPassword->text().isEmpty());
	jabberAccount.setRememberPassword(RememberPassword->isChecked());
	// bad code: order of calls is important here
	// we have to set identity after password
	// so in cache of identity status container it already knows password and can do status change without asking user for it
	jabberAccount.setAccountIdentity(Identity->currentIdentity());

	JabberAccountDetails *details = dynamic_cast<JabberAccountDetails *>(jabberAccount.details());
	if (details)
	{
		details->setState(StorableObject::StateNew);
		details->setResource("Kadu");
		details->setPriority(5);

		bool isGoogleAppsAccount = Factory->name() == "gmail/google talk" && !Domain->currentText().contains("gmail");
		// Google Apps account sometimes needs custom host/port settings to work
		if (isGoogleAppsAccount)
		{
			details->setUseCustomHostPort(true);
			details->setCustomHost("talk.google.com");
			details->setCustomPort(5222);
		}
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
	Domain->setEditText(Factory->defaultServer());
	RememberPassword->setChecked(true);
	IdentityManager::instance()->removeUnused();
	Identity->setCurrentIndex(0);
	AddAccountButton->setDisabled(true);

	simpleStateNotifier()->setState(StateNotChanged);
}

void JabberAddAccountWidget::showWhatIsMyUsername()
{
	MessageDialog::show(KaduIcon("dialog-information"), Factory->displayName(), Factory->whatIsMyUsername());
}

#include "moc_jabber-add-account-widget.cpp"
