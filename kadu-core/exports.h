/*
 * %kadu copyright begin%
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef EXPORTS_H
#define EXPORTS_H

#include <QtCore/QtGlobal>

#ifdef libkadu_EXPORTS
#	define KADUAPI Q_DECL_EXPORT

#	ifdef KADU_EXPORT_TESTS
#		define KADUAPI_TESTS Q_DECL_EXPORT
#	else
#		define KADUAPI_TESTS
#	endif
#else
#	define KADUAPI Q_DECL_IMPORT

#	ifdef KADU_EXPORT_TESTS
#		define KADUAPI_TESTS Q_DECL_IMPORT
#	else
#		define KADUAPI_TESTS
#	endif
#endif

#endif // EXPORTS_H
