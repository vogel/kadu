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
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QFormLayout>

#include "icons/kadu-icon.h"

#include "password-dialog-widget.h"

PasswordDialogWidget::PasswordDialogWidget(const QString &message, QVariant data, QWidget *parent) :
		DialogWidget(tr("Incorrect password"), message, QPixmap(), parent), Data(data)
{
	QIcon icon = KaduIcon("dialog-password").icon();
	Pixmap = icon.pixmap(icon.actualSize(QSize(64, 64)));

	createGui();
}

PasswordDialogWidget::~PasswordDialogWidget()
{
}

void PasswordDialogWidget::createGui()
{
	formLayout = new QFormLayout(this);
	formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

	Password = new QLineEdit(this);
	Password->setEchoMode(QLineEdit::Password);
	Password->setFocus();
	connect(Password, SIGNAL(textChanged(const QString &)), this, SLOT(passwordTextChanged(const QString &)));

	QLabel *passwordLabel = new QLabel(tr("Password") + ":", this);
	formLayout->addRow(passwordLabel, Password);

	Store = new QCheckBox(tr("Store this password"), this);
	formLayout->addWidget(Store);

	setLayout(formLayout);
}

void PasswordDialogWidget::dialogAccepted()
{
	emit passwordEntered(Data, Password->text(), Store->isChecked());
}

void PasswordDialogWidget::dialogRejected()
{
	emit passwordEntered(Data, QString(), false);
}

void PasswordDialogWidget::passwordTextChanged(const QString &text)
{
	emit valid(!text.isEmpty());
}

#include "moc_password-dialog-widget.cpp"
