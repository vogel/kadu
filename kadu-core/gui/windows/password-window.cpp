/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "icons/icons-manager.h"

#include "password-window.h"

void PasswordWindow::getPassword(const QString &message, QVariant data, QObject *receiver, const char *slot)
{
	PasswordWindow *window = new PasswordWindow(message, data, 0);
	connect(window, SIGNAL(passwordEntered(const QVariant &, const QString &, bool)), receiver, slot);

	window->exec();
}

PasswordWindow::PasswordWindow(const QString &message, QVariant data, QWidget *parent) :
		QDialog(parent), DesktopAwareObject(this), Data(data)
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
	emit passwordEntered(Data, Password->text(), Store->isChecked());
	close();
}

void PasswordWindow::canceled()
{
	emit passwordEntered(Data, QString(), false);
	close();
}
