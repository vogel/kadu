/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qaccel.h>
#include <qapplication.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qcombobox.h>
#include <qfontdialog.h>
#include <qgrid.h>
#include <qhgroupbox.h>
#include <qhbuttongroup.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qradiobutton.h>
#include <qscrollview.h>
#include <qsignal.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qtooltip.h>
#include <qvariant.h>
#include <qvbuttongroup.h>
#include <qvgroupbox.h>

#include "config_dialog.h"
#include "debug.h"
#include "icons_manager.h"
#include "misc.h"

QString ConfigDialog::currentTab;
ConfigDialog *ConfigDialog::configdialog = NULL;
QApplication *ConfigDialog::appHandle = NULL;

QMap<QString, QValueList<ConfigDialog::ElementConnections> > ConfigDialog::SlotsOnCreateTab;
QMap<QString, QValueList<ConfigDialog::ElementConnections> > ConfigDialog::SlotsOnCloseTab;
QMap<QString, QValueList<ConfigDialog::ElementConnections> > ConfigDialog::SlotsOnApplyTab;

QMap<QString, QValueList <ConfigDialog::RegisteredControl> > ConfigDialog::Tabs;
QStringList ConfigDialog::TabNames;
QMap<QString, int> ConfigDialog::TabSizes;
QDict<QSignal> ConfigDialog::tabChangesIn;
QDict<QSignal> ConfigDialog::tabChangesOut;

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

