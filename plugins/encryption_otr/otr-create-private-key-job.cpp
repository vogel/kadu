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

#include <QtCore/QThread>

extern "C" {
#   include <libotr/privkey.h>
}

#include "accounts/account.h"

#include "otr-create-private-key-worker.h"
#include "otr-user-state-service.h"

#include "otr-create-private-key-job.h"

OtrCreatePrivateKeyJob::OtrCreatePrivateKeyJob(QObject *parent) :
		QObject(parent), KeyPointer(0)
{
}

OtrCreatePrivateKeyJob::~OtrCreatePrivateKeyJob()
{
	if (!CreationThread || !KeyPointer)
		return;

	if (UserStateService)
		otrl_privkey_generate_cancelled(UserStateService->userState(), KeyPointer);
	CreationThread->wait();
	KeyPointer = 0;
}

void OtrCreatePrivateKeyJob::setUserStateService(OtrUserStateService *userStateService)
{
	UserStateService = userStateService;
}

void OtrCreatePrivateKeyJob::setAccount(const Account &account)
{
	MyAccount = account;
}

void OtrCreatePrivateKeyJob::setPrivateStoreFileName(const QString &privateStoreFileName)
{
	PrivateStoreFileName = privateStoreFileName;
}

void OtrCreatePrivateKeyJob::createPrivateKey()
{
	if (!MyAccount || !UserStateService || PrivateStoreFileName.isEmpty() || CreationThread || KeyPointer)
	{
		emit finished(MyAccount, false);
		return;
	}

	gcry_error_t err = otrl_privkey_generate_start(UserStateService->userState(), MyAccount.id().toUtf8().data(),
												   MyAccount.protocolName().toUtf8().data(), &KeyPointer);
	if (err)
	{
		emit finished(MyAccount, false);
		return;
	}

	CreationThread = new QThread();
	OtrCreatePrivateKeyWorker *worker = new OtrCreatePrivateKeyWorker(KeyPointer);
	worker->moveToThread(CreationThread.data());

	connect(CreationThread.data(), SIGNAL(started()), worker, SLOT(start()));
	connect(CreationThread.data(), SIGNAL(finished()), CreationThread.data(), SLOT(deleteLater()));
	connect(worker, SIGNAL(finished(bool)), this, SLOT(workerFinished(bool)));
	connect(worker, SIGNAL(finished(bool)), worker, SLOT(deleteLater()));

	CreationThread->start();
}

void OtrCreatePrivateKeyJob::workerFinished(bool ok)
{
	if (CreationThread)
		CreationThread->quit();

	if (!ok)
	{
		emit finished(MyAccount, false);
		return;
	}

	Q_ASSERT(KeyPointer);

	gcry_error_t err = otrl_privkey_generate_finish(UserStateService->userState(), KeyPointer, PrivateStoreFileName.toUtf8().data());
	KeyPointer = 0;

	emit finished(MyAccount, 0 == err);
}
