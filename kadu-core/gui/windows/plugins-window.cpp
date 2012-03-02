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

#include "QtGui/QDialogButtonBox"
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

#include "debug.h"
#include "gui/widgets/plugin-list-widget.h"
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
	: QWidget(0, Qt::Window), DesktopAwareObject(this)
{
	kdebugf();

	setWindowRole("kadu-modules");

	setWindowTitle(tr("Configure Plugins"));
	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(10);
	layout->setSpacing(10);

	PluginsList = new PluginListWidget(this);
	layout->addWidget(PluginsList);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("OK"), this);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);

	QPushButton *applyButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Apply"), this);
	applyButton->setEnabled(false);
	buttons->addButton(applyButton, QDialogButtonBox::ApplyRole);
	connect(PluginsList, SIGNAL(changed(bool)), applyButton, SLOT(setEnabled(bool)));

	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Close"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(applyAndClose()));
	connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(applyChanges()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	layout->addWidget(buttons);

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

void PluginsWindow::applyChanges()
{
	PluginsList->applyChanges();
}

void PluginsWindow::applyAndClose()
{
	applyChanges();
	close();
}

