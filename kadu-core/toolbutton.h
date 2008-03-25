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

class ActionDrag : public DragSimple
{

public:
	ActionDrag(const QString &actionName, bool showLabel, QWidget* dragSource = 0, const char* name = 0);

	static bool decode(const QMimeSource *source, QString &actionName, bool &showLabel);

};

#endif
