/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtGui/QMenu>
#include <QtGui/QWidgetAction>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "configuration/main-configuration-holder.h"
#include "gui/status-icon.h"
#include "gui/widgets/status-menu.h"
#include "icons/kadu-icon.h"
#include "protocols/protocol.h"
#include "status/status-container.h"

#include "status-button.h"

StatusButton::StatusButton(StatusContainer *statusContainer, QWidget *parent) :
		QToolButton(parent), MyStatusContainer(statusContainer), DisplayStatusName(false)
{
	Icon = new StatusIcon(MyStatusContainer, this);

	createGui();
	setPopupMode(InstantPopup);

	statusUpdated();
	connect(MyStatusContainer, SIGNAL(statusUpdated()), this, SLOT(statusUpdated()));
	connect(Icon, SIGNAL(iconUpdated(KaduIcon)), this, SLOT(iconUpdated(KaduIcon)));
}

StatusButton::~StatusButton()
{
}

void StatusButton::createGui()
{
	QMenu *menu = new QMenu(this);
	addTitleToMenu(MyStatusContainer->statusContainerName(), menu);
	new StatusMenu(MyStatusContainer, false, menu);

	setMenu(menu);
	setIcon(Icon->icon().icon());
}

void StatusButton::addTitleToMenu(const QString &title, QMenu *menu)
{
	MenuTitleAction = new QAction(menu);
	QFont font = MenuTitleAction->font();
	font.setBold(true);

	MenuTitleAction->setFont(font);
	MenuTitleAction->setText(title);
	MenuTitleAction->setIcon(MyStatusContainer->statusIcon().icon());

	QWidgetAction *action = new QWidgetAction(this);
	action->setObjectName("status_menu_title");
	QToolButton *titleButton = new QToolButton(this);
	titleButton->installEventFilter(this); // prevent clicks on the title of the menu
	titleButton->setDefaultAction(MenuTitleAction);
	titleButton->setDown(true); // prevent hover style changes in some styles
	titleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	action->setDefaultWidget(titleButton);

	menu->addAction(action);
}

bool StatusButton::eventFilter(QObject *object, QEvent *event)
{
	Q_UNUSED(object);

	if (event->type() == QEvent::ActionChanged || event->type() == QEvent::Paint || event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
		return false;

	event->accept();

	return true;
}

void StatusButton::updateStatus()
{
	if (DisplayStatusName)
	{
		setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		setText(MyStatusContainer->status().displayName());
		setToolTip(QString());
	}
	else
	{
		if (MainConfigurationHolder::instance()->isSetStatusPerIdentity())
		{
			setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			setText(MyStatusContainer->statusContainerName());
			setToolTip(QString());
		}
		else
		{
			setToolButtonStyle(Qt::ToolButtonIconOnly);
			setText(QString());
			setToolTip(MyStatusContainer->statusContainerName());
		}
	}
}

void StatusButton::statusUpdated()
{
	updateStatus();
}

void StatusButton::configurationUpdated()
{
	updateStatus();
}

void StatusButton::setDisplayStatusName(bool displayStatusName)
{
	if (DisplayStatusName != displayStatusName)
	{
		DisplayStatusName = displayStatusName;
		updateStatus();
	}
}

void StatusButton::iconUpdated(const KaduIcon &icon)
{
	setIcon(icon.icon());
	MenuTitleAction->setIcon(icon.icon());
}
