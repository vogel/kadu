/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
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
	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *layout = new QVBoxLayout(this);

	QLabel *messageLabel = new QLabel(message, this);
	Password = new QLineEdit(this);
	Password->setEchoMode(QLineEdit::Password);
	Password->setFocus();

	Store = new QCheckBox(tr("Store this password"), this);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Ok"), this);
	okButton->setDefault(true);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(accepted()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

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
