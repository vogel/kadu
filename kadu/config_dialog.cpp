/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qaccel.h>
#include <qtooltip.h>
#include <qcolordialog.h>
#include <qcolor.h>

#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"

QString ConfigDialog::acttab = QT_TRANSLATE_NOOP("@default", "General");
ConfigDialog *ConfigDialog::configdialog = NULL;
QApplication *ConfigDialog::appHandle = NULL;

QValueList<ConfigDialog::RegisteredControl> ConfigDialog::RegisteredControls;
QValueList<ConfigDialog::ElementConnections> ConfigDialog::SlotsOnCreate;
QValueList<ConfigDialog::ElementConnections> ConfigDialog::SlotsOnDestroy;

void ConfigDialog::showConfigDialog(QApplication* application) {
	ConfigDialog *cd;
	
	if (configdialog)
		configdialog->setActiveWindow();
	else {
		cd = new ConfigDialog(application);
		cd->show();
		}
}

ConfigDialog::ConfigDialog(QApplication *application, QWidget *parent, const char *name) : QTabDialog(parent, name) {

	ConfigDialog::appHandle=application;
	setWFlags(Qt::WDestructiveClose);
	
	int actualtab= 0;
	int nexttab= 0;
	int actualparent=0;
	QString actualparentname= "";
	int i;

	for(QValueList<RegisteredControl>::iterator i=RegisteredControls.begin(); i!=RegisteredControls.end(); i++)
	{
// wyswietla cala liste 
//		kdebug("(%d) "+(*i).group+"->"+(*i).parent+"->"+(*i).caption+"->"+(*i).name+"\n",(*i).nrOfControls);
		QWidget* parent=NULL;
		if((*i).type!=CONFIG_TAB)
		{
		    actualtab= findTab((*i).group,actualtab);
		    nexttab= findTab(actualtab+1);
		    
		      if (nexttab == -1) 
			nexttab= RegisteredControls.count()-1;
		    
		    int z;
		      if ((*i).parent == actualparentname)
		        parent=RegisteredControls[actualparent].widget;
		      else
		    
		        for (z=actualtab; z<nexttab; z++)
			 	 if ((*i).parent == RegisteredControls[z].caption)
				     {
					parent= RegisteredControls[z].widget;
					actualparent=z;
					actualparentname=(*i).parent;
					break;
				     }
		}
		
		switch((*i).type)
		{
			case CONFIG_CHECKBOX:
			{
				QCheckBox* check=new QCheckBox(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				check->setChecked(config_file.readBoolEntry((*i).group, (*i).entry, (*i).defaultS));
				(*i).widget=check;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			
			case CONFIG_COLORBUTTON:
			{
				ColorButton* colorbutton=new ColorButton(QColor((*i).defaultS), parent, (*i).name);
				QPixmap pm(35,10);
				pm.fill(QColor((*i).defaultS));
				colorbutton->setPixmap(pm);
				colorbutton->setMaximumSize(QSize(50,25));
				(*i).widget=colorbutton;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_COMBOBOX:
			{
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QComboBox* combo=new QComboBox(hbox, (*i).name);
				(*i).widget=combo;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_GRID:
				{
				QGrid* grid=new QGrid((*i).defaultS.toInt(), parent, (*i).caption);
				(*i).widget=grid;
				break;
			}
			case CONFIG_HBOX:
			{
				QHBox* box = new QHBox(parent,(*i).caption);
				box->setSpacing(2);
				(*i).widget=box;
				break;

			}
			case CONFIG_HGROUPBOX:
			{
				QHGroupBox* box = new QHGroupBox(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				(*i).widget=box;
				break;
			}
			case CONFIG_HOTKEYEDIT:
			{
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				HotKey* hotkey=new HotKey(hbox, (*i).name);
				hotkey->setText(config_file.readEntry((*i).group, (*i).entry, (*i).defaultS));
				(*i).widget=hotkey;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_LABEL:
			{
				QLabel* label=new QLabel(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				(*i).widget=label;
				break;
			}	
			case CONFIG_LINEEDIT:
			{
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QLineEdit* line=new QLineEdit(hbox, (*i).name);
				line->setText(config_file.readEntry((*i).group, (*i).entry, (*i).defaultS));
				(*i).widget=line;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_LINEEDIT2:
			{
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QLineEdit* line=new QLineEdit(hbox, (*i).name);
				line->setText((*i).defaultS);
				(*i).widget=line;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_LISTBOX:
			{	
				QListBox* listbox= new QListBox(parent, (*i).caption);
				(*i).widget=listbox;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_PUSHBUTTON:
			{
				QPushButton *button =new QPushButton(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				button->setIconSet(QPixmap(QString(DATADIR)+ "/kadu/icons/" + (*i).defaultS));
				(*i).widget=button;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_SLIDER:
			{
				int minVal;
				int maxVal;
				int pageStep;
				int value;
				QStringList values= QStringList::split(",", (*i).defaultS);

				minVal=values[0].toInt();
				maxVal=values[1].toInt();
				pageStep=values[2].toInt();
				value=values[3].toInt();
				QSlider *slider=new QSlider(minVal, maxVal, pageStep, value, Qt::Horizontal, parent, (*i).caption);
				slider->setValue(config_file.readNumEntry((*i).group, (*i).entry,value));
				slider->setTickmarks(QSlider::Below);
				(*i).widget=slider;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_SPINBOX:
			{	
				QHBox* hbox=new QHBox(parent);
				QLabel* label=new QLabel(appHandle->translate("@default",(*i).caption), hbox);	
				QStringList values= QStringList::split(",", (*i).defaultS);
				int minVal=values[0].toInt();
				int maxVal=values[1].toInt();
				int step=values[2].toInt();
				int value=values[3].toInt();
				QSpinBox *spinbox=new QSpinBox(minVal, maxVal, step, hbox);
				int val=config_file.readNumEntry((*i).group, (*i).entry,value);
				if (val) value=val;
				spinbox->setValue(value);
				(*i).widget=spinbox;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_TAB:
			{
				QVBox* box = new QVBox(this,(*i).caption);
				box->setMargin(2);
				(*i).widget=box;
				addTab(box,  appHandle->translate("@default",(*i).caption));
		    		   if ((*i).caption==acttab) 
				   {
				   showPage(box);
				   }
				break;
			}
			case CONFIG_VBOX:
			{
				QVBox* box = new QVBox(parent,(*i).caption);
				(*i).widget=box;
				box->setSpacing(2);
				break;
			}
			case CONFIG_VGROUPBOX:
			{
				QVGroupBox* box = new QVGroupBox(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				(*i).widget=box;
				break;
			}


		};
			for(QValueList<ElementConnections>::iterator k=(*i).ConnectedSlots.begin(); k!=(*i).ConnectedSlots.end(); k++)
			{
				if (!connect((*i).widget, (*k).signal, (*k).receiver, (*k).slot))
				    kdebug("unable to connect signal: "+(*k).signal+" slot: "+(*k).slot+"\n");
			}
		
	};

		for(QValueList<ElementConnections>::iterator a=SlotsOnCreate.begin(); a!=SlotsOnCreate.end(); a++)
			connect(this, SIGNAL(create()), (*a).receiver, (*a).slot);


	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(updateConfig()));
	setCancelButton(tr("Cancel"));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close()));
	setCaption(tr("Kadu configuration"));
	resize(480, 500);

	configdialog = this;
	emit create();
}

ConfigDialog::~ConfigDialog() {
	configdialog = NULL;
	acttab = QString(currentPage()->name());
};


void ConfigDialog::updateConfig(void) 
{

	for(QValueList<RegisteredControl>::iterator i=RegisteredControls.begin(); i!=RegisteredControls.end(); i++)
	{
		switch((*i).type)
		{
			case CONFIG_CHECKBOX:
			{
				config_file.writeEntry((*i).group, (*i).entry, ((QCheckBox*)((*i).widget))->isChecked());
				break;
			}
			case CONFIG_HOTKEYEDIT:
			{
				config_file.writeEntry((*i).group, (*i).entry, ((HotKey*)((*i).widget))->text());
				break;
			}
			case CONFIG_LINEEDIT:
			{
				config_file.writeEntry((*i).group, (*i).entry, ((QLineEdit*)((*i).widget))->text());
				break;
			}
			case CONFIG_LISTBOX:
			{
				break;
			}
			case CONFIG_SLIDER:
			{
				config_file.writeEntry((*i).group, (*i).entry, ((QSlider*)((*i).widget))->value());
				break;
			}
			case CONFIG_SPINBOX:
			{
				config_file.writeEntry((*i).group, (*i).entry, ((QSpinBox*)((*i).widget))->value());
				break;
			}
		}
	}
		for(QValueList<ElementConnections>::iterator a=SlotsOnDestroy.begin(); a!=SlotsOnDestroy.end(); a++)
			connect(this, SIGNAL(destroy()), (*a).receiver, (*a).slot);

	
    emit destroy();
    
    kdebug("ConfigDialog: Configuration saved\n");
    config_file.sync();
}


void ConfigDialog::addCheckBox(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const bool defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_CHECKBOX;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;
		c.entry=entry;
		c.defaultS=QString::number(defaultS);
		c.tip=tip;
		c.nrOfControls=0;
		if (addControl(groupname,c) == 0)
		    if (config_file.readEntry(groupname, entry) == "")		
			  config_file.writeEntry(groupname, entry, defaultS);

							}
};


void ConfigDialog::addColorButton(const QString& groupname,
			    const QString& parent, const QString& caption,
			    const QColor& color, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_COLORBUTTON;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;
		c.defaultS=color.name();
	    	c.tip=tip;
		c.nrOfControls=0;
		addControl(groupname,c);
						}
};



void ConfigDialog::addComboBox(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_COMBOBOX;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;
		c.tip=tip;
		c.nrOfControls=0;
		addControl(groupname,c);
						}

}

void ConfigDialog::addGrid(const QString& groupname,
			const QString& parent, const QString& caption, const int nrColumns, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_GRID;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;		
		c.defaultS=QString::number(nrColumns);
		c.nrOfControls=0;
		addControl(groupname,c);
						}

}

void ConfigDialog::addHBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_HBOX;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;		
		c.nrOfControls=0;
		addControl(groupname,c);
							  }
};


void ConfigDialog::addHGroupBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_HGROUPBOX;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;		
		c.nrOfControls=0;
		addControl(groupname,c);
							  }
};

void ConfigDialog::addHotKeyEdit(const QString& groupname,
			const QString& parent, const QString& caption,
		        const QString& entry, const QString& defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_HOTKEYEDIT;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;
		c.nrOfControls=0;
		
		if (addControl(groupname,c) == 0)
		// zapisujemy warto¶æ domy¶ln±, aby ju¿ wiêcej nie musieæ
		// jej podawaæ przy czytaniu z pliku conf		
		    if (config_file.readEntry(groupname, entry)=="")		
			  config_file.writeEntry(groupname, entry, defaultS);
						}
};

void ConfigDialog::addLineEdit2(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_LINEEDIT2;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;
		c.defaultS=defaultS;
	    	c.tip=tip;
		c.nrOfControls=0;
		addControl(groupname,c);
							 }
}


void ConfigDialog::addLineEdit(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_LINEEDIT;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;
		c.entry=entry;
		c.defaultS=defaultS;
	    	c.tip=tip;
		c.nrOfControls=0;

		if (addControl(groupname,c) == 0)
		    if (config_file.readEntry(groupname, entry) == "")
			  config_file.writeEntry(groupname, entry, defaultS);

							   }
};

void ConfigDialog::addLabel(const QString& groupname,
			const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_LABEL;
		c.parent=parent;
		c.group=groupname;		
		c.name=name;
		c.caption=caption;
		c.nrOfControls=0;
		addControl(groupname,c);
						}
};


