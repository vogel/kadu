#ifndef ACTION_H
#define ACTION_H

#include <qaction.h>
#include <qmainwindow.h>
#include <qmap.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qvaluelist.h>

#include "toolbar.h"
#include "usergroup.h"

class Action : public QAction
{
	Q_OBJECT

	private:
		QIconSet OnIcon;
		QString OnText;
		QString DockAreaGroupRestriction;
		const char *Slot;
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
			Restrict action to one dockarea group only, for example:
			action_1->setClassRestriction("chatDockAreaGroup")
			to set action to be allowed in chat windows only.
		**/
		void setDockAreaGroupRestriction(const QString& dockarea_group);
		QString dockAreaGroupRestriction();
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
	private:
		struct Default
		{
			QString action_name;
			bool uses_text_label;
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
