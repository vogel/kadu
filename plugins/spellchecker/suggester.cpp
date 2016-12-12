/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QMouseEvent>
#include <QtWidgets/QTextEdit>

#include "actions/actions.h"
#include "actions/action-description.h"
#include "actions/action.h"
#include "widgets/custom-input-menu-manager.h"
#include "widgets/custom-input.h"
#include "icons/kadu-icon.h"
#include "plugin/plugin-injected-factory.h"

#include "configuration/spellchecker-configuration.h"
#include "spellchecker.h"
#include "suggest-action.h"

#include "suggester.h"

Suggester::Suggester(QObject *parent) :
		QObject{parent}
{
}

Suggester::~Suggester()
{
	clearWordMenu();
}

void Suggester::setActions(Actions *actions)
{
	m_actions = actions;
}

void Suggester::setCustomInputMenuManager(CustomInputMenuManager *customInputMenuManager)
{
	m_customInputMenuManager = customInputMenuManager;
}

void Suggester::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void Suggester::setSpellcheckerConfiguration(SpellcheckerConfiguration *spellcheckerConfiguration)
{
	m_spellcheckerConfiguration = spellcheckerConfiguration;
}

void Suggester::setSpellChecker(SpellChecker *spellChecker)
{
	m_spellChecker = spellChecker;
}

void Suggester::buildSuggestList(const QString &word)
{
	SuggestionWordList = m_spellChecker->buildSuggestList(word);
}

void Suggester::addWordListToMenu(const QTextCursor &textCursor)
{
	CurrentTextSelection = textCursor;

	// Add new actions
	for (auto const &listWord : SuggestionWordList)
		SuggestActions.append(m_pluginInjectedFactory->makeInjected<SuggestAction>(listWord, this));

	unsigned int actionPriority = 0;
	for (auto action : SuggestActions)
		m_customInputMenuManager->addActionDescription(action, CustomInputMenuItem::MenuCategorySuggestion, actionPriority++);
}

void Suggester::clearWordMenu()
{
	// Remove old actions
	foreach (ActionDescription *action, SuggestActions)
		m_customInputMenuManager->removeActionDescription(action);

	qDeleteAll(SuggestActions);
	SuggestActions.clear();
}

bool Suggester::eventFilter(QObject *object, QEvent *event)
{
	CustomInput *inputBox = qobject_cast<CustomInput *>(object);

	if ((inputBox) && (event->type() == QEvent::MouseButtonPress))
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

		if (mouseEvent->button() == Qt::RightButton)
		{
			QTextCursor textCursor = inputBox->cursorForPosition(mouseEvent->pos());
			textCursor.select(QTextCursor::WordUnderCursor);

			if ((!textCursor.selectedText().isEmpty()) &&
					(!m_spellChecker->checkWord(textCursor.selectedText())) &&
					 (m_spellcheckerConfiguration->suggester()))
			{
				buildSuggestList(textCursor.selectedText());
				clearWordMenu();
				addWordListToMenu(textCursor);
			}
			else
			{
				clearWordMenu();
			}
		}
	}
	else if((inputBox) && (event->type() == QEvent::ContextMenu))
	{
		QContextMenuEvent *contextMenuEvent = static_cast<QContextMenuEvent *>(event);

		/* Don't react on context menu triggered by keyboard key */
		if (contextMenuEvent->reason() == QContextMenuEvent::Keyboard)
			clearWordMenu();
	}

	return QObject::eventFilter(object, event);
}

void Suggester::replaceWith(QString word)
{
	if (word.indexOf(" (") != -1)
		word.truncate(word.indexOf(" ("));

	CurrentTextSelection.insertText(word);
}

#include "moc_suggester.cpp"
