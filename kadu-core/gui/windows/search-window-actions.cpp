/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include <QtCore/QList>
#include <QtWidgets/QAction>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTreeWidget>

#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/windows/search-window.h"

#include "search-window-actions.h"

SearchWindowActions * SearchWindowActions::Instance = 0;

SearchWindowActions * SearchWindowActions::instance()
{
	if (!Instance)
		Instance = new SearchWindowActions();
	
	return Instance;
}

SearchWindowActions::SearchWindowActions()
{
	Actions::instance()->blockSignals();

	FirstSearch = new ActionDescription(this,
		ActionDescription::TypeSearch, "firstSearchAction",
		this, SLOT(firstSearchActionActivated(QAction*)),
		KaduIcon("edit-find"), tr("&Search")
	);
	connect(FirstSearch, SIGNAL(actionCreated(Action*)), this, SLOT(firstSearchActionCreated(Action*)));

	NextResults = new ActionDescription(this,
		ActionDescription::TypeSearch, "nextResultsAction",
		this, SLOT(nextResultsActionActivated(QAction*)),
		KaduIcon("go-next"), tr("&Next results")
	);
	connect(NextResults, SIGNAL(actionCreated(Action*)), this, SLOT(nextResultsActionCreated(Action*)));

	StopSearch = new ActionDescription(this,
		ActionDescription::TypeSearch, "stopSearchAction",
		this, SLOT(stopSearchActionActivated(QAction*)),
		KaduIcon("dialog-cancel"), tr("Stop")
	);
	connect(StopSearch, SIGNAL(actionCreated(Action*)), this, SLOT(stopSearchActionCreated(Action*)));

	ClearResults = new ActionDescription(this,
		ActionDescription::TypeSearch, "clearSearchAction",
		this, SLOT(clearResultsActionActivated(QAction*)),
		KaduIcon("edit-clear"), tr("Clear results")
	);
	connect(ClearResults, SIGNAL(actionCreated(Action*)), this, SLOT(clearResultsActionCreated(Action*)));

	AddFound = new ActionDescription(this,
		ActionDescription::TypeSearch, "addSearchedAction",
		this, SLOT(addFoundActionActivated(QAction*)),
		KaduIcon("contact-new"), tr("Add selected user")
	);
	connect(AddFound, SIGNAL(actionCreated(Action*)), this, SLOT(actionsFoundActionCreated(Action*)));

	// The last ActionDescription will send actionLoaded() signal.
	Actions::instance()->unblockSignals();

	ChatFound = new ActionDescription(this,
		ActionDescription::TypeSearch, "chatSearchedAction",
		this, SLOT(chatFoundActionActivated(QAction*)),
		KaduIcon("internet-group-chat"), tr("&Chat")
	);
	connect(ChatFound, SIGNAL(actionCreated(Action*)), this, SLOT(actionsFoundActionCreated(Action*)));
}

SearchWindowActions::~SearchWindowActions()
{
	Instance = 0;
}

void SearchWindowActions::firstSearchActionCreated(Action *action)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(action->parentWidget());

	if (!search || search->SearchInProgress || !search->PersonalDataRadioButton ||
			(search->PersonalDataRadioButton->isChecked() && search->isPersonalDataEmpty()) ||
			!search->UinRadioButton || !search->UinEdit || 
			(search->UinRadioButton->isChecked() && search->UinEdit->text().isEmpty()))
		action->setEnabled(false);
}

void SearchWindowActions::nextResultsActionCreated(Action *action)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(action->parentWidget());

	if (!search || search->SearchInProgress ||
			(search->UinRadioButton && search->UinRadioButton->isChecked()) ||
			search->isPersonalDataEmpty())
		action->setEnabled(false);
}

void SearchWindowActions::stopSearchActionCreated(Action *action)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(action->parentWidget());

	if (!search || !search->SearchInProgress)
		action->setEnabled(false);
}

void SearchWindowActions::clearResultsActionCreated(Action *action)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(action->parentWidget());
	if (!search || !search->ResultsListWidget || !search->ResultsListWidget->topLevelItemCount())
		action->setEnabled(false);
}

void SearchWindowActions::actionsFoundActionCreated(Action *action)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(action->parentWidget());

	if (!search || !search->ResultsListWidget || search->ResultsListWidget->selectedItems().isEmpty())
		action->setEnabled(false);
}

void SearchWindowActions::firstSearchActionActivated(QAction *sender)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(sender->parentWidget());
	if (search)
		search->firstSearch();
}

void SearchWindowActions::nextResultsActionActivated(QAction *sender)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(sender->parentWidget());
	if (search)
		search->nextSearch();
}

void SearchWindowActions::stopSearchActionActivated(QAction *sender)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(sender->parentWidget());
	if (search)
		search->stopSearch();
}

void SearchWindowActions::clearResultsActionActivated(QAction *sender)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(sender->parentWidget());
	if (search)
		search->clearResults();
}

void SearchWindowActions::addFoundActionActivated(QAction *sender)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(sender->parentWidget());
	if (search)
		search->addFound();
}

void SearchWindowActions::chatFoundActionActivated(QAction *sender)
{
	SearchWindow *search = qobject_cast<SearchWindow *>(sender->parentWidget());
	if (search)
		search->chatFound();
}

#include "moc_search-window-actions.cpp"
