#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <qtoolbar.h>
#include <qvaluelist.h>

#include "dockarea.h"
#include "toolbutton.h"
#include "usergroup.h"

class ToolButton;

/**
	Klasa tworz?ca pasek narz?dziowy
	\class ToolBar
	\brief Pasek narz?dziowy
**/

class ToolBar : public QToolBar
{
	Q_OBJECT

	struct ToolBarAction {
		QString actionName;
		ToolButton *button;
		bool showLabel;
	};

	QValueList<ToolBarAction> ToolBarActions;
	ToolButton *dragButton; /*!< przeci?gany przycisk akcji */

	ToolButton * addButton(Action *action, bool showLabel, ToolButton *after);

private slots:
	/**
		\fn void addButtonClicked()
		Slot dodaj?cy wybrany przycisk
	**/
	void addButtonClicked(int action_index);
	void removeButtonClicked(ToolButton *button);

	/**
		\fn void deleteToolbar()
		Slot obs?uguj?cy usuwanie paska narz?dzi
	**/
	void deleteToolbar ();

	void actionLoaded(const QString &name);
	void actionUnloaded(const QString &actionName);

	void updateButtons();

protected:
	/**
		\fn virtual void dragEnterEvent(QDragEnterEvent* event)
		Funkcja obs?uguj?ca prz?ci?ganie akcji mi?dzy paskami
	**/
	virtual void dragEnterEvent ( QDragEnterEvent* event );

	/**
		\fn virtual void dropEvent(QDropEvent* event)
		Funkcja obs?uguj?ca upuszczenie przycisku na pasku
	**/
	virtual void dropEvent ( QDropEvent* event );

	/**
		\fn virtual void contextMenuEvent(QContextMenuEvent* e)
		Funkcja obs?uguj?ca tworzenie menu kontekstowego paska
	**/
	virtual void contextMenuEvent ( QContextMenuEvent* e );

	/**
		\fn virtual void contextMenuEvent(QContextMenuEvent* e)
		Funkcja obs?uguj?ca przeci?ganie paska narz?dziowego.
	**/
	virtual void moveEvent ( QMoveEvent *e );

public:
	/**
		Konstruktor paska narz?dzi
		\fn ToolBar(QWidget* parent, const char *name)
		\param parent rodzic obiektu
		\param name nazwa obiektu
	**/
	ToolBar ( QWidget* parent, const char *name );

	/**
		\fn ~ToolBar()
		Destruktor paska narz?dzi
	**/
	~ToolBar();

	void addAction(const QString &actionName, bool showLabel, ToolButton *button = 0, bool before = true);
	void removeAction(const QString &actionName);
	void moveAction(const QString &actionName, ToolButton *button);

	/**
		\fn DockArea* dockArea()
		Zwraca wska?nik do rodzica paska (miejsca dokowania)
	**/
	DockArea* dockArea();

	/**
		\fn void loadFromConfig(QDomElement parent_element)
		\param parent_element rodzic obiektu
		Wczytuje dane z pliku konfiguracyjnego
	**/
	void loadFromConfig ( QDomElement parent_element );

	/**
		\fn const UserGroup* selectedUsers() const
		Returns list of users that will be affected by activated action.
		It depends on where the toolbar is located. If toolbar is in chat
		window, selected users are the users in chat. If toolbar is the
		main toolbar, selected users are the selected ones in contact
		list, and so on...
		Returns NULL if toolbar is not connected to user list.
	**/
	const UserGroup* selectedUsers() const;

	/**
		\fn QPopupMenu* createContextMenu(QWidget* parent)
		\param parent rodzic obiektu
		Funkcja tworz?ca menu kontekstowe, umo?liwiaj?ce dodanie
		nowych akcji do paska narz?dziowego.
	**/
	QPopupMenu* createContextMenu ( QWidget* parent );

	/**
		\fn hasAction(QString action_name)
		\param action_name nazwa szukanej akcji
		Funkcja zwraca warto?? boolowsk?, okre?laj?c?, czy akcja
		o podanej nazwie znajduje si? ju? na pasku narz?dzi.
	**/
	bool hasAction (const QString &action_name);

public slots:
	/**
		\fn writeToConfig(QDomElement parent_element)
		\param parent_element rodzic obiektu
		Zapisuje ustawienia paska (jak offset), oraz (po?rednio) 
		akcje znajduj?ce si? na pasku.
	**/
	void writeToConfig ( QDomElement parent_element );

};

#endif
