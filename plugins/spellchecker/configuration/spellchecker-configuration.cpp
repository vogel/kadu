/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011 Michał Ziąbkowski (mziab@o2.pl)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration-file.h"
#include "kadu-application.h"

#include "spellchecker-plugin.h"
#include "spellchecker.h"

#include "spellchecker-configuration.h"

SpellcheckerConfiguration *SpellcheckerConfiguration::Instance = 0;

SpellcheckerConfiguration *SpellcheckerConfiguration::instance()
{
	return Instance;
}

void SpellcheckerConfiguration::createInstance()
{
	if (Instance)
		return;

	Instance = new SpellcheckerConfiguration();
	Instance->configurationUpdated();
	Instance->FullyLoaded = true;
}

void SpellcheckerConfiguration::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

SpellcheckerConfiguration::SpellcheckerConfiguration() :
		FullyLoaded{},
		Bold{},
		Italic{},
		Underline{},
		Accents{},
		Case{},
		Suggester{},
		SuggesterWordCount{}
{
	createDefaultConfiguration();
}

SpellcheckerConfiguration::~SpellcheckerConfiguration()
{

}

void SpellcheckerConfiguration::createDefaultConfiguration()
{
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ASpell", "Bold", "false");
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ASpell", "Italic", "false");
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ASpell", "Underline", "true");
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ASpell", "Color", "#FF0101");
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ASpell", "Checked", KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("General", "Language"));
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ASpell", "Accents", "false");
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ASpell", "Case", "false");
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ASpell", "Suggester", "true");
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ASpell", "SuggesterWordCount", "10");
}

void SpellcheckerConfiguration::configurationUpdated()
{
	bool bold = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("ASpell", "Bold", false);
	bool italic = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("ASpell", "Italic", false);
	bool underline = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("ASpell", "Underline", false);
	bool accents = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("ASpell", "Accents", false);
	bool caseSensivity = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("ASpell", "Case", false);
	bool suggester = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("ASpell", "Suggester", true);
	QColor colorMark("#FF0101");
	QColor color = KaduApplication::instance()->deprecatedConfigurationApi()->readColorEntry("ASpell", "Color", &colorMark);
	QStringList checked = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("ASpell", "Checked", KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("General", "Language")).split(',', QString::SkipEmptyParts);
	int suggesterWordCount = KaduApplication::instance()->deprecatedConfigurationApi()->readNumEntry("ASpell", "SuggesterWordCount");

	if (FullyLoaded && bold == Bold && italic == Italic && underline == Underline && accents == Accents &&
			caseSensivity == Case && suggester == Suggester && color == Color &&
			checked == Checked && suggesterWordCount == SuggesterWordCount)
		return;

	Bold = bold;
	Italic = italic;
	Underline = underline;
	Accents = accents;
	Case = caseSensivity;
	Suggester = suggester;
	Color = color;
	Checked = checked;
	SuggesterWordCount = suggesterWordCount;

	SpellCheckerPlugin::instance()->spellChecker()->buildMarkTag();
	SpellCheckerPlugin::instance()->spellChecker()->buildCheckers();
}

void SpellcheckerConfiguration::setChecked(const QStringList &checked)
{
	KaduApplication::instance()->deprecatedConfigurationApi()->writeEntry("ASpell", "Checked", checked.join(","));
}
