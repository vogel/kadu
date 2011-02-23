/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef LINE_EDIT_WITH_CLEAR_BUTTON_H
#define LINE_EDIT_WITH_CLEAR_BUTTON_H

#include <QtGui/QLineEdit>

class LineEditClearButton;

class LineEditWithClearButton : public QLineEdit
{
	Q_OBJECT

	LineEditClearButton *ClearFilterButton;

	bool WideEnoughForClear;
	int Overlap;
	bool ClickInClear;

	void updateClearButton();

private slots:
	void updateClearButtonIcon(const QString &text);

protected:
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

public:
	explicit LineEditWithClearButton(QWidget *parent = 0);
	virtual ~LineEditWithClearButton();

};

#endif // LINE_EDIT_WITH_CLEAR_BUTTON_H
