/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QIntValidator>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "misc/misc.h"
#include "url-handlers/url-handler-manager.h"

#include "gui/widgets/token-widget.h"
#include "server/gadu-server-change-password.h"

#include "gadu-change-password-window.h"

GaduChangePasswordWindow::GaduChangePasswordWindow(UinType uin, Account account, QWidget *parent) :
		QWidget(parent, Qt::Window), Uin(uin), MyAccount(account)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Change Password"));

	createGui();

	dataChanged();

	loadWindowGeometry(this, "General", "GaduChangePasswordGeometry", 0, 50, 550, 200);
}

GaduChangePasswordWindow::~GaduChangePasswordWindow()
{
	saveWindowGeometry(this, "General", "GaduChangePasswordGeometry");
}

void GaduChangePasswordWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	EMail = new QLineEdit(this);
	connect(EMail, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("E-Mail Address") + ':', EMail);

	QLabel *infoLabel = new QLabel(tr("<font size='-1'><i>Type E-Mail Address used during registration.</i></font>"), this);
	layout->addRow(0, infoLabel);

	CurrentPassword = new QLineEdit(this);
	CurrentPassword->setEchoMode(QLineEdit::Password);
	connect(CurrentPassword, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Old Password") + ':', CurrentPassword);

	infoLabel = new QLabel(tr("<font size='-1'><i>Enter current password for your Gadu-Gadu account.</i></font>"), this);
	layout->addRow(0, infoLabel);

	NewPassword = new QLineEdit(this);
	NewPassword->setEchoMode(QLineEdit::Password);
	connect(NewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("New Password") + ':', NewPassword);

	infoLabel = new QLabel(tr("<font size='-1'><i>Enter new password for your Gadu-Gadu account.</i></font>"), this);
	layout->addRow(0, infoLabel);

	ReNewPassword = new QLineEdit(this);
	ReNewPassword->setEchoMode(QLineEdit::Password);
	connect(ReNewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("Retype New Password") + ':', ReNewPassword);

	MyTokenWidget = new TokenWidget(this);
	connect(MyTokenWidget, SIGNAL(modified()), this, SLOT(dataChanged()));
	layout->addRow(tr("Characters") + ':', MyTokenWidget);

	infoLabel = new QLabel(tr("<font size='-1'><i>For verification purposes, please type the characters above.</i></font>"), this);
	layout->addRow(0, infoLabel);

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

void GaduChangePasswordWindow::dataChanged()
{
	bool disable =  EMail->text().indexOf(UrlHandlerManager::instance()->mailRegExp()) < 0
			|| CurrentPassword->text().isEmpty()
			|| NewPassword->text().isEmpty()
			|| ReNewPassword->text().isEmpty()
			|| MyTokenWidget->tokenValue().isEmpty();

	ChangePasswordButton->setEnabled(!disable);
}

void GaduChangePasswordWindow::changePassword()
{
	if (NewPassword->text() != ReNewPassword->text())
	{
		MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"), tr("Error data typed in required fields.\n\n"
			"Passwords typed in both fields (\"Password\" and \"Retype Password\") "
			"must be the same!"), QMessageBox::Ok, parentWidget());
		return;
	}

	GaduServerChangePassword *gscp = new GaduServerChangePassword(Uin, EMail->text(), CurrentPassword->text(), NewPassword->text(),
			MyTokenWidget->tokenId(), MyTokenWidget->tokenValue());
	connect(gscp, SIGNAL(finished(GaduServerChangePassword *)),
			this, SLOT(changingFinished(GaduServerChangePassword *)));

	gscp->performAction();
}

void GaduChangePasswordWindow::changingFinished(GaduServerChangePassword *gscp)
{
	bool result = false;
	if (gscp)
	{
		result = gscp->result();
		delete gscp;
	}

	if (result)
	{
		MessageDialog::show(KaduIcon("dialog-information"), tr("Kadu"), tr("Changing password was successful."), QMessageBox::Ok, parentWidget());

		MyAccount.setPassword(NewPassword->text());
		emit passwordChanged(NewPassword->text());

		close();
	}
	else
		MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"), tr("An error has occurred. Please try again later."), QMessageBox::Ok, parentWidget());
}

void GaduChangePasswordWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
