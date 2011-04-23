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

#ifndef GADU_UNREGISTER_ACCOUNT_WINDOW_H
#define GADU_UNREGISTER_ACCOUNT_WINDOW_H

#include <QtGui/QWidget>

#include "accounts/account.h"

class QLineEdit;
class QPushButton;

class GaduServerUnregisterAccount;
class TokenWidget;

class GaduUnregisterAccountWindow : public QWidget
{
	Q_OBJECT

	Account MyAccount;

	QLineEdit *AccountId;
	QLineEdit *Password;
	TokenWidget *MyTokenWidget;
	QPushButton *RemoveAccountButton;

	void createGui();

private slots:
	void dataChanged();
	void removeAccount();
	void unregisteringFinished(GaduServerUnregisterAccount *gsua);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	explicit GaduUnregisterAccountWindow(Account account, QWidget *parent = 0);
	virtual ~GaduUnregisterAccountWindow();
};

#endif // GADU_UNREGISTER_ACCOUNT_WINDOW_H
