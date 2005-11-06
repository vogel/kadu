#ifndef ACTION_H
#define ACTION_H

#include <qaction.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qvaluelist.h>

#include "toolbar.h"
#include "usergroup.h"

class Action : public QAction
{
	Q_OBJECT

	private:
		bool UsesTextLabel;
		QIconSet OnIcon;
		QString OnText;
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
		/**
			set if ToolButtons uses text label.
		**/
		void setUsesTextLabel(bool uses);
		/**
			action works just like toggled but using two shapes
			(pictures and texts)
		**/
		void setOnShape(const QIconSet& icon, const QString& text);
		ToolButton* addToToolbar(ToolBar* toolbar);
		int addToPopupMenu(QPopupMenu* menu, bool connect_signal = true);
		QValueList<ToolButton*> toolButtonsForUserListElements(
			const UserListElements& users);
		bool isOn(const UserListElements& users);
		void setOn(const UserListElements& users, bool on);
		void setPixmaps(const UserListElements& users, const QPixmap& pixmap);
		void setTexts(const UserListElements& users, const QString& text);
		/**
			Activate action
		**/
		void activate(const UserGroup* users);

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
		void refreshIcons();
};

extern Actions KaduActions;

#endif
