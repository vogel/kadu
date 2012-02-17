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

#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QtCore/QString>

#include "gui/actions/action-description.h"

class Shortcut
{
	bool Initialized;
	ActionDescription::ActionType Type;
	QString KeySequence; // or QKeySequence
	QString Name;
	QString ProviderName;
	
public:
	Shortcut();

	bool type() const {return Type; }
	void setType(ActionDescription::ActionType type);

	const QString & keySequence() const { return KeySequence; }
	void setKeySequence(const QString &keySequence);

	const QString & name() const { return Name; }
	void setName(const QString &name);

	const QString & providerName() const { return ProviderName; }
	void setProviderName(const QString &providerName);

	bool isInitialized() const { return Initialized; }
};

#endif // SHORTCUT_H
