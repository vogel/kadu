/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include "core/core.h"
#include "gui/windows/kadu-window.h"

#include "certificates/certificate-display-dialog.h"
#include "certificates/certificate-helpers.h"
#include "certificates/trusted-certificates-manager.h"
#include "client/mini-client.h"

#include "certificate-error-window.h"

CertificateErrorWindow::CertificateErrorWindow(const QString& title, const QString& host, const QCA::Certificate& cert,
	int result, QCA::Validity validity, const QString &domainOverride, QObject *receiver, const char *slot)
	: QDialog(), CurrentCertificate(cert), Result(result), Validity(validity), DomainOverride(domainOverride), Host(host)
{
	setWindowRole("kadu-certificate-error");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(title);
	resize(500, 180);

	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnStretch(2, 4);

	QLabel *firstLabel = new QLabel(tr("Cannot establish secure connection with server <i>%1</i>.").arg(host), this);
	QLabel *messageLabel = new QLabel(CertificateHelpers::resultToString(result, validity), this);
	QPushButton *ShowButton = new QPushButton(tr("Show certificate..."), this);
	QLabel *finalQuestionLabel = new QLabel(tr("If you do not trust <i>%1</i>, cancel the connection.").arg(host), this);
	RememberCheckbox = new QCheckBox(tr("Remember my choice for this certificate"), this);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	ConnectButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Connect"), this);
	ConnectButton->setDefault(true);
	buttons->addButton(ConnectButton, QDialogButtonBox::AcceptRole);

	CancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	buttons->addButton(CancelButton, QDialogButtonBox::RejectRole);

	connect(ShowButton, SIGNAL(clicked(bool)), this, SLOT(showCertificate()));
	connect(ConnectButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));

	layout->addWidget(firstLabel, 0, 0, 1, 4);
	layout->addWidget(messageLabel, 1, 0, 1, 4);
	layout->addWidget(ShowButton, 2, 0, 1, 1);
	layout->addWidget(finalQuestionLabel, 3, 0, 1, 4);
	layout->addWidget(RememberCheckbox, 4, 0, 1, 2);
	layout->addWidget(buttons, 4, 2, 1, 2);

	CancelButton->setFocus();

	if (receiver && slot)
		connect(this, SIGNAL(certificateAccepted()), receiver, slot);
}

CertificateErrorWindow::~CertificateErrorWindow()
{
}

void CertificateErrorWindow::showCertificate()
{
	CertificateDisplayDialog dlg(CurrentCertificate, Result, Validity);
	dlg.exec();
}

void CertificateErrorWindow::accept()
{
	TrustedCertificatesManager::instance()->addTrustedCertificate(CurrentCertificate.toDER().toBase64(), RememberCheckbox->isChecked());
	emit certificateAccepted();
	QDialog::accept();
}

#include "moc_certificate-error-window.cpp"