void ConfigDialog::addListBox(const QString& groupname,
			    const QString& parent, const QString& caption, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_LISTBOX;
		c.parent=parent;
		c.name=name;
		c.group=groupname;		
		c.caption=caption;
		c.tip=tip;
		c.nrOfControls=0;
		addControl(groupname,c);
						}
};

void ConfigDialog::addPushButton(const QString& groupname,
			    const QString& parent, const QString& caption,
			    const QString &iconFileName, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_PUSHBUTTON;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;
		c.defaultS=iconFileName;
	    	c.tip=tip;
		c.nrOfControls=0;
		addControl(groupname,c);
						}
};

void ConfigDialog::addSlider(const QString& groupname,
			    const QString& parent, const QString& caption,
			    const QString& entry,
			    const int minValue, const int maxValue,
			    const int pageStep, const int value, const QString& tip, const QString& name)
{

	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_SLIDER;
		c.parent=parent;
		c.caption=caption;
		c.name=name;
		c.group=groupname;
		c.entry=entry;
		c.tip=tip;
		c.defaultS=QString::number(minValue)+","+QString::number(maxValue)+","+QString::number(pageStep)+","+QString::number(value);
		c.nrOfControls=0;

		if (addControl(groupname,c) == 0)
		    if (config_file.readEntry(groupname, entry) == "")
			  config_file.writeEntry(groupname, entry, value);
					    		  }

};

