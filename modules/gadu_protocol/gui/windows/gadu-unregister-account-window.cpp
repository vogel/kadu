/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "accounts/account-manager.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "icons-manager.h"

#include "gadu-id-validator.h"
#include "gui/widgets/token-widget.h"
#include "server/gadu-server-unregister-account.h"

#include "gadu-unregister-account-window.h"

GaduUnregisterAccountWindow::GaduUnregisterAccountWindow(Account account, QWidget *parent) :
		QWidget(parent, Qt::Window), MyAccount(account)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Unregister account"));

	createGui();

	dataChanged();

	loadWindowGeometry(this, "General", "GaduUnregisterAccountGeometry", 0, 50, 500, 350);
}

GaduUnregisterAccountWindow::~GaduUnregisterAccountWindow()
{
	saveWindowGeometry(this, "General", "GaduUnregisterAccountGeometry");
}

void GaduUnregisterAccountWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QLabel *infoLabel = new QLabel(tr("This dialog box allows you to unregister your account. Be aware of using this "
				"option."));
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	mainLayout->addWidget(infoLabel);

	infoLabel = new QLabel(tr("<font size=\"+1\"><b>It will permanently delete your account and you will not be able to use "
				"it later!</b></font>"));
	infoLabel->setWordWrap(true);
	infoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	infoLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	mainLayout->addWidget(infoLabel);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *layout = new QFormLayout(formWidget);

	AccountId = new QLineEdit(this);
	AccountId->setValidator(GaduIdValidator::instance());
	connect(AccountId, SIGNAL(textChanged(QString)), this, SLOT(dataChanged()));
	layout->addRow(tr("Gadu-Gadu number") + ':', AccountId);

	Password = new QLineEdit(this);
	Password->setEchoMode(QLineEdit::Password);
	connect(Password, SIGNAL(textChanged(const QString &)), this, SLOT(dataChanged()));
	layout->addRow(tr("Password") + ':', Password);

	infoLabel = new QLabel(tr("<font size='-1'><i>Enter password for your Gadu-Gadu account.</i></font>"), this);
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

	RemoveAccountButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Unregister Account"), this);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);

	connect(RemoveAccountButton, SIGNAL(clicked(bool)), this, SLOT(removeAccount()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(RemoveAccountButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
}

void GaduUnregisterAccountWindow::dataChanged()
{
	bool disable =  AccountId->text().isEmpty()
			|| Password->text().isEmpty()
			|| MyTokenWidget->tokenValue().isEmpty();

	RemoveAccountButton->setEnabled(!disable);
}

void GaduUnregisterAccountWindow::removeAccount()
{
	GaduServerUnregisterAccount *gsua = new GaduServerUnregisterAccount(AccountId->text().toUInt(), Password->text(),
			MyTokenWidget->tokenId(), MyTokenWidget->tokenValue());
	connect(gsua, SIGNAL(finished(GaduServerUnregisterAccount *)),
			this, SLOT(unregisteringFinished(GaduServerUnregisterAccount *)));

	gsua->performAction();
}


void GaduUnregisterAccountWindow::unregisteringFinished(GaduServerUnregisterAccount *gsua)
{
	bool result = false;
	if (gsua)
	{
		result = gsua->result();
		delete gsua;
	}

	if (result)
	{
		MessageDialog::show("dialog-information", tr("Kadu"), tr("Unregistation was successful. Now you don't have any GG number :("), QMessageBox::Ok, parentWidget());
		AccountManager::instance()->removeAccountAndBuddies(MyAccount);

		close();
	}
	else
		MessageDialog::show("dialog-error", tr("Kadu"), tr("An error has occurred while unregistration. Please try again later."), QMessageBox::Ok, parentWidget());
}

void GaduUnregisterAccountWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
