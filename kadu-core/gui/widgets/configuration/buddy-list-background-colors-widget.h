/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#ifndef BUDDY_LIST_BACKGROUND_COLORS_WIDGET_H
#define BUDDY_LIST_BACKGROUND_COLORS_WIDGET_H

#include <QtGui/QWidget>

class ColorButton;
class MainConfigurationWindow;

class BuddyListBackgroundColorsWidget : public QWidget
{
	Q_OBJECT

	ColorButton *colorButton;
	ColorButton *alternateColorButton;

	void createGui(MainConfigurationWindow* mainWindow);
	void loadConfiguration();

private slots:
	void configurationApplied();

public:
	explicit BuddyListBackgroundColorsWidget(MainConfigurationWindow *mainWindow);
};

#endif // BUDDY_LIST_BACKGROUND_COLORS_WIDGET_H
