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

#pragma once

#include "plugin/plugin-object.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ClipboardHtmlTransformerService;
class ConfigurationUiHandlerRepository;
class DomProcessorService;
class EmoticonClipboardHtmlTransformer;
class EmoticonConfigurator;
class EmoticonExpanderDomVisitorProvider;
class EmoticonsConfigurationUiHandler;
class InsertEmoticonAction;
class PathsProvider;

class EmoticonsPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit EmoticonsPluginObject(QObject *parent = nullptr);
	virtual ~EmoticonsPluginObject();

	virtual void done() override;

private:
	QPointer<ClipboardHtmlTransformerService> m_clipboardHtmlTransformerService;
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<DomProcessorService> m_domProcessorService;
	QPointer<EmoticonClipboardHtmlTransformer> m_emoticonClipboardHtmlTransformer;
	QPointer<EmoticonConfigurator> m_emoticonConfigurator;
	QPointer<EmoticonExpanderDomVisitorProvider> m_emoticonExpanderDomVisitorProvider;
	QPointer<EmoticonsConfigurationUiHandler> m_emoticonsConfigurationUiHandler;
	QPointer<InsertEmoticonAction> m_insertEmoticonAction;
	QPointer<PathsProvider> m_pathsProvider;

private slots:
	INJEQT_INIT void init();
	INJEQT_SETTER void setClipboardHtmlTransformerService(ClipboardHtmlTransformerService *clipboardHtmlTransformerService);
	INJEQT_SETTER void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SETTER void setDomProcessorService(DomProcessorService *domProcessorService);
	INJEQT_SETTER void setEmoticonClipboardHtmlTransformer(EmoticonClipboardHtmlTransformer *emoticonClipboardHtmlTransformer);
	INJEQT_SETTER void setEmoticonConfigurator(EmoticonConfigurator *emoticonConfigurator);
	INJEQT_SETTER void setEmoticonExpanderDomVisitorProvider(EmoticonExpanderDomVisitorProvider *emoticonExpanderDomVisitorProvider);
	INJEQT_SETTER void setEmoticonsConfigurationUiHandler(EmoticonsConfigurationUiHandler *emoticonsConfigurationUiHandler);
	INJEQT_SETTER void setInsertEmoticonAction(InsertEmoticonAction *insertEmoticonAction);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);

};
