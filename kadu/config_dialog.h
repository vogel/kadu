#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <qtabdialog.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qvgroupbox.h>
#include <qhgroupbox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qgrid.h>
#include <qrect.h>
#include <qslider.h>

void loadKaduConfig(void);


/**
	Okno z konfiguracj± Kadu
**/
class ConfigDialog : public QTabDialog	{
	Q_OBJECT


	public:
		ConfigDialog(QWidget *parent = 0, const char *name = 0);
		~ConfigDialog();
		static void showConfigDialog();
		// nowy mechanizm
		enum RegisteredControlType
		{
			CONFIG_CHECKBOX,
			CONFIG_COMBOBOX,
			CONFIG_GRID,
			CONFIG_HGROUPBOX,
			CONFIG_HOTKEYEDIT,
			CONFIG_LABEL,
			CONFIG_LINEEDIT,
			CONFIG_LISTBOX,
			CONFIG_PUSHBUTTON,
			CONFIG_SLIDER,
			CONFIG_SPINBOX,
			CONFIG_TAB,
			CONFIG_VGROUPBOX
		};

		struct ElementConnections
		{
			QString signal;
			QObject* receiver;
			QString slot;
		};

		struct RegisteredControl
		{
			RegisteredControlType type;
			QString parent;
			QString name;
			QString caption;
			QString group;
			QString entry;
			QString defaultS;
			QString tip;
			QWidget* widget;
			QValueList <ElementConnections> ConnectedSlots;
		};


		static QValueList <RegisteredControl> RegisteredControls;
		static QValueList <ElementConnections> SlotsOnCreate;
		static QValueList <ElementConnections> SlotsOnDestroy;
		
 		static void registerCheckBox(
		    	    const QString& parent,const QString& caption,
			    const QString& group,const QString& entry,const bool defaultS=false,const QString &tip="",const QString& name="");
			    			    
		static void registerComboBox(
			    const QString& parent,const QString& caption,
			    const QString& group,const QString& entry,const QString& tip="",const QString& name="");
			    
		static void registerGrid(
			    const QString& parent,const QString& caption,const int nrColumns=3,const QString& name="");

		static void registerHGroupBox(
			    const QString& parent,const QString& caption,const QString& name="");
			    
		static void registerHotKeyEdit(
			    const QString& parent,const QString& caption,
			    const QString& group,const QString& entry,const QString& defaultS="",const QString& tip="",const QString& name="");
			    
		static void registerLabel(
			    const QString& parent,const QString& caption,const QString& name="");
			    
		static void registerLineEdit(
			    const QString& parent,const QString& caption,
			    const QString& group,const QString& entry,const QString& defaultS="",const QString& tip="",const QString& name="");
			    
		static void registerListBox(
			    const QString& parent,const QString& caption,const QString& tip,const QString& name="");
			    
		static void registerPushButton(
			    const QString& parent,const QString& caption,
			    const QString& iconFileName="",const QString& tip="",const QString& name="");
			    
		static void registerSlider(
			    const QString& parent,const QString& caption,
			    const QString& group,const QString& entry,
			    const int minValue=0, const int maxValue=100,
			    const int pageStep=1,const int value=50,const QString& tip="",const QString& name="");
			    
		static void registerSpinBox(
			    const QString& parent,const QString& caption,
			    const QString& group,const QString& entry,
			    const int minValue=0, const int maxValue=100,const int step=1,
			    const int value=50,const QString& tip="",const QString& name="");
			    
		static void registerTab(const QString& caption);
		
		static void registerVGroupBox(
			    const QString& parent,const QString& caption,const QString& name="");

		static void connectSlot(const QString& caption,const char* signal,
			    const QObject* receiver,const char* slot);

		static void registerSlotOnCreate(const QObject* receiver,const char* slot);
		static void registerSlotOnDestroy(const QObject* receiver,const char* slot);
		
		static QWidget* getWidget(const QString& parent,const QString& caption,const QString& name="");
		static bool existControl(const QString& parent,const QString& caption,const QString& name="");
		
		//
		static void initModule();
	
	protected:

		static ConfigDialog *configdialog;
		static QString acttab;

	signals:
		void create();
		void destroy();


	protected slots:
		void generateMyKeys();
		void updateConfig();		
};

class ConfigSlots: public QObject
{
	Q_OBJECT
	
	    public slots:
	    
	    void onCreateConfigDialog();
	    void onDestroyConfigDialog();
	    
	    void ifDccEnabled(bool);
	    void ifDccIpEnabled(bool);
	    void ifDefServerEnabled(bool);
	    void useTlsEnabled(bool);

};

#endif
