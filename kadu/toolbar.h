#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <qdockarea.h>
#include <qdom.h>
#include <qdragobject.h>
#include <qmainwindow.h>
#include <qmap.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>

#include "usergroup.h"

class ToolButton : public QToolButton
{
	Q_OBJECT

	private:
		QString ActionName;

	private slots:
		void deleteButtonClicked();

	protected:
		void mouseMoveEvent(QMouseEvent* e);
		void contextMenuEvent(QContextMenuEvent* e);

	public:
		ToolButton(QWidget* parent, const QString& action_name);
		~ToolButton();
		QString actionName();
		void writeToConfig(QDomElement parent_element);
};

class ToolButtonDrag : public QTextDrag
{
	private:
		ToolButton* Button;

	public:
		ToolButtonDrag(ToolButton* button, QWidget* dragSource = 0, const char* name = 0);
};

class ToolBar : public QToolBar
{
	Q_OBJECT

	private:
		ToolButton *dragButton;

	private slots:
		void addButtonClicked(int action_index);

	protected:
		virtual void dragEnterEvent(QDragEnterEvent* event);
		virtual void dropEvent(QDropEvent* event);
		virtual void contextMenuEvent(QContextMenuEvent* e);
		virtual void dragLeaveEvent(QDragLeaveEvent *e);

	public:
		ToolBar(QMainWindow* parent, const QString& label);
		~ToolBar();
		void writeToConfig(QDomElement parent_element);
		void loadFromConfig(QDomElement parent_element);
		/**
			Returns list of users that will be affected by activated action.
			It depends on where the toolbar is located. If toolbar is in chat
			window, selected users are the users in chat. If toolbar is the
			main toolbar, selected users are the selected ones in contact
			list, and so on...
			Returns NULL if toolbar is no connected to user list.
		**/
		const UserGroup* selectedUsers();
};

class DockArea : public QDockArea
{
	Q_OBJECT

	protected:
		void contextMenuEvent(QContextMenuEvent* e);

	private slots:
		void createNewToolbar();

	public:
		DockArea(Orientation o, HandlePosition h = Normal,
			QWidget * parent = 0, const char * name = 0);
		~DockArea();
		void writeToConfig();
		bool loadFromConfig(QMainWindow* toolbars_parent);
		/**
			Returns list of users that will be affected by activated action.
			It depends on where the dockarea is located. If dockarea is in chat
			window, selected users are the users in chat. If dockarea is the
			main window, selected users are the selected ones in contact
			list, and so on...
			Returns NULL if toolbar is no connected to user list.
		**/
		const UserGroup* selectedUsers();

	signals:
		/**
			Signal is emited when dockarea needs to know what users
			will be affected by activated action. It depends on
			where the dockarea is located. If dockarea is in chat window,
			selected users are the users in chat. If dockarea is the
			main window, selected users are the selected ones in contact
			list, and so on...
			Slot should change users pointer. NULL (default) means: do not
			execute action.
		**/
		void selectedUsersNeeded(const UserGroup*& users);
};

/**
	Toolbar Kadu
**/
class MainToolBar : public QToolBar
{
	private:
		struct ToolButton
		{
			QString iconname;
			QString caption, name;
			QObject* receiver;
			QString slot;
			QToolButton* button;
			int position;
		};
		static QValueList<ToolButton> RegisteredToolButtons;
		void createControls();

	public:
		static MainToolBar* instance;
		MainToolBar(QMainWindow* parent);
		~MainToolBar();
		static void registerButton(const QString &iconname, const QString& caption,
			QObject* receiver, const char* slot, const int position=-1, const char* name="");
		static void unregisterButton(const char* name);
		static void registerSeparator(int position=-1);
		static QToolButton* getButton(const char* name);
		static void refreshIcons(const QString &caption=QString::null, const QString &newIconName=QString::null, const QString &newCaption=QString::null);
};

#endif
