/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qaccel.h>
#include <qcolordialog.h>
#include <qcolor.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qfiledialog.h>
#include <qfontdialog.h>
#include <qgrid.h>
#include <qhbuttongroup.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qradiobutton.h>
#include <qscrollview.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qtooltip.h>
#include <qvgroupbox.h>
#include <qvbuttongroup.h>

#include "config_dialog.h"
#include "misc.h"
#include "debug.h"

QString ConfigDialog::acttab = QT_TRANSLATE_NOOP("@default", "General");
ConfigDialog *ConfigDialog::configdialog = NULL;
QApplication *ConfigDialog::appHandle = NULL;

QValueList<ConfigDialog::RegisteredControl> ConfigDialog::RegisteredControls;
QValueList<ConfigDialog::ElementConnections> ConfigDialog::SlotsOnCreate;
QValueList<ConfigDialog::ElementConnections> ConfigDialog::SlotsOnClose;
QValueList<ConfigDialog::ElementConnections> ConfigDialog::SlotsOnApply;

ConfigDialog::ElementConnections::ElementConnections()
	: signal(QString::null), receiver(NULL), slot(QString::null)
{
}

ConfigDialog::ElementConnections::ElementConnections(QString signal1, const QObject *receiver1, QString slot1)
	: signal(signal1), receiver(receiver1), slot(slot1)
{
}

bool ConfigDialog::ElementConnections::operator== (const ElementConnections& r) const
{
	return (signal==r.signal && receiver==r.receiver && slot==r.slot);
}

void ConfigDialog::showConfigDialog(QApplication* application) 
{
	kdebugf();
	
	if (configdialog)
	{
		configdialog->setActiveWindow();
		configdialog->raise();
	}
	else 	
		(new ConfigDialog(application, 0, "configDialog"))->show();
	kdebugf2();
}

void ConfigDialog::closeDialog()
{
	kdebugf();
	if (configdialog)
	{
		configdialog->close();
		delete configdialog;
	}
	kdebugf2();
}

void ConfigDialog::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Key_Escape)
	{
		e->accept();
		closeDialog();
	}
	else
		QVBox::keyPressEvent(e);
}

