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
#include <qlayout.h>
#include <qscrollview.h>
#include <qfiledialog.h>
#include <qfontdialog.h>

#include "config_dialog.h"
#include "misc.h"
#include "debug.h"
#include "kadu.h"

QString ConfigDialog::acttab = QT_TRANSLATE_NOOP("@default", "General");
ConfigDialog *ConfigDialog::configdialog = NULL;
QApplication *ConfigDialog::appHandle = NULL;

QValueList<ConfigDialog::RegisteredControl> ConfigDialog::RegisteredControls;
QValueList<ConfigDialog::ElementConnections> ConfigDialog::SlotsOnCreate;
QValueList<ConfigDialog::ElementConnections> ConfigDialog::SlotsOnClose;
QValueList<ConfigDialog::ElementConnections> ConfigDialog::SlotsOnApply;

bool ConfigDialog::ElementConnections::operator== (const ElementConnections& r) const
{
	return (signal==r.signal && receiver==r.receiver && slot==r.slot);
}

void ConfigDialog::showConfigDialog(QApplication* application) {
	kdebugf();
	ConfigDialog *cd;
	
	if (configdialog)
	{
		configdialog->setActiveWindow();
		configdialog->raise();
	}
	else 	
	{
		cd = new ConfigDialog(application, kadu, "config_dialog");
		cd->show();
	}
}

void ConfigDialog::closeDialog()
{
	kdebugf();
	if (configdialog)
	{
		configdialog->close();
		delete configdialog;
	}
}

