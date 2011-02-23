/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_MULTILOGON_SESSION_H
#define GADU_MULTILOGON_SESSION_H

#include "multilogon/multilogon-session.h"

#include "gadu-features.h"

class GaduMultilogonSession : public MultilogonSession
{
	Q_OBJECT

#ifdef GADU_HAVE_MULTILOGON
	gg_multilogon_id_t Id;
#endif // GADU_HAVE_MULTILOGON

public:
	explicit GaduMultilogonSession(Account account, QObject *parent = 0);
	virtual ~GaduMultilogonSession();

#ifdef GADU_HAVE_MULTILOGON
	GaduMultilogonSession(Account account, const gg_multilogon_session &session, QObject *parent = 0);

	const gg_multilogon_id_t & id() const;
#endif // GADU_HAVE_MULTILOGON

};

#endif // GADU_MULTILOGON_SESSION_H