void ConfigDialog::addSpinBox(const QString& groupname,
			    const QString& parent, const QString& caption,
			    const QString& entry,
			    const int minValue, const int maxValue, const int step, const int value, const QString& tip, const QString& name)
{

	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_SPINBOX;
		c.parent=parent;
		c.caption=caption;
		c.group=groupname;
		c.entry=entry;
		c.tip=tip;
		c.defaultS=QString::number(minValue)+","+QString::number(maxValue)+","+QString::number(step)+","+QString::number(value);
		c.nrOfControls=0;
		
		if (addControl(groupname,c) == 0)
		    if (config_file.readEntry(groupname, entry) == "")
			  config_file.writeEntry(groupname, entry, value);
		
					    		  }

};

void ConfigDialog::registerTab(const QString& caption)
{
		if (findTab(caption) == -1)
		    {
			RegisteredControl c;
			c.type=CONFIG_TAB;
			c.caption=caption;
			c.nrOfControls=0;
			RegisteredControls.append(c);
		    }	
};

void ConfigDialog::addVBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_VBOX;
		c.parent=parent;
		c.name=name;
		c.caption=caption;
		c.group=groupname;		
		c.nrOfControls=0;
		addControl(groupname,c);
							  }
};


void ConfigDialog::addVGroupBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c;
		c.type=CONFIG_VGROUPBOX;
		c.parent=parent;
		c.name=name;
		c.group=groupname;
		c.caption=caption;
		c.nrOfControls=0;
		addControl(groupname,c);
						}
};

