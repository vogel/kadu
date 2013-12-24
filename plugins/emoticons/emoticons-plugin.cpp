/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/core.h"
#include "dom/dom-processor-service.h"
#include "gui/services/clipboard-html-transformer-service.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/kadu-paths.h"

#include "configuration/emoticon-configurator.h"
#include "expander/emoticon-expander-dom-visitor-provider.h"
#include "gui/emoticon-clipboard-html-transformer.h"
#include "gui/emoticons-configuration-ui-handler.h"
#include "gui/insert-emoticon-action.h"
#include "theme/emoticon-theme-manager.h"

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

void EmoticonsPlugin::registerEmoticonClipboardHtmlTransformer()
{
	ClipboardTransformer.reset(new EmoticonClipboardHtmlTransformer());
	Core::instance()->clipboardHtmlTransformerService()->registerTransformer(ClipboardTransformer.data());
}

void EmoticonsPlugin::unregisterEmoticonClipboardHtmlTransformer()
{
	Core::instance()->clipboardHtmlTransformerService()->unregisterTransformer(ClipboardTransformer.data());
	ClipboardTransformer.reset();
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

bool EmoticonsPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	registerConfigurationUi();
	registerEmoticonExpander();
	registerEmoticonClipboardHtmlTransformer();
	registerActions();
	startConfigurator();

	return true;
}

void EmoticonsPlugin::done()
{
	stopConfigurator();
	unregisterActions();
	unregisterEmoticonExpander();
	unregisterEmoticonClipboardHtmlTransformer();
	unregisterConfigurationUi();
}

Q_EXPORT_PLUGIN2(emoticons, EmoticonsPlugin)

#include "moc_emoticons-plugin.cpp"
