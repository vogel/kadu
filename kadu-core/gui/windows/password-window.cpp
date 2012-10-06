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
#include <QtGui/QHBoxLayout>
#include <QtGui/QFormLayout>

#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "gui/widgets/dialog/title-widget.h"

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
	setWindowTitle(tr("Incorrect password") + " - Kadu");
	horizontalLayout = new QHBoxLayout(this);

	iconLabel = new QLabel(this);
	QIcon icon = KaduIcon("dialog-password").icon();
	iconLabel->setPixmap(icon.pixmap(icon.actualSize(QSize(64, 64))));
	horizontalLayout->addWidget(iconLabel);

	QSpacerItem *horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
	horizontalLayout->addItem(horizontalSpacer);

	verticalLayout = new QVBoxLayout();

	QWidget *widget = new QWidget(this);
	verticalLayout->addWidget(widget);

	Title = new TitleWidget(this);
	Title->setText(message);

	verticalLayout->addWidget(Title);

	formLayout = new QFormLayout();
	formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

	Password = new QLineEdit(this);
	Password->setEchoMode(QLineEdit::Password);
	Password->setFocus();

	QLabel *passwordLabel = new QLabel(tr("Password") + ":", this);
	formLayout->addRow(passwordLabel, Password);

	Store = new QCheckBox(tr("Store this password"), this);
	formLayout->addWidget(Store);

	verticalLayout->addLayout(formLayout);

	horizontalLayout->addLayout(verticalLayout);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), this);
	okButton->setDefault(true);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	connect(this, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
	connect(this, SIGNAL(rejected()), this, SLOT(dialogRejected()));

	verticalLayout->addWidget(buttons);
}

PasswordWindow::~PasswordWindow()
{
}

void PasswordWindow::dialogAccepted()
{
	emit passwordEntered(Data, Password->text(), Store->isChecked());
	close();
}

void PasswordWindow::dialogRejected()
{
	emit passwordEntered(Data, QString(), false);
	close();
}
