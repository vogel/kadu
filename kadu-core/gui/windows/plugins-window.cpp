/*
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "QtGui/QGroupBox"
#include "QtGui/QHBoxLayout"
#include "QtGui/QLabel"
#include "QtGui/QLineEdit"
#include "QtGui/QPushButton"
#include "QtGui/QListView"
#include "QtGui/QTreeWidgetItem"
#include "QtGui/QVBoxLayout"
#include "QtGui/QWidget"
#include <QtGui/QKeyEvent>
#include <QtGui/QScrollBar>

#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "debug.h"
#include "gui/widgets/plugin-list-view.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "activate.h"

#include "plugins-window.h"


PluginsWindow * PluginsWindow::Instance = 0;

PluginsWindow * PluginsWindow::instance()
{
	return Instance;
}

void PluginsWindow::show()
{
	if (!Instance)
		Instance = new PluginsWindow();

	_activateWindow(Instance);
}

PluginsWindow::PluginsWindow()
	: QWidget(0, Qt::Window), DesktopAwareObject(this),
	PluginsList(0), ModuleInfo(0)
{
	kdebugf();

	setWindowRole("kadu-modules");

	setWindowTitle(tr("Manage plugins"));
	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(10);
	layout->setSpacing(10);

	PluginListView *PluginsList = new PluginListView(this);
	layout->addWidget(PluginsList);

	setLayout(layout);

	loadWindowGeometry(this, "General", "PluginsWindowGeometry", 0, 50, 600, 620);

	kdebugf2();
}

PluginsWindow::~PluginsWindow()
{
	kdebugf();
	Instance = 0;
 	saveWindowGeometry(this, "General", "PluginsWindowGeometry");
	kdebugf2();
}

void PluginsWindow::loadItemPlugin(Plugin *itemPlugin)
{
	PluginsManager::instance()->activatePlugin(itemPlugin, PluginActivationReasonUserRequest);

	// do it unconditionally as dependent plugins might have been loaded even if the requested one failed to do so
	//refreshList();

	ConfigurationManager::instance()->flush();
}

void PluginsWindow::unloadItemPlugin(Plugin *itemPlugin)
{
	if (PluginsManager::instance()->deactivatePlugin(itemPlugin, PluginDeactivationReasonUserRequest))
	{
	//	refreshList();

		ConfigurationManager::instance()->flush();
	}
}

void PluginsWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		event->accept();
		close();
	}
	else
		QWidget::keyPressEvent(event);
}
