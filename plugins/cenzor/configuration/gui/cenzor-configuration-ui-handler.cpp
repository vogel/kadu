/*
 * %kadu copyright begin%
 * Copyright 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/kadu-paths.h"

#include "configuration/cenzor-configuration.h"
#include "gui/widgets/list-edit-widget.h"
#include "cenzor.h"

#include "cenzor-configuration-ui-handler.h"

CenzorConfigurationUiHandler * CenzorConfigurationUiHandler::Instance = 0;

void CenzorConfigurationUiHandler::registerConfigurationUi()
{
	if (!Instance)
	{
		Instance = new CenzorConfigurationUiHandler();
		MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/cenzor.ui"));
		MainConfigurationWindow::registerUiHandler(Instance);
	}
}

void CenzorConfigurationUiHandler::unregisterConfigurationUi()
{
	if (Instance)
	{
		MainConfigurationWindow::unregisterUiHandler(Instance);
		MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/cenzor.ui"));
		delete Instance;
		Instance = 0;
	}
}

CenzorConfigurationUiHandler::CenzorConfigurationUiHandler() :
		SwearwordsWidget{},
		ExclusionsWidget{}
{
}

CenzorConfigurationUiHandler::~CenzorConfigurationUiHandler()
{
}

void CenzorConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));

	ConfigGroupBox *swearwordGroupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "Cenzor", "Swearwords");
	SwearwordsWidget = new ListEditWidget(swearwordGroupBox->widget());
	swearwordGroupBox->addWidgets(0, SwearwordsWidget);

	ConfigGroupBox *exclusionsGroupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "Cenzor", "Exclusions");
	ExclusionsWidget = new ListEditWidget(exclusionsGroupBox->widget());
	exclusionsGroupBox->addWidgets(0, ExclusionsWidget);

	SwearwordsWidget->setList(CenzorConfiguration::toStringList(Cenzor::instance()->configuration().swearList()));
	ExclusionsWidget->setList(CenzorConfiguration::toStringList(Cenzor::instance()->configuration().exclusionList()));
}

void CenzorConfigurationUiHandler::configurationWindowApplied()
{
	Cenzor::instance()->configuration().setSwearList(CenzorConfiguration::toRegExpList(SwearwordsWidget->list()));
	Cenzor::instance()->configuration().setExclusionList(CenzorConfiguration::toRegExpList(ExclusionsWidget->list()));
	Cenzor::instance()->configuration().saveConfiguration();
}

#include "moc_cenzor-configuration-ui-handler.cpp"
