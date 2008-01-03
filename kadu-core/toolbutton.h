/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <qdom.h>
#include <qtoolbutton.h>

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
		QIconSet OffIcon;
		QString OffText;
		QIconSet OnIcon;
		QString OnText;
		QPoint MouseStart;

		bool IsEnabled;
		Action::ActionType Type;

	private slots:
		void buttonClicked();
		void deleteButtonClicked();
		void showTextLabelClicked();
		void toolbarPlaceChanged(QDockWindow::Place p);

	protected:
		void mousePressEvent(QMouseEvent* e);
		void mouseMoveEvent(QMouseEvent* e);
		void contextMenuEvent(QContextMenuEvent* e);

	public:
		ToolButton(QWidget* parent, const QString& action_name, Action::ActionType actionType);
		~ToolButton();

		void setEnabled(bool enabled);

		/**
			button works just like toggled but using two shapes
			(pictures and texts)
		**/
		void setOnShape(const QIconSet& icon, const QString& text);
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
