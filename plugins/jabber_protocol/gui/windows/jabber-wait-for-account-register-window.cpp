/*
 * %kadu copyright begin%
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QCloseEvent>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QMovie>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "icons/icons-manager.h"

#include "server/jabber-server-register-account.h"

#include "jabber-wait-for-account-register-window.h"

JabberWaitForAccountRegisterWindow::JabberWaitForAccountRegisterWindow(JabberServerRegisterAccount *jsra, QWidget *parent) :
		ProgressWindow(tr("Registering new XMPP account"), parent)
{
	connect(jsra, SIGNAL(finished(JabberServerRegisterAccount *)),
			this, SLOT(registerNewAccountFinished(JabberServerRegisterAccount *)));

	addProgressEntry("dialog-information", tr("Plase wait. New XMPP account is being registered."));
	jsra->performAction();
}

JabberWaitForAccountRegisterWindow::~JabberWaitForAccountRegisterWindow()
{
}

void JabberWaitForAccountRegisterWindow::registerNewAccountFinished(JabberServerRegisterAccount* jsra)
{
	if (jsra && jsra->result())
	{
		QString message(tr("Registration was successful. Your new XMPP username is %1.\nStore it in a safe place along with the password.\n"
				   "Now please add your friends to the buddy list."));
		progressFinished(true, "dialog-information", message.arg(jsra->jid()));

		emit jidRegistered(jsra->jid(), jsra->client()->tlsOverrideDomain());
	}
	else
	{
		QString message(tr("An error has occurred during registration. Please try again later."));
		progressFinished(false, "dialog-error", message);

		emit jidRegistered(QString(), QString());
	}

	delete jsra;
}

#include "moc_jabber-wait-for-account-register-window.cpp"