void ConfigDialog::connectSlot(const QString& groupname, const QString& caption, const char* signal, const QObject* receiver, const char* slot,const QString& name)
{
		for(QValueList<RegisteredControl>::iterator j=RegisteredControls.begin(); j!=RegisteredControls.end(); j++)
				if(((*j).caption == caption)&& ((*j).name == name))
			{
	ElementConnections c;
	c.signal=signal;
	c.receiver=(QObject *)receiver;
	c.slot=slot;
	(*j).ConnectedSlots.append(c);
	kdebug("Slot connected:: %s\n",slot);
		    break;
			}
}

void ConfigDialog::registerSlotOnCreate(const QObject* receiver, const char* name)
{
	ElementConnections c;
	c.receiver=(QObject *)receiver;
	c.slot=name;
	SlotsOnCreate.append(c);

};

void ConfigDialog::registerSlotOnDestroy(const QObject* receiver, const char* name)
{
	ElementConnections c;
	c.receiver=(QObject *)receiver;
	c.slot=name;
	SlotsOnDestroy.append(c);

};

int ConfigDialog::findPreviousTab(const int startpos)
{
    if (RegisteredControls.isEmpty())
	return -1;


    int position=startpos;
    if (startpos<0) position=0;
    if (startpos>RegisteredControls.count()-1) position=RegisteredControls.count()-1;
    
    int j;
	for(j=startpos; j>=0; j--)
		    {
				if  (RegisteredControls[j].type == CONFIG_TAB)
				    return position;
		    position--;
		    }
// -1 oznacza ze nie ma listy
}

