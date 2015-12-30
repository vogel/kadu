/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

#include "dom/dom-visitor-provider.h"

class IgnoreLinksDomVisitor;

class GaduUrlDomVisitorProvider : public QObject, public DomVisitorProvider
{
	Q_OBJECT

	QScopedPointer<IgnoreLinksDomVisitor> IgnoreLinks;

public:
	Q_INVOKABLE explicit GaduUrlDomVisitorProvider(QObject *parent = nullptr);
	virtual ~GaduUrlDomVisitorProvider();

	virtual DomVisitor * provide() const;

};
