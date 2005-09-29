#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <qaction.h>
#include <qdockarea.h>
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
	protected:
		void mouseMoveEvent(QMouseEvent* e);
		void contextMenuEvent(QContextMenuEvent* e);

	public:
		ToolButton(QWidget* parent, const char* name = 0);
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

	private slots:
		void addButtonClicked(int action_index);

	protected:
		void dragEnterEvent(QDragEnterEvent* event);
		void dropEvent(QDropEvent* event);
		void contextMenuEvent(QContextMenuEvent* e);

	public:
		ToolBar(const QString& label, QMainWindow* mainWindow, QWidget* parent);
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
};

class Action : public QAction
{
	Q_OBJECT

	private:
		QValueList<ToolButton*> ToolButtons;
		struct ToggleStateStruct
		{
			UserListElements elems;
			bool state;
		};
		QValueList<ToggleStateStruct> ToggleState;

	private slots:
		void toolButtonClicked();
		void toolButtonDestroyed(QObject* obj);

	public:
		Action(const QIconSet& icon, const QString& text, const char* name,
			QKeySequence accel = QKeySequence());
		ToolButton* addToToolbar(ToolBar* toolbar);
		int addToPopupMenu(QPopupMenu* menu, bool connect_signal = true);
		QValueList<ToolButton*> toolButtonsForUserListElements(
			const UserListElements& users);
		bool isOn(const UserListElements& users);
		void setOn(const UserListElements& users, bool on);
		void setPixmaps(const UserListElements& users, const QPixmap& pixmap);

	signals:
		void addedToToolbar(ToolButton* button, ToolBar* toolbar,
			const UserListElements& users);
		/**
			is_on jest zawsze ustawione na false jesli akcja nie jest typu "toggle".
		**/
		void activated(const UserGroup* users, const QWidget* source, bool is_on);
};

class Actions : public QMap<QString, Action*>
{
	public:
		Actions();
};

extern Actions KaduActions;

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
