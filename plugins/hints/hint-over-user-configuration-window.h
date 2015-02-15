/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HINT_OVER_USER_CONFIGURATION_WINDOW_H
#define HINT_OVER_USER_CONFIGURATION_WINDOW_H

#include "buddies/buddy.h"
#include "gui/windows/configuration-window.h"

class QFrame;
class QLabel;
class QTextEdit;

class HintOverUserConfigurationWindow : public ConfigurationWindow
{
	Q_OBJECT

	Buddy ExampleBuddy;

	QFrame *previewFrame;
	QLabel *previewTipLabel;
	QTextEdit *hintSyntax;

	QString bgcolor;
	QString fgcolor;
	QString bdcolor;
	int bdwidth;

private slots:
	void configurationWindowApplied();

	void fontChanged(QFont font);
	void foregroundColorChanged(const QColor &color);
	void backgroundColorChanged(const QColor &color);
	void borderColorChanged(const QColor &color);
	void borderWidthChanged(int width);

	void syntaxChanged();

public:
	HintOverUserConfigurationWindow(Buddy exampleContact);
};

#endif // HINT_OVER_USER_CONFIGURATION_WINDOW_H