ConfigDialog::ConfigDialog(QApplication *application, QWidget *parent, const char *name) : QDialog(parent, name) {

	ConfigDialog::appHandle=application;
	setWFlags(Qt::WDestructiveClose);
	QGridLayout* dialogLayout=new QGridLayout(this, 1, 1, 11, 6);
	listBox= new QListBox(this);
	view = new QScrollView(this);
	view->setResizePolicy(QScrollView::AutoOneFit);

	QVGroupBox* box= new QVGroupBox(view);
	view->addChild(box);

	dialogLayout->addWidget(listBox, 0, 0);
	dialogLayout->addWidget(view, 0, 1);

	int actualtab= 0;
	int nexttab= 0;
	int actualparent=0;
	QString actualparentname= "";
	int num=0;

	QValueList<RegisteredControl>::iterator i=RegisteredControls.begin();
	while (i!=RegisteredControls.end())
	{
		if((*i).type==CONFIG_DELETED)
			i=RegisteredControls.remove(i);
		else
			i++;
	}

	for(QValueList<RegisteredControl>::iterator i=RegisteredControls.begin(); i!=RegisteredControls.end(); i++, num++)
	{
// wyswietla cala liste 
//		kdebugm(KDEBUG_DUMP, "%d: (%d) "+(*i).group+"->"+(*i).parent+"->"+(*i).caption+"->"+(*i).name+"\n", num, (*i).nrOfControls);
		
		QWidget* parent=NULL;
		if((*i).type!=CONFIG_TAB)
		{
			actualtab= findTab((*i).group,actualtab);
			nexttab= findNextTab(actualtab+1);

			if (nexttab == -1) 
				nexttab= RegisteredControls.count()-1;
			
			int z;
			if ((*i).parent == actualparentname)
				parent=RegisteredControls[actualparent].widget;
			else
				for (z=actualtab; z<nexttab; z++)
					if ((*i).parent == RegisteredControls[z].caption && RegisteredControls[z].type!=CONFIG_DELETED)
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
				check->setChecked((*i).config->readBoolEntry((*i).group, (*i).entry, (*i).defaultS));
				(*i).widget=check;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			
			case CONFIG_COLORBUTTON:
			{
				QColor col((*i).defaultS);
				ColorButton* colorbutton=new ColorButton((*i).config->readColorEntry((*i).group, (*i).entry, &col), parent, (*i).name);
				colorbutton->setMaximumSize(QSize(50,25));
				(*i).widget=colorbutton;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_COMBOBOX:
			{
				QHBox* hbox=new QHBox(parent);
				new QLabel(appHandle->translate("@default",(*i).caption), hbox);
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
				box->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum));
				break;
			}
			case CONFIG_HOTKEYEDIT:
			{
				QHBox* hbox=new QHBox(parent);
				QLabel *lab=new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				HotKey* hotkey=new HotKey(hbox, (*i).name);
				hotkey->setShortCut((*i).config->readEntry((*i).group, (*i).entry, (*i).defaultS));
				hbox->setStretchFactor(lab, 1000);
				static int hotwidth=int(hotkey->fontMetrics().width("Ctrl+Alt+Shift+F12")*1.5);
				hotkey->setFixedWidth(hotwidth);
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
				new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QLineEdit* line=new QLineEdit(hbox, (*i).name);
				line->setText((*i).config->readEntry((*i).group, (*i).entry, (*i).defaultS));
				(*i).widget=line;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_LINEEDIT2:
			{
				QHBox* hbox=new QHBox(parent);
				new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QLineEdit* line=new QLineEdit(hbox, (*i).name);
				line->setText((*i).defaultS);
				(*i).widget=line;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_TEXTEDIT:
			{
				QVBox* hbox=new QVBox(parent);
				new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QTextEdit* line=new QTextEdit(hbox, (*i).name);
				line->setTextFormat(Qt::PlainText);
				line->setText((*i).config->readEntry((*i).group, (*i).entry, (*i).defaultS));
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
			case CONFIG_LISTVIEW:
			{	
				QListView* listview= new QListView(parent, (*i).caption);
				(*i).widget=listview;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_PUSHBUTTON:
			{
				QPushButton *button =new QPushButton(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				button->setIconSet(icons_manager.loadIcon((*i).defaultS));
				(*i).widget=button;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_SELECTFONT:
			{
				QFont def_font((*i).defaultS);
				(*i).widget=new SelectFont((*i).caption, (*i).config->readFontEntry((*i).group, (*i).entry, &def_font), parent, (*i).name, (*i).tip);
				break;
			}
			case CONFIG_SELECTPATHS:
			{
				QPushButton *button =new QPushButton(appHandle->translate("@default",(*i).caption), parent);
				SelectPaths* paths=new SelectPaths(button, (*i).name);
				button->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
				(*i).widget=paths;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				connect(button, SIGNAL(clicked()), paths, SLOT(show()));
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
				slider->setValue((*i).config->readNumEntry((*i).group, (*i).entry,value));
				slider->setTickmarks(QSlider::Below);
				(*i).widget=slider;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_SPINBOX:
			{	
				QHBox* hbox=new QHBox(parent);
				new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QStringList values= QStringList::split(",", (*i).defaultS);
				int minVal=values[0].toInt();
				int maxVal=values[1].toInt();
				int step=values[2].toInt();
				int value=values[3].toInt();
				QSpinBox *spinbox=new QSpinBox(minVal, maxVal, step, hbox);
				int val=(*i).config->readNumEntry((*i).group, (*i).entry,value);
				spinbox->setValue(val);
				(*i).widget=spinbox;
				if ((*i).tip.length()) QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_TAB:
			{
				listBox->insertItem(icons_manager.loadIcon((*i).defaultS), appHandle->translate("@default",(*i).caption));
				QVBox *subbox= new QVBox(box);
				(*i).widget=subbox;
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
				box->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum));
				break;
			}
			case CONFIG_DELETED:
				kdebugm(KDEBUG_ERROR, "CONFIG_DELETED found!\n");
				break;
		}

		for(QValueList<ElementConnections>::iterator k=(*i).ConnectedSlots.begin(); k!=(*i).ConnectedSlots.end(); k++)
			if (!connect((*i).widget, (*k).signal, (*k).receiver, (*k).slot))
				kdebugm(KDEBUG_ERROR, "unable to connect signal: "+(*k).signal+" slot: "+(*k).slot+"\n");
	}

	for(QValueList<ElementConnections>::iterator a=SlotsOnCreate.begin(); a!=SlotsOnCreate.end(); a++)
		connect(this, SIGNAL(create()), (*a).receiver, (*a).slot);
	
	for(QValueList<ElementConnections>::iterator a=SlotsOnApply.begin(); a!=SlotsOnApply.end(); a++)
		connect(this, SIGNAL(apply()), (*a).receiver, (*a).slot);

	for(QValueList<ElementConnections>::iterator a=SlotsOnClose.begin(); a!=SlotsOnClose.end(); a++)
		connect(this, SIGNAL(destroy()), (*a).receiver, (*a).slot);


	listBox->setCurrentItem(listBox->findItem(appHandle->translate("@default",acttab)));

	listBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, listBox->sizePolicy().hasHeightForWidth()));
	
	QHBox* buttonbox= new QHBox(this);
	okButton= new QPushButton(tr("Ok"), buttonbox, "configdialog_ok_button");
	applyButton = new QPushButton(tr("Apply"), buttonbox, "configdialog_apply_button");
	cancelButton= new QPushButton(tr("Cancel"), buttonbox, "configdialog_cancel_button");
	
	dialogLayout->addWidget(buttonbox, 1, 1,Qt::AlignRight);
	
	connect(okButton, SIGNAL(clicked()), this, SLOT(updateAndCloseConfig()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(updateConfig()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	connect(listBox, SIGNAL(highlighted(const QString&)), this, SLOT(changeTab(const QString&)));
	
	setCaption(tr("Kadu configuration"));

	loadGeometry(this, "General", "ConfigGeometry", 0, 0, 790, 480);

	configdialog = this;
	emit create();

	changeTab(appHandle->translate("@default",acttab));
}

ConfigDialog::~ConfigDialog() {
	emit destroy();
	configdialog = NULL;
}

void ConfigDialog::changeTab(const QString& name)
{
	kdebugf();

	int tab=0;
	while (tab != -1)
	{
		if (appHandle->translate("@default",RegisteredControls[tab].caption) == name)
		{
			RegisteredControls[tab].widget->show();
			acttab= RegisteredControls[tab].caption;
		}
		else 
			RegisteredControls[tab].widget->hide();

		tab=findNextTab(tab+1);
	}
	kdebugm(KDEBUG_FUNCTION_END, "ConfigDialog::changeTab(): active Tab=%s\n", (const char *)unicode2latin(acttab));
}

void ConfigDialog::updateConfig(void) 
{
	for(QValueList<RegisteredControl>::iterator i=RegisteredControls.begin(); i!=RegisteredControls.end(); i++)
	{
		if (!(*i).widget)
			continue;
		switch((*i).type)
		{
			case CONFIG_CHECKBOX:
			{
				(*i).config->writeEntry((*i).group, (*i).entry, ((QCheckBox*)((*i).widget))->isChecked());
				break;
			}
			case CONFIG_HOTKEYEDIT:
			{
				(*i).config->writeEntry((*i).group, (*i).entry, ((HotKey*)((*i).widget))->getShortCutString());
				break;
			}
			case CONFIG_LINEEDIT:
			{
				(*i).config->writeEntry((*i).group, (*i).entry, ((QLineEdit*)((*i).widget))->text());
				break;
			}
			case CONFIG_TEXTEDIT:
			{
				(*i).config->writeEntry((*i).group, (*i).entry, ((QTextEdit*)((*i).widget))->text());
				break;
			}
			case CONFIG_SLIDER:
			{
				(*i).config->writeEntry((*i).group, (*i).entry, ((QSlider*)((*i).widget))->value());
				break;
			}
			case CONFIG_SPINBOX:
			{
				(*i).config->writeEntry((*i).group, (*i).entry, ((QSpinBox*)((*i).widget))->value());
				break;
			}
			case CONFIG_COLORBUTTON:
			{
				(*i).config->writeEntry((*i).group, (*i).entry, ((ColorButton*)((*i).widget))->color());
				break;
			}
			case CONFIG_SELECTFONT:
			{
				(*i).config->writeEntry((*i).group, (*i).entry, ((SelectFont*)((*i).widget))->font());
				break;
			}
			default:
				break;
		}
	}
	
	saveGeometry(this, "General", "ConfigGeometry");

	emit apply();

	config_file.sync();
	kdebugf2();
}

void ConfigDialog::updateAndCloseConfig()
{
	updateConfig();
	
	close();
}

void ConfigDialog::addCheckBox(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const bool defaultS, const QString& tip, const QString& name)
{
	addCheckBox(&config_file, groupname, parent, caption, entry, defaultS, tip, name);
}

void ConfigDialog::addCheckBox(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const bool defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_CHECKBOX, groupname, parent, caption, name);
		c.config=config;
		c.entry=entry;
		c.defaultS=QString::number(defaultS);
		c.tip=tip;
		if (addControl(groupname,c) == 0)
			c.config->addVariable(groupname, entry, defaultS);
	}
}


void ConfigDialog::addColorButton(ConfigFile *config, const QString& groupname,
				const QString& parent, const QString& caption, const QString &entry,
				const QColor& color, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_COLORBUTTON, groupname, parent, caption, name);
		c.config=config;
		c.entry=entry;
		c.defaultS=color.name();
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addColorButton(const QString& groupname,
				const QString& parent, const QString& caption, const QString &entry,
				const QColor& color, const QString& tip, const QString& name)
{
	addColorButton(&config_file, groupname, parent, caption, entry, color, tip, name);
}


void ConfigDialog::addComboBox(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_COMBOBOX, groupname, parent, caption, name);
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addGrid(const QString& groupname,
			const QString& parent, const QString& caption, const int nrColumns, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_GRID, groupname, parent, caption, name);
		c.defaultS=QString::number(nrColumns);
		addControl(groupname,c);
	}
}

void ConfigDialog::addHBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_HBOX, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}


void ConfigDialog::addHGroupBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_HGROUPBOX, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}