int ConfigDialog::findTab(const int startpos)
{
    if (RegisteredControls.isEmpty())
	return -1;

    int position=startpos;
    if (startpos<0) position=0;
    if (startpos>RegisteredControls.count()-1) position=RegisteredControls.count()-1;

    position=findPreviousTab(position);
			int val=position+ RegisteredControls[position].nrOfControls+1;
			  if (val>RegisteredControls.count()-1)
				val--;
			if (RegisteredControls[val].type == CONFIG_TAB)
			   return val;
			else
			    return -1;
// zwraca miejsce znalezienia TAB'a
// jesli nie znajdzie to zwraca -1	
};

int ConfigDialog::findTab(const QString& groupname,const int startpos)
{
    if (RegisteredControls.isEmpty())
	return -1;
    int position;
    if (startpos<0) position=0;
    if (startpos>RegisteredControls.count()-1) position=RegisteredControls.count()-1;

    position=findPreviousTab(startpos);
    int i;
	for(i=startpos;i<=RegisteredControls.count()-1;i++)
		    {
				   if (RegisteredControls[position].caption == groupname)
				   {
				    return position;
				   }    
				   else position=findTab(position);
				   if (position == -1) return -1;
		    }
	return -1;
// jesli znajdzie odpowiedni TAB to wzraca miejsce znalezienia 
// jesli nie znajdzie to zwraca -1
};

void ConfigDialog::updateNrOfControls(const int startpos, const int endpos, const QString& parent)
{
    int j;
    QString localparent=parent;
	for (j=endpos;j>=startpos;j--)
	    {
		if (RegisteredControls[j].caption==localparent)
		    {
			RegisteredControls[j].nrOfControls+=1;
			localparent= RegisteredControls[j].parent;
			  if (RegisteredControls[j].type == CONFIG_TAB)
				    break;
		    }
	    }

}

int ConfigDialog::addControl(const QString& groupname, const ConfigDialog::RegisteredControl& control)
{
	int position= findTab(groupname);
	   if (position == -1) 
		    {
			kdebug("There is no Tab: "+groupname+"\n");
			return -1;
		    }
	int nexttab= findTab(position+1);
	if (nexttab == -1) nexttab= RegisteredControls.count()-1;

	

	int j;
	for (j=position;j<=nexttab;j++)
	    if (RegisteredControls[j].caption == control.parent)
		    {
		    int nrOfControls=RegisteredControls[j].nrOfControls;
		    updateNrOfControls(position,j,control.parent);
			if (RegisteredControls[j].caption == RegisteredControls.last().caption)
			RegisteredControls.append(control);
			else
			RegisteredControls.insert(RegisteredControls.at(j+nrOfControls+1),control);
			return 0;
		    }	
	
	    return -2;

// jesli wartosc zwrocona jest -1 to nie znaleziono takiego TAB'a
// jesli wartosc zwrocona jest -2 to nie znaleziono takiego parent'a
// jesli wartosc zwrocona jest 0 to wszystko jest wporzadku
};



QWidget* ConfigDialog::getWidget(const QString& groupname, const QString& caption, const QString& name)
{	
	int nr=existControl(groupname,caption,name);
	    if (nr!=-1)
					return (RegisteredControls[nr].widget);
	kdebug("Warning there is no \\" +groupname+ "\\"+ caption+ "\\"+ name+ "\\ control\n");
}


QCheckBox* ConfigDialog::getCheckBox(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QCheckBox*>(getWidget(groupname,caption,name));
}

ColorButton* ConfigDialog::getColorButton(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<ColorButton*>(getWidget(groupname,caption,name));
}


QComboBox* ConfigDialog::getComboBox(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QComboBox*>(getWidget(groupname,caption,name));
}

QGrid* ConfigDialog::getGrid(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QGrid*>(getWidget(groupname,caption,name));
}

QHBox* ConfigDialog::getHBox(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QHBox*>(getWidget(groupname,caption,name));
}

QHGroupBox* ConfigDialog::getHGroupBox(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QHGroupBox*>(getWidget(groupname,caption,name));
}


