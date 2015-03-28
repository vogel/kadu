/*
 * %kadu copyright begin%
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "jabber-wait-for-account-register-window.h"

#include "services/jabber-register-account.h"

#include "icons/icons-manager.h"

JabberWaitForAccountRegisterWindow::JabberWaitForAccountRegisterWindow(JabberRegisterAccount *jabberRegisterAccount, QWidget *parent) :
		// using C++ initializers breaks Qt's lupdate
		ProgressWindow(tr("Registering new XMPP account"), parent),
		m_jabberRegisterAccount(jabberRegisterAccount)
{
	setCancellable(true);

	connect(m_jabberRegisterAccount, SIGNAL(statusMessage(QString)), this, SLOT(statusMessage(QString)));
	connect(m_jabberRegisterAccount, SIGNAL(success()), this, SLOT(success()));
	connect(m_jabberRegisterAccount, SIGNAL(error(QString)), this, SLOT(error(QString)));

	addProgressEntry("dialog-information", tr("Connecting with server."));
	m_jabberRegisterAccount->start();
}

JabberWaitForAccountRegisterWindow::~JabberWaitForAccountRegisterWindow()
{
	if (m_jabberRegisterAccount)
	{
		disconnect(m_jabberRegisterAccount, nullptr, this, nullptr);
		m_jabberRegisterAccount->deleteLater();
	}
}

void JabberWaitForAccountRegisterWindow::statusMessage(const QString &statusMessage)
{
	addProgressEntry("dialog-information", statusMessage);
}

void JabberWaitForAccountRegisterWindow::success()
{
	auto message = tr("Registration was successful. Your new XMPP username is %1.\nStore it in a safe place along with the password.\n"
		"Now please add your friends to the buddy list.");

	progressFinished(true, "dialog-information", message);
	emit jidRegistered(m_jabberRegisterAccount->jid());
}

void JabberWaitForAccountRegisterWindow::error(const QString &errorMessage)
{
	progressFinished(false, "dialog-error", tr("Registration failed.\n\nServer message: %1").arg(errorMessage));
	emit jidRegistered(Jid());
	deleteLater();
}

#include "moc_jabber-wait-for-account-register-window.cpp"
