/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011 Michał Ziąbkowski (mziab@o2.pl)
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"

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
	Application::instance()->configuration()->deprecatedApi()->addVariable("ASpell", "Bold", "false");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ASpell", "Italic", "false");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ASpell", "Underline", "true");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ASpell", "Color", "#FF0101");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ASpell", "Checked", Application::instance()->configuration()->deprecatedApi()->readEntry("General", "Language"));
	Application::instance()->configuration()->deprecatedApi()->addVariable("ASpell", "Accents", "false");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ASpell", "Case", "false");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ASpell", "Suggester", "true");
	Application::instance()->configuration()->deprecatedApi()->addVariable("ASpell", "SuggesterWordCount", "10");
}

void SpellcheckerConfiguration::configurationUpdated()
{
	bool bold = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("ASpell", "Bold", false);
	bool italic = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("ASpell", "Italic", false);
	bool underline = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("ASpell", "Underline", false);
	bool accents = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("ASpell", "Accents", false);
	bool caseSensivity = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("ASpell", "Case", false);
	bool suggester = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("ASpell", "Suggester", true);
	QColor colorMark("#FF0101");

	auto color = Application::instance()->configuration()->deprecatedApi()->readColorEntry("ASpell", "Color", &colorMark);
	auto checkedEntry = Application::instance()->configuration()->deprecatedApi()->readEntry("ASpell", "Checked", Application::instance()->configuration()->deprecatedApi()->readEntry("General", "Language"));
	auto checked = checkedEntry == "empty" ? QStringList{} : checkedEntry.split(',', QString::SkipEmptyParts);
	int suggesterWordCount = Application::instance()->configuration()->deprecatedApi()->readNumEntry("ASpell", "SuggesterWordCount");

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
	if (checked.empty())
		Application::instance()->configuration()->deprecatedApi()->writeEntry("ASpell", "Checked", "empty");
	else
		Application::instance()->configuration()->deprecatedApi()->writeEntry("ASpell", "Checked", checked.join(","));
}