void ConfigDialog::addHotKeyEdit(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name)
{
	addHotKeyEdit(&config_file, groupname, parent, caption, entry, defaultS, tip, name);
}

void ConfigDialog::addHotKeyEdit(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_HOTKEYEDIT, groupname, parent, caption, name);
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;
		
		if (addControl(groupname,c) == 0)
		// zapisujemy warto¶æ domy¶ln±, aby ju¿ wiêcej nie musieæ
		// jej podawaæ przy czytaniu z pliku conf		
			config_file.addVariable(groupname, entry, defaultS);	
	}
}

void ConfigDialog::addLineEdit2(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_LINEEDIT2, groupname, parent, caption, name);
		c.defaultS=defaultS;
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addLineEdit(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name)
{
	addLineEdit(&config_file, groupname, parent, caption, entry, defaultS, tip, name);
}


void ConfigDialog::addLineEdit(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_LINEEDIT, groupname, parent, caption, name);
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;

		if (addControl(groupname,c) == 0)
			c.config->addVariable(groupname, entry, defaultS);	
	}
}

void ConfigDialog::addTextEdit(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name)
{
	addTextEdit(&config_file, groupname, parent, caption, entry, defaultS, tip, name);
}

void ConfigDialog::addTextEdit(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_TEXTEDIT, groupname, parent, caption, name);
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;

		if (addControl(groupname,c) == 0)
			c.config->addVariable(groupname, entry, defaultS);	
	}
}

