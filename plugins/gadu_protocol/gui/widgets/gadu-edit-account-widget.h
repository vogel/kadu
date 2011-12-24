/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef GADU_EDIT_ACCOUNT_WIDGET_H
#define GADU_EDIT_ACCOUNT_WIDGET_H

#include "gui/widgets/account-edit-widget.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QVBoxLayout;

class GaduAccountDetails;
class GaduPersonalInfoWidget;
class IdentitiesComboBox;
class ProxyComboBox;

class GaduEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

	GaduAccountDetails *Details;

	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;

	QCheckBox *LimitImageSize;
	QSpinBox *MaximumImageSize;
	QCheckBox *ImageSizeAsk;
	QCheckBox *ReceiveImagesDuringInvisibility;
	QSpinBox *MaximumImageRequests;

	QCheckBox *ChatImageSizeWarning;

	IdentitiesComboBox *Identities;

	GaduPersonalInfoWidget *gpiw;

	QCheckBox *useDefaultServers;
	QLineEdit *ipAddresses;
	QCheckBox *AllowFileTransfers;
	QCheckBox *UseTlsEncryption;

	QCheckBox *ShowStatusToEveryone;
	QCheckBox *SendTypingNotification;
	QCheckBox *ReceiveSpam;

	QLineEdit *ExternalIp;
	QLineEdit *ExternalPort;

	ProxyComboBox *ProxyCombo;

	QPushButton *ApplyButton;
	QPushButton *CancelButton;

	void createGui();
	void createGeneralTab(QTabWidget *);
	void createGeneralGroupBox(QVBoxLayout *layout);
	void createPersonalInfoTab(QTabWidget *);
	void createBuddiesTab(QTabWidget *);
	void createConnectionTab(QTabWidget *);
	void createOptionsTab(QTabWidget *);

	void loadAccountData();

	void resetState();

private slots:
	void dataChanged();
	void removeAccount();
	void remindPasssword();
	void changePasssword();
	void passwordChanged(const QString &newPassword);
	void showStatusToEveryoneToggled(bool toggled);

public:
	explicit GaduEditAccountWidget(Account account, QWidget *parent = 0);
	virtual ~GaduEditAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

};

#endif // GADU_EDIT_ACCOUNT_WIDGET_H
