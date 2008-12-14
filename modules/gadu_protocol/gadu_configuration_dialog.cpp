/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "gadu_account_data.h"
#include "icons_manager.h"

#include "gadu_configuration_dialog.h"

GaduConfigurationDialog::GaduConfigurationDialog(GaduAccountData *accountData, QWidget *parent)
	: QDialog(parent), AccountData(accountData)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Gadu-Gadu account"));

	createGui();
	loadAccountData();
}

GaduConfigurationDialog::~GaduConfigurationDialog()
{
}

void GaduConfigurationDialog::createGui()
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);

	QGridLayout *grid_layout = new QGridLayout();

	grid_layout->addWidget(new QLabel(tr("Uin"), this), 0, 0);
	grid_layout->addWidget(new QLabel(tr("Password"), this), 1, 0);

	UinLineEdit = new QLineEdit(this);
	PasswordLineEdit = new QLineEdit(this);
	PasswordLineEdit->setEchoMode(QLineEdit::Password);

	grid_layout->addWidget(UinLineEdit, 0, 1);
	grid_layout->addWidget(PasswordLineEdit, 1, 1);

	QDialogButtonBox *buttons_layout = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Ok"), this);
	buttons_layout->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	buttons_layout->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked()), this, SLOT(updateAccountData()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	main_layout->addItem(grid_layout);
	main_layout->addWidget(buttons_layout);
}

void GaduConfigurationDialog::loadAccountData()
{
	UinLineEdit->setText(QString::number(AccountData->uin()));
	PasswordLineEdit->setText(AccountData->password());
}

void GaduConfigurationDialog::updateAccountData()
{
	AccountData->setId(UinLineEdit->text());
	AccountData->setPassword(PasswordLineEdit->text());

	close();
}