void ConfigDialog::addLabel(const QString& groupname,
			const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_LABEL, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}


void ConfigDialog::addListBox(const QString& groupname,
			const QString& parent, const QString& caption, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_LISTBOX, groupname, parent, caption, name);
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addListView(const QString& groupname,
			const QString& parent, const QString& caption, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_LISTVIEW, groupname, parent, caption, name);
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addPushButton(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString &iconFileName, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_PUSHBUTTON, groupname, parent, caption, name);
		c.defaultS=iconFileName;
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addSelectPaths(const QString& groupname,
			const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_SELECTPATHS, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}

void ConfigDialog::addSlider(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry,
			const int minValue, const int maxValue,
			const int pageStep, const int value, const QString& tip, const QString& name)
{
	addSlider(&config_file, groupname, parent, caption, entry, minValue, maxValue,
			pageStep, value, tip, name);
}

void ConfigDialog::addSlider(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry,
			const int minValue, const int maxValue,
			const int pageStep, const int value, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_SLIDER, groupname, parent, caption, name);
		c.config=config;
		c.entry=entry;
		c.tip=tip;
		c.defaultS=QString::number(minValue)+","+QString::number(maxValue)+","+QString::number(pageStep)+","+QString::number(value);

		if (addControl(groupname,c) == 0)
			c.config->addVariable(groupname, entry, value);
	}
}

void ConfigDialog::addSpinBox(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry,
			const int minValue, const int maxValue, const int step, const int value, const QString& tip, const QString& name)
{
	addSpinBox(&config_file, groupname, parent, caption, entry, minValue, maxValue,
			step, value, tip, name);

}

void ConfigDialog::addSpinBox(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry,
			const int minValue, const int maxValue, const int step, const int value, const QString& tip, const QString& name)
{

	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_SPINBOX, groupname, parent, caption, name);
		c.config=config;
		c.entry=entry;
		c.tip=tip;
		c.defaultS=QString::number(minValue)+","+QString::number(maxValue)+","+QString::number(step)+","+QString::number(value);
		
		if (addControl(groupname,c) == 0)
			c.config->addVariable(groupname, entry, value);
	}
}

void ConfigDialog::addTab(const QString& caption, const QString& iconFileName)
{
	if (findTab(caption) == -1)
	{
		RegisteredControl c(CONFIG_TAB, QString::null, QString::null, caption);
		c.defaultS=iconFileName;
		RegisteredControls.append(c);
	}	
}

void ConfigDialog::addVBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_VBOX, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}

void ConfigDialog::addVGroupBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_VGROUPBOX, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}

void ConfigDialog::addSelectFont(const QString& groupname, const QString& parent,
				const QString& caption, const QString& entry, const QString& defaultS,
				const QString &tip, const QString& name)
{
	addSelectFont(&config_file, groupname, parent, caption, entry, defaultS, tip, name);
}

void ConfigDialog::addSelectFont(ConfigFile *config, const QString& groupname, const QString& parent,
				const QString& caption, const QString& entry, const QString& defaultS,
				const QString &tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1){
		RegisteredControl c(CONFIG_SELECTFONT, groupname, parent, caption, name);
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;

		if (addControl(groupname,c) == 0)
			c.config->addVariable(groupname, entry, defaultS);	
	}
}

