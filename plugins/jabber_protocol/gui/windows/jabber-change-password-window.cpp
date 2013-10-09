/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 badboy (badboy@gen2.org)
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

#include <QtGui/QApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "configuration/config-file-variant-wrapper.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "os/generic/window-geometry-manager.h"

#include "server/jabber-server-change-password.h"

#include "jabber-change-password-window.h"

JabberChangePasswordWindow::JabberChangePasswordWindow(Account account, QWidget *parent) :
		QWidget(parent, Qt::Window), MyAccount(account)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Change Password"));

	createGui();

	dataChanged();

	new WindowGeometryManager(new ConfigFileVariantWrapper("General", "JabberChangePasswordGeometry"), QRect(50, 50, 550, 200), this);
}

JabberChangePasswordWindow::~JabberChangePasswordWindow()
{
}

void JabberChangePasswordWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	CurrentPassword = new QLineEdit(this);
	CurrentPassword->setEchoMode(QLineEdit::Password);
	connect(CurrentPassword, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Old Password") + ':', CurrentPassword);

	QLabel *infoLabel = new QLabel(tr("<font size='-1'><i>Enter current password for your XMPP/Jabber account.</i></font>"), this);
	layout->addRow(0, infoLabel);

	NewPassword = new QLineEdit(this);
	NewPassword->setEchoMode(QLineEdit::Password);
	connect(NewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("New password") + ':', NewPassword);

	infoLabel = new QLabel(tr("<font size='-1'><i>Enter new password for your XMPP/Jabber account.</i></font>"), this);
	layout->addRow(0, infoLabel);

	ReNewPassword = new QLineEdit(this);
	ReNewPassword->setEchoMode(QLineEdit::Password);
	connect(ReNewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("Retype new password") + ':', ReNewPassword);

	mainLayout->addStretch(100);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	ChangePasswordButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Change Password"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	connect(ChangePasswordButton, SIGNAL(clicked(bool)), this, SLOT(changePassword()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(ChangePasswordButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
}

void JabberChangePasswordWindow::dataChanged()
{
	bool disable =  CurrentPassword->text().isEmpty()
			|| NewPassword->text().isEmpty()
			|| ReNewPassword->text().isEmpty();

	ChangePasswordButton->setEnabled(!disable);
}


void JabberChangePasswordWindow::changePassword()
{
	if (NewPassword->text() != ReNewPassword->text())
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Invalid data entered in required fields.\n\n"
			"Password entered in both fields (\"Password\" and \"Retype password\") "
			"must be the same!"), QMessageBox::Ok, this);
		return;
	}

	JabberServerChangePassword *gscp = new JabberServerChangePassword(MyAccount, CurrentPassword->text(), NewPassword->text());
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
		gscp->deleteLater();
	}

	if (result)
	{
		// using 'this' as parent is invalid, as close below will delete 'this' object
		MessageDialog::show(KaduIcon("dialog-information"), tr("Kadu"),tr("Changing password was successful."), QMessageBox::Ok);

		MyAccount.setPassword(NewPassword->text());
		emit passwordChanged(NewPassword->text());

		close();
	}
	else
		MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"), tr("An error has occurred. Please try again later."), QMessageBox::Ok, this);
}

void JabberChangePasswordWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

#include "moc_jabber-change-password-window.cpp"
