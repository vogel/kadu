/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "dom-processor-service-impl.h"

#include "dom/dom-processor.h"
#include "dom/dom-visitor-provider-repository.h"
#include "dom/dom-visitor-provider.h"
#include "dom/dom-visitor.h"

#include <QtXml/QDomDocument>

DomProcessorServiceImpl::DomProcessorServiceImpl(QObject *parent) :
		DomProcessorService{parent}
{
}

DomProcessorServiceImpl::~DomProcessorServiceImpl()
{
}

void DomProcessorServiceImpl::setDomVisitorProviderRepository(DomVisitorProviderRepository *domVisitorProviderRepository)
{
	m_domVisitorProviderRepository = domVisitorProviderRepository;
}

void DomProcessorServiceImpl::process(QDomDocument &domDocument)
{
	auto visitors = std::vector<const DomVisitor *>();
	for (auto visitorProvider : m_domVisitorProviderRepository)
		if (auto visitor = visitorProvider->provide())
			visitors.push_back(visitor);

	auto domProcessor = DomProcessor{domDocument};
	domProcessor.accept(std::begin(visitors), std::end(visitors));
}

QString DomProcessorServiceImpl::process(const QString &xml)
{
	try {
		auto domDocument = toDomDocument(xml);
		process(domDocument);
		return toString(domDocument);
	}
	catch (invalid_xml &)
	{
		return xml;
	}
}

#include "dom-processor-service-impl.moc"
