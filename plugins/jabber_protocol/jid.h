/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QString>

class Jid
{

public:
	static Jid parse(QString jid);

	Jid();
	explicit Jid(QString node, QString domain, QString resource);

	Jid withNode(QString node) const;
	Jid withDomain(QString domain) const;
	Jid withResource(QString resource) const;

	bool isEmpty() const;

	QString full() const;
	QString bare() const;
	QString node() const;
	QString domain() const;
	QString resource() const;

private:
	QString m_full;
	QString m_bare;
	QString m_node;
	QString m_domain;
	QString m_resource;

};
