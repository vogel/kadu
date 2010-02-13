/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2005 Roman Krzystyniak (Ron_K@tlen.pl)
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

#include <QtGui/QGroupBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "config_file.h"
#include "debug.h"
#include "../modules/gadu_protocol/gadu.h"
#include "icons-manager.h"
#include "message_box.h"
#include "misc/misc.h"
#include "remind_password.h"

/**
 * @ingroup account_management
 * @{
 */
RemindPassword::RemindPassword(QDialog *parent) : QWidget(parent, Qt::Window),
	emailedit(0)
{
	kdebugf();

	setWindowTitle(tr("Remind password"));
	setAttribute(Qt::WA_DeleteOnClose);

	// create main QLabel widgets (icon and app info)
	QWidget *left = new QWidget();

	QLabel *l_icon = new QLabel;
	l_icon->setPixmap(IconsManager::instance()->loadPixmap("RemindPasswordWindowIcon"));

	QWidget *blank = new QWidget;
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBoxLayout *left_layout = new QVBoxLayout(left);
	left_layout->addWidget(l_icon);
	left_layout->addWidget(blank);

	QWidget *center = new QWidget;

	QLabel *l_info = new QLabel();

	l_info->setText(tr("This dialog box allows you to ask server to remind your current password."));
	l_info->setWordWrap(true);
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	// end create main QLabel widgets (icon and app info)

	//our QVGroupBox
	QGroupBox *vgb_email = new QGroupBox(tr("Email"));
	QVBoxLayout *email_layout = new QVBoxLayout(vgb_email);
	//end our QGroupBox

	// create needed fields
	emailedit = new QLineEdit();
	email_layout->addWidget(new QLabel(tr("Email (as during registration):")));
	email_layout->addWidget(emailedit);
	// end create needed fields

	// buttons
	QWidget *bottom = new QWidget;

	QWidget *blank2 = new QWidget;
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	QPushButton *pb_ok = new QPushButton(IconsManager::instance()->loadIcon("RemindPasswordButton"), tr("OK"), bottom, "ok");
	connect(pb_ok, SIGNAL(clicked()), this, SLOT(start()));

	QPushButton *pb_close = new QPushButton(IconsManager::instance()->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *bottom_layout = new QHBoxLayout(bottom);
	bottom_layout->addWidget(blank2);
	bottom_layout->addWidget(pb_ok);
	bottom_layout->addWidget(pb_close);
	// end buttons

	QVBoxLayout *center_layout = new QVBoxLayout(center);
	center_layout->addWidget(l_info);
	center_layout->addWidget(vgb_email);
	center_layout->addWidget(bottom);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(left);
	layout->addWidget(center);

	loadWindowGeometry(this, "General", "RemindPasswordDialogGeometry", 0, 50, 355, 200);
	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	connect(gadu, SIGNAL(reminded(bool)), this, SLOT(reminded(bool)));

	kdebugf2();
}

RemindPassword::~RemindPassword()
{
	kdebugf();

	saveWindowGeometry(this, "General", "RemindPasswordDialogGeometry");

	kdebugf2();
}

void RemindPassword::keyPressEvent(QKeyEvent* ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void RemindPassword::start()
{
	kdebugf();

	setEnabled(false);
	GaduProtocol *gadu = dynamic_cast<GaduProtocol *>(AccountManager::instance()->defaultAccount()->protocol());
	gadu->remindPassword(config_file.readNumEntry("General", "UIN"), emailedit->text());

	kdebugf2();
}

void RemindPassword::reminded(bool ok)
{
	kdebugf();

	if (ok)
		MessageDialog::msg(tr("Your password has been send on your email"), false, "32x32/dialog-information.png", this);
	else
		MessageDialog::msg(tr("Error during remind password"), false, "32x32/dialog-error_big.png", this);
	close();

	kdebugf2();
}

/** @} */
