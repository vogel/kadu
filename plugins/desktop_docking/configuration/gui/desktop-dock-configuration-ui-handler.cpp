/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QSpinBox>

#include "gui/widgets/configuration/configuration-widget.h"

#include "gui/windows/desktop-dock-window.h"
#include "desktop-dock.h"

#include "desktop-dock-configuration-ui-handler.h"

DesktopDockConfigurationUiHandler * DesktopDockConfigurationUiHandler::Instance = 0;

void DesktopDockConfigurationUiHandler::createInstance()
{
	if (!Instance)
		Instance = new DesktopDockConfigurationUiHandler();
}

void DesktopDockConfigurationUiHandler::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

DesktopDockConfigurationUiHandler* DesktopDockConfigurationUiHandler::instance()
{
	return Instance;
}

DesktopDockConfigurationUiHandler::DesktopDockConfigurationUiHandler() :
		XSpinBox(0), YSpinBox(0)
{
}

DesktopDockConfigurationUiHandler::~DesktopDockConfigurationUiHandler()
{
}

void DesktopDockConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widget()->widgetById("desktop_docking/transparent"), SIGNAL(toggled(bool)),
			mainConfigurationWindow->widget()->widgetById("desktop_docking/color"), SLOT(setDisabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("desktop_docking/move"), SIGNAL(clicked()),
			DesktopDock::instance()->dockWindow(), SLOT(startMoving()));

	XSpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("desktop_docking/x"));
	YSpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("desktop_docking/y"));

	connect(DesktopDock::instance()->dockWindow(), SIGNAL(dropped(QPoint)),
			this, SLOT(dockWindowDropped(QPoint)));
}

void DesktopDockConfigurationUiHandler::dockWindowDropped(const QPoint &pos)
{
	DesktopDockWindow *window = DesktopDock::instance()->dockWindow();

	window->move(pos);
	window->update();
	window->show();

	QDesktopWidget *fullDesktop = QApplication::desktop();
	int posX, posY;

	if (pos.x() > fullDesktop->width() - window->pixmap()->width())
		posX = fullDesktop->width() - window->pixmap()->width();
	else
		posX = pos.x();

	if (pos.y() > fullDesktop->height() - window->pixmap()->height())
		posY = fullDesktop->height() - window->pixmap()->height();
	else
		posY = pos.y();

	XSpinBox->setValue(posX);
	YSpinBox->setValue(posY);
}

#include "moc_desktop-dock-configuration-ui-handler.cpp"
