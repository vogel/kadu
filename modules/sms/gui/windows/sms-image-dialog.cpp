/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMovie>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "icons-manager.h"

#include "sms-image-dialog.h"

#include <stdio.h>

SmsImageDialog::SmsImageDialog(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod, QWidget *parent) :
		QDialog(parent), CallbackObject(callbackObject), CallbackMethod(callbackMethod)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
	loadTokenImage(tokenImageUrl);
}

SmsImageDialog::~SmsImageDialog()
{
}

void SmsImageDialog::createGui()
{
	QMovie *pleaseWaitMovie = new QMovie(IconsManager::instance()->iconPath("kadu_icons/16x16/please-wait.gif"));
	pleaseWaitMovie->start();

	PixmapLabel = new QLabel(this);
	PixmapLabel->setMovie(pleaseWaitMovie);

	QLabel* label = new QLabel(tr("Enter text from the picture:"), this);
	TokenEdit = new QLineEdit(this);

	QGridLayout *grid = new QGridLayout(this);
	grid->addWidget(PixmapLabel, 0, 0, 1, 2, Qt::AlignCenter);
	grid->addWidget(label, 1, 0, 1, 1);
	grid->addWidget(TokenEdit, 1, 1, 1, 1);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), buttons);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), buttons);

	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::DestructiveRole);

	grid->addWidget(buttons, 2, 0, 1, 2);

	connect(TokenEdit, SIGNAL(returnPressed()), this, SLOT(accept()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void SmsImageDialog::loadTokenImage(const QString& tokenImageUrl)
{
	QNetworkAccessManager *network = new QNetworkAccessManager(this);
	TokenNetworkReply = network->get(QNetworkRequest(tokenImageUrl));

	connect(TokenNetworkReply, SIGNAL(finished()), this, SLOT(tokenImageDownloaded()));
}

void SmsImageDialog::result(const QString &value)
{
	QScriptValueList arguments;
	arguments.append(value);
	CallbackMethod.call(CallbackObject, arguments);
}

void SmsImageDialog::tokenImageDownloaded()
{
	if (QNetworkReply::NoError != TokenNetworkReply->error())
	{
		reject(); // TODO: 0.6.6 display some message here
		return;
	}

	QPixmap tokenPixmap;
	if (!tokenPixmap.loadFromData(TokenNetworkReply->readAll()))
	{
		reject();
		return;
	}

	PixmapLabel->setPixmap(tokenPixmap);
}

void SmsImageDialog::accept()
{
	result(TokenEdit->text());
	QDialog::accept();
}

void SmsImageDialog::reject()
{
	result(QString());
	QDialog::reject();
}