void ConfigDialog::createWidget(QValueListIterator <RegisteredControl> i)
{
//	kdebugf();
	if ((*i).widget)
		return;

	{
// wyswietla cala liste
//		kdebugm(KDEBUG_DUMP, "%d: (%d) "+(*i).group+"->"+(*i).parent+"->"+(*i).caption+"->"+(*i).name+"\n", num, (*i).nrOfControls);

		QWidget* parent = NULL;
		if ((*i).type != CONFIG_TAB)
		    parent = (*(*i).parentControl).widget;
//		kdebugm(KDEBUG_DUMP, "creating widget\n");

		switch((*i).type)
		{
			case CONFIG_CHECKBOX:
			{
				QCheckBox* check=new QCheckBox(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				check->setChecked((*i).config->readBoolEntry((*i).group, (*i).entry, (*i).defaultS));
				(*i).widget=check;
				(*i).entireWidget = (*i).widget;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
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
				(*i).entireWidget = hbox;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_COMBOBOX:
			{
				QHBox* hbox=new QHBox(parent);
				new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QComboBox* combo=new QComboBox(hbox, (*i).name);
				(*i).widget=combo;
				(*i).entireWidget = hbox;
				if (!(*i).tip.isEmpty())
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
				(*i).entireWidget = (*i).widget;
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
				(*i).entireWidget = (*i).widget;
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
				(*i).entireWidget = (*i).widget;
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
				(*i).entireWidget = (*i).widget;
				group->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum));
//				group->setExclusive(true);
				group->setRadioButtonExclusive(true);

				QStringList options=(*i).additionalParams[0].toStringList();
				QStringList values=(*i).additionalParams[1].toStringList();
				CONST_FOREACH(option, options)
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
				(*i).entireWidget = hbox;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_LABEL:
			{
				QLabel* label=new QLabel(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				(*i).widget=label;
				(*i).entireWidget = (*i).widget;
				break;
			}
			case CONFIG_LINEEDIT:
			{
				QHBox* hbox=new QHBox(parent);
				new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QLineEdit* line=new QLineEdit(hbox, (*i).name);
				line->setText((*i).config->readEntry((*i).group, (*i).entry, (*i).defaultS));
				(*i).widget=line;
				(*i).entireWidget = hbox;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_LINEEDIT2:
			{
				QHBox* hbox=new QHBox(parent);
				new QLabel(appHandle->translate("@default",(*i).caption), hbox);
				QLineEdit* line=new QLineEdit(hbox, (*i).name);
				line->setText((*i).defaultS);
				(*i).widget=line;
				(*i).entireWidget = hbox;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
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
				(*i).entireWidget = hbox;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_LISTBOX:
			{
				QListBox* listbox= new QListBox(parent, (*i).caption);
				(*i).widget=listbox;
				(*i).entireWidget = (*i).widget;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_LISTVIEW:
			{
				QListView* listview= new QListView(parent, (*i).caption);
				(*i).widget=listview;
				(*i).entireWidget = (*i).widget;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_PUSHBUTTON:
			{
				QPushButton *button =new QPushButton(appHandle->translate("@default",(*i).caption), parent, (*i).name);
				if (!(*i).defaultS.isEmpty())
					button->setIconSet(icons_manager->loadIcon((*i).defaultS));
				(*i).widget=button;
				(*i).entireWidget = (*i).widget;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_SELECTFONT:
			{
				QFont def_font((*i).defaultS);
				(*i).widget=new SelectFont((*i).caption, (*i).config->readFontEntry((*i).group, (*i).entry, &def_font), parent, (*i).name, (*i).tip);
				(*i).entireWidget = (*i).widget;
				break;
			}
			case CONFIG_SELECTPATHS:
			{
				QPushButton *button =new QPushButton(appHandle->translate("@default",(*i).caption), parent);
				SelectPaths* paths=new SelectPaths(NULL, (*i).name);
				insertChild(paths);//musimy go wstawiæ na listê dzieci, bo inaczej wycieknie nam...
				button->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
				(*i).widget=paths;
				(*i).entireWidget = button;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				connect(button, SIGNAL(clicked()), paths, SLOT(show()));
				break;
			}
			case CONFIG_SLIDER:
			{
				QStringList values= QStringList::split(",", (*i).defaultS);

				int minVal = values[0].toInt();
				int maxVal = values[1].toInt();
				int pageStep = values[2].toInt();
				int value = values[3].toInt();
				QSlider *slider=new QSlider(minVal, maxVal, pageStep, value, Qt::Horizontal, parent, (*i).caption);
				slider->setValue((*i).config->readNumEntry((*i).group, (*i).entry,value));
				slider->setTickmarks(QSlider::Below);
				(*i).widget=slider;
				(*i).entireWidget = (*i).widget;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
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
				(*i).entireWidget = hbox;
				if (!(*i).tip.isEmpty())
					QToolTip::add((*i).widget, appHandle->translate("@default",(*i).tip));
				break;
			}
			case CONFIG_TAB:
			{
				if (!(*i).defaultS.isEmpty())
					listBox->insertItem(icons_manager->loadIcon((*i).defaultS), appHandle->translate("@default",(*i).caption));
				else
					listBox->insertItem(appHandle->translate("@default",(*i).caption));
				QVBox *subbox= new QVBox(box);
				(*i).widget=subbox;
				(*i).entireWidget = (*i).widget;
				(*i).widget->hide();
				break;
			}
			case CONFIG_NULL:
				break;

		}

		CONST_FOREACH(connection, (*i).ConnectedSlots)
		{
//			kdebugm(KDEBUG_DUMP, "connecting %p %s to %p %s\n", (*i).widget, (*connection).signal.local8Bit().data(),
//								(*connection).receiver, (*connection).slot.local8Bit().data());
			if (!connect((*i).widget, (*connection).signal, (*connection).receiver, (*connection).slot))
				kdebugm(KDEBUG_ERROR, "unable to connect signal: %s to slot: %s\n",
					(*connection).signal.local8Bit().data(), (*connection).slot.local8Bit().data());
		}
//		++num;
	}

//	kdebugf2();

}


void ConfigDialog::createTabAndWidgets(const QString& tab)
{
	kdebugf();

	if (Tabs[tab].back().widget)
		return;

	FOREACH(i, Tabs[tab])
		createWidget(i);
		
	FOREACH(j, SlotsOnCreateTab[tab])
		connect(this, SIGNAL(createTab()), (*j).receiver, (*j).slot);
	
	emit createTab();

	FOREACH(j, SlotsOnCreateTab[tab])
		disconnect(this, SIGNAL(createTab()), (*j).receiver, (*j).slot);


	kdebugm(KDEBUG_INFO, "connecting SlotsOnApplyTab\n");
	CONST_FOREACH(conn, SlotsOnApplyTab[tab])
		connect(this, SIGNAL(apply()), (*conn).receiver, (*conn).slot);

	kdebugm(KDEBUG_INFO, "connecting SlotsOnCloseTab\n");
	CONST_FOREACH(conn, SlotsOnCloseTab[tab])
		connect(this, SIGNAL(destroy()), (*conn).receiver, (*conn).slot);

	kdebugf2();
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

	box= new QVGroupBox(view, "groupBox");
	box->setFrameStyle(QFrame::NoFrame);
	view->addChild(box);

//	int num = 0;

	CONST_FOREACH(tabName, TabNames)
	{
		FOREACH(i, Tabs[*tabName])
		{
			(*i).widget= NULL;
			(*i).entireWidget= NULL;
		}
		createWidget(Tabs[*tabName].begin());
	}


	// buttons
	QHBox *bottom = new QHBox(this, "buttons");
	bottom->setMargin(10);
	bottom->setSpacing(5);

	userLevel = static_cast<UserLevel>(config_file.readNumEntry("General", "UserLevel"));

	new QLabel(tr("User experience level:"), bottom);
	QComboBox *userLevelComboBox = new QComboBox(bottom);
	userLevelComboBox->insertItem(tr("Beginner"));
	userLevelComboBox->insertItem(tr("Advanced"));
	userLevelComboBox->insertItem(tr("Expert"));
	userLevelComboBox->setCurrentItem(userLevel);

	connect(userLevelComboBox, SIGNAL(activated(int)), this, SLOT(changeUserLevel(int)));

	(new QWidget(bottom, "blank"))->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

	okButton = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Ok"), bottom, "okButton");
	applyButton = new QPushButton(icons_manager->loadIcon("ApplyWindowButton"), tr("Apply"), bottom, "applyButton");
	cancelButton = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), bottom, "cancelButton");

	connect(okButton, SIGNAL(clicked()), this, SLOT(updateAndCloseConfig()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(updateConfig()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
	// end buttons

	configdialog = this;


	if (currentTab.isEmpty())
		currentTab = config_file.readEntry("General", "ConfigDialogLastTab", QT_TRANSLATE_NOOP("@default", "General"));
	if (TabNames.contains(currentTab))
		changeTab(appHandle->translate("@default", currentTab));
	else
		changeTab(appHandle->translate("@default", "General"));
	listBox->setCurrentItem(listBox->findItem(appHandle->translate("@default", currentTab)));
	connect(listBox, SIGNAL(highlighted(const QString&)), this, SLOT(changeTab(const QString&)));

	emit create();

	loadGeometry(this, "General", "ConfigGeometry", 0, 30, 790, 480);

	changeUserLevel(static_cast<int>(userLevel));

	kdebugf2();
}

ConfigDialog::~ConfigDialog()
{
	saveGeometry(this, "General", "ConfigGeometry");
	config_file.writeEntry("General", "ConfigDialogLastTab", currentTab);
	config_file.writeEntry("General", "UserLevel", userLevel);
	emit destroy();
	configdialog = NULL;
	config_file.sync();
}

void ConfigDialog::changeTab(const QString& name)
{
	kdebugmf(KDEBUG_FUNCTION_START, "current tab:%s changeTo:%s\n", currentTab.local8Bit().data(), name.local8Bit().data());

	int counter = 0;
	QSignal *sig;
	CONST_FOREACH(tab, Tabs)
	{
		const RegisteredControl &tabControl = (*tab).front();
		if (appHandle->translate("@default", tabControl.caption) == name)
		{
			if (!((*tab).back().widget))
				createTabAndWidgets(tabControl.caption);

				tabControl.widget->show();
			currentTab = tabControl.caption;
			if ((sig = tabChangesIn[tabControl.caption]))
				sig->activate();
			++counter;

			updateUserLevel(tab);
		}
		else
			if (tabControl.widget && tabControl.widget->isVisible())
			{
				tabControl.widget->hide();
				if ((sig = tabChangesOut[tabControl.caption]))
					sig->activate();
				++counter;
			}
		if (counter == 2)
			break;
	}

	kdebugmf(KDEBUG_FUNCTION_END, "current tab:%s\n", currentTab.local8Bit().data());
}

void ConfigDialog::updateUserLevel(QMapConstIterator<QString,
			QValueList<ConfigDialog::RegisteredControl> > tab)
{
	kdebugf();

	CONST_FOREACH(i, *tab)
	{
		if (!(*i).entireWidget)
			continue;
		if ((*i).userLevelRequired <= userLevel)
			(*i).entireWidget->show();
		else
			(*i).entireWidget->hide();
	}

	kdebugf2();
}

void ConfigDialog::changeUserLevel(int newUserLevel)
{
	kdebugf();

	userLevel = static_cast<UserLevel>(newUserLevel);

	CONST_FOREACH(tab, Tabs)
	{
		const RegisteredControl &tabControl = (*tab).front();
		if (tabControl.caption == currentTab)
			updateUserLevel(tab);
	}

	kdebugf2();
}


void ConfigDialog::updateConfig(void)
{
	kdebugf();
	CONST_FOREACH(tab, Tabs)
	CONST_FOREACH(i, *tab)
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
			case CONFIG_NULL:
			default:
				break;
		}
	}

	emit apply();

	kdebugf2();
}

void ConfigDialog::updateAndCloseConfig()
{
	updateConfig();

	close();
}

void ConfigDialog::addCheckBox(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const bool defaultS, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	addCheckBox(&config_file, groupname, parent, caption, entry, defaultS, tip, name, userLevelRequired);
}

void ConfigDialog::addCheckBox(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const bool defaultS, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_CHECKBOX, groupname, parent, caption, name, userLevelRequired);
		c.config=config;
		c.entry=entry;
		c.defaultS=QString::number(defaultS);
		c.tip=tip;
		if (addControl(groupname,c))
			c.config->addVariable(groupname, entry, defaultS);
	}
}


void ConfigDialog::addColorButton(ConfigFile *config, const QString& groupname,
				const QString& parent, const QString& caption, const QString &entry,
				const QColor& color, const QString& tip, const QString& name,
				UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_COLORBUTTON, groupname, parent, caption, name, userLevelRequired);
		c.config=config;
		c.entry=entry;
		c.defaultS=color.name();
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addColorButton(const QString& groupname,
				const QString& parent, const QString& caption, const QString &entry,
				const QColor& color, const QString& tip, const QString& name,
				UserLevel userLevelRequired)
{
	addColorButton(&config_file, groupname, parent, caption, entry, color, tip, name, userLevelRequired);
}


void ConfigDialog::addComboBox(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_COMBOBOX, groupname, parent, caption, name, userLevelRequired);
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addComboBox(const QString& groupname,
				const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS, const QString& tip, const QString& name,
				UserLevel userLevelRequired)
{
	addComboBox(&config_file, groupname, parent, caption, entry, options, values, defaultS, tip, name, userLevelRequired);
}

void ConfigDialog::addComboBox(ConfigFile* config, const QString& groupname,
				const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS, const QString& tip, const QString& name,
				UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_COMBOBOX, groupname, parent, caption, name, userLevelRequired);
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
			const QString& parent, const QString& caption, const int nrColumns, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_GRID, groupname, parent, caption, name, userLevelRequired);
		c.defaultS=QString::number(nrColumns);
		addControl(groupname,c);
	}
}

void ConfigDialog::addHBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name,
	UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_HBOX, groupname, parent, caption, name, userLevelRequired);
		addControl(groupname,c);
	}
}


