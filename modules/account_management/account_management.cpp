/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "kadu.h"
#include "register.h"
#include "unregister.h"
#include "change_password.h"
#include "remind_password.h"
#include "account_management.h"

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
	
	unregisterMenuId=MainMenu->insertItem(icons_manager.loadIcon("UnregisterUser"),tr("Unregister user"), this, SLOT(unregisterUser()), 0, -1, index);
	registerMenuId=MainMenu->insertItem(icons_manager.loadIcon("RegisterUser"),tr("Register &new user"), this, SLOT(registerUser()), 0, -1, index);
	changeMenuId=MainMenu->insertItem(icons_manager.loadIcon("ChangePassMail"),tr("&Change password/email"), this, SLOT(changePassword()), 0, -1, index);
	remindMenuId=MainMenu->insertItem(icons_manager.loadIcon("ChangePass"),tr("Remind &password"), this, SLOT(remindPassword()), 0, -1, index);
	
	icons_manager.registerMenuItem(kadu->mainMenu(), tr("Unregister user"), "UnregisterUser");
	icons_manager.registerMenuItem(kadu->mainMenu(), tr("Register &new user"), "RegisterUser");
	icons_manager.registerMenuItem(kadu->mainMenu(), tr("&Change password/email"), "ChangePassMail");
	icons_manager.registerMenuItem(kadu->mainMenu(), tr("Remind &password"), "ChangePass");

	connect(kadu, SIGNAL(wantRegister()), this, SLOT(registerUser()));

	kdebugf2();
}

AccountManagement::~AccountManagement()
{
	kdebugf();
	
	disconnect(kadu, SIGNAL(wantRegister()), this, SLOT(registerUser()));
	
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
	(new RemindPassword())->start();
}

void AccountManagement::changePassword()
{
	(new ChangePassword())->show();
}

AccountManagement *account_management=NULL;
