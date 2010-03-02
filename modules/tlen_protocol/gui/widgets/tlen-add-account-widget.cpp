/*
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
#include "protocols/protocols-manager.h"
#include "icons-manager.h"

#include "tlen-account-details.h"
#include "tlen-protocol-factory.h"

#include "tlen-add-account-widget.h"

TlenAddAccountWidget::TlenAddAccountWidget(QWidget *parent) :
		AccountAddWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

TlenAddAccountWidget::~TlenAddAccountWidget()
{
}

void TlenAddAccountWidget::apply()
{
}

void TlenAddAccountWidget::cancel()
{
}

void TlenAddAccountWidget::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	AccountName = new QLineEdit(this);
	connect(AccountName, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Name") + ":", AccountName);

	AccountId = new QLineEdit(this);
	//AccountId->setValidator(new LongValidator(1, 3999999999U, this));
	connect(AccountId, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Tlen.pl login") + ":", AccountId);

	AccountPassword = new QLineEdit(this);
	connect(AccountPassword, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	AccountPassword->setEchoMode(QLineEdit::Password);
	layout->addRow(tr("Password") + ":", AccountPassword);
	
	RememberPassword = new QCheckBox(tr("Remember Password"), this);
	RememberPassword->setChecked(true);
	layout->addRow(0, RememberPassword);

// 	RemindPassword = new QLabel(QString("<a href='remind'>%1</a>").arg(tr("Forgot Your Password?")));
// 	RemindPassword->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
// 	layout->addRow(0, RemindPassword);
// 	connect(RemindPassword, SIGNAL(linkActivated(QString)), this, SLOT(remindPasssword()));

	Identity = new IdentitiesComboBox(this);
	connect(Identity, SIGNAL( identityChanged()), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ":", Identity);

	layout->addWidget(new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.<i></font>"), this));

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	AddAccountButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Add Account"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	buttons->addButton(AddAccountButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::DestructiveRole);

	connect(AddAccountButton, SIGNAL(clicked(bool)), this, SLOT(addAccountButtonClicked()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancelButtonClicked()));

	dataChanged();
}

void TlenAddAccountWidget::addAccountButtonClicked()
{
	Account tlenAccount = Account::create();
	// TODO: 0.6.6 set protocol after details because of crash
	//tlenAccount.setProtocolName("tlen");
	TlenAccountDetails *details = new TlenAccountDetails(tlenAccount);
	details->setState(StorableObject::StateNew);
	tlenAccount.setDetails(details);
	tlenAccount.setProtocolName("tlen");
	tlenAccount.setName(AccountName->text());
	tlenAccount.setId(AccountId->text());
	tlenAccount.setPassword(AccountPassword->text());
	tlenAccount.setHasPassword(!AccountPassword->text().isEmpty());
	tlenAccount.setRememberPassword(RememberPassword->isChecked());

	emit accountCreated(tlenAccount);
}

void TlenAddAccountWidget::cancelButtonClicked()
{
}

void TlenAddAccountWidget::dataChanged()
{
	//RemindPassword->setEnabled(!AccountId->text().isEmpty());

	AddAccountButton->setEnabled(
		!AccountName->text().isEmpty() &&
		!AccountManager::instance()->byName(AccountName->text()) &&
		!AccountId->text().isEmpty() &&
		!AccountPassword->text().isEmpty()
	);
}