ConfigDialog::ConfigDialog(QApplication *application, QWidget *parent, const char *name) : QVBox(parent, name)
{
	kdebugf();

	ConfigDialog::appHandle=application;
	setWFlags(Qt::WDestructiveClose);
	
	configdialog = this;
	
	setCaption(tr("Kadu configuration"));
		
	QHBox *center = new QHBox(this,"center");
	center->setMargin(10);
	center->setSpacing(10);
	
	QVBox *left = new QVBox(center,"left");
	
	listBox = new QListBox(left,"listbox");
	listBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, 
	listBox->sizePolicy().hasHeightForWidth()));
	
	QVGroupBox* vgb_viewcontainer = new QVGroupBox(center,"mainGroupBox");
	
	view = new QScrollView(vgb_viewcontainer,"scrollView");
	view->setResizePolicy(QScrollView::AutoOneFit);
	view->setFrameStyle(QFrame::NoFrame);
 
	QVGroupBox* box= new QVGroupBox(view, "groupBox");
	box->setFrameStyle(QFrame::NoFrame);
	view->addChild(box);
	
	int actualtab = 0;
	int nexttab = 0;
	int actualparent = 0;
	QString actualparentname = "";
	int num = 0;

	QValueList<RegisteredControl>::iterator i = RegisteredControls.begin();
	while (i != RegisteredControls.end())
	{
		if((*i).type == CONFIG_DELETED)
			i = RegisteredControls.remove(i);
		else
			++i;
	}

	FOREACH(i, RegisteredControls)
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
				for (z=actualtab; z<nexttab; ++z)
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
				QHBox* hbox=new QHBox(parent);
				new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QColor col((*i).defaultS);
				ColorButton* colorbutton=new ColorButton((*i).config->readColorEntry((*i).group, (*i).entry, &col), hbox, (*i).name);
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
				if ((*i).tip.length())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				if ((*i).additionalParams.size()>=2)
				{
					combo->insertStringList((*i).additionalParams[0].toStringList());
					combo->setCurrentItem((*i).additionalParams[1].toStringList().findIndex(
						(*i).config->readEntry((*i).group, (*i).entry, (*i).defaultS)));
				}
				break;
			}
			case CONFIG_GRID:
			{
				QGrid* grid=new QGrid((*i).defaultS.toInt(), parent, (*i).caption);
				(*i).widget=grid;
				break;
			}
			case CONFIG_VBOX:
			case CONFIG_HBOX:
			{
				QHBox* box;
				if ((*i).type==CONFIG_HBOX)
					box = new QHBox(parent,(*i).caption);
				else
					box = new QVBox(parent,(*i).caption);
				box->setSpacing(2);
				(*i).widget=box;
				break;
			}
			case CONFIG_VGROUPBOX:
			case CONFIG_HGROUPBOX:
			{
				QGroupBox* box;
				if ((*i).type==CONFIG_HGROUPBOX)
					box = new QHGroupBox(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				else
					box = new QVGroupBox(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				(*i).widget=box;
				box->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum));
				break;
			}
			case CONFIG_VRADIOGROUP:
			case CONFIG_HRADIOGROUP:
			case CONFIG_RADIOGROUP:
			{
				QButtonGroup* group;
				if ((*i).type==CONFIG_HRADIOGROUP)
					group = new QHButtonGroup(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				else if ((*i).type==CONFIG_VRADIOGROUP)
					group = new QVButtonGroup(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				else
					group = new QButtonGroup((*i).additionalParams[2].toInt(),
							(Qt::Orientation)(*i).additionalParams[3].toInt(),
							appHandle->translate("@default",(*i).caption),
							parent,
							(*i).name);
				
				(*i).widget=group;
				group->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum));
//				group->setExclusive(true);
				group->setRadioButtonExclusive(true);

				QStringList options=(*i).additionalParams[0].toStringList();
				QStringList values=(*i).additionalParams[1].toStringList();
				FOREACH(option, options)
					new QRadioButton(*option, group, (*i).name+(*option));
				group->setButton(values.findIndex((*i).config->readEntry((*i).group, (*i).entry, (*i).defaultS)));

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
				if ((*i).defaultS!="")
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
				SelectPaths* paths=new SelectPaths(NULL, (*i).name);
				insertChild(paths);//musimy go wstawiæ na listê dzieci, bo inaczej wycieknie nam...
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
				if ((*i).defaultS!="")
					listBox->insertItem(icons_manager.loadIcon((*i).defaultS), appHandle->translate("@default",(*i).caption));
				else
					listBox->insertItem(appHandle->translate("@default",(*i).caption));
				QVBox *subbox= new QVBox(box);
				(*i).widget=subbox;
				break;
			}
			case CONFIG_DELETED:
			{
				kdebugm(KDEBUG_ERROR, "CONFIG_DELETED found!\n");
				break;
			}
		}

		FOREACH(connection, (*i).ConnectedSlots)
		{
//			kdebugm(KDEBUG_DUMP, "connecting %p %s to %p %s\n", (*i).widget, (*connection).signal.local8Bit().data(),
//								(*connection).receiver, (*connection).slot.local8Bit().data());
			if (!connect((*i).widget, (*connection).signal, (*connection).receiver, (*connection).slot))
				kdebugm(KDEBUG_ERROR, "unable to connect signal: %s to slot: %s\n",
					(*connection).signal.local8Bit().data(), (*connection).slot.local8Bit().data());
		}
		++num;
	}

	FOREACH(conn, SlotsOnCreate)
		connect(this, SIGNAL(create()), (*conn).receiver, (*conn).slot);
	
	FOREACH(conn, SlotsOnApply)
		connect(this, SIGNAL(apply()), (*conn).receiver, (*conn).slot);

	FOREACH(conn, SlotsOnClose)
		connect(this, SIGNAL(destroy()), (*conn).receiver, (*conn).slot);


	listBox->setCurrentItem(listBox->findItem(appHandle->translate("@default",acttab)));
	
	// buttons
	QHBox *bottom = new QHBox(this, "buttons");
	bottom->setMargin(10);
	bottom->setSpacing(5);
	
	(new QWidget(bottom, "blank"))->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	okButton = new QPushButton(icons_manager.loadIcon("OkWindowButton"), tr("Ok"), bottom, "okButton");
	applyButton = new QPushButton(icons_manager.loadIcon("ApplyWindowButton"), tr("Apply"), bottom, "applyButton");
	cancelButton = new QPushButton(icons_manager.loadIcon("CloseWindowButton"), tr("Cancel"), bottom, "cancelButton");
	
	connect(okButton, SIGNAL(clicked()), this, SLOT(updateAndCloseConfig()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(updateConfig()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	// end buttons
	
	connect(listBox, SIGNAL(highlighted(const QString&)), this, SLOT(changeTab(const QString&)));
	
	changeTab(appHandle->translate("@default",acttab));
 
	configdialog = this;
	emit create();

	loadGeometry(this, "General", "ConfigGeometry", 0, 0, 790, 480);
	kdebugf2();
}

ConfigDialog::~ConfigDialog()
{
	saveGeometry(this, "General", "ConfigGeometry");
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
	kdebugmf(KDEBUG_FUNCTION_END, "active Tab=%s\n", acttab.local8Bit().data());
}

void ConfigDialog::updateConfig(void) 
{
	kdebugf();
	FOREACH(i, RegisteredControls)
	{
		if (!(*i).widget)
			continue;
		switch((*i).type)
		{
			case CONFIG_CHECKBOX:
				(*i).config->writeEntry((*i).group, (*i).entry, ((QCheckBox*)((*i).widget))->isChecked());
				break;
			case CONFIG_COMBOBOX:
				if ((*i).additionalParams.size()>=2)
				{
					int selected=((QComboBox*)((*i).widget))->currentItem();
					if (selected>=0)
						(*i).config->writeEntry((*i).group, (*i).entry, (*i).additionalParams[1].toStringList()[selected]);
					else
						kdebugm(KDEBUG_ERROR, "type:COMBO group:%s entry:%s selected:%d\n", (*i).group.local8Bit().data(), (*i).entry.local8Bit().data(), selected);
				}
				break;
			case CONFIG_HOTKEYEDIT:
				(*i).config->writeEntry((*i).group, (*i).entry, ((HotKey*)((*i).widget))->getShortCutString());
				break;
			case CONFIG_LINEEDIT:
				(*i).config->writeEntry((*i).group, (*i).entry, ((QLineEdit*)((*i).widget))->text());
				break;
			case CONFIG_TEXTEDIT:
				(*i).config->writeEntry((*i).group, (*i).entry, ((QTextEdit*)((*i).widget))->text());
				break;
			case CONFIG_SLIDER:
				(*i).config->writeEntry((*i).group, (*i).entry, ((QSlider*)((*i).widget))->value());
				break;
			case CONFIG_SPINBOX:
				(*i).config->writeEntry((*i).group, (*i).entry, ((QSpinBox*)((*i).widget))->value());
				break;
			case CONFIG_COLORBUTTON:
				(*i).config->writeEntry((*i).group, (*i).entry, ((ColorButton*)((*i).widget))->color());
				break;
			case CONFIG_SELECTFONT:
				(*i).config->writeEntry((*i).group, (*i).entry, ((SelectFont*)((*i).widget))->font());
				break;
			case CONFIG_RADIOGROUP:
			case CONFIG_VRADIOGROUP:
			case CONFIG_HRADIOGROUP:
			{
				QStringList values=(*i).additionalParams[1].toStringList();
				QButtonGroup *group=(QButtonGroup*)(*i).widget;
				int selectedId=group->id(group->selected());
				if (selectedId==-1)
					kdebugm(KDEBUG_ERROR, "type:%d group:%s entry:%s selected:%p selected_id==-1\n", (*i).type, (*i).group.local8Bit().data(), (*i).entry.local8Bit().data(), group->selected());
				else
					(*i).config->writeEntry((*i).group, (*i).entry, values[selectedId]);
				break;
			}
			default:
				break;
		}
	}

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
	if (existControl(groupname, caption, name) == -1)
	{
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
	if (existControl(groupname, caption, name) == -1)
	{
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
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_COMBOBOX, groupname, parent, caption, name);
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addComboBox(const QString& groupname, 
				const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS, const QString& tip, const QString& name)
{
	addComboBox(&config_file, groupname, parent, caption, entry, options, values, defaultS, tip, name);
}

void ConfigDialog::addComboBox(ConfigFile* config, const QString& groupname, 
				const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_COMBOBOX, groupname, parent, caption, name);
		c.tip=tip;
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.additionalParams.append(QVariant(options));
		c.additionalParams.append(QVariant(values));
		addControl(groupname,c);
	}
}


void ConfigDialog::addGrid(const QString& groupname,
			const QString& parent, const QString& caption, const int nrColumns, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_GRID, groupname, parent, caption, name);
		c.defaultS=QString::number(nrColumns);
		addControl(groupname,c);
	}
}

