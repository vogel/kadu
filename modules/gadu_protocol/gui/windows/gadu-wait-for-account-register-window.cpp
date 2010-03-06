/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "icons-manager.h"

#include "server/gadu-server-register-account.h"

#include "gadu-wait-for-account-register-window.h"

GaduWaitForAccountRegisterWindow::GaduWaitForAccountRegisterWindow(GaduServerRegisterAccount *gsra, QWidget *parent) :
		QDialog(parent), CanClose(false)
{
	createGui();

	connect(gsra, SIGNAL(finished(GaduServerRegisterAccount *)),
			this, SLOT(registerNewAccountFinished(GaduServerRegisterAccount *)));

	gsra->performAction();
}

GaduWaitForAccountRegisterWindow::~GaduWaitForAccountRegisterWindow()
{
}

void GaduWaitForAccountRegisterWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *topWidget = new QWidget(this);
	mainLayout->addWidget(topWidget);

	QHBoxLayout *topWidgetLayout = new QHBoxLayout(topWidget);

	WaitLabel = new QLabel(topWidget);
	QMovie *waitMovie = new QMovie(IconsManager::instance()->iconPath("kadu_icons/please-wait.gif"));
	waitMovie->start();
	WaitLabel->setMovie(waitMovie);

	topWidgetLayout->addWidget(WaitLabel);

	MessageLabel = new QLabel(tr("Plase wait. New Gadu-Gadu account is being registered."), topWidget);
	topWidgetLayout->addWidget(MessageLabel, 0, Qt::AlignTop);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	CloseButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"));
	CloseButton->setEnabled(false);
	connect(CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(CloseButton, QDialogButtonBox::DestructiveRole);

	mainLayout->addWidget(buttons);
}

void GaduWaitForAccountRegisterWindow::enableClosing()
{
	CanClose = true;
	CloseButton->setEnabled(true);
}

void GaduWaitForAccountRegisterWindow::registerNewAccountFinished(GaduServerRegisterAccount* gsra)
{
	if (gsra && gsra->result())
	{
		WaitLabel->setPixmap(IconsManager::instance()->pixmapByPath("32x32/dialog-information.png"));
		QString message(tr("Registration was successful. Your new number is %1.\nStore it in a safe place along with the password.\nNow add your friends to the userlist."));
		MessageLabel->setText(message.arg(gsra->uin()));

		emit uinRegistered(gsra->uin());
	}
	else
	{
		WaitLabel->setPixmap(IconsManager::instance()->pixmapByPath("32x32/dialog-warning.png"));
		QString message(tr("An error has occured while registration. Please try again later."));
		MessageLabel->setText(message);

		emit uinRegistered(0);
	}

	if (gsra)
		delete gsra;

	enableClosing();
}

void GaduWaitForAccountRegisterWindow::closeEvent(QCloseEvent *e)
{
	if (!CanClose)
		e->ignore();
	else
		QDialog::closeEvent(e);
}
