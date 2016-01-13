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

#pragma once

#include "accounts/account.h"
#include "gui/widgets/account-create-widget.h"
#include "gui/widgets/identities-combo-box.h"

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <injeqt/injeqt.h>

class AccountManager;
class AccountStorage;
class ChooseIdentityWidget;
class IdentityManager;
class JabberServersService;
class Jid;

class QGridLayout;
class QLineEdit;
class QPushButton;

class JabberCreateAccountWidget : public AccountCreateWidget
{
	Q_OBJECT

public:
	explicit JabberCreateAccountWidget(bool showButtons, QWidget *parent = 0);
	virtual ~JabberCreateAccountWidget();

	void setJabberServersService(JabberServersService *serversService);

public slots:
	virtual void apply();
	virtual void cancel();

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<AccountStorage> m_accountStorage;
	QPointer<IdentityManager> m_identityManager;

	bool m_showButtons;
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
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setAccountStorage(AccountStorage *accountStorage);
	INJEQT_SET void setIdentityManager(IdentityManager *identityManager);
	INJEQT_INIT void init();

	void dataChanged();
	void jidRegistered(const Jid &jid);

};
