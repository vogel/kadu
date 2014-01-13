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

#ifndef DEFAULT_PROVIDER_H
#define DEFAULT_PROVIDER_H

#include <memory>

#include "provider/provider.h"

/**
 * @addtogroup Provider
 * @{
 */

/**
 * @class DefaultProvider
 * @author Rafał 'Vogel' Malinowski
 * @short Implementation of Provider interface that wraps one or two other Providers.
 * @param T type of value to provide
 *
 * This implementation of Provider interface holds at most two other instances of Provider iterface.
 * First is called Default, second is called Custom. If Custom Provider is available then it is used
 * to provide value. In other case Default Provider is used.
 *
 * Default Provider is set in constructor.
 *
 * Custom Provider can be set by @link installCustomProvider() @endlink method but only if there is
 * no Custom Provider currently installed. To remove Custom Provider call @link removeCustomProvider
 * @endlink.
 */
template<typename T>
class DefaultProvider : public Provider<T>
{
	std::shared_ptr<Provider<T>> Default;
	std::shared_ptr<Provider<T>> Custom;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create instance of DefaultProvider with given Default Provider.
	 * @param defaultProvider Default Provider value
	 *
	 * Given defaultProvider parameter must not be null.
	 */
	explicit DefaultProvider(const std::shared_ptr<Provider<T>> &defaultProvider) : Default{defaultProvider}
	{
		Q_ASSERT(Default);
	}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Install Custom Provider.
	 * @param custom Custom Provider to install
	 * @return true if installation was successfull
	 *
	 * This method will take effect only if there is no Custom Provider already set.
	 * This method will return true if after call given custom Provider is properly installed.
	 */
	bool installCustomProvider(const std::shared_ptr<Provider<T>> &custom)
	{
		if (Custom)
			return custom == Custom;

		Custom = custom;
		return true;
	}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Remove Custom Provider.
	 * @param custom Custom Provider to remove
	 * @return true if removing was successfull
	 *
	 * This method will take effect only if there is Custom Provider set to value passed as parameter.
	 * This method will return true if after call no custom Provider is installed.
	 */
	bool removeCustomProvider(const std::shared_ptr<Provider<T>> &custom)
	{
		if (!Custom || Custom != custom)
			return false;

		Custom.reset();
		return true;
	}

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return value.
	 *
	 * This method will return value provided by Default Provider if no Custom Provider is installed and
	 * value provided by Custom Provider otherwise.
	 */
	virtual T provide() const
	{
		return Custom
				? Custom->provide()
				: Default->provide();
	}

};

/**
 * @}
 */

#endif // DEFAULT_PROVIDER_H
