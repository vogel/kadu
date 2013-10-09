/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef DOM_PROCESSOR_SERVICE_H
#define DOM_PROCESSOR_SERVICE_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include "exports.h"

class QDomDocument;

class DomVisitorProvider;

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class DomProcessorService
 * @short Service for processing DOM documents.
 * @author Rafał 'Vogel' Malinowski
 *
 * Use this service to process DOM documents that are about to be displayed - like messages.
 * Plugins like emoticons or imagelink can register it this class to allow for processing emoticons, urls and other
 * stuff to make messages more pleasant for users.
 *
 * DomVisitorProvider objects are asked for DomVisitor object in order of increasing priorities - that means
 * DomVisitorProvider with smallest priority will be called first.
 */
class KADUAPI DomProcessorService: public QObject
{
	Q_OBJECT

	QMap<DomVisitorProvider *, int> Priorities;
	QList<DomVisitorProvider *> VisitorProviders;
	bool VisitorProvidersDirty;

	QList<DomVisitorProvider *> getVisitorProviders();

public:
	/**
	 * @short Create new instance of DomProcessorService.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent of new instance
	 */
	explicit DomProcessorService(QObject *parent = 0);
	virtual ~DomProcessorService();

	/**
	 * @short Register new DomVisitorProvider with given priority.
	 * @author Rafał 'Vogel' Malinowski
	 * @param visitorProvider visitor provider to register
	 * @param priority priority of new provider, smaller priorities are called first
	 *
	 * If given DomVisitorProvider is already registered, nothing will happen.
	 */
	void registerVisitorProvider(DomVisitorProvider *visitorProvider, int priority);

	/**
	 * @short Unegister new DomVisitorProvider with given priority.
	 * @author Rafał 'Vogel' Malinowski
	 * @param visitorProvider visitor provider to unregister
	 *
	 * If given DomVisitorProvider is not registered, nothing will happen.
	 */
	void unregisterVisitorProvider(DomVisitorProvider *visitorProvider);

	/**
	 * @short Process domDocument with all available DomVisitor instances.
	 * @author Rafał 'Vogel' Malinowski
	 * @param domDocument to process
	 *
	 * This method will ask all registered DomVisitorProvider for DomVisitor instances. All non-null
	 * instances will be called in order of increasing priority on given domDocument and will be allowed to
	 * modify it.
	 */
	void process(QDomDocument &domDocument);

	/**
	 * @short Process xml with all available DomVisitor instances.
	 * @author Rafał 'Vogel' Malinowski
	 * @param domDocument to process
	 * @todo remove
	 *
	 * This method will convert xml string to domDocument and call other process() method. Result of that
	 * will be again serialized to string and returned.
	 *
	 * Value of xml must be valid XML. If not, it will be truncated at first invalid character.
	 */
	QString process(const QString &xml);

};

/**
 * @}
 */

#endif // DOM_PROCESSOR_SERVICE_H
