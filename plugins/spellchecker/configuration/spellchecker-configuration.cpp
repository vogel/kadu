/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
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

#include "spellchecker.h" 
#include "spellchecker-plugin.h"

#include "spellchecker-configuration.h"

SpellcheckerConfiguration *SpellcheckerConfiguration::Instance = 0;

SpellcheckerConfiguration *SpellcheckerConfiguration::instance()
{
	return Instance;
}

void SpellcheckerConfiguration::createInstance()
{
	if (!Instance)
		Instance = new SpellcheckerConfiguration();

	Instance->configurationUpdated();
}

void SpellcheckerConfiguration::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

SpellcheckerConfiguration::SpellcheckerConfiguration()
{
	createDefaultConfiguration();
}

SpellcheckerConfiguration::~SpellcheckerConfiguration()
{

}

void SpellcheckerConfiguration::createDefaultConfiguration()
{
	config_file.addVariable("ASpell", "Bold", "false");
	config_file.addVariable("ASpell", "Italic", "false");
	config_file.addVariable("ASpell", "Underline", "true");
	config_file.addVariable("ASpell", "Color", "#FF0101");
	config_file.addVariable("ASpell", "Checked", "pl");
	config_file.addVariable("ASpell", "Accents", "false");
	config_file.addVariable("ASpell", "Case", "false");
	config_file.addVariable("ASpell", "Suggester", "true");
	config_file.addVariable("ASpell", "SuggesterWordCount", "10");
}

void SpellcheckerConfiguration::configurationUpdated()
{
	Bold = config_file.readBoolEntry("ASpell", "Bold", false);
	Italic = config_file.readBoolEntry("ASpell", "Italic", false);
	Underline = config_file.readBoolEntry("ASpell", "Underline", false);
	Accents = config_file.readBoolEntry("ASpell", "Accents", false);
	Case = config_file.readBoolEntry("ASpell", "Case", false);
	Suggester = config_file.readBoolEntry("ASpell", "Suggester", true);
	QColor colorMark("#FF0101");
	Color = config_file.readColorEntry("ASpell", "Color", &colorMark);
	Checked = config_file.readEntry("ASpell", "Checked", "pl");
	SuggesterWordCount = config_file.readNumEntry("ASpell", "SuggesterWordCount");

	SpellCheckerPlugin::instance()->spellChecker()->buildMarkTag();
	SpellCheckerPlugin::instance()->spellChecker()->buildCheckers();
}

void SpellcheckerConfiguration::setChecked(const QStringList &checked)
{
	config_file.writeEntry("ASpell", "Checked", checked.join(","));
}
