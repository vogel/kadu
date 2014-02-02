/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
class QLineEdit;
class QPushButton;

class IdentitiesComboBox;

class GaduAddAccountWidget : public AccountAddWidget
{
	Q_OBJECT

	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;
	IdentitiesComboBox *Identity;
	QPushButton *AddAccountButton;

	void createGui(bool showButtons);
	void resetGui();

private slots:
	void dataChanged();
	void registerAccount();
	void remindUin();
	void remindPassword();

public:
	explicit GaduAddAccountWidget(bool showButtons, QWidget *parent = 0);
	virtual ~GaduAddAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

};

#endif // GADU_ADD_ACCOUNT_WIDGET_H
