/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef TLEN_EDIT_ACCOUNT_WIDGET_H
#define TLEN_EDIT_ACCOUNT_WIDGET_H

#include "gui/widgets/account-edit-widget.h"

class QCheckBox;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QVBoxLayout;

class ChooseIdentityWidget;
class ProxyGroupBox;
class TlenPersonalInfoWidget;

class TlenEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

	QCheckBox *ConnectAtStart;
	QLineEdit *AccountName;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;

	QPushButton *ApplyButton;
	QPushButton *CancelButton;

	ChooseIdentityWidget *ChooseIdentity;
	ProxyGroupBox *proxy;

	TlenPersonalInfoWidget *gpiw;

	void createGui();
	void createGeneralTab(QTabWidget *);
	void createPersonalDataTab(QTabWidget *);
	void createBuddiesTab(QTabWidget *);
	void createConnectionTab(QTabWidget *);

	void loadAccountData();
	void loadConnectionData();

private slots:
	void removeAccount();

public:
	explicit TlenEditAccountWidget(Account account, QWidget *parent = 0);
	virtual ~TlenEditAccountWidget();

	virtual void apply();
	virtual void cancel();

};

#endif // TLEN_EDIT_ACCOUNT_WIDGET_H
