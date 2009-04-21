/*
 * sslcertdlg.h
 * Copyright (C) 2003  Justin Karneges
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef SSLCERTDLG_H
#define SSLCERTDLG_H

#include <QtCrypto>
#include <QtGui/QDialog>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

class SSLCertDlg : public QDialog
{
	Q_OBJECT
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout1;
    QLabel *textLabel4;
    QLabel *lb_valid;
    QLabel *textLabel2;
    QLabel *lb_notBefore;
    QLabel *textLabel3;
    QLabel *lb_notAfter;
    QLabel *textLabel1;
    QLabel *lb_sn;
    QSpacerItem *spacerItem;
    QTextBrowser *tb_cert;
    QFrame *line1;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem1;
    QPushButton *pb_close;
public:
	SSLCertDlg(QWidget *parent=0);

	void setCert(const QCA::Certificate &, int result, QCA::Validity);
	static void showCert(const QCA::Certificate &, int result, QCA::Validity);

protected:
	static void setLabelStatus(QLabel& l, bool ok);
	static QString makePropEntry(QCA::CertificateInfoType var, const QString &name, const QCA::CertificateInfo &list);
	QString makePropTable(const QString &heading, const QCA::CertificateInfo &props);

private:
	void setupUi(QDialog *SSLCert);
	void retranslateUi(QDialog *SSLCert);
};

#endif
