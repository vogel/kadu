/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef PROTOCOL_FILTER_H
#define PROTOCOL_FILTER_H

#include "accounts/filter/abstract-account-filter.h"

class ProtocolFilter : public AbstractAccountFilter
{
	Q_OBJECT

	QString ProtocolName;

protected:
	virtual bool acceptAccount(Account account);

public:
	explicit ProtocolFilter(QObject *parent = 0);
	virtual ~ProtocolFilter();

	void setProtocolName(const QString &protocolName);

};

#endif // PROTOCOL_FILTER_H
