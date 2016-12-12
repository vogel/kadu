/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "cenzor-configuration-ui-handler.h"

#include "configuration/cenzor-configuration.h"
#include "gui/widgets/list-edit-widget.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "misc/paths-provider.h"
#include "widgets/configuration/config-group-box.h"
#include "widgets/configuration/configuration-widget.h"
#include "windows/main-configuration-window.h"

CenzorConfigurationUiHandler::CenzorConfigurationUiHandler(QObject *parent) :
		QObject{parent},
		m_swearwordsWidget{},
		m_exclusionsWidget{}
{
}

CenzorConfigurationUiHandler::~CenzorConfigurationUiHandler()
{
}

void CenzorConfigurationUiHandler::setCenzorConfiguration(CenzorConfiguration *cenzorConfiguration)
{
	m_cenzorConfiguration = cenzorConfiguration;
}

void CenzorConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	ConfigGroupBox *swearwordGroupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "Cenzor", "Swearwords");
	m_swearwordsWidget = new ListEditWidget(swearwordGroupBox->widget());
	swearwordGroupBox->addWidgets(0, m_swearwordsWidget);

	ConfigGroupBox *exclusionsGroupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "Cenzor", "Exclusions");
	m_exclusionsWidget = new ListEditWidget(exclusionsGroupBox->widget());
	exclusionsGroupBox->addWidgets(0, m_exclusionsWidget);

	m_swearwordsWidget->setList(CenzorConfiguration::toStringList(m_cenzorConfiguration->swearList()));
	m_exclusionsWidget->setList(CenzorConfiguration::toStringList(m_cenzorConfiguration->exclusionList()));
}

void CenzorConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
}

void CenzorConfigurationUiHandler::mainConfigurationWindowApplied()
{
	m_cenzorConfiguration->setSwearList(CenzorConfiguration::toRegExpList(m_swearwordsWidget->list()));
	m_cenzorConfiguration->setExclusionList(CenzorConfiguration::toRegExpList(m_exclusionsWidget->list()));
	m_cenzorConfiguration->saveConfiguration();
}

#include "moc_cenzor-configuration-ui-handler.cpp"
