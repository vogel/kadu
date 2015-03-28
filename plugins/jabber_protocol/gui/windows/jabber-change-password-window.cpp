/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-change-password-window.h"

#include "services/jabber-change-password-service.h"
#include "services/jabber-change-password.h"

#include "configuration/config-file-variant-wrapper.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "os/generic/window-geometry-manager.h"
#include "protocols/protocol.h"

#include <QtGui/QKeyEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QVBoxLayout>

JabberChangePasswordWindow::JabberChangePasswordWindow(JabberChangePasswordService *changePasswordService, Account account, QWidget *parent) :
		// using C++ initializers breaks Qt's lupdate
		QWidget(parent, Qt::Window),
		m_changePasswordService(changePasswordService),
		m_account(account)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Change Password"));

	createGui();

	dataChanged();

	new WindowGeometryManager{new ConfigFileVariantWrapper{"General", "JabberChangePasswordGeometry"}, QRect{50, 50, 550, 200}, this};
}

JabberChangePasswordWindow::~JabberChangePasswordWindow()
{
}

void JabberChangePasswordWindow::createGui()
{
	auto mainLayout = new QVBoxLayout(this);

	auto formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	auto layout = new QFormLayout(formWidget);

	m_newPassword = new QLineEdit(this);
	m_newPassword->setEchoMode(QLineEdit::Password);
	connect(m_newPassword, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("New password") + ':', m_newPassword);

	auto infoLabel = new QLabel(tr("<font size='-1'><i>Enter new password for your XMPP/Jabber account.</i></font>"), this);
	layout->addRow(0, infoLabel);

	m_reNewPassword = new QLineEdit(this);
	m_reNewPassword->setEchoMode(QLineEdit::Password);
	connect(m_reNewPassword, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("Retype new password") + ':', m_reNewPassword);

	mainLayout->addStretch(100);

	auto buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	m_changePasswordButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Change Password"), this);
	auto cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	connect(m_changePasswordButton, SIGNAL(clicked(bool)), this, SLOT(changePassword()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(m_changePasswordButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
}

void JabberChangePasswordWindow::dataChanged()
{
	auto disable =
			m_newPassword->text().isEmpty() ||
			m_reNewPassword->text().isEmpty();

	m_changePasswordButton->setEnabled(!disable);
}


void JabberChangePasswordWindow::changePassword()
{
	if (!m_account.protocolHandler()->isConnected())
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Log in before changing password."), QMessageBox::Ok, this);
		return;
	}

	if (m_newPassword->text() != m_reNewPassword->text())
	{
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Invalid data entered in required fields.\n\n"
			"Password entered in both fields (\"Password\" and \"Retype password\") "
			"must be the same!"), QMessageBox::Ok, this);
		return;
	}

	auto changePassword = m_changePasswordService->changePassword(m_account.id(), m_newPassword->text());
	connect(changePassword, SIGNAL(passwordChanged()), this, SLOT(passwordChanged()));
	connect(changePassword, SIGNAL(error(QString)), this, SLOT(error(QString)));
}


void JabberChangePasswordWindow::passwordChanged()
{
	// using 'this' as parent is invalid, as close below will delete 'this' object
	MessageDialog::show(KaduIcon("dialog-information"), tr("Kadu"), tr("Changing password was successful."), QMessageBox::Ok);

	m_account.setPassword(m_newPassword->text());
	emit passwordChanged(m_newPassword->text());

	close();
}

void JabberChangePasswordWindow::error(const QString& errorMessage)
{
	MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"), tr("Password change failed.\n\nError: %1").arg(errorMessage), QMessageBox::Ok, this);

	close();
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
