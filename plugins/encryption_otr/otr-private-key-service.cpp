/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

extern "C" {
#   include <libotr/privkey.h>
}

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "misc/kadu-paths.h"

#include "otr-create-private-key-job.h"
#include "otr-op-data.h"
#include "otr-path-service.h"
#include "otr-user-state-service.h"

#include "otr-private-key-service.h"

void OtrPrivateKeyService::wrapperOtrCreatePrivateKey(void *data, const char *accountName, const char *protocol)
{
	Q_UNUSED(accountName);
	Q_UNUSED(protocol);

	OtrOpData *opData = static_cast<OtrOpData *>(data);
	if (opData->privateKeyService())
	{
		Account account = opData->contact().contactAccount();
		opData->privateKeyService()->createPrivateKey(account);
	}
}

OtrPrivateKeyService::OtrPrivateKeyService(QObject *parent) :
		QObject(parent)
{
}

OtrPrivateKeyService::~OtrPrivateKeyService()
{
	qDeleteAll(CreateJobs);
}

void OtrPrivateKeyService::setPathService(OtrPathService *pathService)
{
	PathService = pathService;
}

void OtrPrivateKeyService::setUserStateService(OtrUserStateService *userStateService)
{
	UserStateService = userStateService;
}

void OtrPrivateKeyService::createPrivateKey(const Account &account)
{
	if (CreateJobs.contains(account))
		return;

	OtrCreatePrivateKeyJob *job = new OtrCreatePrivateKeyJob(this);
	job->setAccount(account);
	job->setPrivateStoreFileName(PathService->privateKeysStoreFilePath());
	job->setUserStateService(UserStateService.data());
	job->createPrivateKey();

	connect(job, SIGNAL(finished(Account, bool)), this, SLOT(jobFinished(Account, bool)));

	CreateJobs.insert(account, job);
	emit createPrivateKeyStarted(account);
}

void OtrPrivateKeyService::jobFinished(const Account &account, bool ok)
{
	emit createPrivateKeyFinished(account, ok);
	CreateJobs.value(account)->deleteLater();
	CreateJobs.remove(account);
}

void OtrPrivateKeyService::readPrivateKeys()
{
	if (!UserStateService)
		return;

	OtrlUserState userState = UserStateService->userState();
	otrl_privkey_read(userState, PathService->privateKeysStoreFilePath().toUtf8().data());
}
