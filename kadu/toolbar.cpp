/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "toolbar.h"

QValueList<ToolBar::ToolButton> ToolBar::RegisteredToolButtons;
ToolBar* ToolBar::instance=NULL;

ToolBar::ToolBar(QMainWindow* parent) : QToolBar(parent, "mainToolbar")
{
	kdebugf();
	setCloseMode(QDockWindow::Undocked);
	setLabel(qApp->translate("ToolBar", "Main toolbar"));

	config_file.addVariable("General", "ToolBarHidden", false);
	if (config_file.readBoolEntry("General", "ToolBarHidden"))
		hide();

	setVerticallyStretchable(true);
	setHorizontallyStretchable(true);

	createControls();
	instance=this;
	kdebugf2();
}

ToolBar::~ToolBar()
{
	config_file.writeEntry("General", "ToolBarHidden", isHidden());
	instance=NULL;
}

void ToolBar::createControls()
{
	kdebugf();
	FOREACH(j, RegisteredToolButtons)
		if ((*j).caption== "--separator--")
			addSeparator();
		else
			(*j).button = new QToolButton(icons_manager->loadIcon((*j).iconname), (*j).caption,
				QString::null, (*j).receiver, (*j).slot, this, (*j).name);

	setStretchableWidget(new QWidget(this));
	kdebugf2();
}

void ToolBar::registerSeparator(int position)
{
	kdebugf();
	if(instance!=NULL)
		instance->clear();

	ToolButton RToolButton;
	RToolButton.caption="--separator--";

	if ((RegisteredToolButtons.count()<(uint)(position+1)) || (position == -1))
		RegisteredToolButtons.append(RToolButton);
	else
		RegisteredToolButtons.insert(RegisteredToolButtons.at(position), RToolButton);

	if(instance!=NULL)
		instance->createControls();
	kdebugf2();
}

void ToolBar::registerButton(const QString &iconname, const QString& caption,
			QObject* receiver, const char* slot, int position, const char* name)
{
	kdebugf();
	if(instance!=NULL)
		instance->clear();

	ToolButton RToolButton;

	RToolButton.iconname= iconname;
	RToolButton.caption= caption;
	RToolButton.receiver= receiver;
	RToolButton.slot= slot;
	RToolButton.position= position;
	RToolButton.name= name;

	if ((RegisteredToolButtons.count()<(uint)(position+1)) || (position == -1))
		RegisteredToolButtons.append(RToolButton);
	else
		RegisteredToolButtons.insert(RegisteredToolButtons.at(position), RToolButton);

	if(instance!=NULL)
		instance->createControls();
	kdebugf2();
}

void ToolBar::unregisterButton(const char* name)
{
	kdebugf();
	if(instance!=NULL)
		instance->clear();

	FOREACH(j, RegisteredToolButtons)
		if ((*j).name == name)
		{
			RegisteredToolButtons.remove(j);
			break;
		}

	if(instance!=NULL)
		instance->createControls();
	kdebugf2();
}

QToolButton* ToolBar::getButton(const char* name)
{
	CONST_FOREACH(j, RegisteredToolButtons)
		if ((*j).name == name)
			return (*j).button;
	kdebugmf(KDEBUG_WARNING, "'%s' return NULL\n", name?name:"[null]");
	return NULL;
}

void ToolBar::refreshIcons(const QString &caption, const QString &newIconName, const QString &newCaption)
{
	kdebugf();
	if (caption==QString::null) //wszystkie siê od¶wie¿aj±
	{
		FOREACH(j, RegisteredToolButtons)
			if ((*j).caption!="--separator--")
				(*j).button->setIconSet(icons_manager->loadIcon((*j).iconname));
		if (kadu->isVisible())
		{
			kadu->hide();
			kadu->show();
		}
	}
	else
		FOREACH(j, RegisteredToolButtons)
			if ((*j).caption == caption)
			{
				if (newIconName!=QString::null)
					(*j).iconname=newIconName;
				(*j).button->setIconSet(icons_manager->loadIcon((*j).iconname));
				if (newCaption!=QString::null)
				{
					(*j).caption=newCaption;
					(*j).button->setTextLabel(newCaption);
				}
				break;
			}
	kdebugf2();
}
