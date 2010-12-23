/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "icons-manager.h"

#include "password-window.h"

void PasswordWindow::getPassword(const QString &message, QObject *receiver, const char *slot)
{
	PasswordWindow *window = new PasswordWindow(message, 0);
	connect(window, SIGNAL(passwordEntered(const QString &, bool)), receiver, slot);

	window->exec();
}

PasswordWindow::PasswordWindow(const QString &message, QWidget *parent) :
		QDialog(parent)
{
	setWindowRole("kadu-password");

	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *layout = new QVBoxLayout(this);

	QLabel *messageLabel = new QLabel(message, this);
	Password = new QLineEdit(this);
	Password->setEchoMode(QLineEdit::Password);
	Password->setFocus();

	Store = new QCheckBox(tr("Store this password"), this);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), this);
	okButton->setDefault(true);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(accepted()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(canceled()));

	layout->addWidget(messageLabel);
	layout->addWidget(Password);
	layout->addWidget(Store);
	layout->addWidget(buttons);
}

PasswordWindow::~PasswordWindow()
{
}

void PasswordWindow::accepted()
{
	emit passwordEntered(Password->text(), Store->isChecked());
	close();
}

void PasswordWindow::canceled()
{
	emit passwordEntered(QString(), false);
	close();
}
