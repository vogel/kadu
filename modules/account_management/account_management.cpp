/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qpopupmenu.h>

#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "register.h"
#include "unregister.h"
#include "change_password.h"
#include "remind_password.h"
#include "account_management.h"

/** @ingroup account_management
 * @{
 */
extern "C" int account_management_init()
{
	kdebugf();
	account_management=new AccountManagement();
	kdebugf2();
	return 0;
}

extern "C" void account_management_close()
{
	kdebugf();
	delete account_management;
	account_management=NULL;
	kdebugf2();
}

AccountManagement::AccountManagement()
{
	kdebugf();
	QPopupMenu *MainMenu=kadu->mainMenu();
	int index=MainMenu->indexOf(kadu->personalInfoMenuId);

	unregisterMenuId=MainMenu->insertItem(icons_manager->loadIcon("UnregisterUser"),tr("Unregister user"), this, SLOT(unregisterUser()), 0, -1, index);
	registerMenuId=MainMenu->insertItem(icons_manager->loadIcon("RegisterUser"),tr("Register &new user"), this, SLOT(registerUser()), 0, -1, index);
	changeMenuId=MainMenu->insertItem(icons_manager->loadIcon("ChangePassMail"),tr("&Change password / email"), this, SLOT(changePassword()), 0, -1, index);
	remindMenuId=MainMenu->insertItem(icons_manager->loadIcon("RemindPass"),tr("Remind &password"), this, SLOT(remindPassword()), 0, -1, index);

	icons_manager->registerMenuItem(MainMenu, tr("Unregister user"), "UnregisterUser");
	icons_manager->registerMenuItem(MainMenu, tr("Register &new user"), "RegisterUser");
	icons_manager->registerMenuItem(MainMenu, tr("&Change password / email"), "ChangePassMail");
	icons_manager->registerMenuItem(MainMenu, tr("Remind &password"), "RemindPass");

	kdebugf2();
}

AccountManagement::~AccountManagement()
{
	kdebugf();

	QPopupMenu *MainMenu=kadu->mainMenu();
	MainMenu->removeItem(remindMenuId);
	MainMenu->removeItem(changeMenuId);
	MainMenu->removeItem(registerMenuId);
	MainMenu->removeItem(unregisterMenuId);

	kdebugf2();
}

void AccountManagement::registerUser()
{
	(new Register())->show();
}

void AccountManagement::unregisterUser()
{
	(new Unregister())->show();
}

void AccountManagement::remindPassword()
{
	(new RemindPassword())->show();
}

void AccountManagement::changePassword()
{
	(new ChangePassword())->show();
}

AccountManagement *account_management=NULL;

/** @} */

