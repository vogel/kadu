#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <qtoolbar.h>
#include "dockarea.h"
#include "toolbutton.h"
#include "usergroup.h"

class ToolButton;

/**
	Klasa tworz±ca pasek narzêdziowy
	\class ToolBar
	\brief Pasek narzêdziowy
**/

class ToolBar : public QToolBar
{
	Q_OBJECT

	private:
		ToolButton *dragButton; /*!< przeci±gany przycisk akcji */

	private slots:
		/**
			\fn void addButtonClicked()
			Slot dodaj±cy wybrany przycisk
		**/
		void addButtonClicked(int action_index);

		/**
			\fn void deleteToolbar()
			Slot obs³uguj±cy usuwanie paska narzêdzi
		**/
		void deleteToolbar(void);

	protected:
		/**
			\fn virtual void dragEnterEvent(QDragEnterEvent* event)
			Funkcja obs³uguj±ca przêci±ganie akcji miêdzy paskami
		**/
		virtual void dragEnterEvent(QDragEnterEvent* event);

		/**
			\fn virtual void dropEvent(QDropEvent* event)
			Funkcja obs³uguj±ca upuszczenie przycisku na pasku
		**/
		virtual void dropEvent(QDropEvent* event);

		/**
			\fn virtual void contextMenuEvent(QContextMenuEvent* e)
			Funkcja obs³uguj±ca tworzenie menu kontekstowego paska
		**/
		virtual void contextMenuEvent(QContextMenuEvent* e);

		/**
			\fn virtual void dragLeaveEvent(QDragLeaveEvent *e)
			Funkcja obs³uguj±ca upuszczanie przycisku
		**/
		virtual void dragLeaveEvent(QDragLeaveEvent *e);

		/**
			\fn virtual void contextMenuEvent(QContextMenuEvent* e)
			Funkcja obs³uguj±ca przeci±ganie paska narzêdziowego.
		**/
		virtual void moveEvent(QMoveEvent *e);

	public:
		/**
			Konstruktor paska narzêdzi
			\fn ToolBar(QWidget* parent, const char *name)
			\param parent rodzic obiektu
			\param name nazwa obiektu
		**/
		ToolBar(QWidget* parent, const char *name);

		/**
			\fn ~ToolBar()
			Destruktor paska narzêdzi
		**/
		~ToolBar();

		/**
			\fn DockArea* dockArea()
			Zwraca wska¼nik do rodzica paska (miejsca dokowania)
		**/
		DockArea* dockArea();

		/**
			\fn void loadFromConfig(QDomElement parent_element)
			\param parent_element rodzic obiektu
			Wczytuje dane z pliku konfiguracyjnego
		**/
		void loadFromConfig(QDomElement parent_element);

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
			Funkcja tworz±ca menu kontekstowe, umo¿liwiaj±ce dodanie
			nowych akcji do paska narzêdziowego.
		**/
		QPopupMenu* createContextMenu(QWidget* parent);

		/**
			\fn hasAction(QString action_name)
			\param action_name nazwa szukanej akcji
			Funkcja zwraca warto¶æ boolowsk±, okre¶laj±c±, czy akcja
			o podanej nazwie znajduje siê ju¿ na pasku narzêdzi.
		**/
		bool hasAction(QString action_name);

	public slots:
		/**
			\fn writeToConfig(QDomElement parent_element)
			\param parent_element rodzic obiektu
			Zapisuje ustawienia paska (jak offset), oraz (po¶rednio) 
			akcje znajduj±ce siê na pasku.
		**/
		void writeToConfig(QDomElement parent_element);
};

#endif
