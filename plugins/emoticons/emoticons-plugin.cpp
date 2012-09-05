/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core/core.h"
#include "dom/dom-processor-service.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/kadu-paths.h"

#include "configuration/emoticon-configurator.h"
#include "expander/emoticon-expander-dom-visitor-provider.h"
#include "theme/emoticon-theme-manager.h"
#include "gui/emoticons-configuration-ui-handler.h"
#include "gui/insert-emoticon-action.h"

#include "emoticons-plugin.h"

EmoticonsPlugin::EmoticonsPlugin(QObject *parent) :
		QObject(parent)
{
}

EmoticonsPlugin::~EmoticonsPlugin()
{
}

void EmoticonsPlugin::registerConfigurationUi()
{
	ConfigurationUiHandler.reset(new EmoticonsConfigurationUiHandler(this));

	MainConfigurationWindow::registerUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/emoticons.ui"));
	MainConfigurationWindow::registerUiHandler(ConfigurationUiHandler.data());
}

void EmoticonsPlugin::unregisterConfigurationUi()
{
	MainConfigurationWindow::unregisterUiHandler(ConfigurationUiHandler.data());
	MainConfigurationWindow::unregisterUiFile(KaduPaths::instance()->dataPath() + QLatin1String("plugins/configuration/emoticons.ui"));

	ConfigurationUiHandler.reset();
}

void EmoticonsPlugin::registerEmoticonExpander()
{
	ExpanderDomVisitorProvider.reset(new EmoticonExpanderDomVisitorProvider());
	Core::instance()->domProcessorService()->registerVisitorProvider(ExpanderDomVisitorProvider.data(), 2000);
}

void EmoticonsPlugin::unregisterEmoticonExpander()
{
	Core::instance()->domProcessorService()->unregisterVisitorProvider(ExpanderDomVisitorProvider.data());
	ExpanderDomVisitorProvider.reset();
}

void EmoticonsPlugin::registerActions()
{
	InsertAction.reset(new InsertEmoticonAction(this));
}

void EmoticonsPlugin::unregisterActions()
{
	InsertAction.reset();
}

void EmoticonsPlugin::startConfigurator()
{
	Configurator.reset(new EmoticonConfigurator());
	Configurator->setEmoticonExpanderProvider(ExpanderDomVisitorProvider.data());
	Configurator->setInsertAction(InsertAction.data());
	Configurator->configure();
}

void EmoticonsPlugin::stopConfigurator()
{
	Configurator.reset();
}

int EmoticonsPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	registerConfigurationUi();
	registerEmoticonExpander();
	registerActions();
	startConfigurator();

	return 0;
}

void EmoticonsPlugin::done()
{
	stopConfigurator();
	unregisterActions();
	unregisterEmoticonExpander();
	unregisterConfigurationUi();
}

Q_EXPORT_PLUGIN2(emoticons, EmoticonsPlugin)
