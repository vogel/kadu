/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gui/widgets/config-wizard-page.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class QRadioButton;

class InjectedFactory;
class ProtocolFactory;
class ProtocolsComboBox;

class ConfigWizardChooseNetworkPage : public ConfigWizardPage
{
	Q_OBJECT

public:
	explicit ConfigWizardChooseNetworkPage(QWidget *parent = nullptr);
	virtual ~ConfigWizardChooseNetworkPage();

public slots:
	void protocolChanged();

private:
	QPointer<InjectedFactory> m_injectedFactory;

	ProtocolFactory *LastProtocol;
	ProtocolsComboBox *SelectProtocol;

	QRadioButton *SetUpExisting;
	QRadioButton *SetUpNew;
	QRadioButton *Ignore;

	void createGui();

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

};
