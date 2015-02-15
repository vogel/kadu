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

#ifndef OTR_POLICY_SERVICE_H
#define OTR_POLICY_SERVICE_H

#include <QtCore/QObject>

extern "C" {
#	include <libotr/proto.h>
}

class Account;
class Buddy;
class Contact;
class OtrPolicy;

class OtrPolicyService : public QObject
{
	Q_OBJECT

public:
	static OtrlPolicy wrapperOtrPolicy(void *data, ConnContext *context);

	Q_INVOKABLE OtrPolicyService();
	virtual ~OtrPolicyService();

	void setAccountPolicy(const Account &account, const OtrPolicy &policy);
	OtrPolicy accountPolicy(const Account &account) const;

	void setBuddyPolicy(const Buddy &buddy, const OtrPolicy &policy);
	OtrPolicy buddyPolicy(const Buddy &buddy) const;

	OtrPolicy contactPolicy(const Contact &contact) const;

};

#endif // OTR_POLICY_SERVICE_H
