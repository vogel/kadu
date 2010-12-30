/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef GADU_CREATE_ACCOUNT_WIDGET_H
#define GADU_CREATE_ACCOUNT_WIDGET_H

#include <QtGui/QWidget>

#include "accounts/account.h"
#include "gui/widgets/account-create-widget.h"
#include "protocols/protocol.h"

class QCheckBox;
class QGridLayout;
class QLineEdit;
class QPushButton;

class GaduServerRegisterAccount;
class IdentitiesComboBox;
class TokenWidget;

class GaduCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT

	QLineEdit *NewPassword;
	QLineEdit *ReNewPassword;
	QCheckBox *RememberPassword;
	QLineEdit *EMail;
	IdentitiesComboBox *IdentityCombo;
	TokenWidget *MyTokenWidget;
	QPushButton *RegisterAccountButton;

	void createGui(bool showButtons);
	void resetGui();

private slots:
	void dataChanged();

	void uinRegistered(UinType uin);

public:
	explicit GaduCreateAccountWidget(bool showButtons, QWidget *parent = 0);
	virtual ~GaduCreateAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

};

#endif // GADU_CREATE_ACCOUNT_WIDGET_H