ConfigDialog::RegisteredControl::RegisteredControl(RegisteredControlType t,
	const QString &groupname,
	const QString &parent,
	const QString &caption,
	const QString &name)
{
	type=t;
	this->group=groupname;
	this->parent=parent;
	this->caption=caption;
	this->name=name;
	widget=NULL;
	nrOfControls=0;
	this->config=NULL;
}

void ConfigDialog::connectSlot(const QString& groupname, const QString& caption, const char* signal, const QObject* receiver, const char* slot,const QString& name)
{
	for(QValueList<RegisteredControl>::iterator j=RegisteredControls.begin(); j!=RegisteredControls.end(); j++)
		if(((*j).group == groupname) && ((*j).caption == caption) && ((*j).name == name) && (*j).type!=CONFIG_DELETED)
		{
			ElementConnections c;
			c.signal=signal;
			c.receiver=(QObject *)receiver;
			c.slot=slot;
			(*j).ConnectedSlots.append(c);
			kdebugm(KDEBUG_INFO, "Slot connected:: %s\n",slot);
			break;
		}
}

void ConfigDialog::disconnectSlot(const QString& groupname, const QString& caption, const char* signal, const QObject* receiver, const char* slot,const QString& name)
{
	for(QValueList<RegisteredControl>::iterator j=RegisteredControls.begin(); j!=RegisteredControls.end(); j++)
		if(((*j).group == groupname) && ((*j).caption == caption) && ((*j).name == name) && (*j).type!=CONFIG_DELETED)
		{
			ElementConnections c;
			c.signal=signal;
			c.receiver=(QObject *)receiver;
			c.slot=slot;
			(*j).ConnectedSlots.remove((*j).ConnectedSlots.find(c));
			kdebugm(KDEBUG_INFO, "Slot disconnected:: %s\n",slot);
			break;
		}
}


void ConfigDialog::registerSlotOnCreate(const QObject* receiver, const char* name)
{
	ElementConnections c;
	c.receiver=(QObject *)receiver;
	c.slot=name;
	SlotsOnCreate.append(c);
}

void ConfigDialog::unregisterSlotOnCreate(const QObject* receiver, const char* name)
{
	ElementConnections c;
	c.receiver=(QObject *)receiver;
	c.slot=name;
	SlotsOnCreate.remove(SlotsOnCreate.find(c));
}

void ConfigDialog::registerSlotOnClose(const QObject* receiver, const char* name)
{
	ElementConnections c;
	c.receiver=(QObject *)receiver;
	c.slot=name;
	SlotsOnClose.append(c);
}

void ConfigDialog::unregisterSlotOnClose(const QObject* receiver, const char* name)
{
	ElementConnections c;
	c.receiver=(QObject *)receiver;
	c.slot=name;
	SlotsOnClose.remove(SlotsOnClose.find(c));
}

void ConfigDialog::registerSlotOnApply(const QObject* receiver, const char* name)
{
	ElementConnections c;
	c.receiver=(QObject *)receiver;
	c.slot=name;
	SlotsOnApply.append(c);
}

void ConfigDialog::unregisterSlotOnApply(const QObject* receiver, const char* name)
{
	ElementConnections c;
	c.receiver=(QObject *)receiver;
	c.slot=name;
	SlotsOnApply.remove(SlotsOnApply.find(c));
}

int ConfigDialog::findPreviousTab(int pos)
{
	if (RegisteredControls.isEmpty())
		return -1;
	if (pos<0)
		pos=0;
	if ((uint)(pos+1)>RegisteredControls.count())
		pos=RegisteredControls.count()-1;
	for(; pos>=0; pos--)
		if (RegisteredControls[pos].type == CONFIG_TAB)
			return pos;
	return -1;
// -1 oznacza ze nie ma Tab'a
}

int ConfigDialog::findNextTab(int pos)
{
	if (RegisteredControls.isEmpty())
		return -1;
	if (pos<0)
		pos=0;

	int count=RegisteredControls.count();

	if (pos>=count)
		pos=count;

	for(; pos<count; pos++) {
		if (RegisteredControls[pos].type == CONFIG_TAB) {
			return pos;
			}
		}
	return -1;
//	zwraca miejsce znalezienia TAB'a
//	jesli nie znajdzie to zwraca -1	
}

