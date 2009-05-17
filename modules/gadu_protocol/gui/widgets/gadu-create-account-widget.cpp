 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

#include "server/token-fetcher.h"

#include "gadu-create-account-widget.h"

GaduCreateAccountWidget::GaduCreateAccountWidget(QWidget *parent) :
		QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	createGui();
}

GaduCreateAccountWidget::~GaduCreateAccountWidget()
{
}

void GaduCreateAccountWidget::createGui()
{
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(5);

	gridLayout->setColumnMinimumWidth(0, 20);
	gridLayout->setColumnStretch(3, 10);

	createIHaveAccountGui(gridLayout);
	createRegisterAccountGui(gridLayout);

	haveNumberChanged(true);

	haveNumberChanged(true);
}

void GaduCreateAccountWidget::createIHaveAccountGui(QGridLayout *gridLayout)
{
	QRadioButton *haveNumber = new QRadioButton(tr("I have a Gadu-Gadu number"), this);
	haveNumber->setChecked(true);
	connect(haveNumber, SIGNAL(toggled(bool)), this, SLOT(haveNumberChanged(bool)));
	gridLayout->addWidget(haveNumber, 0, 0, 1, 4);

	QLabel *numberLabel = new QLabel(tr("Gadu-Gadu number") + ":", this);
	gridLayout->addWidget(numberLabel, 1, 1, Qt::AlignRight);
	QLineEdit *number = new QLineEdit(this);
	gridLayout->addWidget(number, 1, 2, 1, 2);

	QLabel *passwordLabel = new QLabel(tr("Password") + ":", this);
	gridLayout->addWidget(passwordLabel, 2, 1, Qt::AlignRight);
	QLineEdit *password = new QLineEdit(this);
	password->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(password, 2, 2);

	QPushButton *remindPassword = new QPushButton(tr("Forgot password"), this);
	gridLayout->addWidget(remindPassword, 2, 3, Qt::AlignLeft);

	QCheckBox *rememberPassword = new QCheckBox(tr("Remember password"), this);
	rememberPassword->setChecked(true);
	gridLayout->addWidget(rememberPassword, 3, 2, 1, 2);

	QLabel *descriptionLabel = new QLabel(tr("Account description"), this);
	gridLayout->addWidget(descriptionLabel, 4, 1, Qt::AlignRight);
	QComboBox *description = new QComboBox(this);
	gridLayout->addWidget(description, 4, 2, 1, 2);

	QPushButton *addThisAccount = new QPushButton(tr("Add this account"), this);
	gridLayout->addWidget(addThisAccount, 5, 1, 1, 4);

	HaveNumberWidgets.append(numberLabel);
	HaveNumberWidgets.append(number);
	HaveNumberWidgets.append(passwordLabel);
	HaveNumberWidgets.append(password);
	HaveNumberWidgets.append(remindPassword);
	HaveNumberWidgets.append(rememberPassword);
	HaveNumberWidgets.append(descriptionLabel);
	HaveNumberWidgets.append(description);
	HaveNumberWidgets.append(addThisAccount);
}

void GaduCreateAccountWidget::createRegisterAccountGui(QGridLayout *gridLayout)
{
	QRadioButton *dontHaveNumber = new QRadioButton(tr("I don't have a Gadu-Gadu number"), this);
	gridLayout->addWidget(dontHaveNumber, 6, 0, 1, 4);

	QLabel *newPasswordLabel = new QLabel(tr("New password") + ":", this);
	gridLayout->addWidget(newPasswordLabel, 7, 1, Qt::AlignRight);
	QLineEdit *newPassword = new QLineEdit(this);
	newPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(newPassword, 7, 2, 1, 2);

	QLabel *reNewPasswordLabel = new QLabel(tr("Retype password") + ":", this);
	gridLayout->addWidget(reNewPasswordLabel, 8, 1, Qt::AlignRight);
	QLineEdit *reNewPassword = new QLineEdit(this);
	reNewPassword->setEchoMode(QLineEdit::Password);
	gridLayout->addWidget(reNewPassword, 8, 2, 1, 2);

	QLabel *eMailLabel = new QLabel(tr("Your e-mail address") + ":", this);
	gridLayout->addWidget(eMailLabel, 9, 1, Qt::AlignRight);
	QLineEdit *eMail = new QLineEdit(this);
	gridLayout->addWidget(eMail, 9, 2, 1, 2);

	QLabel *tokenLabel = new QLabel(tr("Type this code") + ":", this);
	gridLayout->addWidget(tokenLabel, 10, 1, Qt::AlignRight);
	TokenImage = new QLabel(this);
	gridLayout->addWidget(TokenImage, 10, 2);
	QLineEdit *tokenCode = new QLineEdit(this);
	gridLayout->addWidget(tokenCode, 10, 3);

	TokenFetcher *fetcher = new TokenFetcher(true, this);
	connect(fetcher, SIGNAL(tokenFetched(const QString &, QPixmap)), this, SLOT(tokenFetched(const QString &, QPixmap)));
	fetcher->fetchToken();

	QPushButton *registerAccount = new QPushButton(tr("Register"), this);
	gridLayout->addWidget(registerAccount, 11, 1, 1, 3);

	QPushButton *addThisAccount = new QPushButton(tr("Add this account"), this);
	gridLayout->addWidget(addThisAccount, 12, 1, 1, 3);

	DontHaveNumberWidgets.append(newPasswordLabel);
	DontHaveNumberWidgets.append(newPassword);
	DontHaveNumberWidgets.append(reNewPasswordLabel);
	DontHaveNumberWidgets.append(reNewPassword);
	DontHaveNumberWidgets.append(eMailLabel);
	DontHaveNumberWidgets.append(eMail);
	DontHaveNumberWidgets.append(registerAccount);
	DontHaveNumberWidgets.append(tokenLabel);
	DontHaveNumberWidgets.append(TokenImage);
	DontHaveNumberWidgets.append(tokenCode);
	DontHaveNumberWidgets.append(addThisAccount);
}

void GaduCreateAccountWidget::haveNumberChanged(bool haveNumber)
{
	foreach (QWidget *widget, HaveNumberWidgets)
		widget->setVisible(haveNumber);
	foreach (QWidget *widget, DontHaveNumberWidgets)
		widget->setVisible(!haveNumber);
}

void GaduCreateAccountWidget::tokenFetched(const QString &tokenId, QPixmap tokenImage)
{
	TokenImage->setPixmap(tokenImage);
}
