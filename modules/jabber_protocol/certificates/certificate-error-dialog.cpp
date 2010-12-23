/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

/*
 * Copyright (C) 2008 Remko Troncon
 * See COPYING file for the detailed license.
 */

#include <QtDebug>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>

#include "certificates/certificate-helpers.h"
#include "certificates/certificate-error-dialog.h"
#include "certificates/certificate-display-dialog.h"
#include "certificates/trusted-certificates-manager.h"
#include "client/mini-client.h"

CertificateErrorDialog::CertificateErrorDialog(const QString& title, const QString& host, const QCA::Certificate& cert, 
	int result, QCA::Validity validity, const QString &domainOverride, QString &tlsOverrideDomain)
	: QObject(), certificate_(cert), result_(result), validity_(validity), domainOverride_(domainOverride), host_(host), tlsOverrideDomain_(tlsOverrideDomain)
{
	messageBox_ = new QMessageBox(QMessageBox::Warning, title, QObject::tr("The %1 certificate failed the authenticity test.").arg(host));
	messageBox_->setInformativeText(CertificateHelpers::resultToString(result, validity));

	detailsButton_ = messageBox_->addButton(QObject::tr("&Details..."), QMessageBox::ActionRole);
	continueButton_ = messageBox_->addButton(QObject::tr("&Connect anyway"), QMessageBox::AcceptRole);

	if (domainOverride.isEmpty())
		saveButton_ = messageBox_->addButton(QObject::tr("&Trust this certificate"), QMessageBox::AcceptRole);
	else
		saveButton_ = messageBox_->addButton(QObject::tr("&Trust this domain"), QMessageBox::AcceptRole);

	cancelButton_ = messageBox_->addButton(QMessageBox::Cancel);

	messageBox_->setDefaultButton(detailsButton_);
}

CertificateErrorDialog::~CertificateErrorDialog()
{
	delete messageBox_;
	messageBox_ = 0;
}

int CertificateErrorDialog::exec()
{
	while (true)
	{
		messageBox_->exec();
		if (messageBox_->clickedButton() == detailsButton_)
		{
			messageBox_->setResult(QDialog::Accepted);
			CertificateDisplayDialog dlg(certificate_, result_, validity_);
			dlg.exec();
		}
		else if (messageBox_->clickedButton() == continueButton_)
		{
			messageBox_->setResult(QDialog::Accepted);
			break;
		}
		else if (messageBox_->clickedButton() == cancelButton_)
		{
			messageBox_->setResult(QDialog::Rejected);
			break;
		}
		else if (messageBox_->clickedButton() == saveButton_)
		{
			messageBox_->setResult(QDialog::Accepted);

			TrustedCertificatesManager::instance()->addTrustedCertificate(certificate_.toDER().toBase64());
			break;
		}
	}
	return messageBox_->result();
}

void CertificateErrorDialog::disconnected(Account account)
{
	Q_UNUSED(account);
	
	if (messageBox_)
		messageBox_->close();
}