int ConfigDialog::findTab(const QString& groupname, int pos)
{
	if (RegisteredControls.isEmpty())
		return -1;
	if (pos<0)
		pos=0;
	int count=RegisteredControls.count();
	if (pos>=count)
		pos=count;

	pos=findPreviousTab(pos);
	if (pos<0)
		pos=0;
	for(; pos<count; pos++)
		if(RegisteredControls[pos].type==CONFIG_TAB && RegisteredControls[pos].caption == groupname)
			return pos;
	return -1;
// jesli znajdzie odpowiedni TAB to wzraca miejsce znalezienia 
// jesli nie znajdzie to zwraca -1
}

void ConfigDialog::removeControl(const QString& groupname, const QString& caption, const QString& name)
{
	int i=existControl(groupname, caption, name);
	
	//kdebugm(KDEBUG_INFO, "nrOfControls=%i "+groupname+"\\"+caption+"\\"+name+"\n", RegisteredControls[i].nrOfControls);
	//
	if(i<0)
	{
		kdebugm(KDEBUG_ERROR, "No such control %s %s %s\n", groupname.ascii(), caption.ascii(), name.ascii());
		return;
	}
 	
	if(RegisteredControls[i].nrOfControls!=0)
	{
		kdebugm(KDEBUG_INFO, "Container not empty: %d %s %s %s\n", RegisteredControls[i].nrOfControls, groupname.ascii(), caption.ascii(), name.ascii());
		return;
	}

	decreaseNrOfControls(i);

	RegisteredControls[i].type=CONFIG_DELETED;
	kdebugm(KDEBUG_INFO, "control deleted "+groupname+"\\"+caption+"\\"+name+"\n");
}

void ConfigDialog::removeTab(const QString& caption)
{
	int pos=findTab(caption);
	
	if(pos<0)
	{
		kdebugm(KDEBUG_WARNING, "Tab %s not found\n", caption.ascii());
		return;
	}

	kdebugm(KDEBUG_INFO, "removeTab: nrOfControls=%i\n", RegisteredControls[pos].nrOfControls);
	
	if(RegisteredControls[pos].nrOfControls==0)
		RegisteredControls[pos].type=CONFIG_DELETED;
	else
		kdebugm(KDEBUG_INFO, "can't remove tab!\n");
}

void ConfigDialog::decreaseNrOfControls(int control)
{
	int j;
	QString localparent=RegisteredControls[control].parent;
	for (j=control; j>=0; j--)
		if(RegisteredControls[j].type!=CONFIG_DELETED)
			if (RegisteredControls[j].caption==localparent)
			{
				RegisteredControls[j].nrOfControls--;
				localparent= RegisteredControls[j].parent;
				if (RegisteredControls[j].type == CONFIG_TAB)
					break;
			}
}

void ConfigDialog::increaseNrOfControls(const int startpos, const int endpos, const QString& parent)
{
	int j;
	QString localparent=parent;
	for (j=endpos;j>=startpos;j--)
		if(RegisteredControls[j].type!=CONFIG_DELETED)
			if (RegisteredControls[j].caption==localparent)
			{
				RegisteredControls[j].nrOfControls++;
				localparent= RegisteredControls[j].parent;
				if (RegisteredControls[j].type == CONFIG_TAB)
					break;
			}
}

int ConfigDialog::addControl(const QString& groupname, const ConfigDialog::RegisteredControl& control)
{
	int position= findTab(groupname);
	if (position == -1) 
	{
		kdebugm(KDEBUG_ERROR, "There is no Tab: "+groupname+"\n");
		return -1;
	}
	int nexttab= findNextTab(position+1);
	if (nexttab == -1)
		nexttab= RegisteredControls.count()-1;

	int j;
	for (j=position; j<=nexttab; j++)
		if(RegisteredControls[j].type!=CONFIG_DELETED)
			if (RegisteredControls[j].caption == control.parent)
			{
				int nrOfControls=RegisteredControls[j].nrOfControls;
				increaseNrOfControls(position,j,control.parent);
				if (RegisteredControls[j].caption == RegisteredControls.last().caption)
					RegisteredControls.append(control);
				else
					RegisteredControls.insert(RegisteredControls.at(j+nrOfControls+1),control);
				return 0;
			}	
	
	return -2;

// je¶li warto¶æ zwrócona jest -1 to nie znaleziono takiego TAB'a
// je¶li warto¶æ zwrócona jest -2 to nie znaleziono takiego parent'a
// je¶li warto¶æ zwrócona jest 0 to wszystko jest w porz±dku
}

QWidget* ConfigDialog::getWidget(const QString& groupname, const QString& caption, const QString& name)
{	
	int nr=existControl(groupname,caption,name);
	if (nr!=-1)
		return (RegisteredControls[nr].widget);
	kdebugm(KDEBUG_PANIC, "Warning there is no \\" +groupname+ "\\"+ caption+ "\\"+ name+ "\\ control\n");
	return NULL;
}

