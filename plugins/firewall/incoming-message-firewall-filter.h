/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef INCOMING_MESSAGE_FIREWALL_FILTER_H
#define INCOMING_MESSAGE_FIREWALL_FILTER_H

#include "message/message-filter.h"

class Firewall;

class IncomingMessageFirewallFilter : public MessageFilter
{
	Q_OBJECT

	Firewall *CurrentFirewall;

public:
	explicit IncomingMessageFirewallFilter(Firewall *firewall);
	virtual ~IncomingMessageFirewallFilter();

	virtual bool acceptMessage(const Message &message);

};

#endif // INCOMING_MESSAGE_FIREWALL_FILTER_H
