/*
 * %kadu copyright begin%
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef PROXY_GROUP_BOX_H
#define PROXY_GROUP_BOX_H

#include "accounts/account.h"
#include "gui/widgets/modal-configuration-widget.h"
#include "exports.h"

class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;

class KADUAPI ProxyGroupBox : public ModalConfigurationWidget
{
	Q_OBJECT

	Account MyAccount;

	QWidget *ProxyAuthWidget;
	QGroupBox *GroupBox;
	QCheckBox *UseProxy;
	QLineEdit *Host;
	QLineEdit *ProxyPort;
	QCheckBox *ProxyAuthentication;
	QLineEdit *Username;
	QLineEdit *Password;

private slots:
	void dataChanged();

public:
	ProxyGroupBox(Account account, const QString &title, QWidget *parent = 0);

	void loadProxyData();

public slots:
	void apply();
	void cancel();

};

#endif // PROXY_GROUP_BOX_H
