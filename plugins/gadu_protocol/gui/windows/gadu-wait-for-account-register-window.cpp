/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QMovie>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "gui/widgets/progress-label.h"
#include "icons/icons-manager.h"

#include "server/gadu-server-register-account.h"

#include "gadu-wait-for-account-register-window.h"

GaduWaitForAccountRegisterWindow::GaduWaitForAccountRegisterWindow(GaduServerRegisterAccount *gsra, QWidget *parent) :
		ProgressWindow(parent)
{
	connect(gsra, SIGNAL(finished(GaduServerRegisterAccount *)),
			this, SLOT(registerNewAccountFinished(GaduServerRegisterAccount *)));

	setState(ProgressIcon::StateInProgress, tr("Plase wait. New Gadu-Gadu account is being registered."));
	gsra->performAction();
}

GaduWaitForAccountRegisterWindow::~GaduWaitForAccountRegisterWindow()
{
}

void GaduWaitForAccountRegisterWindow::registerNewAccountFinished(GaduServerRegisterAccount *gsra)
{
	if (gsra && gsra->result())
	{
		QString message(tr("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist."));
		setState(ProgressIcon::StateFinished, message.arg(gsra->uin()));

		emit uinRegistered(gsra->uin());
	}
	else
	{
		QString message(tr("An error has occurred during registration. Please try again later."));
		setState(ProgressIcon::StateFailed, message);

		emit uinRegistered(0);
	}

	delete gsra;
}
