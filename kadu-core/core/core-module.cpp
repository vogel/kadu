/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "core-module.h"

#include "core/application.h"
#include "core/core.h"
#include "core/injected-factory.h"
#include "core/injector-provider.h"
#include "core/myself.h"
#include "core/scheduler.h"
#include "core/session-service.h"
#include "core/version-service.h"
#include "gui/services/clipboard-html-transformer-service.h"
#include "misc/paths-provider.h"
#include "os/generic/url-opener.h"
#include "protocols/protocols-manager.h"
#include "services/chat-image-request-service.h"
#include "services/image-storage-service-impl.h"
#include "services/raw-message-transformer-service.h"
#include "status/status-changer-manager.h"
#include "status/status-container-manager.h"
#include "storage/storage-point-factory-impl.h"
#include "url-handlers/url-handler-manager.h"
#include "widgets/account-configuration-widget-factory-repository.h"
#include "widgets/chat-configuration-widget-factory-repository.h"
#include "widgets/chat-top-bar-widget-factory-repository.h"
#include "attention-service.h"
#include "languages-manager.h"
#include "translation-loader.h"

CoreModule::CoreModule(QString profileDirectory)
{
	m_pathsProvider = make_not_owned<PathsProvider>(std::move(profileDirectory));

	add_type<AccountConfigurationWidgetFactoryRepository>();
	add_type<Application>();
	add_type<AttentionService>();
	add_type<ChatConfigurationWidgetFactoryRepository>();
	add_type<ChatImageRequestService>();
	add_type<ChatTopBarWidgetFactoryRepository>();
	add_type<ClipboardHtmlTransformerService>();
	add_type<ImageStorageServiceImpl>();
	add_type<InjectedFactory>();
	add_type<InjectorProvider>();
	add_type<LanguagesManager>();
	add_type<Myself>();
	add_ready_object<PathsProvider>(m_pathsProvider.get());
	add_type<ProtocolsManager>();
	add_type<RawMessageTransformerService>();
	add_type<Scheduler>();
	add_type<SessionService>();
	add_type<StatusChangerManager>();
	add_type<StatusContainerManager>();
	add_type<StoragePointFactoryImpl>();
	add_type<TranslationLoader>();
	add_type<UrlHandlerManager>();
	add_type<UrlOpener>();
	add_type<VersionService>();
}

CoreModule::~CoreModule()
{
}
