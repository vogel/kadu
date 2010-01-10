/*
 * %kadu copyright begin%
 * Copyright 2009 Juzef (juzefwt@tlen.pl)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_ADD_ACCOUNT_WIDGET_H
#define GADU_ADD_ACCOUNT_WIDGET_H

#include "gui/widgets/account-add-widget.h"

class QCheckBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class ChooseIdentityWidget;

class GaduAddAccountWidget : public AccountAddWidget
{
	Q_OBJECT

	QLineEdit *AccountName;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;
	QLabel *RemindPassword;
	ChooseIdentityWidget *Identity;
	
	QPushButton *AddThisAccount;

	void createGui();

private slots:
	void dataChanged();
public:
	explicit GaduAddAccountWidget(QWidget *parent = 0);
	virtual ~GaduAddAccountWidget();

	virtual void apply();
};

#endif // GADU_ADD_ACCOUNT_WIDGET_H
