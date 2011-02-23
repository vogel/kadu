/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef IRIS_STATUS_ADAPTER_H
#define IRIS_STATUS_ADAPTER_H

namespace XMPP
{
	class Status;
}

class Status;

namespace IrisStatusAdapter
{
	Status fromIrisStatus(XMPP::Status status);
	XMPP::Status toIrisStatus(Status status);

	bool statusesEqual(Status status1, Status status2);
};

#endif // IRIS_STATUS_ADAPTER_H
