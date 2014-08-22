/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include <QtGui/QTextDocument>
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
		QToolButton(parent), MyStatusContainer(statusContainer), DisplayStatusName(false), MenuTitleAction{nullptr}
{
	Icon = new StatusIcon(MyStatusContainer, this);

	createGui();
	setPopupMode(InstantPopup);

	statusUpdated();
	connect(MyStatusContainer, SIGNAL(statusUpdated(StatusContainer *)), this, SLOT(statusUpdated(StatusContainer *)));
	connect(Icon, SIGNAL(iconUpdated(KaduIcon)), this, SLOT(iconUpdated(KaduIcon)));
}

StatusButton::~StatusButton()
{
}

void StatusButton::createGui()
{
	QMenu *menu = new QMenu(this);
	if (!MyStatusContainer->statusContainerName().isEmpty())
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
	QString tooltip("<table>");

	if (DisplayStatusName)
	{
		setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		setText(MyStatusContainer->status().displayName());
	}
	else
	{
		if (MainConfigurationHolder::instance()->isSetStatusPerIdentity())
		{
			setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			setText(MyStatusContainer->statusContainerName());
			tooltip +=
				QString("<tr><td align='right' style='font-weight:bold; white-space:nowrap;'>%1:</td><td style='white-space:nowrap;'>%2</td></tr>")
					.arg(tr("Identity"), MyStatusContainer->statusContainerName());
		}
		else
		{
			setToolButtonStyle(Qt::ToolButtonIconOnly);
			setText(QString());
			tooltip +=
				QString("<tr><td align='right' style='font-weight:bold; white-space:nowrap;'>%1:</td><td style='white-space:nowrap;'>%2</td></tr>")
					.arg(tr("Account"), MyStatusContainer->statusContainerName());
		}
	}

	tooltip +=
		QString("<tr><td align='right' style='font-weight:bold; white-space:nowrap;'>%1:</td><td style='white-space:nowrap;'>%2</td></tr>")
			.arg(tr("Status"), MyStatusContainer->status().displayName());
	tooltip
		+= QString("<tr><td align='right' style='font-weight:bold; white-space:nowrap;'>%1:</td><td>%2</td></tr>")
			.arg(tr("Description"), prepareDescription(MyStatusContainer->status().description()));

	tooltip += QString("</table>");

	setToolTip(tooltip);
}

void StatusButton::statusUpdated(StatusContainer *container)
{
	Q_UNUSED(container)

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
	if (MenuTitleAction)
		MenuTitleAction->setIcon(icon.icon());
}

QString StatusButton::prepareDescription(const QString &description) const
{
	QColor color = palette().windowText().color();
	color.setAlpha(128);
	QString colorString = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());

	QString html = Qt::escape(description);
	html.replace('\n', QString(QLatin1String("<span style='color:%1;'> ") + QChar(0x21B5) + QLatin1String("</span><br />")).arg(colorString));

	return html;
}

#include "moc_status-button.cpp"
