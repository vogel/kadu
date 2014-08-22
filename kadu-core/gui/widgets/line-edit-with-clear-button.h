/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "exports.h"

class LineEditClearButton;

class KADUAPI LineEditWithClearButton : public QLineEdit
{
	Q_OBJECT

	bool ClearButtonVisible;

	LineEditClearButton *ClearButton;

	bool WideEnoughForClear;
	bool ClickInClear;

	void createClearButton();
	void updateClearButton();
	bool canShowClearButton();

private slots:
	void updateClearButtonIcon();

protected:
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

public:
	explicit LineEditWithClearButton(QWidget *parent = 0);
	virtual ~LineEditWithClearButton();

	bool isClearButtonVisible() const { return ClearButtonVisible; }
	void setClearButtonVisible(bool clearButtonVisible);

	virtual void setReadOnly(bool readonly);
	virtual void setEnabled(bool enabled);

signals:
	void cleared();

};

#endif // LINE_EDIT_WITH_CLEAR_BUTTON_H
