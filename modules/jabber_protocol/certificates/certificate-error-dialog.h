 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Copyright (C) 2008  Remko Troncon
 * See COPYING file for the detailed license.
 */

#ifndef CERTIFICATEERRORDIALOG_H
#define CERTIFICATEERRORDIALOG_H

#include <QtCrypto>
#include <QString>

class QMessageBox;
class QPushButton;

class CertificateErrorDialog
{

		QMessageBox* messageBox_;
		QPushButton* detailsButton_;
		QPushButton* continueButton_;
		QPushButton* cancelButton_;
		QPushButton* saveButton_;
		QCA::Certificate certificate_;
		int result_;
		QCA::Validity validity_;
		QString domainOverride_;
		QString host_;
		QObject *Parent;
		QString &tlsOverrideDomain_;
		QByteArray &tlsOverrideCert_;

	public:
		CertificateErrorDialog(const QString& title, const QString& host, const QCA::Certificate& cert, int result, QCA::Validity validity, const QString &domainOverride, QObject *parent, QString &tlsOverrideDomain, QByteArray &tlsOverrideCert);

		QMessageBox* getMessageBox() {
			return messageBox_;
		}

		int exec();
};

#endif
