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
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_MODULES_WINDOW
#define KADU_MODULES_WINDOW

#include <QtCore/QObject>
#include <QtGui/QWidget>

#include "os/generic/desktop-aware-object.h"
#include "plugins/plugin-info.h"
#include "plugins/plugin.h"
#include "plugins/plugins-manager.h"

class QCheckBox;
class QKeyEvent;
class QLabel;
class QListView;
class QTreeWidgetItem;

class PluginListWidget;
class QLineEdit;
class KCategorizedView;


class PluginsWindow : public QWidget, DesktopAwareObject
{
	Q_OBJECT

	static PluginsWindow *Instance;

	PluginListWidget *PluginsList;

	PluginsWindow();
	virtual ~PluginsWindow();

private slots:
	void applyChanges();
	void applyAndClose();

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	static PluginsWindow * instance();
	static void show();

};

#endif // KADU_MODULES_WINDOW
