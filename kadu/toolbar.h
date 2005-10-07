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

	private slots:
		void addButtonClicked(int action_index);

	protected:
		void dragEnterEvent(QDragEnterEvent* event);
		void dropEvent(QDropEvent* event);
		void contextMenuEvent(QContextMenuEvent* e);

	public:
		ToolBar(const QString& label, QMainWindow* mainWindow, QWidget* parent);
		void writeToConfig(QDomElement parent_element);
		void loadFromConfig(QDomElement parent_element);
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
		void writeToConfig();
		bool loadFromConfig(QMainWindow* toolbars_parent);
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