void ConfigDialog::addHGroupBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name,
	UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_HGROUPBOX, groupname, parent, caption, name, userLevelRequired);
		addControl(groupname,c);
	}
}

void ConfigDialog::addHRadioGroup(
	const QString& groupname, const QString& parent, const QString& caption,
	const QString &entry, const QStringList &options, const QStringList &values,
	const QString &defaultS, const QString& tip, const QString& name,
	UserLevel userLevelRequired)
{
	addHRadioGroup(&config_file, groupname, parent, caption, entry, options, values, defaultS, tip, name,
		userLevelRequired);
}

void ConfigDialog::addHRadioGroup(ConfigFile* config,
	const QString& groupname, const QString& parent, const QString& caption,
	const QString &entry, const QStringList &options, const QStringList &values,
	const QString &defaultS, const QString& tip, const QString& name,
	UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_HRADIOGROUP, groupname, parent, caption, name, userLevelRequired);
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
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	addHotKeyEdit(&config_file, groupname, parent, caption, entry, defaultS, tip, name, userLevelRequired);
}

void ConfigDialog::addHotKeyEdit(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_HOTKEYEDIT, groupname, parent, caption, name, userLevelRequired);
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;

		if (addControl(groupname,c))
		// zapisujemy warto¶æ domy¶ln±, aby ju¿ wiêcej nie musieæ
		// jej podawaæ przy czytaniu z pliku conf
			config->addVariable(groupname, entry, defaultS);
	}
}

