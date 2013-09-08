/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QThread>

extern "C" {
#   include <libotr/privkey.h>
}

#include "accounts/account.h"

#include "otr-create-private-key-worker.h"
#include "otr-user-state.h"

#include "otr-create-private-key-job.h"

OtrCreatePrivateKeyJob::OtrCreatePrivateKeyJob(QObject *parent) :
		QObject(parent), KeyPointer(0)
{
}

OtrCreatePrivateKeyJob::~OtrCreatePrivateKeyJob()
{
	if (CreationThread && KeyPointer)
	{
		otrl_privkey_generate_cancelled(UserState->userState(), KeyPointer);
		CreationThread.data()->wait();
		KeyPointer = 0;
	}
}

void OtrCreatePrivateKeyJob::setUserState(OtrUserState *userState)
{
	UserState = userState;
}

void OtrCreatePrivateKeyJob::setPrivateStoreFileName(const QString &privateStoreFileName)
{
	PrivateStoreFileName = privateStoreFileName;
}

void OtrCreatePrivateKeyJob::createPrivateKey(const Account &account)
{
	if (!UserState || PrivateStoreFileName.isEmpty() || CreationThread || KeyPointer)
	{
		emit finished(false);
		deleteLater();
		return;
	}

	gcry_error_t err = otrl_privkey_generate_start(UserState->userState(), account.id().toUtf8().data(),
												   account.protocolName().toUtf8().data(), &KeyPointer);
	if (err)
	{
		emit finished(false);
		deleteLater();
		return;
	}

	CreationThread = new QThread();
	OtrCreatePrivateKeyWorker *worker = new OtrCreatePrivateKeyWorker(KeyPointer);
	worker->moveToThread(CreationThread.data());

	connect(CreationThread.data(), SIGNAL(started()), worker, SLOT(start()));
	connect(CreationThread.data(), SIGNAL(finished()), CreationThread.data(), SLOT(deleteLater()));
	connect(worker, SIGNAL(finished(bool)), this, SLOT(workerFinished(bool)));
	connect(worker, SIGNAL(finished(bool)), worker, SLOT(deleteLater()));

	CreationThread.data()->start();
}

void OtrCreatePrivateKeyJob::workerFinished(bool ok)
{
	if (CreationThread)
		CreationThread.data()->quit();

	if (!ok)
	{
		emit finished(false);
		deleteLater();
		return;
	}

	Q_ASSERT(KeyPointer);

	gcry_error_t err = otrl_privkey_generate_finish(UserState->userState(), KeyPointer, PrivateStoreFileName.toUtf8().data());
	KeyPointer = 0;

	emit finished(0 == err);
	deleteLater();
}
