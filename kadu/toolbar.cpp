/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcursor.h>
#include <qobjectlist.h>
#include <qtooltip.h>

#include "action.h"
#include "chat.h" // TODO: akcje powinny byæ niezale¿ne od chat
#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "toolbar.h"


ToolButton::ToolButton(QWidget* parent, const QString& action_name)
	: QToolButton(parent, 0)
{
	kdebugf();
	ActionName = action_name;
	kdebugf2();
}

void ToolButton::mouseMoveEvent(QMouseEvent* e)
{
//	kdebugf();
	QToolButton::mouseMoveEvent(e);
	if (e->state() & LeftButton)
	{
		QDragObject* d = new ToolButtonDrag(this, parentWidget());
		d->dragMove();
	}	
//	kdebugf2();
}

void ToolButton::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();
	QPopupMenu* p = new QPopupMenu(this);
	p->insertItem(tr("Delete button"), this, SLOT(deleteButtonClicked()));
	p->exec(QCursor::pos());
	delete p;
	e->accept();
	kdebugf2();
}

void ToolButton::deleteButtonClicked()
{
	kdebugf();
	ToolBar* toolbar = (ToolBar*)parent();
	toolbar->removeChild(this); // tymczasowo usun z listy, dla writeToConfig()
	DockArea* dockarea = (DockArea*)toolbar->area();
	dockarea->writeToConfig();
	toolbar->insertChild(this); // przywroc
	deleteLater();
	kdebugf2();
}

QString ToolButton::actionName()
{
	return ActionName;
}

void ToolButton::writeToConfig(QDomElement parent_element)
{
	kdebugf();
	QDomElement button_elem = xml_config_file->createElement(parent_element, "ToolButton");
	button_elem.setAttribute("action_name", ActionName);
	kdebugf2();
}

ToolButtonDrag::ToolButtonDrag(ToolButton* button, QWidget* dragSource, const char* name)
	: QTextDrag(QString::number(button->winId()), dragSource, name)
{
	kdebugf();
	kdebugf2();
}

ToolBar::ToolBar(const QString& label, QMainWindow* mainWindow, QWidget* parent)
	: QToolBar(label, mainWindow, parent)
{
	kdebugf();
	setAcceptDrops(true);
	kdebugf2();
}

void ToolBar::addButtonClicked(int action_index)
{
	kdebugf();
	kdebug("action_index = %d\n", action_index);
	KaduActions[KaduActions.keys()[action_index]]->addToToolbar(this);
	DockArea* dockarea = (DockArea*)area();
	dockarea->writeToConfig();
	kdebugf2();
}

void ToolBar::dragEnterEvent(QDragEnterEvent* event)
{
	kdebugf();
	event->accept(dynamic_cast<ToolBar*>(event->source()) != NULL);
	kdebugf2();
}

void ToolBar::dropEvent(QDropEvent* event)
{
	kdebugf();
	ToolBar* source = dynamic_cast<ToolBar*>(event->source());
	if (source != NULL)
	{
		QString text;
		if (QTextDrag::decode(event, text))
		{
			
			ToolBar* source_toolbar = (ToolBar*)event->source();
			// TODO: uzywanie WId moze nie byc zbyt przenosne ;)
			// Jakis lepszy pomysl?
			source_toolbar->find(text.toULong())->reparent(this, QPoint(0,0), true);
			// zapisujemy dockarea
			DockArea* source_dockarea = (DockArea*)source_toolbar->area();
			source_dockarea->writeToConfig();
			DockArea* dockarea = (DockArea*)area();
			dockarea->writeToConfig();
		}
	}
	kdebugf2();
}

void ToolBar::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();
	QPopupMenu* p = new QPopupMenu(this);
	p->insertItem(tr("Delete toolbar"), this, SLOT(deleteLater()));
	QPopupMenu* p2 = new QPopupMenu(p);
	int param = 0;
	CONST_FOREACH(a, KaduActions)
	{
		int id = (*a)->addToPopupMenu(p2, false);
		p2->setItemParameter(id, param);
		p2->connectItem(id, this, SLOT(addButtonClicked(int)));
		param++;
	}
	p->insertItem(tr("Add new button"), p2);
	p->exec(QCursor::pos());
	delete p;
	e->accept();
	kdebugf2();
}

