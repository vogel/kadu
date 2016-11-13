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

#include "next-results-action.h"

#include "gui/windows/search-window.h"

#include <QtWidgets/QRadioButton>

NextResultsAction::NextResultsAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"go-next"});
	setName(QStringLiteral("nextResultsAction"));
	setText(tr("&Next results"));
	setType(ActionDescription::TypeSearch);
}

NextResultsAction::~NextResultsAction()
{
}

void NextResultsAction::init()
{
	registerAction(actionsRegistry());
}

void NextResultsAction::actionInstanceCreated(Action* action)
{
	auto search = qobject_cast<SearchWindow *>(action->parentWidget());
	if (!search || search->SearchInProgress ||
			(search->UinRadioButton && search->UinRadioButton->isChecked()) ||
			search->isPersonalDataEmpty())
		action->setEnabled(false);
}

void NextResultsAction::actionTriggered(QAction *sender, bool)
{
	auto search = qobject_cast<SearchWindow *>(sender->parentWidget());
	if (search)
		search->nextSearch();
}

#include "moc_next-results-action.cpp"
