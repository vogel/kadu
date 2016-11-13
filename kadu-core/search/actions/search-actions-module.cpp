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

#include "search-actions-module.h"

#include "search/actions/add-found-buddy-action.h"
#include "search/actions/chat-found-action.h"
#include "search/actions/clear-results-action.h"
#include "search/actions/first-search-action.h"
#include "search/actions/next-results-action.h"
#include "search/actions/stop-search-action.h"

SearchActionsModule::SearchActionsModule()
{
	add_type<AddFoundBuddyAction>();
	add_type<ChatFoundAction>();
	add_type<ClearResultsAction>();
	add_type<FirstSearchAction>();
	add_type<NextResultsAction>();
	add_type<StopSearchAction>();
}

SearchActionsModule::~SearchActionsModule()
{
}
