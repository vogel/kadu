/*
 * %kadu copyright begin%
 * Copyright 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#ifndef KADU_MODULES_WINDOW
#define KADU_MODULES_WINDOW

#include <QtCore/QObject>
#include <QtGui/QWidget>

class QCheckBox;
class QKeyEvent;
class QLabel;
class QTreeWidget;
class QTreeWidgetItem;

/**
	Klasa reprezentuj�ca okno dialogowe, s�u��ce do �adowanie i wy�adowywania modu��w.
	�adowanie/wy�adowywanie, oraz inne operacje na modu�ach z poziomu C++ dokonywane
	s� za pomoc� klasy ModulesManager. Ta klasa tworzy jedynie okno dialogowe, b�d�ce
	interfejsem do ww. klasy dla u�ytkownika Kadu.
	\class ModulesWindow
	\brief "Zarz�dca modu��w"
**/
class ModulesWindow : public QWidget
{
	Q_OBJECT

	QTreeWidget *lv_modules;
	QLabel *l_moduleinfo;
	QCheckBox *hideBaseModules;

	QTreeWidgetItem * getSelected();

	void loadItem(const QString &item);
	void unloadItem(const QString &item);

private slots:
	void moduleAction(QTreeWidgetItem *);
	void itemsChanging();
	void getInfo();
	void refreshList();

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	/**
		\fn ModulesWindow()
		Konstruktor standardowy.
	**/
	ModulesWindow(QWidget *parent = 0);
	~ModulesWindow();

};

#endif
