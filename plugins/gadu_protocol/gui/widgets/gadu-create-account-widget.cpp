/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QIntValidator>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>

#include "gui/widgets/identities-combo-box.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "identities/identity-manager.h"
#include "protocols/protocols-manager.h"
#include "url-handlers/url-handler-manager.h"

#include "gui/windows/gadu-wait-for-account-register-window.h"
#include "server/gadu-server-register-account.h"
#include "gadu-account-details.h"
#include "gadu-protocol-factory.h"
#include "token-widget.h"

#include "gadu-create-account-widget.h"

GaduCreateAccountWidget::GaduCreateAccountWidget(bool showButtons, QWidget *parent) :
		AccountCreateWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui(showButtons);
	resetGui();
}

GaduCreateAccountWidget::~GaduCreateAccountWidget()
{
}

void GaduCreateAccountWidget::createGui(bool showButtons)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

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

	EMail = new QLineEdit(this);
	connect(EMail, SIGNAL(textEdited(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("E-Mail Address") + ':', EMail);

	IdentityCombo = new IdentitiesComboBox(true, this);
	connect(IdentityCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(dataChanged()));
	layout->addRow(tr("Account Identity") + ':', IdentityCombo);

	QLabel *infoLabel = new QLabel(tr("<font size='-1'><i>Select or enter the identity that will be associated with this account.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout->addRow(0, infoLabel);

	MyTokenWidget = new TokenWidget(this);
	connect(MyTokenWidget, SIGNAL(modified()), this, SLOT(dataChanged()));
	layout->addRow(tr("Characters") + ':', MyTokenWidget);

	infoLabel = new QLabel(tr("<font size='-1'><i>For verification purposes, please type the characters above.</i></font>"), this);
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	layout->addRow(0, infoLabel);

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	RegisterAccountButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Regster Account"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	buttons->addButton(RegisterAccountButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(RegisterAccountButton, SIGNAL(clicked(bool)), this, SLOT(apply()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));

	if (!showButtons)
		buttons->hide();
}

void GaduCreateAccountWidget::resetGui()
{
	NewPassword->clear();
	ReNewPassword->clear();
	RememberPassword->setChecked(true);
	EMail->clear();
	IdentityCombo->setCurrentIdentity(Identity::null);
	MyTokenWidget->setTokenValue(QString());
	RegisterAccountButton->setEnabled(false);

	IdentityManager::instance()->removeUnused();
	setState(StateNotChanged);
}

void GaduCreateAccountWidget::dataChanged()
{
	bool valid = !NewPassword->text().isEmpty()
			&& !ReNewPassword->text().isEmpty()
			&& UrlHandlerManager::instance()->mailRegExp().exactMatch(EMail->text())
			&& !MyTokenWidget->tokenValue().isEmpty()
			&& IdentityCombo->currentIdentity();

	RegisterAccountButton->setEnabled(valid);

	if (NewPassword->text().isEmpty()
			&& ReNewPassword->text().isEmpty()
			&& RememberPassword->isChecked()
			&& EMail->text().isEmpty()
			&& !IdentityCombo->currentIdentity()
			&& MyTokenWidget->tokenValue().isEmpty())
		setState(StateNotChanged);
	else
		setState(valid ? StateChangedDataValid : StateChangedDataInvalid);
}

void GaduCreateAccountWidget::apply()
{
	if (NewPassword->text() != ReNewPassword->text())
	{
		MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"), tr("Error data typed in required fields.\n\n"
			"Passwords typed in both fields (\"Password\" and \"Retype Password\") "
			"must be the same!"));
		return;
	}

	GaduServerRegisterAccount *gsra = new GaduServerRegisterAccount(EMail->text(), NewPassword->text(),
			MyTokenWidget->tokenId(), MyTokenWidget->tokenValue());

	GaduWaitForAccountRegisterWindow *window = new GaduWaitForAccountRegisterWindow(gsra);
	connect(window, SIGNAL(uinRegistered(UinType)), this, SLOT(uinRegistered(UinType)));
	window->exec();
}

void GaduCreateAccountWidget::cancel()
{
	resetGui();
}

void GaduCreateAccountWidget::uinRegistered(UinType uin)
{
	// in any case that token will not be valid anymore
	MyTokenWidget->refreshToken();

	if (!uin)
	{
		emit accountCreated(Account());
		return;
	}

	Account gaduAccount = Account::create();
	gaduAccount.setAccountIdentity(IdentityCombo->currentIdentity());
	gaduAccount.setProtocolName("gadu");
	gaduAccount.setId(QString::number(uin));
	gaduAccount.setHasPassword(true);
	gaduAccount.setPassword(NewPassword->text());
	gaduAccount.setRememberPassword(RememberPassword->isChecked());

	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(gaduAccount.details());
	if (details)
		details->setState(StorableObject::StateNew);

	resetGui(); // don't need that data anymore

	emit accountCreated(gaduAccount);
}
