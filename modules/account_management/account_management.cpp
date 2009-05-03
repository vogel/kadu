/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "action.h"
#include "debug.h"
#include "icons-manager.h"
#include "kadu.h"
#include "register.h"
#include "unregister.h"
#include "change_password.h"
#include "remind_password.h"
#include "account_management.h"

/** @ingroup account_management
 * @{
 */
extern "C" KADU_EXPORT int account_management_init(bool firstLoad)
{
	kdebugf();

	account_management = new AccountManagement();

	kdebugf2();
	return 0;
}

extern "C" KADU_EXPORT void account_management_close()
{
	kdebugf();

	delete account_management;
	account_management = 0;

	kdebugf2();
}

AccountManagement::AccountManagement()
{
	kdebugf();

	int index = kadu->personalInfoMenuId;

	unregisterMenuActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "unregisterUserAction",
		this, SLOT(unregisterUser(QAction *, bool)),
		"UnregisterUser", tr("Unregister user")
	);
	kadu->insertMenuActionDescription(index, unregisterMenuActionDescription);

	registerMenuActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "registerUserAction",
		this, SLOT(registerUser(QAction *, bool)),
		"RegisterUser", tr("Register &new user")
	);
	kadu->insertMenuActionDescription(index, registerMenuActionDescription);

	changeMenuActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "changePasswordAction",
		this, SLOT(changePassword(QAction *, bool)),
		"ChangePassMail", tr("&Change password / email")
	);
	kadu->insertMenuActionDescription(index, changeMenuActionDescription);

	remindMenuActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "remidPasswordAction",
		this, SLOT(remindPassword(QAction *, bool)),
		"RemindPass", tr("Remind &password")
	);
	kadu->insertMenuActionDescription(index, remindMenuActionDescription);

	kdebugf2();
}

AccountManagement::~AccountManagement()
{
	kdebugf();

	kadu->removeMenuActionDescription(remindMenuActionDescription);
	delete remindMenuActionDescription;

	kadu->removeMenuActionDescription(changeMenuActionDescription);
	delete changeMenuActionDescription;

	kadu->removeMenuActionDescription(registerMenuActionDescription);
	delete registerMenuActionDescription;

	kadu->removeMenuActionDescription(unregisterMenuActionDescription);
	delete unregisterMenuActionDescription;

	kdebugf2();
}

void AccountManagement::registerUser(QAction *sender, bool toggled)
{
	(new Register())->show();
}

void AccountManagement::unregisterUser(QAction *sender, bool toggled)
{
	(new Unregister())->show();
}

void AccountManagement::remindPassword(QAction *sender, bool toggled)
{
	(new RemindPassword())->show();
}

void AccountManagement::changePassword(QAction *sender, bool toggled)
{
	(new ChangePassword())->show();
}

AccountManagement *account_management = NULL;

/** @} */
