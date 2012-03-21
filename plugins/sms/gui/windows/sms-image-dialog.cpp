/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMovie>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"

#include "sms-image-dialog.h"

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
	QVBoxLayout *layout = new QVBoxLayout(this);

	QMovie *pleaseWaitMovie = new QMovie(KaduIcon("kadu_icons/16x16/please-wait.gif").fullPath());
	pleaseWaitMovie->start();

	PixmapLabel = new QLabel(this);
	PixmapLabel->setMovie(pleaseWaitMovie);

	TokenEdit = new QLineEdit(this);

	QWidget *formWidget = new QWidget(this);
	layout->addWidget(formWidget);

	QFormLayout *formLayout = new QFormLayout(formWidget);
	formLayout->setRowWrapPolicy(QFormLayout::WrapAllRows);
	formLayout->addRow(0, PixmapLabel);
	formLayout->addRow(tr("Enter text from the picture:"), TokenEdit);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), buttons);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), buttons);

	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancelButton, QDialogButtonBox::DestructiveRole);

	layout->addSpacing(16);
	layout->addWidget(buttons);

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
		MessageDialog::exec(KaduIcon("dialog-error"), tr("SMS"), tr("Unable to fetch required token"));
		reject();
		return;
	}

	QPixmap tokenPixmap;
	if (!tokenPixmap.loadFromData(TokenNetworkReply->readAll()))
	{
		reject();
		return;
	}

	PixmapLabel->setPixmap(tokenPixmap);

	updateGeometry();
	setFixedSize(sizeHint());
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
