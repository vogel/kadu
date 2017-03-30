/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration-holder.h"
#include "misc/misc.h"

#include "configuration-aware-object.h"

QList<ConfigurationAwareObject *> ConfigurationAwareObject::Objects;

void ConfigurationAwareObject::notifyAll()
{
    for (auto configurationHolder : ConfigurationHolder::instances())
        configurationHolder->configurationUpdated();
    for (auto object : Objects)
        object->configurationUpdated();
}

ConfigurationAwareObject::ConfigurationAwareObject()
{
    Objects.append(this);
}

ConfigurationAwareObject::~ConfigurationAwareObject()
{
    Objects.removeAll(this);
}
