/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "shortcut.h"

Shortcut::Shortcut()
{

}
void Shortcut::setType(ActionDescription::ActionType type)
{
	Type = type;
}

void Shortcut::setKeySequence(const QString &keySequence)
{
	KeySequence = keySequence;
}

void Shortcut::setName(const QString &name)
{
	Name = name;
}

void Shortcut::setProviderName(const QString &providerName)
{
	ProviderName = providerName;
}
