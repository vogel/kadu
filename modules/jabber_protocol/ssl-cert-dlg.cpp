/*
 * sslcertdlg.cpp
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

#include <QtCrypto>
#include <QDateTime>
#include <QLabel>
#include <QPushButton>

#include "cert-util.h"
#include "ssl-cert-dlg.h"


SSLCertDlg::SSLCertDlg(QWidget *parent)
:QDialog(parent)
{
	setupUi(this);
	setModal(true);
	setWindowTitle("Kadu: " + caption());

	connect(pb_close, SIGNAL(clicked()), SLOT(close()));
	pb_close->setDefault(true);
	pb_close->setFocus();
}

void SSLCertDlg::setupUi(QDialog *SSLCert)
    {
    if (SSLCert->objectName().isEmpty())
        SSLCert->setObjectName(QString::fromUtf8("SSLCert"));
    SSLCert->resize(518, 369);
    vboxLayout = new QVBoxLayout(SSLCert);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(11);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    textLabel4 = new QLabel(SSLCert);
    textLabel4->setObjectName(QString::fromUtf8("textLabel4"));

    vboxLayout1->addWidget(textLabel4);

    lb_valid = new QLabel(SSLCert);
    lb_valid->setObjectName(QString::fromUtf8("lb_valid"));

    vboxLayout1->addWidget(lb_valid);

    textLabel2 = new QLabel(SSLCert);
    textLabel2->setObjectName(QString::fromUtf8("textLabel2"));

    vboxLayout1->addWidget(textLabel2);

    lb_notBefore = new QLabel(SSLCert);
    lb_notBefore->setObjectName(QString::fromUtf8("lb_notBefore"));

    vboxLayout1->addWidget(lb_notBefore);

    textLabel3 = new QLabel(SSLCert);
    textLabel3->setObjectName(QString::fromUtf8("textLabel3"));

    vboxLayout1->addWidget(textLabel3);

    lb_notAfter = new QLabel(SSLCert);
    lb_notAfter->setObjectName(QString::fromUtf8("lb_notAfter"));

    vboxLayout1->addWidget(lb_notAfter);

    textLabel1 = new QLabel(SSLCert);
    textLabel1->setObjectName(QString::fromUtf8("textLabel1"));

    vboxLayout1->addWidget(textLabel1);

    lb_sn = new QLabel(SSLCert);
    lb_sn->setObjectName(QString::fromUtf8("lb_sn"));

    vboxLayout1->addWidget(lb_sn);

    spacerItem = new QSpacerItem(20, 106, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout1->addItem(spacerItem);


    hboxLayout->addLayout(vboxLayout1);

    tb_cert = new QTextBrowser(SSLCert);
    tb_cert->setObjectName(QString::fromUtf8("tb_cert"));
    tb_cert->setMinimumSize(QSize(350, 300));
    tb_cert->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    hboxLayout->addWidget(tb_cert);


    vboxLayout->addLayout(hboxLayout);

    line1 = new QFrame(SSLCert);
    line1->setObjectName(QString::fromUtf8("line1"));
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    line1->setFrameShape(QFrame::HLine);

    vboxLayout->addWidget(line1);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    spacerItem1 = new QSpacerItem(421, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem1);

    pb_close = new QPushButton(SSLCert);
    pb_close->setObjectName(QString::fromUtf8("pb_close"));

    hboxLayout1->addWidget(pb_close);


    vboxLayout->addLayout(hboxLayout1);


    retranslateUi(SSLCert);

    QMetaObject::connectSlotsByName(SSLCert);
} // setupUi

void SSLCertDlg::retranslateUi(QDialog *SSLCert)
    {
    SSLCert->setWindowTitle(QApplication::translate("SSLCert", "SSL Cert", 0, QApplication::UnicodeUTF8));
    textLabel4->setText(QApplication::translate("SSLCert", "Certificate Validation:", 0, QApplication::UnicodeUTF8));
    lb_valid->setText(QString());
    textLabel2->setText(QApplication::translate("SSLCert", "Valid From:", 0, QApplication::UnicodeUTF8));
    lb_notBefore->setText(QString());
    textLabel3->setText(QApplication::translate("SSLCert", "Valid Until:", 0, QApplication::UnicodeUTF8));
    lb_notAfter->setText(QString());
    textLabel1->setText(QApplication::translate("SSLCert", "Serial Number:", 0, QApplication::UnicodeUTF8));
    lb_sn->setText(QString());
    pb_close->setText(QApplication::translate("SSLCert", "Close", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(SSLCert);
    } // retranslateUi

QString SSLCertDlg::makePropTable(const QString &heading, const QCA::CertificateInfo &list)
{
	QString str;
	str += "<tr><td><i>" + heading + "</i><br>";
	str += "<table>";
	str += makePropEntry(QCA::Organization, tr("Organization:"), list);
	str += makePropEntry(QCA::OrganizationalUnit, tr("Organizational unit:"), list);
	str += makePropEntry(QCA::Locality, tr("Locality:"), list);
	str += makePropEntry(QCA::State, tr("State:"), list);
	str += makePropEntry(QCA::Country, tr("Country:"), list);
	str += makePropEntry(QCA::CommonName, tr("Common name:"), list);
	str += makePropEntry(QCA::DNS, tr("Domain name:"), list);
	str += makePropEntry(QCA::XMPP, tr("XMPP name:"), list);
	str += makePropEntry(QCA::Email, tr("Email:"), list);
	str += "</table></td></tr>";
	return str;
}

void SSLCertDlg::setCert(const QCA::Certificate &cert, int result, QCA::Validity validity)
{
	if(cert.isNull())
		return;

	if(result == QCA::TLS::Valid) {
		lb_valid->setText(tr("The certificate is valid."));
		setLabelStatus(*lb_valid,true);
	}
	else {
		lb_valid->setText(tr("The certificate is NOT valid!") + "\n" + QString(tr("Reason: %1.")).arg(CertUtil::resultToString(result, validity)));
		setLabelStatus(*lb_valid,false);
	}

	QDateTime now = QDateTime::currentDateTime();
	QDateTime notBefore = cert.notValidBefore();
	QDateTime notAfter = cert.notValidAfter();
	lb_notBefore->setText(cert.notValidBefore().toString());
	setLabelStatus(*lb_notBefore, now > notBefore);
	lb_notAfter->setText(cert.notValidAfter().toString());
	setLabelStatus(*lb_notAfter, now < notAfter);

	lb_sn->setText(cert.serialNumber().toString());

	QString str;
	str += "<table>";
	str += makePropTable(tr("Subject Details:"), cert.subjectInfo());
	str += makePropTable(tr("Issuer Details:"), cert.issuerInfo());
	str += "</table>";
	for (int i=0; i < 2; i++) {
		QString hashstr = QCA::Hash(i == 0 ? "md5" : "sha1").hashToString(cert.toDER())
					.toUpper().replace(QRegExp("(..)"), ":\\1").mid(1);
		str += QString("Fingerprint(%1): %2<br>").arg(i == 0 ? "MD5" : "SHA-1").arg(hashstr);
	}
	tb_cert->setText(str);
}

void SSLCertDlg::showCert(const QCA::Certificate &certificate, int result, QCA::Validity validity)
{
	SSLCertDlg *w = new SSLCertDlg(0);
	w->setCert(certificate, result, validity);
	w->exec();
	delete w;
}

void SSLCertDlg::setLabelStatus(QLabel& l, bool ok)
{
	l.setPaletteForegroundColor(ok ? QColor("#2A993B") : QColor("#810000"));
}

QString SSLCertDlg::makePropEntry(QCA::CertificateInfoType var, const QString &name, const QCA::CertificateInfo &list)
{
	QString val;
	QList<QString> values = list.values(var);
	for (int i = 0; i < values.size(); ++i)
		val += values.at(i) + "<br>";

	if(val.isEmpty())
		return "";
	else
		return QString("<tr><td><nobr><b>") + name + "</b></nobr></td><td>" + val + "</td></tr>";
}
