/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "widgets/account-configuration-widget.h"
#include "widgets/modal-configuration-widget.h"
#include "exports.h"

#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QTabWidget;
class QPushButton;

class AccountConfigurationWidgetFactoryRepository;
class AccountConfigurationWidgetFactory;
class AccountConfigurationWidget;
class CompositeConfigurationValueStateNotifier;
class SimpleConfigurationValueStateNotifier;

class KADUAPI AccountEditWidget : public AccountConfigurationWidget
{
	Q_OBJECT

public:
	explicit AccountEditWidget(Account account, QWidget *parent = nullptr);
	virtual ~AccountEditWidget();

	virtual const ConfigurationValueStateNotifier * stateNotifier() const;

	QList<AccountConfigurationWidget *> accountConfigurationWidgets() const;

public slots:
	virtual void apply() = 0;
	virtual void cancel() = 0;

signals:
	void widgetAdded(AccountConfigurationWidget *widget);
	void widgetRemoved(AccountConfigurationWidget *widget);

protected:
	AccountConfigurationWidgetFactoryRepository * accountConfigurationWidgetFactoryRepository() const;

	void applyAccountConfigurationWidgets();
	void cancelAccountConfigurationWidgets();

	SimpleConfigurationValueStateNotifier * simpleStateNotifier() const;

private:
	QPointer<AccountConfigurationWidgetFactoryRepository> m_accountConfigurationWidgetFactoryRepository;

	QMap<AccountConfigurationWidgetFactory *, AccountConfigurationWidget *> AccountConfigurationWidgets;
	SimpleConfigurationValueStateNotifier *StateNotifier;
	CompositeConfigurationValueStateNotifier *CompositeStateNotifier;

private slots:
	INJEQT_SET void setAccountConfigurationWidgetFactoryRepository(AccountConfigurationWidgetFactoryRepository *accountConfigurationWidgetFactoryRepository);
	INJEQT_INIT void init();

	void factoryRegistered(AccountConfigurationWidgetFactory *factory);
	void factoryUnregistered(AccountConfigurationWidgetFactory *factory);
	virtual void removeAccount() = 0;

};
