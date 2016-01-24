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

#pragma once

#include "buddies/buddy.h"
#include "gui/windows/configuration-window.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;
class HintManager;
class Parser;
class PathsProvider;

class QFrame;
class QLabel;
class QTextEdit;

class HintOverUserConfigurationWindow : public ConfigurationWindow
{
	Q_OBJECT

public:
	explicit HintOverUserConfigurationWindow(HintManager *hintManager, Buddy exampleContact, ConfigurationWindowDataManager *dataManager);
	virtual ~HintOverUserConfigurationWindow();

private:
	QPointer<Configuration> m_configuration;
	QPointer<Parser> m_parser;
	QPointer<PathsProvider> m_pathsProvider;
	Buddy ExampleBuddy;

	HintManager *m_hintManager;
	QFrame *previewFrame;
	QLabel *previewTipLabel;
	QTextEdit *hintSyntax;

	QString bgcolor;
	QString fgcolor;
	QString bdcolor;
	int bdwidth;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_SET void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_INIT void init();

	void configurationWindowApplied();

	void fontChanged(QFont font);
	void foregroundColorChanged(const QColor &color);
	void backgroundColorChanged(const QColor &color);
	void borderColorChanged(const QColor &color);
	void borderWidthChanged(int width);

	void syntaxChanged();

};
