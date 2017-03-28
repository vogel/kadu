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

#include "spellchecker-configuration.h"

#include "spellchecker.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"

SpellcheckerConfiguration::SpellcheckerConfiguration(QObject *parent)
        : QObject{parent}, Bold{}, Italic{}, Underline{}, Accents{}, Case{}, Suggester{}, SuggesterWordCount{}
{
}

SpellcheckerConfiguration::~SpellcheckerConfiguration()
{
}

void SpellcheckerConfiguration::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void SpellcheckerConfiguration::init()
{
    createDefaultConfiguration();
    configurationUpdated();
}

void SpellcheckerConfiguration::createDefaultConfiguration()
{
    m_configuration->deprecatedApi()->addVariable("ASpell", "Bold", "false");
    m_configuration->deprecatedApi()->addVariable("ASpell", "Italic", "false");
    m_configuration->deprecatedApi()->addVariable("ASpell", "Underline", "true");
    m_configuration->deprecatedApi()->addVariable("ASpell", "Color", "#FF0101");
    m_configuration->deprecatedApi()->addVariable(
        "ASpell", "Checked", m_configuration->deprecatedApi()->readEntry("General", "Language"));
    m_configuration->deprecatedApi()->addVariable("ASpell", "Accents", "false");
    m_configuration->deprecatedApi()->addVariable("ASpell", "Case", "false");
    m_configuration->deprecatedApi()->addVariable("ASpell", "Suggester", "true");
    m_configuration->deprecatedApi()->addVariable("ASpell", "SuggesterWordCount", "10");
}

void SpellcheckerConfiguration::configurationUpdated()
{
    bool bold = m_configuration->deprecatedApi()->readBoolEntry("ASpell", "Bold", false);
    bool italic = m_configuration->deprecatedApi()->readBoolEntry("ASpell", "Italic", false);
    bool underline = m_configuration->deprecatedApi()->readBoolEntry("ASpell", "Underline", false);
    bool accents = m_configuration->deprecatedApi()->readBoolEntry("ASpell", "Accents", false);
    bool caseSensivity = m_configuration->deprecatedApi()->readBoolEntry("ASpell", "Case", false);
    bool suggester = m_configuration->deprecatedApi()->readBoolEntry("ASpell", "Suggester", true);
    QColor colorMark("#FF0101");

    auto color = m_configuration->deprecatedApi()->readColorEntry("ASpell", "Color", &colorMark);
    auto checkedEntry = m_configuration->deprecatedApi()->readEntry(
        "ASpell", "Checked", m_configuration->deprecatedApi()->readEntry("General", "Language"));
    auto checked = checkedEntry == "empty" ? QStringList{} : checkedEntry.split(',', QString::SkipEmptyParts);
    int suggesterWordCount = m_configuration->deprecatedApi()->readNumEntry("ASpell", "SuggesterWordCount");

    if (bold == Bold && italic == Italic && underline == Underline && accents == Accents && caseSensivity == Case &&
        suggester == Suggester && color == Color && checked == Checked && suggesterWordCount == SuggesterWordCount)
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

    emit updated();
}

void SpellcheckerConfiguration::setChecked(const QStringList &checked)
{
    if (checked.empty())
        m_configuration->deprecatedApi()->writeEntry("ASpell", "Checked", "empty");
    else
        m_configuration->deprecatedApi()->writeEntry("ASpell", "Checked", checked.join(","));
}

#include "moc_spellchecker-configuration.cpp"
