/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

class IdentitiesComboBox;

class GaduAddAccountWidget : public AccountAddWidget
{
	Q_OBJECT

	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;
	QLabel *RemindPassword;
	IdentitiesComboBox *Identity;
	QPushButton *AddAccountButton;

	void createGui(bool showButtons);
	void resetGui();

private slots:
	void dataChanged();

public:
	explicit GaduAddAccountWidget(bool showButtons, QWidget *parent = 0);
	virtual ~GaduAddAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

	void remindPasssword();
};

#endif // GADU_ADD_ACCOUNT_WIDGET_H
