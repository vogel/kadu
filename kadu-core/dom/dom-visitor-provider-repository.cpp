/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "dom-visitor-provider-repository.h"

#include "dom/dom-processor.h"
#include "dom/dom-visitor-provider.h"
#include "dom/dom-visitor.h"

#include <QtXml/QDomDocument>

DomVisitorProviderRepository::DomVisitorProviderRepository(QObject *parent) :
		QObject(parent), VisitorProvidersDirty(false)
{
}

DomVisitorProviderRepository::~DomVisitorProviderRepository()
{
}

QList<DomVisitorProvider *> DomVisitorProviderRepository::getVisitorProviders()
{
	if (!VisitorProvidersDirty)
		return VisitorProviders;

	VisitorProviders.clear();

	QMultiMap<int, DomVisitorProvider *> inverted;
	foreach (DomVisitorProvider *visitorProvider, Priorities.keys())
		inverted.insert(Priorities.value(visitorProvider), visitorProvider);

	foreach (int priority, inverted.keys())
		VisitorProviders.append(inverted.values(priority));

	return VisitorProviders;
}

void DomVisitorProviderRepository::addVisitorProvider(DomVisitorProvider *visitorProvider, int priority)
{
	if (Priorities.contains(visitorProvider))
		return;

	Priorities.insert(visitorProvider, priority);
	VisitorProvidersDirty = true;
}

void DomVisitorProviderRepository::removeVisitorProvider(DomVisitorProvider *visitorProvider)
{
	if (0 < Priorities.remove(visitorProvider))
		VisitorProvidersDirty = true;
}

#include "dom-visitor-provider-repository.moc"
