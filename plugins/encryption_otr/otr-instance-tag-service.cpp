/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "core/application.h"
#include "misc/paths-provider.h"

#include "otr-op-data.h"
#include "otr-path-service.h"
#include "otr-user-state-service.h"

#include "otr-instance-tag-service.h"

void OtrInstanceTagService::wrapperOtrCreateInstanceTag(void *data, const char *accountName, const char *protocol)
{
	Q_UNUSED(accountName);
	Q_UNUSED(protocol);

	OtrOpData *opData = static_cast<OtrOpData *>(data);
	if (opData->instanceTagService())
		opData->instanceTagService()->createInstanceTag(opData->contact().contactAccount());
}

OtrInstanceTagService::OtrInstanceTagService()
{
}

OtrInstanceTagService::~OtrInstanceTagService()
{
}

void OtrInstanceTagService::setPathService(OtrPathService *pathService)
{
	PathService = pathService;
}

void OtrInstanceTagService::setUserStateService(OtrUserStateService *userStateService)
{
	UserStateService = userStateService;
}

void OtrInstanceTagService::readInstanceTags()
{
	if (!UserStateService)
		return;

	QString fileName = PathService->instanceTagsStoreFilePath();
	otrl_instag_read(UserStateService.data()->userState(), fileName.toUtf8().data());
}

void OtrInstanceTagService::writeInstanceTags()
{
	if (!UserStateService)
		return;

	QString fileName = PathService->instanceTagsStoreFilePath();
	otrl_instag_write(UserStateService.data()->userState(), fileName.toUtf8().data());
}

void OtrInstanceTagService::createInstanceTag(const Account &account)
{
	if (!UserStateService)
		return;

	QString fileName = PathService->instanceTagsStoreFilePath();
	otrl_instag_generate(UserStateService.data()->userState(), fileName.toUtf8().data(),
						 account.id().toUtf8().data(), account.protocolName().toUtf8().data());
	writeInstanceTags();
}