void ConfigDialog::addLineEdit2(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& defaultS, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_LINEEDIT2, groupname, parent, caption, name, userLevelRequired);
		c.defaultS=defaultS;
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addLineEdit(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	addLineEdit(&config_file, groupname, parent, caption, entry, defaultS, tip, name, userLevelRequired);
}


void ConfigDialog::addLineEdit(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_LINEEDIT, groupname, parent, caption, name, userLevelRequired);
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;

		if (addControl(groupname,c))
			c.config->addVariable(groupname, entry, defaultS);
	}
}

void ConfigDialog::addRadioGroup(
			const QString& groupname, const QString& parent, const QString& caption,
			const QString &entry, const QStringList &options, const QStringList &values,
			int strips, Orientation orientation,
			const QString &defaultS, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	addRadioGroup(&config_file, groupname, parent, caption, entry, options, values,
			strips, orientation, defaultS, tip, name, userLevelRequired);
}

void ConfigDialog::addRadioGroup(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				int strips, Orientation orientation,
				const QString &defaultS, const QString& tip, const QString& name,
				UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_RADIOGROUP, groupname, parent, caption, name, userLevelRequired);
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
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	addTextEdit(&config_file, groupname, parent, caption, entry, defaultS, tip, name, userLevelRequired);
}

void ConfigDialog::addTextEdit(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry, const QString& defaultS, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_TEXTEDIT, groupname, parent, caption, name, userLevelRequired);
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;

		if (addControl(groupname,c))
			c.config->addVariable(groupname, entry, defaultS);
	}
}

void ConfigDialog::addLabel(const QString& groupname,
			const QString& parent, const QString& caption, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_LABEL, groupname, parent, caption, name, userLevelRequired);
		addControl(groupname,c);
	}
}


void ConfigDialog::addListBox(const QString& groupname,
			const QString& parent, const QString& caption, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_LISTBOX, groupname, parent, caption, name, userLevelRequired);
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addListView(const QString& groupname,
			const QString& parent, const QString& caption, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_LISTVIEW, groupname, parent, caption, name, userLevelRequired);
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addPushButton(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString &iconFileName, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_PUSHBUTTON, groupname, parent, caption, name, userLevelRequired);
		c.defaultS=iconFileName;
		c.tip=tip;
		addControl(groupname,c);
	}
}

void ConfigDialog::addSelectPaths(const QString& groupname,
			const QString& parent, const QString& caption, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_SELECTPATHS, groupname, parent, caption, name, userLevelRequired);
		addControl(groupname,c);
	}
}

void ConfigDialog::addSlider(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry,
			const int minValue, const int maxValue,
			const int pageStep, const int value, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	addSlider(&config_file, groupname, parent, caption, entry, minValue, maxValue,
			pageStep, value, tip, name, userLevelRequired);
}

void ConfigDialog::addSlider(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry,
			const int minValue, const int maxValue,
			const int pageStep, const int value, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_SLIDER, groupname, parent, caption, name, userLevelRequired);
		c.config=config;
		c.entry=entry;
		c.tip=tip;
		c.defaultS=QString::number(minValue)+","+QString::number(maxValue)+","+QString::number(pageStep)+","+QString::number(value);

		if (addControl(groupname,c))
			c.config->addVariable(groupname, entry, value);
	}
}

void ConfigDialog::addSpinBox(const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry,
			const int minValue, const int maxValue, const int step, const int value, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{
	addSpinBox(&config_file, groupname, parent, caption, entry, minValue, maxValue,
			step, value, tip, name, userLevelRequired);

}

void ConfigDialog::addSpinBox(ConfigFile* config, const QString& groupname,
			const QString& parent, const QString& caption,
			const QString& entry,
			const int minValue, const int maxValue, const int step, const int value, const QString& tip, const QString& name,
			UserLevel userLevelRequired)
{

	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_SPINBOX, groupname, parent, caption, name, userLevelRequired);
		c.config=config;
		c.entry=entry;
		c.tip=tip;
		c.defaultS=QString::number(minValue)+","+QString::number(maxValue)+","+QString::number(step)+","+QString::number(value);

		if (addControl(groupname,c))
			c.config->addVariable(groupname, entry, value);
	}
}

void ConfigDialog::addVBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name,
	UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_VBOX, groupname, parent, caption, name, userLevelRequired);
		addControl(groupname,c);
	}
}

void ConfigDialog::addVGroupBox(const QString& groupname,
	const QString& parent, const QString& caption, const QString& name,
	UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_VGROUPBOX, groupname, parent, caption, name, userLevelRequired);
		addControl(groupname,c);
	}
}

void ConfigDialog::addVRadioGroup(
	const QString& groupname, const QString& parent, const QString& caption,
	const QString &entry, const QStringList &options, const QStringList &values,
	const QString &defaultS, const QString& tip, const QString& name,
	UserLevel userLevelRequired)
{
	addVRadioGroup(&config_file, groupname, parent, caption, entry, options, values, defaultS, tip, name,
		userLevelRequired);
}

