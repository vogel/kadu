/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012 Piotr Dąbrowski (ultr@ultr.pl)
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

#include <QtCore/QTimer>
#include <QtGui/QMenu>
#include <QtGui/QTextDocument>

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
	new StatusMenu(MyStatusContainer, false, menu);

	setMenu(menu);
	setIcon(Icon->icon().icon());
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
}

QString StatusButton::prepareDescription(const QString &description) const
{
	QColor color = palette().windowText().color();
	color.setAlpha(128);
	QString colorString = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());

	QString html = Qt::escape(description);
	html.replace('\n', QString::fromUtf8("<span style='color:%1;'> \u21B5</span><br />").arg(colorString));

	return html;
}
