/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HINT_OVER_USER_CONFIGURATION_WINDOW_H
#define HINT_OVER_USER_CONFIGURATION_WINDOW_H

#include "gui/windows/configuration-window.h"
#include "buddies/buddy.h"

class QFrame;
class QLabel;
class QTextEdit;

class HintOverUserConfigurationWindow : public ConfigurationWindow
{
	Q_OBJECT

	Contact ExampleContact;

	QFrame *previewFrame;
	QLabel *previewIconLabel;
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
	void iconSizeChanged(int index);

	void syntaxChanged();

public:
	HintOverUserConfigurationWindow(Contact exampleContact);
};

#endif // HINT_OVER_USER_CONFIGURATION_WINDOW_H
