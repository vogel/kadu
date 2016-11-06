/*
 * %kadu copyright begin%
 * Copyright 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "actions-module.h"

#include "actions/add-group-action.h"
#include "actions/add-user-action.h"
#include "actions/exit-action.h"
#include "actions/open-forum-action.h"
#include "actions/open-get-involved-action.h"
#include "actions/open-redmine-action.h"
#include "actions/open-search-action.h"
#include "actions/open-translate-action.h"
#include "actions/show-configuration-window-action.h"
#include "actions/show-multilogons-action.h"
#include "actions/show-your-accounts-action.h"

ActionsModule::ActionsModule()
{
	add_type<AddGroupAction>();
	add_type<AddUserAction>();
	add_type<ExitAction>();
	add_type<OpenForumAction>();
	add_type<OpenGetInvolvedAction>();
	add_type<OpenRedmineAction>();
	add_type<OpenSearchAction>();
	add_type<OpenTranslateAction>();
	add_type<ShowConfigurationWindowAction>();
	add_type<ShowMultilogonsAction>();
	add_type<ShowYourAccountsAction>();
}

ActionsModule::~ActionsModule()
{
}
