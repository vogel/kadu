/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
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

#include "protocols/protocol-factory.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class GaduListHelper;
class GaduServersManager;
class PluginInjectedFactory;

class GaduProtocolFactory : public ProtocolFactory
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit GaduProtocolFactory(QObject *parent = nullptr);
	virtual ~GaduProtocolFactory();

	virtual Protocol * createProtocolHandler(Account account) override;
	virtual AccountAddWidget * newAddAccountWidget(bool showButtons, QWidget *parent) override;
	virtual AccountCreateWidget * newCreateAccountWidget(bool showButtons, QWidget *parent) override;
	virtual AccountEditWidget * newEditAccountWidget(Account, QWidget *parent) override;
	virtual QWidget * newContactPersonalInfoWidget(Contact contact, QWidget *parent = nullptr) override;
	virtual QList<StatusType> supportedStatusTypes() override;
	virtual Status adaptStatus(Status) const override;
	virtual QString idLabel() override;
	virtual QValidator::State validateId(QString id) override;
	virtual bool canRegister() override;
	virtual bool canRemoveAvatar() override { return false; } // this is so lame for gadu-gadu, so so lame ...

	virtual QString name() override { return "gadu"; }
	virtual QString displayName() override { return "Gadu-Gadu"; }
	virtual KaduIcon icon() override;

private:
	QPointer<GaduListHelper> m_gaduListHelper;
	QPointer<GaduServersManager> m_gaduServersManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;

	QList<StatusType> SupportedStatusTypes;

private slots:
	INJEQT_SET void setGaduListHelper(GaduListHelper *gaduListHelper);
	INJEQT_SET void setGaduServersManager(GaduServersManager *gaduServersManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);

};
