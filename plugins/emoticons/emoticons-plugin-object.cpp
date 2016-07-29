/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "emoticons-plugin-object.h"

#include "configuration/emoticon-configurator.h"
#include "expander/emoticon-expander-dom-visitor-provider.h"
#include "gui/emoticon-clipboard-html-transformer.h"
#include "gui/emoticons-configuration-ui-handler.h"
#include "gui/insert-emoticon-action.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/actions/actions.h"
#include "dom/dom-visitor-provider-repository.h"
#include "gui/windows/main-configuration-window-service.h"
#include "gui/windows/main-configuration-window.h"
#include "gui/services/clipboard-html-transformer-service.h"
#include "misc/paths-provider.h"

EmoticonsPluginObject::EmoticonsPluginObject(QObject *parent) :
		QObject{parent}
{
}

EmoticonsPluginObject::~EmoticonsPluginObject()
{
}

void EmoticonsPluginObject::setActions(Actions *actions)
{
	m_actions = actions;
}

void EmoticonsPluginObject::setClipboardHtmlTransformerService(ClipboardHtmlTransformerService *clipboardHtmlTransformerService)
{
	m_clipboardHtmlTransformerService = clipboardHtmlTransformerService;
}

void EmoticonsPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void EmoticonsPluginObject::setDomVisitorProviderRepository(DomVisitorProviderRepository *domVisitorProviderRepository)
{
	m_domVisitorProviderRepository = domVisitorProviderRepository;
}

void EmoticonsPluginObject::setEmoticonClipboardHtmlTransformer(EmoticonClipboardHtmlTransformer *emoticonClipboardHtmlTransformer)
{
	m_emoticonClipboardHtmlTransformer = emoticonClipboardHtmlTransformer;
}

void EmoticonsPluginObject::setEmoticonConfigurator(EmoticonConfigurator *emoticonConfigurator)
{
	m_emoticonConfigurator = emoticonConfigurator;
}

void EmoticonsPluginObject::setEmoticonExpanderDomVisitorProvider(EmoticonExpanderDomVisitorProvider *emoticonExpanderDomVisitorProvider)
{
	m_emoticonExpanderDomVisitorProvider = emoticonExpanderDomVisitorProvider;
}

void EmoticonsPluginObject::setEmoticonsConfigurationUiHandler(EmoticonsConfigurationUiHandler *emoticonsConfigurationUiHandler)
{
	m_emoticonsConfigurationUiHandler = emoticonsConfigurationUiHandler;
}

void EmoticonsPluginObject::setInsertEmoticonAction(InsertEmoticonAction *insertEmoticonAction)
{
	m_insertEmoticonAction = insertEmoticonAction;
}

void EmoticonsPluginObject::setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService)
{
	m_mainConfigurationWindowService = mainConfigurationWindowService;
}

void EmoticonsPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void EmoticonsPluginObject::init()
{
	m_actions->insert(m_insertEmoticonAction);
	m_mainConfigurationWindowService->registerUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/emoticons.ui"));
	m_clipboardHtmlTransformerService->registerTransformer(m_emoticonClipboardHtmlTransformer);
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_emoticonsConfigurationUiHandler);
	m_domVisitorProviderRepository->addVisitorProvider(m_emoticonExpanderDomVisitorProvider, 2000);
	m_emoticonConfigurator->configure();
}

void EmoticonsPluginObject::done()
{
	m_domVisitorProviderRepository->removeVisitorProvider(m_emoticonExpanderDomVisitorProvider);
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_emoticonsConfigurationUiHandler);
	m_clipboardHtmlTransformerService->unregisterTransformer(m_emoticonClipboardHtmlTransformer);
	m_mainConfigurationWindowService->unregisterUiFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/configuration/emoticons.ui"));
}

#include "moc_emoticons-plugin-object.cpp"
