/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef DETAILS_HOLDER_H
#define DETAILS_HOLDER_H

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @class DetailsHolder
 * @author Rafal 'Vogel' Malinowski
 * @param Class class type, derivering from @link<DetailsHolder> DetailsHolder @endlink (used for casting)
 * @param DetailsClass class that will be holded by this one
 * @param ManagerClass singleton class that will be informed about details loading/unloading from this holder
 * @short Object that holds extra data and informs ManagerClass singleton about loading/unloading this extra data.
 *
 * Some object (@link<Account> Account @endlink, @link<Contact> Contact @endlink and others)
 * have common data that should be always available and protocol data that can only be loaded
 * when protocol plugin is loaded. Common data is stored in normal @link<StorableObject>
 * StorableObject @endlink that can be extended by @link<DetailsHolder> DetailsHolder @endlink
 * class that holds object of Details type.
 *
 * Object of DetailsHolder type can hold extra information in instance of DetailsClass type. When
 * new value is assigned to Details field a @link<Manager::detailsLoaded detailsLoaded @endlink method of
 * ManagerClass is called with this object as a parameter (casted to Class type to avoid casting
 * at manager side). When old value is removed from Details field a @link<Manager::detailsUnloaded
 * detailsUnloaded @endlink method ManagerClass is called with this object as a parameter.
 * This allows Managers to show only these object, that have details loaded.
 *
 * Example of inheritance:
 *
 * <pre>
 * class Contact: public DetailsHolder&lt;Contact, ContactDetails, ContactManager&gt;
 * </pre>
 *
 * First template parameter allows object to be properly casted.
 */
template <class DetailsClass>
class DetailsHolder
{
	DetailsClass *Details;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Sets new details.
	 * @param details new details to set
	 *
	 * Sets details holded by this object to new value: details. Also calls
	 * virtual method @link detailsAdded @endlink that can be overridden
	 * in subclasses to do something with new details.
	 *
	 * After that manager's method detailsLoaded is called with this as
	 * parameter.
	 */
	void setNewDetails(DetailsClass *details)
	{
		if (details)
			detailsAboutToBeAdded();

		Details = details;
		if (!Details)
			return;

		detailsAdded();
		afterDetailsAdded();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removes details from object.
	 * @param details removes details from object
	 *
	 * This method calls * virtual method @link detailsAboutToBeRemoved @endlink
	 * that can be overridden in subclasses to do something with old details.
	 * Then details value is cleared.
	 *
	 * After that manager's method detailsUnloaded is called with this as
	 * parameter.
	 */
	void removeOldDetails()
	{
		if (!Details)
			return;

		detailsAboutToBeRemoved();
		delete Details;
		Details = 0;

		detailsRemoved();
		afterDetailsRemoved();
	}

protected:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Contructs object with empty details.
	 *
	 * Contructs object with empty details.
	 */
	DetailsHolder() :
			Details(0)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removes current details.
	 *
	 * Removes current details. Method @link detailsAboutToBeRemoved @endlink
	 * is called when details was not NULL.
	 */
	virtual ~DetailsHolder()
	{
		removeOldDetails();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called before adding new details.
	 *
	 * Method is called always before adding new details.
	 */
	virtual void detailsAboutToBeAdded()
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called after adding new details.
	 *
	 * Method is called always after adding new details.
	 */
	virtual void detailsAdded()
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called after informing manager about new details.
	 *
	 * Method is called after adding informing manager about new detais.
	 */
	virtual void afterDetailsAdded()
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called before removing old details.
	 *
	 * Method is called always before removing old details. Included
	 * destroying object with not-NULL details.
	 */
	virtual void detailsAboutToBeRemoved()
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called after removing old details.
	 *
	 * Method is called always after removing old details. Included
	 * destroying object with not-NULL details.
	 */
	virtual void detailsRemoved()
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called after informing manager about removing old details.
	 *
	 * Method is called always after informing manager about removing old details.
	 */
	virtual void afterDetailsRemoved()
	{
	}

public:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Set new details to object.
	 * @param details new details to set
	 *
	 * Set new details to object. Can result in calling @link detailsAboutToBeRemoved @endlink
	 * and @link detailsAdded @endlink virtual methods.
	 */
	void setDetails(DetailsClass *details)
	{
		removeOldDetails();
		setNewDetails(details);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns current details.
	 * @return current details
	 *
	 * Returns current details.
	 */
	DetailsClass * details() const
	{
		return Details;
	}

};

/**
 * @}
 */

#endif // DETAILS_HOLDER_H
