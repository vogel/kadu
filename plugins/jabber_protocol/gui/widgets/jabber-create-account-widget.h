/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_CREATE_ACCOUNT_WIDGET_H
#define JABBER_CREATE_ACCOUNT_WIDGET_H

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

#include <QtWidgets/QWidget>

#include "accounts/account.h"
#include "gui/widgets/account-create-widget.h"
#include "gui/widgets/identities-combo-box.h"

class QGridLayout;
class QLineEdit;
class QPushButton;

class ChooseIdentityWidget;
class JabberServersService;
class Jid;

class JabberCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT

	QLineEdit *Username;
	QComboBox *Domain;
	QLineEdit *NewPassword;
	QLineEdit *ReNewPassword;
	QCheckBox *RememberPassword;
	QLineEdit *EMail;
	IdentitiesComboBox *IdentityCombo;
	QPushButton *RegisterAccountButton;

	void createGui(bool showButtons);
	void resetGui();

private slots:
	void dataChanged();
	void jidRegistered(const Jid &jid);

public:
	explicit JabberCreateAccountWidget(bool showButtons, QWidget *parent = 0);
	virtual ~JabberCreateAccountWidget();

	void setJabberServersService(JabberServersService *serversService);

public slots:
	virtual void apply();
	virtual void cancel();

};

#endif // JABBER_CREATE_ACCOUNT_WIDGET_H
