/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "suggest-action.h"

#include "suggester.h"

SuggestAction::SuggestAction(QString word, QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent),
          m_word(std::move(word))
{
    setName(QString{"spellcheckerSuggest#%1"}.arg(m_word));
    setText(m_word);
    setType(ActionDescription::TypeGlobal);
}

SuggestAction::~SuggestAction()
{
}

void SuggestAction::setSuggester(Suggester *suggester)
{
    m_suggester = suggester;
}

void SuggestAction::actionTriggered(QAction *, bool)
{
    m_suggester->replaceWith(m_word);
}

#include "moc_suggest-action.cpp"