void ConfigDialog::addVRadioGroup(ConfigFile* config,
	const QString& groupname, const QString& parent, const QString& caption,
	const QString &entry, const QStringList &options, const QStringList &values,
	const QString &defaultS, const QString& tip, const QString& name,
	UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_VRADIOGROUP, groupname, parent, caption, name, userLevelRequired);
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
				const QString &tip, const QString& name,
				UserLevel userLevelRequired)
{
	addSelectFont(&config_file, groupname, parent, caption, entry, defaultS, tip, name, userLevelRequired);
}

void ConfigDialog::addSelectFont(ConfigFile *config, const QString& groupname, const QString& parent,
				const QString& caption, const QString& entry, const QString& defaultS,
				const QString &tip, const QString& name,
				UserLevel userLevelRequired)
{
	if (!controlExists(groupname, caption, name))
	{
		RegisteredControl c(CONFIG_SELECTFONT, groupname, parent, caption, name, userLevelRequired);
		c.config=config;
		c.entry=entry;
		c.defaultS=defaultS;
		c.tip=tip;

		if (addControl(groupname,c))
			c.config->addVariable(groupname, entry, defaultS);
	}
}

ConfigDialog::RegisteredControl::RegisteredControl(RegisteredControlType t,
	const QString &groupname,
	const QString &parent,
	const QString &caption,
	const QString &name,
	const UserLevel userLevelRequired)
{
	this->type = t;
	this->group = groupname;
	this->parent = parent;
	this->caption = caption;
	this->name = name;
	this->widget = 0;
	this->nrOfControls = 0;
	this->config = 0;
	this->userLevelRequired = userLevelRequired;
}

void ConfigDialog::connectSlot(const QString& groupname, const QString& caption,
	const char* signal, const QObject* receiver, const char* slot, const QString& name)
{
	kdebugf();
	if (!Tabs.contains(groupname))
	{
		kdebugmf(KDEBUG_ERROR, "no such tab! (%s)\n", groupname.local8Bit().data());
		return;
	}
	FOREACH(j, Tabs[groupname])
	{
//		kdebugm(KDEBUG_INFO, ">>> '%s' '%s' '%s'\n", groupname.local8Bit().data(),
//					(*j).caption.local8Bit().data(), (*j).name.local8Bit().data());
		if ((*j).caption == caption && (*j).name == name)
		{
			ElementConnections c(signal, receiver, slot);
			(*j).ConnectedSlots.append(c);
			kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_INFO, "Slot connected:: %s\n",slot);
			return;
		}
	}
	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_ERROR,
			"\nno such control ('%s' '%s' '%s')!\nsignal:%s receiver:%p slot:%s\n",
			groupname.local8Bit().data(), caption.local8Bit().data(),
			name.local8Bit().data(), signal, receiver, slot);
}

void ConfigDialog::disconnectSlot(const QString& groupname, const QString& caption,
	const char* signal, const QObject* receiver, const char* slot, const QString& name)
{
	kdebugf();
	if (!Tabs.contains(groupname))
	{
		kdebugmf(KDEBUG_ERROR, "no such tab! (%s)\n", groupname.local8Bit().data());
		return;
	}
	FOREACH(j, Tabs[groupname])
	{
//		kdebugm(KDEBUG_INFO, ">>> '%s' '%s' '%s'\n", groupname.local8Bit().data(),
//				(*j).caption.local8Bit().data(), (*j).name.local8Bit().data());
		if(((*j).group == groupname) && ((*j).caption == caption) && ((*j).name == name))
		{
			ElementConnections c(signal, receiver, slot);
			(*j).ConnectedSlots.remove((*j).ConnectedSlots.find(c));
			kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_INFO, "Slot disconnected:: %s\n",slot);
			return;
		}
	}
	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_ERROR,
			"\nno such control ('%s' '%s' '%s')!\nsignal:%s receiver:%p slot:%s\n",
			groupname.local8Bit().data(), caption.local8Bit().data(),
			name.local8Bit().data(), signal, receiver, slot);
}


void ConfigDialog::registerSlotOnCreateTab(const QString& tab, const QObject* receiver, const char* name)
{
	SlotsOnCreateTab[tab].append(ElementConnections(QString::null, receiver, name));
}

void ConfigDialog::unregisterSlotOnCreateTab(const QString& tab, const QObject* receiver, const char* name)
{
	SlotsOnCreateTab[tab].remove(SlotsOnCreateTab[tab].find(ElementConnections(QString::null, receiver, name)));
}

void ConfigDialog::registerSlotOnCloseTab(const QString& tab, const QObject* receiver, const char* name)
{
	SlotsOnCloseTab[tab].append(ElementConnections(QString::null, receiver, name));
}

void ConfigDialog::unregisterSlotOnCloseTab(const QString& tab, const QObject* receiver, const char* name)
{
	SlotsOnCloseTab[tab].remove(SlotsOnCloseTab[tab].find(ElementConnections(QString::null, receiver, name)));
}

void ConfigDialog::registerSlotOnApplyTab(const QString& tab, const QObject* receiver, const char* name)
{
	SlotsOnApplyTab[tab].append(ElementConnections(QString::null, receiver, name));
}

void ConfigDialog::unregisterSlotOnApplyTab(const QString& tab, const QObject* receiver, const char* name)
{
	SlotsOnApplyTab[tab].remove(SlotsOnApplyTab[tab].find(ElementConnections(QString::null, receiver, name)));
}

