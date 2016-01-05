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
	INJEQT_DONE void done();
	INJEQT_SET void setClipboardHtmlTransformerService(ClipboardHtmlTransformerService *clipboardHtmlTransformerService);
	INJEQT_SET void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SET void setDomProcessorService(DomProcessorService *domProcessorService);
	INJEQT_SET void setEmoticonClipboardHtmlTransformer(EmoticonClipboardHtmlTransformer *emoticonClipboardHtmlTransformer);
	INJEQT_SET void setEmoticonConfigurator(EmoticonConfigurator *emoticonConfigurator);
	INJEQT_SET void setEmoticonExpanderDomVisitorProvider(EmoticonExpanderDomVisitorProvider *emoticonExpanderDomVisitorProvider);
	INJEQT_SET void setEmoticonsConfigurationUiHandler(EmoticonsConfigurationUiHandler *emoticonsConfigurationUiHandler);
	INJEQT_SET void setInsertEmoticonAction(InsertEmoticonAction *insertEmoticonAction);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);

};
