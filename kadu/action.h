#ifndef ACTION_H
#define ACTION_H

#include <qmainwindow.h>
#include <qmap.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qvaluelist.h>

#include "usergroup.h"

class ToolButton;
class ToolBar;

class Action : public QObject
{
	Q_OBJECT

	public:
		enum ActionType {
			TypeGlobal		= 0x0001, //!< actions with TypeGlobal type does not require access to user list or anything window-dependend
			TypeUser		= 0x0002, //!< actions with TypeUser type requires access to one or more users from user list
			TypeChat		= 0x0004, //!< actions with TypeChat type requires access to chat window
			TypeSearch		= 0x0008, //!< actions with TypeSearch type requires access to search window
			TypeUserList	= 0x0010, //!< actions with TypeUserList type requires access to user list widget
			TypeAll			= 0xFFFF  //!< TypeAll is used to set masks for all types of actions
		};

	private:
		QIconSet Icon;
		QString Text;
		QKeySequence Accel;
		bool ToggleAction;
		QIconSet OnIcon;
		QString OnText;
		const char *Slot;
		QValueList<ToolButton*> ToolButtons;
		struct ToggleStateStruct
		{
			UserListElements elems;
			bool state;
			ToggleStateStruct(UserListElements elems, bool state);
			ToggleStateStruct();
		};
		QValueList<ToggleStateStruct> ToggleState;
		ActionType Type;

	private slots:
		void toolButtonClicked();
		void toolButtonDestroyed(QObject* obj);

	public:
		Action(const QIconSet& icon, const QString& text, const char* name,
			ActionType Type, QKeySequence accel = QKeySequence());
		void setToggleAction(bool toggle);
		/**
			action works just like toggled but using two shapes
			(pictures and texts)
		**/
		void setOnShape(const QIconSet& icon, const QString& text);
		ToolButton* addToToolbar(ToolBar* toolbar, bool uses_text_label = false);
		int addToPopupMenu(QPopupMenu* menu, bool connect_signal = true);
		QValueList<ToolButton*> toolButtonsForUserListElements(
			const UserListElements& users);
		bool isOn(const UserListElements& users);
		void setOn(const UserListElements& users, bool on);
		void setPixmaps(const UserListElements& users, const QPixmap& pixmap);
		void setTexts(const UserListElements& users, const QString& text);
		/**
			Sets enabled state of all buttons in dockareas that has specified
			parent.
		**/
		void setEnabled(QWidget* parent, bool enabled);
		/**
			Sets slot of dockarea's parent that will be called when action
			is activated. You should ensure that class of parent's class
			is known using setDockAreaGroupRestriction().
		**/
		void setSlot(const char *slot);
		/**
			Activate action
		**/
		void activate(const UserGroup* users);

		ActionType actionType();

	signals:
		void addedToToolbar(ToolButton* button, ToolBar* toolbar,
			const UserListElements& users);
		/**
			is_on jest zawsze ustawione na false jesli akcja nie jest typu "toggle".
		**/
		void activated(const UserGroup* users, const QWidget* source, bool is_on);
	private:
		Action(const Action &) {}
		Action &operator=(const Action &){return *this;}
};

class Actions : public QMap<QString, Action*>
{
	private:
		struct Default
		{
			QString action_name;
			bool uses_text_label;
			Default(QString action_name, bool uses_text_label);
			Default();
		};
		QMap< QString, QValueList<Default> > DefaultToolbarActions;

	public:
		Actions();
		void refreshIcons();
		void addDefaultToolbarAction(
			const QString& toolbar, const QString& action, int index = -1,
			bool uses_text_label = false);
		void addDefaultActionsToToolbar(ToolBar* toolbar);
};

extern Actions KaduActions;

#endif
