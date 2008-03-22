/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QDomElement>

#include "config_file.h"
#include "custom_input.h"
#include "toolbar.h"

#include "chat_edit_box.h"

ChatEditBox::ChatEditBox(QWidget *parent)
	: QMainWindow(parent)
{
	InputBox = new CustomInput(this);
	InputBox->setWordWrapMode(QTextOption::WordWrap);
	InputBox->setTextFormat(Qt::RichText);
	InputBox->setVisible(true);

	setCentralWidget(InputBox);
	InputBox->show();

	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement toolbars_elem = xml_config_file->findElement(root_elem, "Toolbars");

	if (toolbars_elem.isNull())
		return;

	QDomElement dockarea_elem = xml_config_file->findElementByProperty(toolbars_elem, "DockArea", "name", "topDockArea");
	if (dockarea_elem.isNull())
		return;

	for (QDomNode n = dockarea_elem.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &toolbar_elem = n.toElement();
		if (toolbar_elem.isNull())
			continue;
		if (toolbar_elem.tagName() != "ToolBar")
			continue;

		ToolBar* toolbar = new ToolBar(this);
// 		moveDockWindow(toolbar);
		toolbar->loadFromConfig(toolbar_elem);
		toolbar->show();
// 		setAcceptDockWindow(toolbar, true);

		addToolBar(Qt::TopToolBarArea, toolbar);
	}

	show();
}

ChatEditBox::~ChatEditBox()
{
}

CustomInput * ChatEditBox::inputBox()
{
	return InputBox;
}
