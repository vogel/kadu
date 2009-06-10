/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

	struct ToolBarAction {
		QString actionName;
		QAction *action;
		QToolButton *button;
		bool showLabel;

		bool operator == (struct ToolBarAction action) {
			return actionName == action.actionName;
		}
	};

	QList<ToolBarAction> ToolBarActions;

	// TODO: remove, used only when reading from config
	int XOffset;
	int YOffset;

	QPoint MouseStart;

	void addAction(const QString &actionName, bool showLabel, QAction *after = 0);

	static QMap<QString, QList<ToolBarAction> > DefaultActions;

private slots:
	/**
		\fn void addButtonClicked()
		Slot dodaj?cy wybrany przycisk
	**/
	void addButtonClicked(QAction *action);
	void showTextLabel();
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
 	void moveAction(const QString &actionName, bool showLabel, QAction *after);

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
	ActionDrag(const QString &actionName, bool showLabel, QWidget* dragSource = 0);
	static bool decode(QDropEvent *event, QString &actionName, bool &showLabel);
};

#endif