void ConfigDialog::addHBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_HBOX, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}


void ConfigDialog::addHGroupBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_HGROUPBOX, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}

void ConfigDialog::addHRadioGroup(
	const QString& groupname, const QString& parent, const QString& caption,
	const QString &entry, const QStringList &options, const QStringList &values,
	const QString &defaultS, const QString& tip, const QString& name)
{
	addHRadioGroup(&config_file, groupname, parent, caption, entry, options, values, defaultS, tip, name);
}

void ConfigDialog::addHRadioGroup(ConfigFile* config,
	const QString& groupname, const QString& parent, const QString& caption,
	const QString &entry, const QStringList &options, const QStringList &values,
	const QString &defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_HRADIOGROUP, groupname, parent, caption, name);
		c.tip=tip;
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.additionalParams.append(QVariant(options));
		c.additionalParams.append(QVariant(values));
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
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_HOTKEYEDIT, groupname, parent, caption, name);
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;
		
		if (addControl(groupname,c) == 0)
		// zapisujemy warto¶æ domy¶ln±, aby ju¿ wiêcej nie musieæ
		// jej podawaæ przy czytaniu z pliku conf		
			config->addVariable(groupname, entry, defaultS);	
	}
}

void ConfigDialog::addLineEdit2(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
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
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_LINEEDIT, groupname, parent, caption, name);
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;

		if (addControl(groupname,c) == 0)
			c.config->addVariable(groupname, entry, defaultS);	
	}
}

