/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2005, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QtGui/QDrag>
#include <QtGui/QToolBar>
#include <QtXml/QDomElement>

#include "configuration/configuration-aware-object.h"

#include "exports.h"

class QMenu;
class QToolButton;

class UserGroup;

/**
	Klasa tworz?ca pasek narz?dziowy
	\class ToolBar
	\brief Pasek narz?dziowy
**/

class KADUAPI ToolBar : public QToolBar, public ConfigurationAwareObject
{
	Q_OBJECT

	friend class DisabledActionsWatcher;

	// TODO: ugly hack
	QToolButton *currentButton;
	QAction *IconsOnly, *TextOnly, *Text, *TextUnder;

	struct ToolBarAction {
		QString actionName;
		QAction *action;
		QToolButton *button;
		Qt::ToolButtonStyle style;

		bool operator == (struct ToolBarAction action) {
			return actionName == action.actionName;
		}
	};

	QList<ToolBarAction> ToolBarActions;

	// TODO: remove, used only when reading from config
	int XOffset;
	int YOffset;

	QPoint MouseStart;

	void addAction(const QString &actionName, Qt::ToolButtonStyle style, QAction *after = 0);

	static QMap<QString, QList<ToolBarAction> > DefaultActions;

private slots:
	/**
		\fn void addButtonClicked()
		Slot dodaj?cy wybrany przycisk
	**/
	void addButtonClicked(QAction *action);
	void deleteButton();

	/**
		\fn void deleteToolbar()
		Slot obs?uguj?cy usuwanie paska narz?dzi
	**/
	void deleteToolbar();

	void setBlockToolbars(bool checked);

	void actionLoaded(const QString &name);
	void actionUnloaded(const QString &actionName);

	void updateButtons();
	void buttonPressed();

	QMenu * createContextMenu(QToolButton *button);

	void slotContextIcons();
	void slotContextText();
	void slotContextTextUnder(); 
	void slotContextTextRight();

	void slotContextAboutToShow();

protected:
	/**
		\fn virtual void dragEnterEvent(QDragEnterEvent* event)
		Funkcja obs?uguj?ca prz?ci?ganie akcji mi?dzy paskami
	**/
	virtual void dragEnterEvent(QDragEnterEvent *event);

	/**
		\fn virtual void dropEvent(QDropEvent* event)
		Funkcja obs?uguj?ca upuszczenie przycisku na pasku
	**/
	virtual void dropEvent(QDropEvent *event);

	/**
		\fn virtual void contextMenuEvent(QContextMenuEvent* e)
		Funkcja obs?uguj?ca tworzenie menu kontekstowego paska
	**/
	virtual void contextMenuEvent(QContextMenuEvent *e);

	virtual void mouseMoveEvent(QMouseEvent* e);

	virtual void configurationUpdated();

public:
	static bool isBlockToolbars();

	/**
		Konstruktor paska narz?dzi
		\fn ToolBar(QWidget* parent, const char *name)
		\param parent rodzic obiektu
		\param name nazwa obiektu
	**/
	ToolBar(QWidget *parent);

	/**
		\fn ~ToolBar()
		Destruktor paska narz?dzi
	**/
	~ToolBar();

 	void deleteAction(const QString &actionName);
	void moveAction(const QString &actionName, Qt::ToolButtonStyle style, QAction *after);

	/**
		\fn void loadFromConfig(QDomElement parent_element)
		\param parent_element rodzic obiektu
		Wczytuje dane z pliku konfiguracyjnego
	**/
	void loadFromConfig(QDomElement parent_element);

	/**
		\fn hasAction(QString action_name)
		\param action_name nazwa szukanej akcji
		Funkcja zwraca warto?? boolowsk?, okre?laj?c?, czy akcja
		o podanej nazwie znajduje si? ju? na pasku narz?dzi.
	**/
	bool hasAction (const QString &action_name);

	int xOffset() { return XOffset; }
	int yOffset() { return YOffset; }

public slots:
	/**
		\fn writeToConfig(QDomElement parent_element)
		\param parent_element rodzic obiektu
		Zapisuje ustawienia paska (jak offset), oraz (po?rednio) 
		akcje znajduj?ce si? na pasku.
	**/
	void writeToConfig(QDomElement parent_element);

};

class KADUAPI ActionDrag : public QDrag
{

public:
	ActionDrag(const QString &actionName, Qt::ToolButtonStyle style, QWidget* dragSource = 0);
	static bool decode(QDropEvent *event, QString &actionName, Qt::ToolButtonStyle &style);
};

#endif
