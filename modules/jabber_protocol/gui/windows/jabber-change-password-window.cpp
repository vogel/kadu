/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "icons-manager.h"

#include "server/jabber-server-change-password.h"

#include "jabber-change-password-window.h"

JabberChangePasswordWindow::JabberChangePasswordWindow(Account account, QWidget *parent) :
		QWidget(parent, Qt::Window), MyAccount(account)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Change Password"));

	createGui();

	dataChanged();

	loadWindowGeometry(this, "General", "JabberChangePasswordGeometry", 50, 50, 370, 230);
}

JabberChangePasswordWindow::~JabberChangePasswordWindow()
{
	saveWindowGeometry(this, "General", "JabberChangePasswordGeometry");
}

void JabberChangePasswordWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QLabel *infoLabel = new QLabel(tr("This dialog box allows you to change your current password.\n"));
	infoLabel->setWordWrap(true);
	mainLayout->addWidget(infoLabel);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	OldPassword = new QLineEdit(this);
	OldPassword->setEchoMode(QLineEdit::Password);
	connect(OldPassword, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Old Password") + ":", OldPassword);

	layout->addRow(0, new QLabel(tr("<font size='-1'><i>Enter current password for your XMPP/Jabber account.</i></font>"), this));

	NewPassword = new QLineEdit(this);
	NewPassword->setEchoMode(QLineEdit::Password);
	connect(NewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("New Password") + ":", NewPassword);

	layout->addRow(0, new QLabel(tr("<font size='-1'><i>Enter new password for your XMPP/Jabber account.</i></font>"), this));

	ReNewPassword = new QLineEdit(this);
	ReNewPassword->setEchoMode(QLineEdit::Password);
	connect(ReNewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("Retype New Password") + ":", ReNewPassword);

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	ChangePasswordButton = new QPushButton(IconsManager::instance()->iconByPath("16x16/dialog-apply.png"), tr("Change Password"), this);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->iconByPath("16x16/dialog-cancel.png"), tr("Cancel"), this);

	connect(ChangePasswordButton, SIGNAL(clicked(bool)), this, SLOT(changePassword()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(ChangePasswordButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
}

void JabberChangePasswordWindow::dataChanged()
{
	bool disable =  OldPassword->text().isEmpty()
			|| NewPassword->text().isEmpty()
			|| ReNewPassword->text().isEmpty();

	ChangePasswordButton->setEnabled(!disable);
}


void JabberChangePasswordWindow::changePassword()
{
	if (NewPassword->text() != ReNewPassword->text())
	{
		MessageDialog::msg(tr("Error data typed in required fields.\n\n"
			"Passwords typed in both fields (\"Password\" and \"Retype Password\") "
			"should be the same!"));
		return;
	}

	JabberServerChangePassword *gscp = new JabberServerChangePassword(MyAccount, OldPassword->text(), NewPassword->text());
	connect(gscp, SIGNAL(finished(JabberServerChangePassword *)),
			this, SLOT(changingFinished(JabberServerChangePassword *)));

	gscp->performAction();
}


void JabberChangePasswordWindow::changingFinished(JabberServerChangePassword *gscp)
{
	bool result = false;
	if (gscp)
	{
		result = gscp->result();
		delete gscp;
	}

	if (result)
	{
		MessageDialog::msg(tr("Changing password was successful."), false, "Information", parentWidget());

		MyAccount.setPassword(NewPassword->text());
		emit passwordChanged(NewPassword->text());

		close();
	}
	else
		MessageDialog::msg(tr("An error has occured. Please try again later."), false, "Critical", parentWidget());
}
