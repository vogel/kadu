/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "standard-url-expander-configuration.h"

StandardUrlExpanderConfiguration::StandardUrlExpanderConfiguration() :
		FoldLink(false), FoldLinkThreshold(0)
{
}

StandardUrlExpanderConfiguration::StandardUrlExpanderConfiguration(const StandardUrlExpanderConfiguration &copyMe)
{
	FoldLink = copyMe.FoldLink;
	FoldLinkThreshold = copyMe.FoldLinkThreshold;
}

StandardUrlExpanderConfiguration & StandardUrlExpanderConfiguration::operator = (const StandardUrlExpanderConfiguration &copyMe)
{
	FoldLink = copyMe.FoldLink;
	FoldLinkThreshold = copyMe.FoldLinkThreshold;

	return *this;
}

void StandardUrlExpanderConfiguration::setFoldLink(bool foldLink)
{
	FoldLink = foldLink;
}

bool StandardUrlExpanderConfiguration::foldLink() const
{
	return FoldLink;
}

void StandardUrlExpanderConfiguration::setFoldLinkThreshold(int foldLinkThreshold)
{
	FoldLinkThreshold = foldLinkThreshold;
}

int StandardUrlExpanderConfiguration::foldLinkThreshold() const
{
	return FoldLinkThreshold;
}
