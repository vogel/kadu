#ifndef ACTION_H
#define ACTION_H

#include <qglobal.h>

#include <q3mainwindow.h>
#include <qmap.h>
#include <q3popupmenu.h>
#include <qstring.h>
#include <q3valuelist.h>
//Added by qt3to4:
#include <QPixmap>

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
		TypeUserList		= 0x0010, //!< actions with TypeUserList type requires access to user list widget
		TypeAll			= 0xFFFF  //!< TypeAll is used to set masks for all types of actions
	};

private:
	QString IconName;
	QString Text;
	QKeySequence KeySeq0, KeySeq1; //!< skróty klawiszowe wywo³uj±ce akcjê
	bool ToggleAction;
	QString OnIcon;
	QString OnText;
	const char *Slot;
	Q3ValueList<ToolButton*> ToolButtons;
	bool ToggleState;
	ActionType Type;

	Action(const Action &) {}
	Action &operator = (const Action &) {return *this;}

private slots:
	void toolButtonClicked();
	void toolButtonDestroyed(QObject* obj);

public:
	Action(const QString& icon, const QString& text, const char* name, ActionType Type,
		QKeySequence Seq0 = QKeySequence(), QKeySequence Seq1 = QKeySequence());
	virtual ~Action();

	QString iconName() { return IconName; }
	QString text() { return Text; }
	bool toggleAction() { return ToggleAction; }
	bool toggleState() { return ToggleState; }
	QString onIcon() { return OnIcon; }
	QString onText() { return OnText; }
	ActionType type() { return Type; }
	QKeySequence keySeq0() { return KeySeq0; }
	QKeySequence keySeq1() { return KeySeq1; }

	void setToggleAction(bool toggle);
	/**
		action works just like toggled but using two shapes
		(pictures and texts)
	**/
	void setOnShape(const QString& icon, const QString& text);
	void buttonAddedToToolbar(ToolBar *toolBar, ToolButton *button);
	int addToPopupMenu(Q3PopupMenu* menu, bool connect_signal = true);
	Q3ValueList<ToolButton*> toolButtonsForUserListElements(
		const UserListElements& users);
	bool isOn(const UserListElements& users);
	void setOn(const UserListElements& users, bool on);
	void setAllOn(bool on);
	void setPixmaps(const UserListElements& users, const QPixmap& pixmap);
	void setIconSets(const UserListElements& users, const QIcon& icon);
	void refreshIcons();
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
	void addedToToolbar(const UserGroup* users, ToolButton* button, ToolBar* toolbar);
	void addedToToolbar(ToolButton* button, ToolBar* toolbar);

	/**
		is_on jest zawsze ustawione na false jesli akcja nie jest typu "toggle".
	**/
	void activated(const UserGroup* users, const QWidget* source, bool is_on);

	/**
		Sygna³ jest emitowany po zmianie zestawu ikon
	**/
	void iconsRefreshed();
};

class Actions : public QObject
{
	Q_OBJECT

	QMap<QString, Action *> ActionsMap;

	void insert(const QString &name, Action *action);
	void remove(const QString &name);
	friend class Action;

public:
	Actions();

	Action * operator [] (const QString &name);
	Action * operator [] (int index);
	bool contains(const QString &name);

	QMap<QString, Action *>::Iterator begin() { return ActionsMap.begin(); }
	QMap<QString, Action *>::Iterator end() { return ActionsMap.end(); }
	QMap<QString, Action *>::ConstIterator begin () const { return ActionsMap.begin(); }
	QMap<QString, Action *>::ConstIterator end () const { return ActionsMap.end(); }
	QMap<QString, Action *>::ConstIterator constBegin () const { return ActionsMap.constBegin(); }
	QMap<QString, Action *>::ConstIterator constEnd () const { return ActionsMap.constEnd(); }

	void refreshIcons();

signals:
	void actionLoaded(const QString &actionName);
	void actionUnloaded(const QString &actionName);
};

extern Actions KaduActions;

#endif
