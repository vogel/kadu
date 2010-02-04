/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QFormLayout>
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "gui/widgets/choose-identity-widget.h"
#include "gui/windows/message-dialog.h"
#include "protocols/protocols-manager.h"
#include "../../server/gadu-server-register-account.h"
#include "html_document.h"
#include "gadu-account-details.h"
#include "gadu-protocol-factory.h"
#include "token-widget.h"

#include "gadu-add-account-widget.h"

GaduAddAccountWidget::GaduAddAccountWidget(QWidget *parent) :
		AccountAddWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

GaduAddAccountWidget::~GaduAddAccountWidget()
{
}

void GaduAddAccountWidget::createGui()
{
	QFormLayout *layout = new QFormLayout(this);

	AccountName = new QLineEdit(this);
	connect(AccountName, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Name") + ":", AccountName);

	AccountId = new QLineEdit(this);
	AccountId->setValidator(new QIntValidator(1, 999999999, this));
	connect(AccountId, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Gadu-Gadu number") + ":", AccountId);

	AccountPassword = new QLineEdit(this);
	connect(AccountPassword, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	AccountPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ":", AccountPassword);
	
	RememberPassword = new QCheckBox(tr("Remember Password"), this);
	RememberPassword->setChecked(true);
	layout->addRow(RememberPassword);

	layout->addRow(tr("Forgot Your Password?"), RemindPassword);

	Identity = new ChooseIdentityWidget(this);
	connect(Identity, SIGNAL(identityChanged()), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ":", Identity);

	layout->addWidget(new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.<i></font>"), this));
}

void GaduAddAccountWidget::dataChanged()
{
	RemindPassword->setEnabled(!AccountId->text().isEmpty());
}

void GaduAddAccountWidget::apply()
{
	Account gaduAccount = Account::create();
	// TODO: 0.6.6 set protocol after details because of crash
	//gaduAccount.setProtocolName("gadu");
	GaduAccountDetails *gad = new GaduAccountDetails(gaduAccount);
	gad->setState(StorableObject::StateNew);
	gaduAccount.setDetails(gad);
	gaduAccount.setProtocolName("gadu");
	gaduAccount.setName(AccountName->text());
	gaduAccount.setId(AccountId->text());
	gaduAccount.setPassword(AccountPassword->text());
	gaduAccount.setHasPassword(!AccountPassword->text().isEmpty());
	gaduAccount.setRememberPassword(RememberPassword->isChecked());

	emit accountCreated(gaduAccount);
}
