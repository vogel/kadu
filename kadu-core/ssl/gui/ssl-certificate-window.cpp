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

#include "ssl-certificate-window.h"

#include "ssl/gui/ssl-certificate-widget.h"

#include <QtNetwork/QSslCertificate>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

SslCertificateWindow::SslCertificateWindow(QSslCertificate certificate, QWidget *parent) :
		QWidget{parent}
{
	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumHeight(75);
	setMinimumWidth(250);
	setWindowRole("kadu-ssl-certificate-window");
	setWindowTitle("SSL Certificate Preview");

	createGui(std::move(certificate));
}

SslCertificateWindow::~SslCertificateWindow()
{
}

void SslCertificateWindow::createGui(QSslCertificate certificate)
{
	auto buttons = new QDialogButtonBox{this};
	buttons->addButton(tr("Close"), QDialogButtonBox::DestructiveRole);

	auto dataWidget = new SslCertificateWidget{std::move(certificate), this};

	auto layout = new QVBoxLayout{this};
	layout->addWidget(dataWidget);
	layout->addSpacing(16);
	layout->addWidget(buttons);
}

#include "moc_ssl-certificate-window.cpp"
