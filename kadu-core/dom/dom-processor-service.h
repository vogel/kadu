/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "exports.h"

class QDomDocument;

class DomVisitorProviderRepository;
class DomVisitor;

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
class KADUAPI DomProcessorService : public QObject
{
	Q_OBJECT

public:
	/**
	 * @short Create new instance of DomProcessorService.
	 * @author Rafał 'Vogel' Malinowski
	 * @param parent QObject parent of new instance
	 */
	explicit DomProcessorService(QObject *parent = nullptr);
	virtual ~DomProcessorService();

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
	virtual QString process(const QString &xml) = 0;
	virtual QString process(const QString &xml, const DomVisitor &domVisitor) = 0;

};

/**
 * @}
 */