void ConfigDialog::addRadioGroup(
			const QString& groupname, const QString& parent, const QString& caption,
			const QString &entry, const QStringList &options, const QStringList &values,
			int strips, Orientation orientation,
			const QString &defaultS, const QString& tip, const QString& name)
{
	addRadioGroup(&config_file, groupname, parent, caption, entry, options, values,
			strips, orientation, defaultS, tip, name);
}

void ConfigDialog::addRadioGroup(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				int strips, Orientation orientation,
				const QString &defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_RADIOGROUP, groupname, parent, caption, name);
		c.tip=tip;
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.additionalParams.append(QVariant(options));
		c.additionalParams.append(QVariant(values));
		c.additionalParams.append(QVariant(strips));
		c.additionalParams.append(QVariant((int)orientation));
		addControl(groupname,c);
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
	if (existControl(groupname, caption, name) == -1)
	{
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
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_LABEL, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}


void ConfigDialog::addListBox(const QString& groupname,
			const QString& parent, const QString& caption, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_LISTBOX, groupname, parent, caption, name);
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addListView(const QString& groupname,
			const QString& parent, const QString& caption, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_LISTVIEW, groupname, parent, caption, name);
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addPushButton(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString &iconFileName, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_PUSHBUTTON, groupname, parent, caption, name);
		c.defaultS=iconFileName;
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addSelectPaths(const QString& groupname,
			const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
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
	if (existControl(groupname, caption, name) == -1)
	{
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

	if (existControl(groupname, caption, name) == -1)
	{
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
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_VBOX, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}

void ConfigDialog::addVGroupBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_VGROUPBOX, groupname, parent, caption, name);
		addControl(groupname,c);
	}
}

