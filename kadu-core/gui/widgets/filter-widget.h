/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QtWidgets/QWidget>
#include "exports.h"

class QAbstractItemView;
class QLabel;
class QLineEdit;

class KADUAPI FilterWidget : public QWidget
{
	Q_OBJECT

	QLabel *Label;
	QLineEdit *NameFilterEdit;

	QAbstractItemView *View;
	bool AutoVisibility;

	void updateVisibility();
	bool sendKeyEventToView(QKeyEvent *);

private slots:
	void filterTextChanged(const QString &);

protected:
	virtual void keyPressEvent(QKeyEvent *);

public:
	explicit FilterWidget(QWidget *parent = 0);
	virtual ~FilterWidget();

	void setLabel(const QString &label);
	void setFilter(const QString &filter);
	void setView(QAbstractItemView *view);
	void setAutoVisibility(bool autoVisibility);

	QString filterText() const;

signals:
	void textChanged(const QString &text);

};

#endif // FILTERWIDGET_H
