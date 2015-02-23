/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
 * Copyright (C) 2008  Remko Troncon
 * See COPYING file for the detailed license.
 */

#ifndef CERTIFICATE_ERROR_WINDOW_H
#define CERTIFICATE_ERROR_WINDOW_H

#include <QtCore/QString>
#include <QtCrypto/QtCrypto>
#include <QtWidgets/QDialog>

class QCheckBox;
class QPushButton;

class CertificateErrorWindow : public QDialog
{
	Q_OBJECT

	QPushButton *ConnectButton;
	QPushButton *CancelButton;
	QCheckBox *RememberCheckbox;

	QCA::Certificate CurrentCertificate;
	int Result;
	QCA::Validity Validity;
	QString DomainOverride;
	QString Host;

private slots:
	void showCertificate();

public:
	CertificateErrorWindow(const QString& title, const QString& host, const QCA::Certificate& cert, int result, QCA::Validity validity,
			       const QString &domainOverride, QObject *receiver, const char *slot);
	virtual ~CertificateErrorWindow();

public slots:
	virtual void accept();

signals:
	void certificateAccepted();
};

#endif
