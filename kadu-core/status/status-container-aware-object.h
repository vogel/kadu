/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef STATUS_CONTAINER_AWARE_OBJECT
#define STATUS_CONTAINER_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class StatusContainer;

/**
 * @addtogroup Status
 * @{
 */

/**
 * @class StatusContainerAwareObject
 * @author Rafał 'Vogel' Malinowski
 * @short Class used for receiving notifications about registered and unregistered status containers.
 * @see StatusContainer
 * @see StatusContainerManager
 *
 * All objects of classes dervered from StatusContainerAwareObject are notified about status containers registered and
 * unregistered in StatusContainerManager.
 */
class KADUAPI StatusContainerAwareObject : public AwareObject<StatusContainerAwareObject>
{

protected:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called when new status container is registered in StatusContainerManager singleton.
	 * @param statusContainer registered status container
	 *
	 * This method is called every time new status container is registered in StatusContainerManager singleton.
	 * It can also be called for every registered status container by triggerAllStatusContainerRegistered method.
	 */
	virtual void statusContainerRegistered(StatusContainer *statusContainer) = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Method called when status container is unregistered from StatusContainerManager singleton.
	 * @param statusContainer unregistered status container
	 *
	 * This method is called every time a status container is unregistered in StatusContainerManager singleton.
	 * It can also be called for every registered status container by triggerAllStatusContainerUnregistered method.
	 */
	virtual void statusContainerUnregistered(StatusContainer *statusContainer) = 0;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Call this method to call statusContainerRegistered for every StatusContainerAwareObject instance.
	 * @param statusContainer registered status container
	 *
	 * Calling this method calls statusContainerRegistered for every StatusContainerAwareObject instance.
	 */
	static void notifyStatusContainerRegistered(StatusContainer *statusContainer);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Call this method to call statusContainerUnregistered for every StatusContainerAwareObject instance.
	 * @param statusContainer runregistered status container
	 *
	 * Calling this method calls statusContainerUnregistered for every StatusContainerAwareObject instance.
	 */
	static void notifyStatusContainerUnregistered(StatusContainer *statusContainer);

	StatusContainerAwareObject() {}
	virtual ~StatusContainerAwareObject() {}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Call this method on instance of StatusContainerAwareObject to call statusContainerRegistered for every
	 * registered status contaner.
	 * @param statusContainer registered status container
	 *
	 * Call this method on instance of StatusContainerAwareObject to call statusContainerRegistered for every
	 * registered status contaner.
	 */
	void triggerAllStatusContainerRegistered();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Call this method on instance of StatusContainerAwareObject to call statusContainerUnregistered for every
	 * registered status contaner.
	 * @param statusContainer registered status container
	 *
	 * Call this method on instance of StatusContainerAwareObject to call statusContainerUnregistered for every
	 * registered status contaner.
	 */
	void triggerAllStatusContainerUnregistered();

};

/**
 * @}
 */

#endif // STATUS_CONTAINER_AWARE_OBJECT
