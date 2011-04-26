/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef GADU_CHANGE_PASSWORD_WINDOW_H
#define GADU_CHANGE_PASSWORD_WINDOW_H

#include <QtGui/QWidget>

#include "accounts/account.h"

#include "gadu-protocol.h" //for UinType

class QLineEdit;
class QPushButton;

class GaduServerChangePassword;
class TokenWidget;

class GaduChangePasswordWindow : public QWidget
{
	Q_OBJECT

	UinType Uin;
	Account MyAccount;

	QLineEdit *EMail;
	QLineEdit *OldPassword;
	QLineEdit *NewPassword;
	QLineEdit *ReNewPassword;
	TokenWidget *MyTokenWidget;
	QPushButton *ChangePasswordButton;

	void createGui();

private slots:
	void dataChanged();
	void changePassword();
	void changingFinished(GaduServerChangePassword *gscp);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	explicit GaduChangePasswordWindow(UinType uin, Account account, QWidget *parent = 0);
	virtual ~GaduChangePasswordWindow();

signals:
	void passwordChanged(const QString &);
};

#endif // GADU_CHANGE_PASSWORD_WINDOW_H
