/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
 * @short Object that holds extra data.
 *
 * Some object (@link<Account> Account @endlink, @link<Contact> Contact @endlink and others)
 * have common data that should be always available and protocol data that can only be loaded
 * when protocol plugin is loaded. Common data is stored in normal @link<StorableObject>
 * StorableObject @endlink that can be extended by @link<DetailsHolder> DetailsHolder @endlink
 * class that holds object of Details type.
 *
 * Object of DetailsHolder type can hold extra information in instance of DetailsClass type.
 *
 * Example of inheritance:
 */
template <class DetailsClass>
class DetailsHolder
{
	DetailsClass *Details;

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
	 * @short Empty destructor.
	 *
	 * Empty destructor. All details must be removed by derivered class before destruction.
	 */
	virtual ~DetailsHolder()
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Set new details to object.
	 * @param details new details to set
	 *
	 * Set new details to object. Folliwing virtual methods are called:
	 * @link detailsAboutToBeAdded @endlink and @link detailsAdded @endlink.
	 * Can only be called if no details are present.
	 */
	void setDetails(DetailsClass *details)
	{
		Q_ASSERT(!hasDetails());
		Q_ASSERT(details);

		detailsAboutToBeAdded();
		Details = details;
		detailsAdded();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removes current details.
	 *
	 * Removes current details. Folliwing virtual methods are called:
	 * @link detailsAboutToBeRemoved @endlink and @link detailsRemoved @endlink.
	 * Can only be called if details are present.
	 */
	void removeDetails()
	{
		Q_ASSERT(hasDetails());

		detailsAboutToBeRemoved();
		delete Details;
		Details = 0;
		detailsRemoved();
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

public:
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

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns true if details are available.
	 * @return true if details are available
	 *
	 * Returns true if details are available.
	 */
	bool hasDetails() const
	{
		return 0 != Details;
	}

};

/**
 * @}
 */

#endif // DETAILS_HOLDER_H