void ConfigDialog::addVRadioGroup(
	const QString& groupname, const QString& parent, const QString& caption,
	const QString &entry, const QStringList &options, const QStringList &values,
	const QString &defaultS, const QString& tip, const QString& name)
{
	addVRadioGroup(&config_file, groupname, parent, caption, entry, options, values, defaultS, tip, name);
}

void ConfigDialog::addVRadioGroup(ConfigFile* config,
	const QString& groupname, const QString& parent, const QString& caption,
	const QString &entry, const QStringList &options, const QStringList &values,
	const QString &defaultS, const QString& tip, const QString& name)
{
	if (existControl(groupname, caption, name) == -1)
	{
		RegisteredControl c(CONFIG_VRADIOGROUP, groupname, parent, caption, name);
		c.tip=tip;
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.additionalParams.append(QVariant(options));
		c.additionalParams.append(QVariant(values));
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
	if (existControl(groupname, caption, name) == -1)
	{
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
	kdebugf();
	FOREACH(j, RegisteredControls)
		if(((*j).group == groupname) && ((*j).caption == caption) && ((*j).name == name) && (*j).type!=CONFIG_DELETED)
		{
			ElementConnections c(signal, receiver, slot);
			(*j).ConnectedSlots.append(c);
			kdebugm(KDEBUG_INFO, "Slot connected:: %s\n",slot);
			break;
		}
	kdebugf2();
}

void ConfigDialog::disconnectSlot(const QString& groupname, const QString& caption, const char* signal, const QObject* receiver, const char* slot,const QString& name)
{
	kdebugf();
	FOREACH(j, RegisteredControls)
		if(((*j).group == groupname) && ((*j).caption == caption) && ((*j).name == name) && (*j).type!=CONFIG_DELETED)
		{
			ElementConnections c(signal, receiver, slot);
			(*j).ConnectedSlots.remove((*j).ConnectedSlots.find(c));
			kdebugm(KDEBUG_INFO, "Slot disconnected:: %s\n",slot);
			break;
		}
	kdebugf2();
}


void ConfigDialog::registerSlotOnCreate(const QObject* receiver, const char* name)
{
	ElementConnections c(QString::null, receiver, name);
	SlotsOnCreate.append(c);
}

void ConfigDialog::unregisterSlotOnCreate(const QObject* receiver, const char* name)
{
	ElementConnections c(QString::null, receiver, name);
	SlotsOnCreate.remove(SlotsOnCreate.find(c));
}

void ConfigDialog::registerSlotOnClose(const QObject* receiver, const char* name)
{
	ElementConnections c(QString::null, receiver, name);
	SlotsOnClose.append(c);
}

void ConfigDialog::unregisterSlotOnClose(const QObject* receiver, const char* name)
{
	ElementConnections c(QString::null, receiver, name);
	SlotsOnClose.remove(SlotsOnClose.find(c));
}

void ConfigDialog::registerSlotOnApply(const QObject* receiver, const char* name)
{
	ElementConnections c(QString::null, receiver, name);
	SlotsOnApply.append(c);
}

void ConfigDialog::unregisterSlotOnApply(const QObject* receiver, const char* name)
{
	ElementConnections c(QString::null, receiver, name);
	SlotsOnApply.remove(SlotsOnApply.find(c));
}

int ConfigDialog::findPreviousTab(int pos)
{
//	kdebugf();
	if (RegisteredControls.isEmpty())
		return -1;
	if (pos<0)
		pos=0;
	if ((uint)(pos+1)>RegisteredControls.count())
		pos=RegisteredControls.count()-1;
	for(; pos>=0; --pos)
		if (RegisteredControls[pos].type == CONFIG_TAB)
			return pos;
	return -1;
// -1 oznacza ze nie ma Tab'a
}

int ConfigDialog::findNextTab(int pos)
{
//	kdebugf();
	if (RegisteredControls.isEmpty())
		return -1;
	if (pos<0)
		pos=0;

	int count=RegisteredControls.count();

	if (pos>=count)
		pos=count;

	for(; pos<count; ++pos)
		if (RegisteredControls[pos].type == CONFIG_TAB)
			return pos;
	return -1;
//	zwraca miejsce znalezienia TAB'a
//	jesli nie znajdzie to zwraca -1	
}

int ConfigDialog::findTab(const QString& groupname, int pos)
{
//	kdebugf();
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
	for(; pos<count; ++pos)
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
//	kdebugf();
	int pos=findTab(caption);
	
	if(pos<0)
	{
		kdebugm(KDEBUG_WARNING, "Tab %s not found\n", caption.ascii());
		return;
	}

	kdebugmf(KDEBUG_INFO, "nrOfControls=%i\n", RegisteredControls[pos].nrOfControls);
	
	if(RegisteredControls[pos].nrOfControls==0)
		RegisteredControls[pos].type=CONFIG_DELETED;
	else
		kdebugm(KDEBUG_INFO, "can't remove tab!\n");
//	kdebugf2();
}

void ConfigDialog::decreaseNrOfControls(int control)
{
//	kdebugf();
	int j;
	QString localparent=RegisteredControls[control].parent;
	for (j=control; j>=0; --j)
		if(RegisteredControls[j].type!=CONFIG_DELETED)
			if (RegisteredControls[j].caption==localparent)
			{
				--RegisteredControls[j].nrOfControls;
				localparent= RegisteredControls[j].parent;
				if (RegisteredControls[j].type == CONFIG_TAB)
					break;
			}
//	kdebugf2();
}

void ConfigDialog::increaseNrOfControls(const int startpos, const int endpos, const QString& parent)
{
//	kdebugf();
	int j;
	QString localparent=parent;
	for (j=endpos; j>=startpos; --j)
		if(RegisteredControls[j].type!=CONFIG_DELETED)
			if (RegisteredControls[j].caption==localparent)
			{
				++RegisteredControls[j].nrOfControls;
				localparent= RegisteredControls[j].parent;
				if (RegisteredControls[j].type == CONFIG_TAB)
					break;
			}
//	kdebugf2();
}

int ConfigDialog::addControl(const QString& groupname, const ConfigDialog::RegisteredControl& control)
{
//	kdebugf();
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
	for (j=position; j<=nexttab; ++j)
		if(RegisteredControls[j].type!=CONFIG_DELETED)
			if (RegisteredControls[j].caption == control.parent)
			{
				int nrOfControls=RegisteredControls[j].nrOfControls;
				increaseNrOfControls(position,j,control.parent);
				if (RegisteredControls[j].caption == RegisteredControls.last().caption)
					RegisteredControls.append(control);
				else
					RegisteredControls.insert(RegisteredControls.at(j+nrOfControls+1),control);
//				kdebugf2();
				return 0;
			}	

//	kdebugf2();
	return -2;

// je¶li warto¶æ zwrócona jest -1 to nie znaleziono takiego TAB'a
// je¶li warto¶æ zwrócona jest -2 to nie znaleziono takiego parent'a
// je¶li warto¶æ zwrócona jest 0 to wszystko jest w porz±dku
}

QWidget* ConfigDialog::getWidget(const QString& groupname, const QString& caption, const QString& name)
{
	if (configdialog==NULL)
	{
		kdebugm(KDEBUG_PANIC, "ConfigDialog is closed! Can't get widget! (%s,%s,%s)\n",
			groupname.local8Bit().data(), caption.local8Bit().data(), name.local8Bit().data());
		return NULL;
	}
	int nr=existControl(groupname,caption,name);
	if (nr!=-1)
		return (RegisteredControls[nr].widget);
	kdebugm(KDEBUG_PANIC, "Warning: there is no \\" +groupname+ "\\"+ caption+ "\\"+ name+ "\\ control\n");
	return NULL;
}

bool ConfigDialog::dialogOpened()
{
	return (configdialog!=NULL);
}

QButtonGroup* ConfigDialog::getButtonGroup(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QButtonGroup*>(getWidget(groupname,caption,name));
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

QHButtonGroup* ConfigDialog::getHButtonGroup(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QHButtonGroup*>(getWidget(groupname,caption,name));
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

QVButtonGroup* ConfigDialog::getVButtonGroup(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QVButtonGroup*>(getWidget(groupname,caption,name));
}

int ConfigDialog::existControl(const QString& groupname, const QString& caption, const QString& name)
{
	int position= findTab(groupname);
	if (position == -1)
		return -1;

	int j;
	int count;
	for(j=position, count=0; count <= RegisteredControls[position].nrOfControls; ++j)
		if(RegisteredControls[j].type!=CONFIG_DELETED)
		{
			++count;
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
	return config_file.readEntry(groupname, name)==keyEventToString(e);
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

void HotKey::keyReleaseEvent(QKeyEvent *)
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
	kdebugf();
	new QLabel(qApp->translate("@default", text), this);
	(new QWidget(this))->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	fontEdit=new QLineEdit(this);
	fontEdit->setReadOnly(true);
	fontEdit->setFixedWidth(int(fontEdit->fontMetrics().width("Bitstream Vera Sans Mono 15")*1.5));
	QPushButton *button=new QPushButton(tr("Select"), this, "selectfont_selectbutton");
	setFont(val);
	connect(button, SIGNAL(clicked()), this, SLOT(onClick()));
	QToolTip::add(button, tip);
	kdebugf2();
}

void SelectFont::setFont(const QFont &font)
{
	currentFont=font;
	fontEdit->setText(narg(QString("%1 %2"), currentFont.family(), QString::number(currentFont.pointSize())));
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

SelectPaths::SelectPaths(QWidget *parent, const char* name) : QHBox(parent, name)
{
	kdebugf();
	setWFlags(Qt::WDestructiveClose|Qt::WShowModal);
	setCaption(tr("Select paths"));

	// create main QLabel widgets (icon and app info)
	QVBox *left=new QVBox(this, "left");
	left->setMargin(10);
	left->setSpacing(10);
	
	QLabel *l_icon = new QLabel(left,"icon");
	QWidget *blank=new QWidget(left,"blank");
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));
	
	QVBox *center=new QVBox(this,"center");
	center->setMargin(10);
	center->setSpacing(10);
	
	QLabel *l_info = new QLabel(center,"info");
	l_icon->setPixmap(icons_manager.loadIcon("SelectPathWindowIcon"));
	l_info->setText(tr("This dialog box allows you to choose directories in which kadu will look for icons or sounds."));
	l_info->setAlignment(Qt::WordBreak);
	// end create main QLabel widgets (icon and app info)
	
	//our QVGroupBox
	QVGroupBox *vgb_pathlist = new QVGroupBox(center,"pathlist");
	vgb_pathlist->setTitle(tr("Paths"));
	QVGroupBox *vgb_pathtoadd = new QVGroupBox(center,"newpath");
	vgb_pathtoadd->setTitle(tr("Path to add"));
	center->setStretchFactor(vgb_pathlist, 1);
	//end our QGroupBox
	
	// create needed fields
	QHBox *hb_pathlist = new QHBox(vgb_pathlist,"box");
	hb_pathlist->setSpacing(5);
	pathListBox = new QListBox(hb_pathlist,"pathBox");
	hb_pathlist->setStretchFactor(pathListBox, 1);
	QVBox *vb_managebuttons = new QVBox(hb_pathlist,"buttons");
	vb_managebuttons->setSpacing(5);
	pb_add = new QPushButton(icons_manager.loadIcon("AddSelectPathDialogButton"), tr("Add"), vb_managebuttons, "addButton");
	pb_change = new QPushButton(icons_manager.loadIcon("ChangeSelectPathDialogButton"), tr("Change"), vb_managebuttons, "changeButton");
	pb_remove = new QPushButton(icons_manager.loadIcon("RemoveSelectPathDialogButton"), tr("Remove"), vb_managebuttons, "removeButton");
	QWidget *w_managebuttons = new QWidget(vb_managebuttons,"blank");
	vb_managebuttons->setStretchFactor(w_managebuttons, 1);
	
	QHBox *hb_selectpath = new QHBox(vgb_pathtoadd,"box");
	hb_selectpath ->setSpacing(5);
	pathEdit = new QLineEdit(hb_selectpath,"newPathLineEdit");
	pb_choose = new QPushButton(icons_manager.loadIcon("ChooseSelectPathDialogButton"), tr("Choose"), hb_selectpath, "chooseButton");
	hb_selectpath->setStretchFactor(pathEdit, 1);
	// end create needed fields

	// buttons
	QHBox *bottom = new QHBox(center, "bottom");
	QWidget *blank2 = new QWidget(bottom,"blank");
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	pb_ok = new QPushButton(icons_manager.loadIcon("OkWindowButton"), tr("OK"), bottom, "okButton");
	pb_cancel = new QPushButton(icons_manager.loadIcon("CancelWindowButton"), tr("&Cancel"), bottom, "cancelButton");
	// end buttons

	connect(pb_ok, SIGNAL(clicked()), this, SLOT(okButton()));
	connect(pb_cancel, SIGNAL(clicked()), this, SLOT(cancelButton()));
	connect(pb_choose, SIGNAL(clicked()), this, SLOT(choosePath()));
	connect(pb_change, SIGNAL(clicked()), this, SLOT(replacePath()));
	connect(pb_add, SIGNAL(clicked()), this, SLOT(addPath()));
	connect(pb_remove, SIGNAL(clicked()), this, SLOT(deletePath()));

	loadGeometry(this, "General", "SelectPathDialogGeometry", 0, 0, 330, 330);
	kdebugf2();
}

QStringList SelectPaths::getPathList()
{
	kdebugf();
	return releaseList;
}


void SelectPaths::setPathList(const QStringList& list)
{
	kdebugf();
	pathListBox->clear();
	pathListBox->insertStringList(list);
	pathListBox->setSelected(0, true);
	releaseList=list;
	kdebugf2();
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
	pathEdit->setText(NULL);
	kdebugf2();
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
	kdebugf2();
}

void SelectPaths::deletePath()
{
	kdebugf();
	if (pathListBox->isSelected(pathListBox->currentItem()))
	{
		pathListBox->removeItem(pathListBox->currentItem());
		pathListBox->setSelected(pathListBox->currentItem(),true);
	}
	kdebugf2();
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
	kdebugf2();
}

void SelectPaths::okButton()
{
	kdebugf();
	releaseList.clear();
	for (unsigned int i=0; i<pathListBox->count(); ++i)
		releaseList.append(pathListBox->text(i));

	pathEdit->setText("");
	hide();
	emit changed(releaseList);
	kdebugf2();
}

void SelectPaths::cancelButton()
{
	kdebugf();
	pathListBox->clear();
	pathListBox->insertStringList(releaseList);
	pathEdit->setText("");
	hide();
	kdebugf2();
}

void SelectPaths::closeEvent(QCloseEvent *e)
{
	e->ignore();
	saveGeometry(this, "General", "SelectPathDialogGeometry");
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
