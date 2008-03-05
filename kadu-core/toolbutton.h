#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <qglobal.h>

#include <QContextMenuEvent>
#include <QDomElement>
#include <QMouseEvent>
#include <QToolButton>

#include "action.h"
#include "drag_simple.h"

class ToolBar;

class ToolButton : public QToolButton
{
	Q_OBJECT

	friend class DisabledToolButtonWatcher;

	private:
		QString ActionName;
		bool InOnState;
		QIcon OffIcon;
		QString OffText;
		QIcon OnIcon;
		QString OnText;
		QPoint MouseStart;

		bool IsEnabled;
		ActionDescription::ActionType Type;

	private slots:
		void buttonClicked();
		void deleteButtonClicked();
		void showTextLabelClicked();
		void toolbarPlaceChanged(Q3DockWindow::Place p);

	protected:
		void mousePressEvent(QMouseEvent* e);
		void mouseMoveEvent(QMouseEvent* e);
		void contextMenuEvent(QContextMenuEvent* e);

	public:
		ToolButton(QWidget* parent, const QString& action_name, ActionDescription::ActionType actionType);
		~ToolButton();

		void setEnabled(bool enabled);

		/**
			button works just like toggled but using two shapes
			(pictures and texts)
		**/
		void setOnShape(const QIcon& on_icon, const QString& on_text);
		bool isOn() const;
		void setOn(bool on);
		QString actionName();
		ToolBar *toolbar();

		void usersChanged();

	signals:
		void removedFromToolbar(ToolButton *);
};

class ActionDrag : public DragSimple
{

public:
	ActionDrag(const QString &actionName, bool showLabel, QWidget* dragSource = 0, const char* name = 0);

	static bool decode(const QMimeSource *source, QString &actionName, bool &showLabel);

};

#endif