void ConfigDialog::registerSlotsOnTabChange(const QString &name, const QObject *receiver, const char *slotIn, const char *slotOut)
{
	kdebugf();
	QSignal *sig;
	if (receiver)
	{
		if (slotIn)
		{
			sig = tabChangesIn[name];
			if (sig == 0)
			{
				sig = new QSignal();
				tabChangesIn.insert(name, sig);
			}
			if (!sig->connect(receiver, slotIn))
				kdebugm(KDEBUG_WARNING, "can't connect slot! (in: %s, %s)\n", name.local8Bit().data(), slotIn);
		}
		if (slotOut)
		{
			sig = tabChangesOut[name];
			if (sig == 0)
			{
				sig = new QSignal();
				tabChangesOut.insert(name, sig);
			}
			if (!sig->connect(receiver, slotOut))
				kdebugm(KDEBUG_WARNING, "can't connect slot! (in: %s, %s)\n", name.local8Bit().data(), slotOut);
		}
	}
	else
		kdebugm(KDEBUG_WARNING, "null object! (tab: %s)\n", name.local8Bit().data());

	kdebugf2();
}

void ConfigDialog::unregisterSlotsOnTabChange(const QString &name, const QObject *receiver, const char *slotIn, const char *slotOut)
{
	kdebugf();
	QSignal *sig;
	if (receiver)
	{
		if (slotIn)
		{
			sig = tabChangesIn[name];
			if (sig)
			{
				if (!sig->disconnect(receiver, slotIn))
					kdebugm(KDEBUG_WARNING, "unregistering not connected slot! (in: %s, %s)\n", name.local8Bit().data(), slotIn);
			}
			else
				kdebugm(KDEBUG_WARNING, "unregistering not connected slot! (in: %s, %s)\n", name.local8Bit().data(), slotIn);
		}
		if (slotOut)
		{
			sig = tabChangesOut[name];
			if (sig)
			{
				if (!sig->disconnect(receiver, slotOut))
					kdebugm(KDEBUG_WARNING, "unregistering not connected slot! (out: %s, %s)\n", name.local8Bit().data(), slotOut);
			}
			else
				kdebugm(KDEBUG_WARNING, "unregistering not connected slot! (out: %s, %s)\n", name.local8Bit().data(), slotOut);
		}
	}
	else
		kdebugm(KDEBUG_WARNING, "null object! (tab: %s)\n", name.local8Bit().data());
	kdebugf2();
}

void ConfigDialog::addTab(const QString& caption, const QString& iconFileName,
	UserLevel userLevelRequired)
{
	if (!Tabs.contains(caption))
	{
		TabNames.append(caption);

		QValueList<RegisteredControl> l;
		RegisteredControl c(CONFIG_TAB, QString::null, QString::null, caption, QString::null, userLevelRequired);
		c.defaultS = iconFileName;
		l.append(c);

		Tabs[caption] = l;
		TabSizes[caption] = 1;
	}
}

void ConfigDialog::tab(const QString &name, QValueListIterator<RegisteredControl> &ret, bool *found)
{
	bool f = Tabs.contains(name);
	if (found)
		*found = f;
	if (f)
		ret = Tabs[name].begin();
	else
	{
		kdebugmf(KDEBUG_ERROR, "tab %s not found!\n", name.local8Bit().data());
		ret = Tabs["null"].begin();
	}
}

void ConfigDialog::tab(const QString &name, QValueListConstIterator<RegisteredControl> &ret, bool *found)
{
	bool f = Tabs.contains(name);
	if (found)
		*found = f;
	if (f)
		ret = Tabs[name].begin();
	else
	{
		kdebugmf(KDEBUG_ERROR, "tab %s not found!\n", name.local8Bit().data());
		ret = Tabs["null"].begin();
	}
}

bool ConfigDialog::controlExists(const QString& groupname, const QString& caption, const QString& name, QValueListIterator<RegisteredControl> *control)
{
	bool ok;
	QValueListIterator<RegisteredControl> curControl;
	tab(groupname, curControl, &ok);

	if (!ok)
	{
		kdebugm(KDEBUG_ERROR,"Tab "+groupname+" not found\n");
		return false;
	}

	int lastItem = TabSizes[groupname];
	for (int j = 1; j <= lastItem; ++j, ++curControl)
		if ((*curControl).caption == caption && (*curControl).name == name)
		{
			if (control)
				*control = curControl;
			return true;
		}
	return false;
}

bool ConfigDialog::controlExists(const QString& groupname, const QString& caption, const QString& name, QValueListConstIterator<RegisteredControl> *control)
{
	bool ok;
	QValueListConstIterator<RegisteredControl> curControl;
	tab(groupname, curControl, &ok);

	if (!ok)
	{
		kdebugm(KDEBUG_ERROR,"Tab "+groupname+" not found\n");
		return false;
	}

	int lastItem = TabSizes[groupname];
	for (int j = 1; j <= lastItem; ++j, ++curControl)
		if ((*curControl).caption == caption && (*curControl).name == name)
		{
			if (control)
				*control = curControl;
			return true;
		}
	return false;
}

void ConfigDialog::removeControl(const QString& groupname, const QString& caption, const QString& name)
{
	QValueListIterator<RegisteredControl> control;

	//kdebugm(KDEBUG_INFO, "nrOfControls=%i "+groupname+"\\"+caption+"\\"+name+"\n", RegisteredControls[i].nrOfControls);
	//
	if (!controlExists(groupname, caption, name, &control))
	{
		kdebugm(KDEBUG_ERROR, "No such control %s %s %s\n", groupname.ascii(), caption.ascii(), name.ascii());
		return;
	}

	if ((*control).nrOfControls != 0)
	{
		kdebugm(KDEBUG_ERROR, "Container not empty: %d %s %s %s\n", (*control).nrOfControls, groupname.ascii(), caption.ascii(), name.ascii());
		return;
	}

	--(*(*control).parentControl).nrOfControls;

	Tabs[groupname].remove(control);
	--TabSizes[groupname];
	kdebugm(KDEBUG_INFO, "control deleted "+groupname+"\\"+caption+"\\"+name+"\n");
}