HotKey* ConfigDialog::getHotKeyEdit(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<HotKey*>(getWidget(groupname,caption,name));
}

QLineEdit* ConfigDialog::getLineEdit(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QLineEdit*>(getWidget(groupname,caption,name));
}

QLabel* ConfigDialog::getLabel(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QLabel*>(getWidget(groupname,caption,name));
}

QListBox* ConfigDialog::getListBox(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QListBox*>(getWidget(groupname,caption,name));
}


QPushButton* ConfigDialog::getPushButton(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QPushButton*>(getWidget(groupname,caption,name));
}


QSlider* ConfigDialog::getSlider(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QSlider*>(getWidget(groupname,caption,name));
}


QSpinBox* ConfigDialog::getSpinBox(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QSpinBox*>(getWidget(groupname,caption,name));
}


QVBox* ConfigDialog::getVBox(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QVBox*>(getWidget(groupname,caption,name));
}


QVGroupBox* ConfigDialog::getVGroupBox(const QString& groupname, const QString& caption, const QString& name)
{	
	    return dynamic_cast<QVGroupBox*>(getWidget(groupname,caption,name));
}


int ConfigDialog::existControl(const QString& groupname, const QString& caption, const QString& name)
{
	int position= findTab(groupname);
	if (position == -1)
		return -1;

	int j;    
	for(j=position; j <= position+ RegisteredControls[position].nrOfControls; j++)
				    if (RegisteredControls[j].caption == caption)
					if (RegisteredControls[j].name == name)
					{
					    return j;
					}
					
	return -1;
}





QKeySequence HotKey::shortCutFromFile(const QString &name)
{
	return QKeySequence(config_file.readEntry("ShortCuts", name));
}

bool HotKey::shortCut(QKeyEvent *e, const QString &name)
{
	return (shortCutFromFile(name)==QKeySequence(keyEventToString(e)));
}

HotKey::HotKey(QWidget *parent, const char* name): QLineEdit(parent, name)
{

}

QString HotKey::keyEventToString(QKeyEvent *e)
{
    QString result;
	if ((e->state()& Qt::ControlButton) || (e->key() == Qt::Key_Control))
		result = "Ctrl+";
		
	    if ((e->state()& Qt::MetaButton) || (e->key() == Qt::Key_Meta))
		{result+= "Shift+Alt+";}
		
	    else {
		    if ((e->state()& Qt::ShiftButton) || (e->key() == Qt::Key_Shift))
				result+= "Shift+";
				
		    if ((e->state()& Qt::AltButton) || (e->key() == Qt::Key_Alt))
				result+= "Alt+";
				
    		 }
		 
	if (!((e->key() == Qt::Key_Control) 
		||(e->key() == Qt::Key_Shift)     
		||(e->key() == Qt::Key_Alt)
		||(e->key() == Qt::Key_Meta)))
             result+= QAccel::keyToString(QKeySequence(e->key()));

    return result;
}


void HotKey::keyPressEvent(QKeyEvent *e)
{
	setText(keyEventToString(e));
}

void HotKey::keyReleaseEvent(QKeyEvent *e)
{
    // sprawdzenie czy ostatnim znakiem jest "+" 
    // jesli tak to nie ma takiego skrotu klawiszowego
	if (text().at(text().length()-1) == QChar(43)) 
		setText("");
}


ColorButton::ColorButton(const QColor &color, QWidget *parent, const char* name): QPushButton(parent, name)
{
	setColor(color);
	connect(this, SIGNAL(released()), this, SLOT(onClick()));
}

void ColorButton::onClick()
{
	QColor color = QColorDialog::getColor(this->color(), this, tr("Color dialog"));
	if ( color.isValid() ) {
		QPixmap pm(35,10);
		pm.fill(QColor(color.name()));
		setPixmap(pm);
		actualcolor=color;
		emit changed();
			      }
			      
}

QColor ColorButton::color()
{
    return actualcolor;
}

void ColorButton::setColor(const QColor &color )
{

    if (color.isValid())
	    {
		actualcolor=color;
		QPixmap pm(35,10);
		pm.fill(QColor(color.name()));
		setPixmap(pm);
	    }	
}
