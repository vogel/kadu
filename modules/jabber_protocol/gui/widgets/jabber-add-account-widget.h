/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef JABBER_ADD_ACCOUNT_WIDGET_H
#define JABBER_ADD_ACCOUNT_WIDGET_H

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

#include "gui/widgets/account-add-widget.h"

class QGridLayout;
class QLineEdit;
class QPushButton;

class ChooseIdentityWidget;

class JabberAddAccountWidget : public AccountAddWidget
{
	Q_OBJECT

	QLineEdit *AccountName;
	QLineEdit *Username;
	QComboBox *Domain;
	QLineEdit *Password;
	QCheckBox *RememberPassword;
	QLabel *RemindPassword;
	ChooseIdentityWidget *Identity;
	
	void createGui();

private slots:
	void dataChanged();
public:
	explicit JabberAddAccountWidget(QWidget *parent = 0);
	virtual ~JabberAddAccountWidget();
	
	virtual void apply();

};

#endif // JABBER_ADD_ACCOUNT_WIDGET_H
