/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "ssl-certificate-error-dialog.h"

#include "ssl/gui/ssl-certificate-widget.h"
#include "ssl/ssl-certificate-repository.h"

#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslError>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QVBoxLayout>

SslCertificateErrorDialog::SslCertificateErrorDialog(SslCertificate certificate, const QList<QSslError> &errors, QWidget *parent) :
		QDialog{parent},
		m_certificate{std::move(certificate)}
{
	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumHeight(150);
	setMinimumWidth(450);
	setWindowRole("kadu-ssl-certificate-error");
	setWindowTitle("SSL Certificate Error");

	createGui(errors);
}

SslCertificateErrorDialog::~SslCertificateErrorDialog()
{
}

void SslCertificateErrorDialog::setSslCertificateRepository(SslCertificateRepository *sslCertificateRepository)
{
	m_sslCertificateRepository = sslCertificateRepository;
}

void SslCertificateErrorDialog::createGui(const QList<QSslError> &errors)
{
	Q_UNUSED(errors);

	auto errorMessage = tr("Certificate for <b>%1</b> failed authenticity validation:").arg(m_certificate.hostName());
	auto errorStrings = QStringList{};
	std::transform(std::begin(errors), std::end(errors), std::back_inserter(errorStrings), [](const QSslError &error){
		return QString{"<br/>%1"}.arg(error.errorString());
	});

	if (!errorStrings.empty())
		errorMessage = QString{"%1%2"}.arg(errorMessage).arg(errorStrings.join(""));
	auto errorLabel = new QLabel{errorMessage, this};
	errorLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto showCertificateButton = new QPushButton{tr("Show certificate details..."), this};
	showCertificateButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto showCertificateWidget = new QWidget{this};
	showCertificateWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto showCertificateLayout = new QHBoxLayout{showCertificateWidget};
	showCertificateLayout->setMargin(0);
	showCertificateLayout->setSpacing(0);
	showCertificateLayout->addStretch(100);
	showCertificateLayout->addWidget(showCertificateButton);

	auto detailsLabel = new QLabel{tr("Certificate details:"), this};
	detailsLabel->hide();

	auto dataWidget = new SslCertificateWidget{QSslCertificate{QByteArray::fromHex(m_certificate.pemHexEncodedCertificate().toLatin1()), QSsl::Pem}, this};
	dataWidget->hide();

	auto connectAnywayButton = new QPushButton{qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Connect anyway"), this};
	connect(connectAnywayButton, SIGNAL(clicked()), this, SLOT(connectAnyway()));

	auto trustCertificateButton = new QPushButton{qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Trust certificate"), this};
	connect(trustCertificateButton, SIGNAL(clicked()), this, SLOT(trustCertificate()));

	auto cancelButton = new QPushButton{qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Cancel"), this};
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	auto buttons = new QDialogButtonBox{this};
	buttons->addButton(connectAnywayButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(trustCertificateButton, QDialogButtonBox::ActionRole);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	auto layout = new QVBoxLayout{this};
	layout->addWidget(errorLabel, 0, Qt::AlignTop);
	layout->addWidget(showCertificateWidget, 0, Qt::AlignTop);
	layout->addSpacing(16);
	layout->addWidget(detailsLabel);
	layout->addWidget(dataWidget);
	layout->addSpacing(16);
	layout->addWidget(buttons);

	connect(showCertificateButton, SIGNAL(clicked(bool)), showCertificateWidget, SLOT(hide()));
	connect(showCertificateButton, SIGNAL(clicked(bool)), detailsLabel, SLOT(show()));
	connect(showCertificateButton, SIGNAL(clicked(bool)), dataWidget, SLOT(show()));
	connect(showCertificateButton, SIGNAL(clicked(bool)), this, SLOT(increaseHeight()));
}

void SslCertificateErrorDialog::increaseHeight()
{
	if (height() < 400)
		resize(width(), 400);
}

void SslCertificateErrorDialog::connectAnyway()
{
	m_sslCertificateRepository->addCertificate(m_certificate);
	accept();
}

void SslCertificateErrorDialog::trustCertificate()
{
	m_sslCertificateRepository->addPersistentCertificate(m_certificate);
	accept();
}

#include "moc_ssl-certificate-error-dialog.cpp"
