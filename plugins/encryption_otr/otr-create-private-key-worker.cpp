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

extern "C" {
#   include <libotr/privkey.h>
}

#include "otr-create-private-key-worker.h"

OtrCreatePrivateKeyWorker::OtrCreatePrivateKeyWorker(void *keyPointer, QObject *parent) :
		QObject(parent), KeyPointer(keyPointer)
{
}

OtrCreatePrivateKeyWorker::~OtrCreatePrivateKeyWorker()
{
}

void OtrCreatePrivateKeyWorker::start()
{
	gcry_error_t err = otrl_privkey_generate_calculate(KeyPointer);
	emit finished(0 == err);
}
