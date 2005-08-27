#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <qmainwindow.h>
#include <qstring.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>

/**
	Toolbar Kadu
**/
class ToolBar : public QToolBar
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
		static ToolBar* instance;
		ToolBar(QMainWindow* parent);
		~ToolBar();
		static void registerButton(const QString &iconname, const QString& caption,
			QObject* receiver, const char* slot, const int position=-1, const char* name="");
		static void unregisterButton(const char* name);
		static void registerSeparator(int position=-1);
		static QToolButton* getButton(const char* name);
		static void refreshIcons(const QString &caption=QString::null, const QString &newIconName=QString::null, const QString &newCaption=QString::null);
};

#endif