bool ConfigDialog::removeTab(const QString& caption)
{
	kdebugf();
	bool ok;
	QValueListConstIterator<RegisteredControl> curControl;
	tab(caption, curControl, &ok);
	if (!ok)
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "Tab %s not found\n", caption.ascii());
		return false;
	}

	if ((*curControl).nrOfControls == 0)
	{
		Tabs.remove(caption);
		TabNames.remove(caption);
		TabSizes.remove(caption);
//		kdebugf2();
		return true;
	}
	else
	{
		kdebugm(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "can't remove tab %s (ctrls: %d)!\n", caption.local8Bit().data(), (*curControl).nrOfControls);
		CONST_FOREACH(i, Tabs[caption])
			kdebugm(KDEBUG_WARNING, ">>> %s\n", (*i).caption.local8Bit().data());
		return false;
	}
}

bool ConfigDialog::tabExists(const QString& caption)
{
	return Tabs.contains(caption);
}

QWidget* ConfigDialog::widget(const QString& groupname, const QString& caption, const QString& name)
{
	if (configdialog == NULL)
	{
		kdebugm(KDEBUG_PANIC, "ConfigDialog is closed! Can't get widget! (%s,%s,%s)\n",
			groupname.local8Bit().data(), caption.local8Bit().data(), name.local8Bit().data());
		printBacktrace("ConfigDialog::widget(): CD is closed!");
		return NULL;
	}
	QValueListIterator<RegisteredControl> control;
	if (controlExists(groupname, caption, name, &control))
	{
		if (!(*control).widget)
		{
			configdialog->createTabAndWidgets((*control).group);
			configdialog->createWidget(control);
		}

		return (*control).widget;
	}

	kdebugm(KDEBUG_PANIC, "Warning: there is no \\" +groupname+ "\\"+ caption+ "\\"+ name+ "\\ control\n");
	return NULL;
}

bool ConfigDialog::addControl(const QString& groupname, ConfigDialog::RegisteredControl& control)
{
//	kdebugf();
	bool ok;
	QValueListIterator<RegisteredControl> curControl, tabControl, end, parent;
	tab(groupname, tabControl, &ok);
	if (!ok)
	{
		kdebugm(KDEBUG_FUNCTION_END|KDEBUG_ERROR, "There is no Tab: "+groupname+"\n");
		return false;
	}
	end = Tabs[groupname].end();

	int counter = 0;
	for (curControl = tabControl; curControl != end; ++curControl)
		if ((*curControl).caption == control.parent)
			if (counter == 0)
			{
				parent = curControl;
				control.parentControl = curControl;
				++counter;
			}
			else
				break;
	if (counter == 1)
	{
		++(*parent).nrOfControls;
		Tabs[groupname].insert(curControl, control);
		++TabSizes[groupname];
//		kdebugf2();
		return true;
	}
	else
	{
//		kdebugf2();
		return false;
	}
}

bool ConfigDialog::dialogOpened()
{
	return (configdialog!=NULL);
}

QButtonGroup* ConfigDialog::getButtonGroup(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QButtonGroup*>(widget(groupname,caption,name));
}

QCheckBox* ConfigDialog::getCheckBox(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QCheckBox*>(widget(groupname,caption,name));
}

ColorButton* ConfigDialog::getColorButton(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<ColorButton*>(widget(groupname,caption,name));
}

QComboBox* ConfigDialog::getComboBox(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QComboBox*>(widget(groupname,caption,name));
}

QGrid* ConfigDialog::getGrid(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QGrid*>(widget(groupname,caption,name));
}

QHBox* ConfigDialog::getHBox(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QHBox*>(widget(groupname,caption,name));
}

QHGroupBox* ConfigDialog::getHGroupBox(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QHGroupBox*>(widget(groupname,caption,name));
}

QHButtonGroup* ConfigDialog::getHButtonGroup(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QHButtonGroup*>(widget(groupname,caption,name));
}

HotKey* ConfigDialog::getHotKeyEdit(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<HotKey*>(widget(groupname,caption,name));
}

QLineEdit* ConfigDialog::getLineEdit(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QLineEdit*>(widget(groupname,caption,name));
}

QTextEdit* ConfigDialog::getTextEdit(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QTextEdit*>(widget(groupname,caption,name));
}

QLabel* ConfigDialog::getLabel(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QLabel*>(widget(groupname,caption,name));
}

QListBox* ConfigDialog::getListBox(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QListBox*>(widget(groupname,caption,name));
}

QListView* ConfigDialog::getListView(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QListView*>(widget(groupname,caption,name));
}

QPushButton* ConfigDialog::getPushButton(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QPushButton*>(widget(groupname,caption,name));
}

SelectFont* ConfigDialog::getSelectFont(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<SelectFont*>(widget(groupname,caption,name));
}

SelectPaths* ConfigDialog::getSelectPaths(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<SelectPaths*>(widget(groupname,caption,name));
}

QSlider* ConfigDialog::getSlider(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QSlider*>(widget(groupname,caption,name));
}

