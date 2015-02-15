/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SPELLCHECKER_CONFIGURATION_H
#define SPELLCHECKER_CONFIGURATION_H

#include <QtCore/QStringList>
#include <QtGui/QColor>

#include "configuration/configuration-aware-object.h"

class SpellcheckerConfiguration : private ConfigurationAwareObject
{
	Q_DISABLE_COPY(SpellcheckerConfiguration)

	static SpellcheckerConfiguration *Instance;

	bool FullyLoaded;
	bool Bold;
	bool Italic;
	bool Underline;
	bool Accents;
	bool Case;
	bool Suggester;
	QColor Color;
	QStringList Checked;
	int SuggesterWordCount;

	explicit SpellcheckerConfiguration();
	virtual ~SpellcheckerConfiguration();

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	static SpellcheckerConfiguration *instance();

	static void createInstance();
	static void destroyInstance();

	bool bold() const { return Bold; }
	bool italic() const { return Italic; }
	bool underline() const { return Underline; }
	bool accents() const { return Accents; }
	bool casesens() const { return Case; }
	bool suggester() const { return Suggester; }
	const QColor & color() const { return Color; }
	const QStringList & checked() const { return Checked; }
	int suggesterWordCount() const { return SuggesterWordCount; }

	void setChecked(const QStringList &checked);

};

#endif /* SPELLCHECKER_CONFIGURATION_H */
