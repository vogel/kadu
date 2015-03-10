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

#pragma once

#include "exports.h"

#include <QtWidgets/QWidget>

class QSslCertificate;
class QTreeWidget;

class KADUAPI SslCertificateWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SslCertificateWidget(QSslCertificate certificate, QWidget *parent = nullptr);
	virtual ~SslCertificateWidget();

private:
	QTreeWidget *m_dataWidget;

	void createGui();
	void fillGui(QSslCertificate certificate);
	QString asHex(const QByteArray &byteArray);
	void addItem(const QString &name, const QString &value, bool valid = true);
	void addItem(const QString &name, const QByteArray &value);
	void addItem(const QString &name, const QStringList &values);
	void addItem(const QString &name, bool value, bool valid = true);

};