QSpinBox* ConfigDialog::getSpinBox(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QSpinBox*>(widget(groupname,caption,name));
}

QVBox* ConfigDialog::getVBox(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QVBox*>(widget(groupname,caption,name));
}

QVGroupBox* ConfigDialog::getVGroupBox(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QVGroupBox*>(widget(groupname,caption,name));
}

QVButtonGroup* ConfigDialog::getVButtonGroup(const QString& groupname, const QString& caption, const QString& name)
{
	return dynamic_cast<QVButtonGroup*>(widget(groupname,caption,name));
}

QWidget* ConfigDialog::getEntireWidget(const QString& groupname, const QString& caption, const QString& name)
{
	if (configdialog == NULL)
	{
		kdebugm(KDEBUG_PANIC, "ConfigDialog is closed! Can't get widget! (%s,%s,%s)\n",
			groupname.local8Bit().data(), caption.local8Bit().data(), name.local8Bit().data());
		printBacktrace("ConfigDialog::widget(): CD is closed!");
		return NULL;
	}
	QValueListConstIterator<RegisteredControl> control;
	if (controlExists(groupname, caption, name, &control))
		return (*control).entireWidget;
	kdebugm(KDEBUG_PANIC, "Warning: there is no \\" +groupname+ "\\"+ caption+ "\\"+ name+ "\\ control\n");
	return NULL;
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
	else
	{
		if ((e->state()& Qt::ShiftButton) || (e->key() == Qt::Key_Shift))
			result+= "Shift+";
		if ((e->state()& Qt::AltButton) || (e->key() == Qt::Key_Alt))
			result+= "Alt+";
	}

	if (!((e->key() == Qt::Key_Control)
		||(e->key() == Qt::Key_Shift)
		||(e->key() == Qt::Key_Alt)
		||(e->key() == Qt::Key_Meta)))
			result += QAccel::keyToString(QKeySequence(e->key()));

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
		setText(QString::null);
}

QKeySequence HotKey::getShortCut() const
{
	return QKeySequence(text());
}

QString HotKey::getShortCutString() const
{
	return text();
}

void HotKey::setShortCut(const QString& shortcut)
{
	QKeySequence str(shortcut);
	if (str == QKeySequence())
		setText(QString::null);
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

const QFont &SelectFont::font() const
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

const QColor &ColorButton::color() const
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
	l_icon->setPixmap(icons_manager->loadIcon("SelectPathWindowIcon"));
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
	pb_add = new QPushButton(icons_manager->loadIcon("AddSelectPathDialogButton"), tr("Add"), vb_managebuttons, "addButton");
	pb_change = new QPushButton(icons_manager->loadIcon("ChangeSelectPathDialogButton"), tr("Change"), vb_managebuttons, "changeButton");
	pb_remove = new QPushButton(icons_manager->loadIcon("RemoveSelectPathDialogButton"), tr("Remove"), vb_managebuttons, "removeButton");
	QWidget *w_managebuttons = new QWidget(vb_managebuttons,"blank");
	vb_managebuttons->setStretchFactor(w_managebuttons, 1);

	QHBox *hb_selectpath = new QHBox(vgb_pathtoadd,"box");
	hb_selectpath ->setSpacing(5);
	pathEdit = new QLineEdit(hb_selectpath,"newPathLineEdit");
	pb_choose = new QPushButton(icons_manager->loadIcon("ChooseSelectPathDialogButton"), tr("Choose"), hb_selectpath, "chooseButton");
	hb_selectpath->setStretchFactor(pathEdit, 1);
	// end create needed fields

	// buttons
	QHBox *bottom = new QHBox(center, "bottom");
	QWidget *blank2 = new QWidget(bottom,"blank");
	bottom->setSpacing(5);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	pb_ok = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("OK"), bottom, "okButton");
	pb_cancel = new QPushButton(icons_manager->loadIcon("CancelWindowButton"), tr("&Cancel"), bottom, "cancelButton");
	// end buttons

	connect(pb_ok, SIGNAL(clicked()), this, SLOT(okButton()));
	connect(pb_cancel, SIGNAL(clicked()), this, SLOT(cancelButton()));
	connect(pb_choose, SIGNAL(clicked()), this, SLOT(choosePath()));
	connect(pb_change, SIGNAL(clicked()), this, SLOT(replacePath()));
	connect(pb_add, SIGNAL(clicked()), this, SLOT(addPath()));
	connect(pb_remove, SIGNAL(clicked()), this, SLOT(deletePath()));

	loadGeometry(this, "General", "SelectPathDialogGeometry", 0, 30, 330, 330);
	kdebugf2();
}

const QStringList &SelectPaths::getPathList() const
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
	if (!dirtoadd.isEmpty())
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
	if (!dirtochange.isEmpty())
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
	if (dir.cd(pathEdit->text()) && (!pathEdit->text().isEmpty()))
		startdir=pathEdit->text();
	QString s= QFileDialog::getExistingDirectory(startdir, this, "getDirectory", tr("Choose a directory"));
	if (!s.isEmpty())
		pathEdit->setText(s);
	kdebugf2();
}

void SelectPaths::okButton()
{
	kdebugf();
	releaseList.clear();
	for (unsigned int i = 0, count = pathListBox->count(); i < count; ++i)
		releaseList.append(pathListBox->text(i));

	pathEdit->setText(QString::null);
	hide();
	emit changed(releaseList);
	kdebugf2();
}

void SelectPaths::cancelButton()
{
	kdebugf();
	pathListBox->clear();
	pathListBox->insertStringList(releaseList);
	pathEdit->setText(QString::null);
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