bool ConfigDialog::dialogOpened()
{
	return (configdialog!=NULL);
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

QTextEdit* ConfigDialog::getTextEdit(const QString& groupname, const QString& caption, const QString& name)
{	
	return dynamic_cast<QTextEdit*>(getWidget(groupname,caption,name));
}

QLabel* ConfigDialog::getLabel(const QString& groupname, const QString& caption, const QString& name)
{	
	return dynamic_cast<QLabel*>(getWidget(groupname,caption,name));
}

QListBox* ConfigDialog::getListBox(const QString& groupname, const QString& caption, const QString& name)
{	
	return dynamic_cast<QListBox*>(getWidget(groupname,caption,name));
}

QListView* ConfigDialog::getListView(const QString& groupname, const QString& caption, const QString& name)
{	
	return dynamic_cast<QListView*>(getWidget(groupname,caption,name));
}

QPushButton* ConfigDialog::getPushButton(const QString& groupname, const QString& caption, const QString& name)
{	
	return dynamic_cast<QPushButton*>(getWidget(groupname,caption,name));
}

SelectFont* ConfigDialog::getSelectFont(const QString& groupname, const QString& caption, const QString& name)
{	
	return dynamic_cast<SelectFont*>(getWidget(groupname,caption,name));
}

SelectPaths* ConfigDialog::getSelectPaths(const QString& groupname, const QString& caption, const QString& name)
{	
	return dynamic_cast<SelectPaths*>(getWidget(groupname,caption,name));
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
	int count;
	for(j=position, count=0; count <= RegisteredControls[position].nrOfControls; j++)
		if(RegisteredControls[j].type!=CONFIG_DELETED)
		{
			count++;
			if (RegisteredControls[j].caption == caption)
				if (RegisteredControls[j].name == name)
					return j;
		}
	return -1;
}

QKeySequence HotKey::shortCutFromFile(const QString& groupname, const QString &name)
{
	return QKeySequence(config_file.readEntry(groupname, name));
}

bool HotKey::shortCut(QKeyEvent *e,const QString& groupname, const QString &name)
{
	return (shortCutFromFile(groupname, name)==QKeySequence(keyEventToString(e)));
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
		result+= "Shift+Alt+";
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

QKeySequence HotKey::getShortCut()
{
	return QKeySequence(text());
}

QString HotKey::getShortCutString()
{
	return text();
}

void HotKey::setShortCut(const QString& shortcut)
{
	QKeySequence str(shortcut);
	if (str == QKeySequence())
		setText("");
	else
		setText(shortcut);
}

void HotKey::setShortCut(const QKeySequence& shortcut)
{
	return setText(shortcut);
}

SelectFont::SelectFont(const QString &text, const QFont &val, QWidget *parent, const char *name, const QString &tip)
	: QHBox(parent, name)
{
	new QLabel(qApp->translate("@default", text), this);
	(new QWidget(this))->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	fontEdit=new QLineEdit(this);
	fontEdit->setReadOnly(true);
	fontEdit->setFixedWidth(int(fontEdit->fontMetrics().width("Bitstream Vera Sans Mono 15")*1.5));
	QPushButton *button=new QPushButton(tr("Select"), this, "selectfont_selectbutton");
	setFont(val);
	connect(button, SIGNAL(clicked()), this, SLOT(onClick()));
	QToolTip::add(button, tip);
}

void SelectFont::setFont(const QFont &font)
{
	currentFont=font;
	fontEdit->setText(QString("%1 %2").arg(currentFont.family()).arg(currentFont.pointSize()));
}

QFont SelectFont::font()
{
	return currentFont;
}

void SelectFont::onClick()
{
	bool ok;
	QFont f=QFontDialog::getFont(&ok, currentFont, NULL, "font_dialog_cd");
	if (ok)
	{
		setFont(f);
		emit changed(name(), f);
	}
}

ColorButton::ColorButton(const QColor &color, QWidget *parent, const char* name): QPushButton(parent, name)
{
	setColor(color);
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void ColorButton::onClick()
{
	QColor color = QColorDialog::getColor(this->color(), this, tr("Color dialog"));
	setColor(color);
	if (color.isValid())
		emit changed(name(), color);
}

QColor ColorButton::color()
{
	return actualcolor;
}

void ColorButton::setColor(const QColor &color)
{
	if (color.isValid())
	{
		actualcolor=color;
		QPixmap pm(35,10);
		pm.fill(QColor(color.name()));
		setPixmap(pm);
	}	
}

SelectPaths::SelectPaths(QWidget *parent, const char* name): QDialog(parent, name)
{	
	kdebugf();
	setCaption(tr("Select paths"));
	QGridLayout *layout= new QGridLayout(this, 1,0, 5, 10);
	
	QVBox *vertical=new QVBox(this);
	pathListBox= new QListBox(vertical);
	pathListBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
	QHBox *buttons= new QHBox(vertical);
	add= new QPushButton(tr("Add"), buttons);
	change= new QPushButton(tr("Replace"), buttons);
	remove= new QPushButton(tr("Remove"), buttons);

	QHBox *editpath=new QHBox(this);
	pathEdit= new QLineEdit(editpath);
	findPath= new QPushButton(tr("Choose"), editpath);

	QHBox *okcancel=new QHBox(this);
	ok= new QPushButton(tr("Ok"), okcancel);
	ok->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	cancel= new QPushButton(tr("Cancel"), okcancel);
	cancel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	
	layout->addWidget(vertical, 0, 0);
	layout->addWidget(editpath, 1, 0);
	layout->addWidget(okcancel, 2, 0, Qt::AlignRight);
	connect(cancel, SIGNAL(clicked()), this, SLOT(hide()));
	connect(ok, SIGNAL(clicked()), this, SLOT(okButton()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(cancelButton()));
	connect(change, SIGNAL(clicked()), this, SLOT(replacePath()));
	connect(add, SIGNAL(clicked()), this, SLOT(addPath()));
	connect(remove, SIGNAL(clicked()), this, SLOT(deletePath()));
	connect(findPath, SIGNAL(clicked()), this, SLOT(choosePath()));
	connect(pathEdit, SIGNAL(returnPressed()), this, SLOT(addPath()));
	resize(330,330);
}

QStringList SelectPaths::getPathList()
{
	kdebugf();
	return releaseList;
}


void SelectPaths::setPathList(QStringList& list)
{
	kdebugf();
	pathListBox->clear();
	pathListBox->insertStringList(list);
	pathListBox->setSelected(0, true);
	releaseList=list;
}

void SelectPaths::addPath()
{
	kdebugf();
	QString dirtoadd=pathEdit->text();
	QDir dir;
	if (dirtoadd!= "")
		if (dir.cd(dirtoadd))
		{
			if (dirtoadd.right(1) != "/")
				dirtoadd+="/";
			if (!pathListBox->findItem(dirtoadd, Qt::ExactMatch)) 
				pathListBox->insertItem(dirtoadd);
		}
	pathListBox->setSelected(pathListBox->currentItem(),true);
}

void SelectPaths::replacePath()
{
	kdebugf();
	QString dirtochange=pathEdit->text();

	QDir dir;
	if (dirtochange!= "")
		if (dir.cd(dirtochange))
			if (pathListBox->isSelected(pathListBox->currentItem()))
			{
				if (dirtochange.right(1) != "/")
					dirtochange+="/";
				if (!pathListBox->findItem(dirtochange, Qt::ExactMatch)) 
					pathListBox->changeItem(dirtochange, pathListBox->currentItem());
				pathListBox->setSelected(pathListBox->currentItem(), true);
			}
}

void SelectPaths::deletePath()
{
	kdebugf();
	if (pathListBox->isSelected(pathListBox->currentItem()))
	{
		pathListBox->removeItem(pathListBox->currentItem());
		pathListBox->setSelected(pathListBox->currentItem(),true);
	}
}

void SelectPaths::choosePath()
{
	kdebugf();

	QDir dir;
	QString startdir="/";
	if (dir.cd(pathEdit->text()) && (pathEdit->text()!= ""))
		startdir=pathEdit->text();
	QString s= QFileDialog::getExistingDirectory(startdir, this, "getDirectory", tr("Choose a directory"));
	if (s!="")
		pathEdit->setText(s);
}

void SelectPaths::okButton()
{
	kdebugf();
	releaseList.clear();
	for (unsigned int i=0; i<pathListBox->count(); i++)
		releaseList.append(pathListBox->text(i));

	pathEdit->setText("");
	hide();
	emit changed(releaseList);
}

void SelectPaths::cancelButton()
{
	kdebugf();
	pathListBox->clear();
	pathListBox->insertStringList(releaseList);
	pathEdit->setText("");
	hide();
}

void SelectPaths::closeEvent(QCloseEvent *e)
{
	e->ignore();
	cancelButton();
}

void SelectPaths::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Delete)
		if (pathListBox->isSelected(pathListBox->currentItem()))
		{
			pathListBox->removeItem(pathListBox->currentItem());
			pathListBox->setSelected(pathListBox->currentItem(), true);
		}

	if (e->key() == Qt::Key_Escape)
		cancelButton();
}

SelectPaths::~SelectPaths()
{
	kdebugf();
}
