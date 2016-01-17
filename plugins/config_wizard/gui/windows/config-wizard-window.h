/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtWidgets/QWizard>
#include <injeqt/injeqt.h>

class ConfigWizardPage;
class InjectedFactory;
class ProtocolsManager;

/**
 * @defgroup config_wizard Config wizard
 * @{
 */
class ConfigWizardWindow : public QWizard
{
	Q_OBJECT

	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<ProtocolsManager> m_protocolsManager;

	QList<ConfigWizardPage *> ConfigWizardPages;

	void setPage(int id, ConfigWizardPage *page);
	bool goToChooseNetwork() const;
	bool goToAccountSetUp() const;

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setProtocolsManager(ProtocolsManager *protocolsManager);
	INJEQT_INIT void init();

	void acceptedSlot();
	void rejectedSlot();

protected:
	virtual int nextId() const;

public:
	enum Pages
	{
		ProfilePage,
		ChooseNetworkPage,
		SetUpAccountPage,
		CompletedPage
	};

	explicit ConfigWizardWindow(QWidget *parent = 0);
	virtual ~ConfigWizardWindow();

};

/** @} */
