/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef CONTACT_NO_UNLOADED_ACCOUNT_FILTER_H
#define CONTACT_NO_UNLOADED_ACCOUNT_FILTER_H

#include "contacts/filter/abstract-contact-filter.h"

class ContactNoUnloadedAccountFilter : public AbstractContactFilter
{
	Q_OBJECT

	bool Enabled;

public:
	explicit ContactNoUnloadedAccountFilter(QObject *parent = 0) :
			AbstractContactFilter(parent), Enabled(false) {}
	virtual ~ContactNoUnloadedAccountFilter() {}

	virtual bool acceptContact(const Contact &contact);

	void setEnabled(bool enabled);

};

#endif // CONTACT_NO_UNLOADED_ACCOUNT_FILTER_H
