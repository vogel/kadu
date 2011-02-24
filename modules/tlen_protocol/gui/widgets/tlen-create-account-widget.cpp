/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "gui/widgets/choose-identity-widget.h"
#include "protocols/protocols-manager.h"
#include "tlen-account-details.h"
#include "tlen-protocol-factory.h"

#include "tlen-create-account-widget.h"

TlenCreateAccountWidget::TlenCreateAccountWidget(QWidget *parent) :
		QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

TlenCreateAccountWidget::~TlenCreateAccountWidget()
{
}

void TlenCreateAccountWidget::createGui()
{
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(5);

	gridLayout->setColumnMinimumWidth(0, 20);
	gridLayout->setColumnStretch(3, 10);

	int row = 0;

	QLabel *nameLabel = new QLabel(tr("Account name") + ':', this);
	gridLayout->addWidget(nameLabel, row, 1, Qt::AlignRight);
	AccountName = new QLineEdit(this);
	gridLayout->addWidget(AccountName, row++, 2, 1, 2);

	createAccountGui(gridLayout, row);
	// TODO: register support
}

void TlenCreateAccountWidget::createAccountGui(QGridLayout *gridLayout, int &row)
{
	QLabel *numberLabel = new QLabel(tr("Tlen.pl login") + ':', this);
	gridLayout->addWidget(numberLabel, row, 1, Qt::AlignRight);
	AccountId = new QLineEdit(this);
	connect(AccountId, SIGNAL(textChanged(QString)), this, SLOT(iHaveAccountDataChanged()));
	gridLayout->addWidget(AccountId, row++, 2, 1, 2);

	QLabel *passwordLabel = new QLabel(tr("Password") + ':', this);
	gridLayout->addWidget(passwordLabel, row, 1, Qt::AlignRight);
	AccountPassword = new QLineEdit(this);
	connect(AccountPassword, SIGNAL(textChanged(QString)), this, SLOT(iHaveAccountDataChanged()));
	AccountPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(AccountPassword, row, 2, 1, 2); // remove 1,2 if remind pass
	//RemindPassword = new QPushButton(tr("Forgot password"), this);
	//RemindPassword->setEnabled(false);
	//gridLayout->addWidget(RemindPassword, row++, 3, Qt::AlignLeft);
	row++;

	QLabel *descriptionLabel = new QLabel(tr("Account description"), this);
	gridLayout->addWidget(descriptionLabel, row, 1, Qt::AlignRight);
	Identity = new ChooseIdentityWidget(this);
	connect(Identity, SIGNAL(identityChanged(Identity)), this, SLOT(iHaveAccountDataChanged()));
	gridLayout->addWidget(Identity, row++, 2, 1, 2);

	RememberPassword = new QCheckBox(tr("Remember password"), this);
	RememberPassword->setChecked(true);
	gridLayout->addWidget(RememberPassword, row++, 2, 1, 2);

	AddThisAccount = new QPushButton(tr("Add this account"), this);
	AddThisAccount->setEnabled(false);
	connect(AddThisAccount, SIGNAL(clicked(bool)), this, SLOT(addThisAccount()));
	gridLayout->addWidget(AddThisAccount, row++, 1, 1, 4);

	Widgets.append(numberLabel);
	Widgets.append(AccountId);
	Widgets.append(passwordLabel);
	Widgets.append(AccountPassword);
	//Widgets.append(RemindPassword);
	Widgets.append(descriptionLabel);
	Widgets.append(Identity);
	Widgets.append(RememberPassword);
	Widgets.append(AddThisAccount);
}

void TlenCreateAccountWidget::iHaveAccountDataChanged()
{
	//RemindPassword->setEnabled(!AccountId->text().isEmpty());
	AddThisAccount->setEnabled(!AccountId->text().isEmpty() && !AccountPassword->text().isEmpty()
				   && !Identity->identityName().isEmpty());
}

void TlenCreateAccountWidget::addThisAccount()
{
	Account tlenAccount = Account::create();

	TlenAccountDetails *details = dynamic_cast<TlenAccountDetails *>(tlenAccount.details());
	if (details)
	{
		details->setState(StorableObject::StateNew);
	}

	tlenAccount.setProtocolName("tlen");
	tlenAccount.setId(AccountId->text());
	tlenAccount.setPassword(AccountPassword->text());
	tlenAccount.setHasPassword(!AccountPassword->text().isEmpty());
	tlenAccount.setRememberPassword(RememberPassword->isChecked());

	emit accountCreated(tlenAccount);
}

void TlenCreateAccountWidget::apply()
{
}
