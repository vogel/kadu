/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "protocols/protocol-factory.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class PluginInjectedFactory;

class FacebookProtocolFactory : public ProtocolFactory
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit FacebookProtocolFactory(QObject *parent = nullptr);
	virtual ~FacebookProtocolFactory();

	virtual Protocol * createProtocolHandler(Account account) override;
	virtual AccountAddWidget * newAddAccountWidget(bool showButtons, QWidget *parent) override;
	virtual AccountEditWidget * newEditAccountWidget(Account, QWidget *parent) override;
	virtual QList<StatusType> supportedStatusTypes() override;
	virtual Status adaptStatus(Status) const override;
	virtual QString idLabel() override;
	virtual bool canRegister() override;

	virtual QString name() override { return "facebook"; }
	virtual QString displayName() override { return "Facebook"; }

	virtual KaduIcon icon() override;

private:
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;

	QList<StatusType> m_supportedStatusTypes;

private slots:
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);

};
