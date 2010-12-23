/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef CONFIG_GROUP_BOX_H
#define CONFIG_GROUP_BOX_H

#include <QtCore/QObject>

#include "exports.h"

class ConfigTab;
class QGridLayout;
class QWidget;
class QGroupBox;
class QLayout;

/**
	@class ConfigGroupBox
	@author Vogel
	@short GroupBox w oknie konfiguracji

	GroupBox w oknie konfiguracji definiowany jest przez następujący tag:
	<code>
		&lt;group-box caption="tytuł" id="id"&gr;
			...
		&lt;/group-box&gt;
	</code>

	GroupBox może znajdować się tylko wewnątrz tagu tab. W jego wnętrzu
	mogą zawierać się dowolne tagi widgetów konfigruacyjnych.

	Dodatkowo, GroupBox'a można stworzyć (lub, jeżeli istnieje, uzyskać)
	wywołując funkcję configGroupBox(section, tab, groupBox) z okna konfiguracyjnego.
	Do tak uzyskanego GroupBox'a można dodawać dowolne widgety (@see addWidget,
	@see addWidgets).
 **/

class KADUAPI ConfigGroupBox : public QObject
{
	Q_OBJECT

	QString Name;
	ConfigTab *MyConfigTab;

	QGroupBox *GroupBox;
	QWidget *Container;
	QGridLayout *GridLayout;

public:
	ConfigGroupBox(const QString &name, ConfigTab *configTab, QGroupBox *groupBox);
	virtual ~ConfigGroupBox();

	const QString & name() const { return Name; }
	QWidget * widget() const { return Container; }

	/**
		Dodaje widget do GroupBoxa.
		@param widget dodawany widget (nie może być NULL)
		@param fullSpace kiedy true, dodany widget zajmuje całą szerokość GroupBox'a,
			w przeciwnym wypadku tylko prawą jego część
	 **/
	void addWidget(QWidget *widget, bool fullSpace = false);
	/**
		Dodaje 2 widget do GroupBoxa, jeden po lewej stronie, drugi po prawej.
		@param widget1 widget dodawany z lewej strony (zazwyczaj etykieta)
		@param widget2 widget dodawany z prawej strony
	 **/
	void addWidgets(QWidget *widget1, QWidget *widget2);

	void insertWidget(int pos, QWidget *widget, bool fullSpace = false);

	void insertWidgets(int pos, QWidget *widget1, QWidget *widget2);

	/**
		Zwraca true, gdy GroupBox nie posiada w sobie żadnych widgetów.
		@return true, gdy GroupBox nie posiada w sobie żadnych widgetów
	 **/
	bool empty() const;

};

#endif
