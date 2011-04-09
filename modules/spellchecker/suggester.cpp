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

#include <QtGui/QMouseEvent>
#include <QtGui/QTextEdit>

#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/custom-input.h"
#include "gui/widgets/custom-input-menu-manager.h"
#include "configuration/configuration-file.h"
#include "icons/kadu-icon.h"

#include "spellchecker.h"

#include "suggester.h"

Suggester * Suggester::Instance = 0;

Suggester *Suggester::instance()
{
	if (!Instance)
		Instance = new Suggester();

	return Instance;
}

void Suggester::destroyInstance()
{
	if (Instance)
		delete Instance;

	Instance = 0;
}

Suggester::Suggester()
{
}

Suggester::~Suggester()
{
	clearWordMenu();
}

void Suggester::buildSuggestList(const QString &word)
{
	SuggestionWordList = spellcheck->buildSuggestList(word);
}

void Suggester::addWordListToMenu(const QTextCursor &textCursor)
{
	CurrentTextSelection = textCursor;

	// Add new actions
	for (QStringList::const_iterator it = SuggestionWordList.constBegin(); it != SuggestionWordList.constEnd(); ++it)
		SuggestActions.append(new ActionDescription(this, ActionDescription::TypeGlobal,
					"spellcheckerSuggest#" + *it, this, SLOT(replaceWithSuggest(QAction *)), KaduIcon(), *it));

	unsigned int actionPriority = 0;
	for (QList<ActionDescription *>::const_iterator action_it = SuggestActions.constBegin(); action_it != SuggestActions.constEnd(); ++action_it)
		CustomInputMenuManager::instance()->addActionDescription(*action_it, CustomInputMenuItem::MenuCategorySuggestion, actionPriority++);
}

void Suggester::clearWordMenu()
{
	// Remove old actions
	for (QList<ActionDescription *>::const_iterator action_it = SuggestActions.constBegin(); action_it != SuggestActions.constEnd(); ++action_it)
		CustomInputMenuManager::instance()->removeActionDescription(*action_it);

	qDeleteAll(SuggestActions);
	SuggestActions.clear();
}

bool Suggester::eventFilter(QObject *object, QEvent *event)
{
	QTextEdit *inputBox = qobject_cast<QTextEdit *>(object);

	if ((inputBox) && (event->type() == QEvent::MouseButtonPress))
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

		if (mouseEvent->button() == Qt::RightButton)
		{
			QTextCursor textCursor = inputBox->cursorForPosition(mouseEvent->pos());
			textCursor.select(QTextCursor::WordUnderCursor);

			if ((!textCursor.selectedText().isEmpty()) &&
					(!spellcheck->checkWord(textCursor.selectedText())) &&
					 (config_file.readBoolEntry("ASpell", "Suggester", true)))
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

void Suggester::replaceWithSuggest(QAction *sender)
{
	Action *action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	CurrentTextSelection.insertText(action->text());
}
