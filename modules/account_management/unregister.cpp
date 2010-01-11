/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QGroupBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "config_file.h"
#include "debug.h"
#include "../modules/gadu_protocol/gadu.h"
#include "icons-manager.h"
#include "message_box.h"
#include "misc/misc.h"
#include "unregister.h"

/**
 * @ingroup account_management
 * @{
 */
Unregister::Unregister(QDialog *parent) : QWidget(parent, Qt::Window),
	uin(0), pwd(0)
{
	kdebugf();

	setWindowTitle(tr("Unregister user"));
	setAttribute(Qt::WA_DeleteOnClose);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget();

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(IconsManager::instance()->loadPixmap("UnregisterWindowIcon"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout(left);
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);

	QWidget *center = new QWidget;

	QLabel *l_info = new QLabel();

	l_info->setText(tr("This dialog box allows you to unregister your account. Be aware of using this "
				"option. <font color=\"red\"><b>It will permanently delete your UIN and you will not be able to use "
				"it later!</b></font>"));
	l_info->setWordWrap(true);
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	// end create main QLabel widgets (icon and app info)

	//our QGroupBox
	QGroupBox *vgb_uinpass = new QGroupBox(tr("UIN and password"));
	QVBoxLayout *uinpass_layout = new QVBoxLayout(vgb_uinpass);
	//end our QGroupBox

	// create needed fields
	uin = new QLineEdit();
	uinpass_layout->addWidget(new QLabel(tr("UIN:")));
	uinpass_layout->addWidget(uin);

	pwd = new QLineEdit();
	pwd->setEchoMode(QLineEdit::Password);
	uinpass_layout->addWidget(new QLabel(tr("Password:")));
	uinpass_layout->addWidget(pwd);
	// end create needed fields

	// buttons
	QWidget *bottom = new QWidget;

	QWidget *blank2 = new QWidget;
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_unregister = new QPushButton(IconsManager::instance()->loadIcon("UnregisterAccountButton"), tr("Unregister"), bottom, "unregister");
	connect(pb_unregister, SIGNAL(clicked()), this, SLOT(doUnregister()));

	QPushButton *pb_close = new QPushButton(IconsManager::instance()->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottom_layout = new QHBoxLayout(bottom);
	bottom_layout->addWidget(blank2);
	bottom_layout->addWidget(pb_unregister);
	bottom_layout->addWidget(pb_close);
	// end buttons

	QVBoxLayout *center_layout = new QVBoxLayout(center);
	center_layout->addWidget(l_info);
	center_layout->addWidget(vgb_uinpass);
	center_layout->setStretchFactor(vgb_uinpass, 1);
	center_layout->addWidget(bottom);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(left);
	layout->addWidget(center);

	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	connect(gadu, SIGNAL(unregistered(bool)), this, SLOT(unregistered(bool)));

 	loadWindowGeometry(this, "General", "UnregisterDialogGeometry", 0, 50, 355, 340);

	kdebugf2();
}

Unregister::~Unregister()
{
	kdebugf();

	saveWindowGeometry(this, "General", "UnregisterDialogGeometry");

	kdebugf2();
}

void Unregister::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void Unregister::doUnregister()
{
	kdebugf();

	if (!uin->text().toUInt() || pwd->text().isEmpty())
	{
		MessageDialog::msg(tr("Please fill out all fields"), false, "Warning", this);
		return;
	}

	setEnabled(false);
	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	gadu->unregisterAccount(uin->text().toUInt(), pwd->text());
	kdebugf2();
}

void Unregister::unregistered(bool ok)
{
	kdebugf();

	if (ok)
	{
		MessageDialog::msg(tr("Unregistation was successful. Now you don't have any GG number :("), false, "Information", this);
		close();
	}
	else
	{
		MessageDialog::msg(tr("An error has occured while unregistration. Please try again later."), false, "Critical", this);
		setEnabled(true);
	}

	kdebugf2();
}

void Unregister::deleteConfig()
{
	kdebugf();

	config_file.writeEntry("General", "UIN", 0);

	qApp->mainWidget()->setWindowTitle(tr("No user"));

	kdebugf2();
}

/** @} */
