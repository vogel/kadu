/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef JABBER_ADD_ACCOUNT_WIDGET_H
#define JABBER_ADD_ACCOUNT_WIDGET_H

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

#include "gui/widgets/account-add-widget.h"
#include "gui/widgets/identities-combo-box.h"

class QGridLayout;
class QLineEdit;
class QPushButton;

class ChooseIdentityWidget;
class JabberProtocolFactory;

class JabberAddAccountWidget : public AccountAddWidget
{
	Q_OBJECT

	JabberProtocolFactory *Factory;
	QLineEdit *Username;
	QComboBox *Domain;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;
	QLabel *AtLabel;
	IdentitiesComboBox *Identity;
	QPushButton *AddAccountButton;

	void createGui();
	void resetGui();

private slots:
	void dataChanged();

public:
	explicit JabberAddAccountWidget(JabberProtocolFactory *factory, QWidget *parent = 0);
	virtual ~JabberAddAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

};

#endif // JABBER_ADD_ACCOUNT_WIDGET_H
