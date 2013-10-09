/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "emoticon-configuration.h"

EmoticonConfiguration::EmoticonConfiguration() :
		Enabled(true), Animate(true)
{
}

EmoticonConfiguration::EmoticonConfiguration(const EmoticonConfiguration &copyMe)
{
	Enabled = copyMe.Enabled;
	Animate = copyMe.Animate;
	Emoticons = copyMe.Emoticons;
}

EmoticonConfiguration & EmoticonConfiguration::operator = (const EmoticonConfiguration &copyMe)
{
	Enabled = copyMe.Enabled;
	Animate = copyMe.Animate;
	Emoticons = copyMe.Emoticons;

	return *this;
}

void EmoticonConfiguration::setEnabled(bool enabled)
{
	Enabled = enabled;
}

bool EmoticonConfiguration::enabled() const
{
	return Enabled;
}

void EmoticonConfiguration::setAnimate(bool animate)
{
	Animate = animate;
}

bool EmoticonConfiguration::animate() const
{
	return Animate;
}

void EmoticonConfiguration::setEmoticonTheme(const EmoticonTheme &emoticonTheme)
{
	Emoticons = emoticonTheme;
}

EmoticonTheme EmoticonConfiguration::emoticonTheme() const
{
	return Emoticons;
}
