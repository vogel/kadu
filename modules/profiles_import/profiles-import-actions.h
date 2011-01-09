/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PROFILES_IMPORT_ACTIONS_H
#define PROFILES_IMPORT_ACTIONS_H

#include <QtCore/QObject>

class QAction;

class ActionDescription;

class ProfilesImportActions : public QObject
{
	Q_OBJECT

	static ProfilesImportActions * Instance;

	ActionDescription *ImportProfiles;
	ActionDescription *ImportExternalProfile;

	ProfilesImportActions();
	virtual ~ProfilesImportActions();

private slots:
	void importProfilesActionActivated(QAction *action, bool toggled);
	void importExternalProfileActionActivated(QAction *action, bool toggled);

public:
	static void registerActions();
	static void unregisterActions();

	static ProfilesImportActions * instance();

	void updateActions();

};

#endif // PROFILES_IMPORT_ACTIONS_H