void ToolBar::writeToConfig(QDomElement parent_element)
{
	kdebugf();
	QDomElement toolbar_elem = xml_config_file->createElement(
		parent_element, "ToolBar");
	toolbar_elem.setAttribute("offset", offset());
	QObjectList* l = queryList("ToolButton");
	for (QObjectList::iterator i = l->begin(); i != l->end(); i++)
		((ToolButton*)(*i))->writeToConfig(toolbar_elem);
	kdebugf2();
}

void ToolBar::loadFromConfig(QDomElement toolbar_element)
{
	kdebugf();
	setOffset(toolbar_element.attribute("offset").toInt());
	QDomNodeList buttons = toolbar_element.elementsByTagName("ToolButton");
	for (unsigned int i = 0; i < buttons.count(); i++)
	{
		QDomElement button_elem = buttons.item(i).toElement();;
		KaduActions[button_elem.attribute("action_name")]->addToToolbar(this);
	}
	kdebugf2();
}

DockArea::DockArea(Orientation o, HandlePosition h,
			QWidget * parent, const char * name)
	: QDockArea(o, h, parent, name)
{
	kdebugf();
	kdebugf2();
}

void DockArea::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();
	QPopupMenu* p = new QPopupMenu(this);
	p->insertItem(tr("Create new toolbar"), this, SLOT(createNewToolbar()));
	p->exec(QCursor::pos());
	delete p;
	e->accept();
	kdebugf2();
}

void DockArea::createNewToolbar()
{
	kdebugf();
	for (QWidget* w = parentWidget(); w != NULL; w = w->parentWidget())
	{
		QMainWindow* mw = dynamic_cast<QMainWindow*>(w);
		if (mw != NULL)
		{
			kdebug("Creating new toolbar\n");
			ToolBar* tb = new ToolBar("New toolbar", mw, mw);
			tb->show();
			moveDockWindow(tb);
			setAcceptDockWindow(tb, true);
			writeToConfig();
			break;
		}
	}
	kdebugf2();
}

void DockArea::writeToConfig()
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement toolbars_elem = xml_config_file->accessElement(root_elem, "Toolbars");
	QDomElement dockarea_elem = xml_config_file->accessElementByProperty(
		toolbars_elem, "DockArea", "name", name());
	xml_config_file->removeChildren(dockarea_elem);
	QPtrList<QDockWindow> dock_windows = dockWindowList();
	for (QPtrList<QDockWindow>::iterator i = dock_windows.begin();
		i != dock_windows.end(); i++)
	{
		ToolBar* toolbar = dynamic_cast<ToolBar*>(*i);
		if (toolbar != NULL)
			toolbar->writeToConfig(dockarea_elem);
	}
	kdebugf2();
}

bool DockArea::loadFromConfig(QMainWindow* toolbars_parent)
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement toolbars_elem = xml_config_file->findElement(root_elem, "Toolbars");
	if (!toolbars_elem.isNull())
	{
		QDomElement dockarea_elem = xml_config_file->findElementByProperty(
			toolbars_elem, "DockArea", "name", name());
		if (!dockarea_elem.isNull())
		{
			QDomNodeList toolbars = dockarea_elem.elementsByTagName("ToolBar");
			for (unsigned int i = 0; i < toolbars.count(); i++)
			{
				ToolBar* toolbar = new ToolBar(QString(),
					toolbars_parent, toolbars_parent);
				toolbar->loadFromConfig(toolbars.item(i).toElement());
				toolbar->show();
				moveDockWindow(toolbar);
				setAcceptDockWindow(toolbar, true);
			}
			return true;
		}
	}
	return false;
	kdebugf2();
}

QValueList<MainToolBar::ToolButton> MainToolBar::RegisteredToolButtons;
MainToolBar* MainToolBar::instance=NULL;

MainToolBar::MainToolBar(QMainWindow* parent) : QToolBar(parent, "mainToolbar")
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

MainToolBar::~MainToolBar()
{
	config_file.writeEntry("General", "ToolBarHidden", isHidden());
	instance=NULL;
}

void MainToolBar::createControls()
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

void MainToolBar::registerSeparator(int position)
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

void MainToolBar::registerButton(const QString &iconname, const QString& caption,
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

void MainToolBar::unregisterButton(const char* name)
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

QToolButton* MainToolBar::getButton(const char* name)
{
	CONST_FOREACH(j, RegisteredToolButtons)
		if ((*j).name == name)
			return (*j).button;
	kdebugmf(KDEBUG_WARNING, "'%s' return NULL\n", name?name:"[null]");
	return NULL;
}

void MainToolBar::refreshIcons(const QString &caption, const QString &newIconName, const QString &newCaption)
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
