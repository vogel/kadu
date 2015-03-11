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

#include "ssl-certificate-widget.h"

#include <QtNetwork/QSslCertificate>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

SslCertificateWidget::SslCertificateWidget(QSslCertificate certificate, QWidget *parent) :
		QWidget{parent}
{
	createGui();
	fillGui(std::move(certificate));
}

SslCertificateWidget::~SslCertificateWidget()
{
}

void SslCertificateWidget::createGui()
{
	m_dataWidget = new QTreeWidget{this};
	m_dataWidget->setColumnCount(2);
	m_dataWidget->setItemsExpandable(false);
	m_dataWidget->setRootIsDecorated(false);

	m_dataWidget->header()->hide();
	m_dataWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_dataWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

	auto layout = new QVBoxLayout{this};
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(m_dataWidget);
}

void SslCertificateWidget::fillGui(QSslCertificate certificate)
{
	addItem(tr("Valid"), certificate.isValid(), certificate.isValid());
	addItem(tr("Blacklisted"), certificate.isBlacklisted(), !certificate.isBlacklisted());
	addItem(tr("Valid from"), certificate.effectiveDate().toString(), certificate.effectiveDate() <= QDateTime::currentDateTime());
	addItem(tr("Valid to"), certificate.expiryDate().toString(), certificate.expiryDate() >= QDateTime::currentDateTime());
	addItem(tr("Digest (Md5)"), asHex(certificate.digest(QCryptographicHash::Algorithm::Md5)));
	addItem(tr("Digest (Sha1)"), asHex(certificate.digest(QCryptographicHash::Algorithm::Sha1)));
	addItem(tr("Serial number"), certificate.serialNumber());

	addItem(tr("Issuer organization"), certificate.issuerInfo(QSslCertificate::SubjectInfo::Organization));
	addItem(tr("Issuer common name"), certificate.issuerInfo(QSslCertificate::SubjectInfo::CommonName));
	addItem(tr("Issuer locality name"), certificate.issuerInfo(QSslCertificate::SubjectInfo::LocalityName));
	addItem(tr("Issuer organizational unit name"), certificate.issuerInfo(QSslCertificate::SubjectInfo::OrganizationalUnitName));
	addItem(tr("Issuer country name"), certificate.issuerInfo(QSslCertificate::SubjectInfo::CountryName));
	addItem(tr("Issuer state or province name"), certificate.issuerInfo(QSslCertificate::SubjectInfo::StateOrProvinceName));
	addItem(tr("Issuer distinguished name qualifier"), certificate.issuerInfo(QSslCertificate::SubjectInfo::DistinguishedNameQualifier));
	addItem(tr("Issuer serial number"), certificate.issuerInfo(QSslCertificate::SubjectInfo::SerialNumber));
	addItem(tr("Issuer email address"), certificate.issuerInfo(QSslCertificate::SubjectInfo::EmailAddress));

	addItem(tr("Subject organization"), certificate.subjectInfo(QSslCertificate::SubjectInfo::Organization));
	addItem(tr("Subject common name"), certificate.subjectInfo(QSslCertificate::SubjectInfo::CommonName));
	for (auto &&subjectAlternativeName : certificate.subjectAlternativeNames())
		addItem(tr("Subject alernative name"), subjectAlternativeName);
	addItem(tr("Subject locality name"), certificate.subjectInfo(QSslCertificate::SubjectInfo::LocalityName));
	addItem(tr("Subject organizational unit name"), certificate.subjectInfo(QSslCertificate::SubjectInfo::OrganizationalUnitName));
	addItem(tr("Subject country name"), certificate.subjectInfo(QSslCertificate::SubjectInfo::CountryName));
	addItem(tr("Subject state or province name"), certificate.subjectInfo(QSslCertificate::SubjectInfo::StateOrProvinceName));
	addItem(tr("Subject distinguished name qualifier"), certificate.subjectInfo(QSslCertificate::SubjectInfo::DistinguishedNameQualifier));
	addItem(tr("Subject serial number"), certificate.subjectInfo(QSslCertificate::SubjectInfo::SerialNumber));
	addItem(tr("Subject email address"), certificate.subjectInfo(QSslCertificate::SubjectInfo::EmailAddress));
}

QString SslCertificateWidget::asHex(const QByteArray &byteArray)
{
	auto hex = byteArray.toHex();
	auto result = hex.mid(0, 2);
	auto length = hex.length();
	for (auto i = 2; i < length; i += 2)
		result += ":" + hex.mid(i, 2);
	return result;
}

void SslCertificateWidget::addItem(const QString &name, const QString &value, bool valid)
{
	auto item = new QTreeWidgetItem{m_dataWidget, {name, value}};
	if (!valid)
	{
		item->setTextColor(0, Qt::red);
		item->setTextColor(1, Qt::red);
	}

	m_dataWidget->addTopLevelItem(item);
}

void SslCertificateWidget::addItem(const QString &name, const QByteArray &value)
{
	addItem(name, QString::fromLatin1(value));
}

void SslCertificateWidget::addItem(const QString &name, const QStringList &values)
{
	for (auto &&value : values)
		addItem(name, value);
}

void SslCertificateWidget::addItem(const QString &name, bool value, bool valid)
{
	addItem(name, value ? tr("Yes") : tr("No"), valid);
}

#include "moc_ssl-certificate-widget.cpp"
