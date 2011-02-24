/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
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

#include <QtGui/QWidget>
#include "exports.h"

#ifdef Q_OS_MAC
#	include <Carbon/Carbon.h>
#endif

class BuddiesListView;

class QLineEdit;

class KADUAPI FilterWidget : public QWidget
{
	Q_OBJECT

#ifdef Q_OS_MAC
	CFStringRef searchFieldText;
	HIViewRef searchField;
#else
	QLineEdit *NameFilterEdit;
#endif
	BuddiesListView *View;
	bool sendKeyEventToView(QKeyEvent *);


private slots:
	void filterTextChanged(const QString &);

protected:
	virtual void keyPressEvent(QKeyEvent *);

public:
	explicit FilterWidget(QWidget *parent = 0);
	virtual ~FilterWidget();

	void setFilter(const QString &filter);
	void setView(BuddiesListView *view);

signals:
	void textChanged(const QString &text);

#ifdef Q_OS_MAC

public:
	void activate(void);
	QSize sizeHint (void) const;
	QString text(void) const;

public slots:
	void clear(void);
	void setText(const QString &text);
	void emitTextChanged(void);

#endif

};

#endif // FILTERWIDGET_H
