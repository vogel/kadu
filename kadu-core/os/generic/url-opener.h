/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef URL_OPENER_H
#define URL_OPENER_H

#include <QtCore/QByteArray>

#include "exports.h"

namespace UrlOpener
{
	bool KADUAPI openUrl(const QByteArray &urlForDesktopServices, const QByteArray &urlForApplication, const QString &application);
	void KADUAPI openUrl(const QByteArray &url);
	void KADUAPI openEmail(const QByteArray &email);
}

#endif // URL_OPENER_H
