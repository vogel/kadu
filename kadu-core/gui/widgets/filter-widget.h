/*
 * %kadu copyright begin%
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

#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QtGui/QWidget>

#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif

class QLineEdit;

class FilterWidget : public QWidget
{
	Q_OBJECT

#ifdef Q_OS_MAC
	CFStringRef searchFieldText;
	HIViewRef searchField;
#else
	QLineEdit *NameFilterEdit;
#endif
	void setFocus();

public:
	FilterWidget(QWidget *parent);
	~FilterWidget();

#ifdef Q_OS_MAC
	QString text(void);
	void setText(const QString &text);
	void emitTextChanged();
	void clear(void);
	void activate(void);
	QSize sizeHint (void) const;
#endif
};

#endif // FILTERWIDGET_H
