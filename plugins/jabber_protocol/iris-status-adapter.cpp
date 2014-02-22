/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "status/status.h"

#include "iris/xmpp_status.h"

#include "iris-status-adapter.h"

namespace IrisStatusAdapter
{
	Status fromIrisStatus(const XMPP::Status &status)
	{
		Status newstatus;
		if (status.isAvailable())
			newstatus.setType(StatusTypeOnline);
		else if (status.isInvisible())
			newstatus.setType(StatusTypeDoNotDisturb);
		else
			newstatus.setType(StatusTypeOffline);

		if (status.show() == "away")
			newstatus.setType(StatusTypeAway);
		else if (status.show() == "xa")
			newstatus.setType(StatusTypeNotAvailable);
		else if (status.show() == "dnd")
			newstatus.setType(StatusTypeDoNotDisturb);
		else if (status.show() == "chat")
			newstatus.setType(StatusTypeFreeForChat);

		QString description = status.status();
		description.replace("\r\n", "\n");
		description.replace('\r', '\n');
		newstatus.setDescription(description);

		return newstatus;
	}

	XMPP::Status toIrisStatus(Status status)
	{
		XMPP::Status s = XMPP::Status();
		StatusType type = status.type();

		if (StatusTypeOnline == type)
			s.setType(XMPP::Status::Online);
		else if (StatusTypeFreeForChat == type)
			s.setType(XMPP::Status::FFC);
		else if (StatusTypeDoNotDisturb == type)
			s.setType(XMPP::Status::DND);
		else if (StatusTypeNotAvailable == type)
			s.setType(XMPP::Status::XA);
		else if (StatusTypeAway == type)
			s.setType(XMPP::Status::Away);
		else if (StatusTypeInvisible == type)
			s.setType(XMPP::Status::DND);
		else
			s.setType(XMPP::Status::Offline);

		s.setStatus(status.description());
		return s;
	}

	bool statusesEqual(Status status1, Status status2)
	{
		if (status1.description() != status2.description())
			return false;

	  	if (status1.type() == status2.type())
			return true;

		if (status1.type() == StatusTypeInvisible && status2.type() == StatusTypeDoNotDisturb)
			return true;

		if (status1.type() == StatusTypeDoNotDisturb && status2.type() == StatusTypeInvisible)
			return true;

		return false;
	}
}
