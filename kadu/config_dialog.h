#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <qtabdialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qslider.h>
#include <qvgroupbox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qhostaddress.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qlabel.h>

#include "libgadu.h"
#include "../config.h"

void loadKaduConfig(void);
void saveKaduConfig(void);

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
			CONFIG_TAB,
			CONFIG_GROUPBOX,
			CONFIG_CHECKBOX,
			CONFIG_LINEEDIT,
			CONFIG_HOTKEYEDIT
		};
		struct RegisteredControl
		{
			RegisteredControlType type;
			QString parent;
			QString caption;
			QString group;
			QString entry;
			QString defaultS;
			QWidget* widget;
		};
		static QValueList<RegisteredControl> RegisteredControls;
		static void registerTab(const QString& caption);
		static void registerGroupBox(
			const QString& parent,const QString& caption);
		static void registerCheckBox(
			const QString& parent,const QString& caption,
			const QString& group,const QString& entry,const bool defaultS=false);
		static void registerLineEdit(
			const QString& parent,const QString& caption,
			const QString& group,const QString& entry,const QString& defaultS="");
		static void registerHotKeyEdit(
			const QString& parent,const QString& caption,
			const QString& group,const QString& entry,const QString& defaultS="");
		//
				
	protected:
		void setupTab1();
		void setupTab2();
		void setupTab3();
		void setupTab4();
		void setupTab5();
		void setupTab6();
		void setupTab7();

		static ConfigDialog *configdialog;
		static QString acttab;

		QVBox *box[10];

		QComboBox *cb_defstatus;
		QCheckBox *b_geometry;
		QCheckBox *b_logging;
		QLineEdit *e_password;
		QLineEdit *e_uin;
		QLineEdit *e_nick;
		QHBox     *smshbox1;
		QHBox     *smshbox2;
		QCheckBox *b_smsbuildin;
		QLineEdit *e_smsapp;
		QLineEdit *e_smsconf;
		QCheckBox *b_smscustomconf;
		QCheckBox *b_autoaway;
		QLineEdit *e_autoawaytime;
		QLineEdit *e_hinttime;
		QCheckBox *b_dock;
		QCheckBox *b_private;
		QCheckBox *b_rdocked;
		QCheckBox *b_grptabs;
		QCheckBox *b_checkupdates;
		QCheckBox *b_addtodescription;
		QCheckBox *b_trayhint;
		QCheckBox *b_hinterror;
		QCheckBox *b_showdesc;
		QCheckBox *b_multicoluserbox;
		QCheckBox *b_disconnectdesc;
		QLineEdit *e_disconnectdesc;

		QLineEdit *e_soundprog;
		QCheckBox *b_playsound;
		QCheckBox *b_playartsdsp;
		QLineEdit *e_msgfile;
		QLineEdit *e_chatfile;
		QCheckBox *b_playchat;
		QCheckBox *b_playchatinvisible;
		QSlider *s_volume;
		QCheckBox *b_soundvolctrl;

		QComboBox *cb_emoticons_style;
		QComboBox *cb_emoticons_theme;
		QHBox	  *emotheme_box;
		QCheckBox *b_autosend;
		QCheckBox *b_scrolldown;
		QLineEdit *e_emoticonspath;
		QCheckBox *b_chatprune;
		QLineEdit *e_chatprunelen;
		QCheckBox *b_msgacks;
		QCheckBox *b_blinkchattitle;
		QHBox     *webhbox1;
		QCheckBox *b_defwebbrowser;
		QLineEdit *e_webbrowser;
		QCheckBox *b_ignoreanonusers;
		QCheckBox *b_hintalert;

		QVGroupBox *notifybox;
		QHBox *panebox;
		QListBox *e_notifies;
		QListBox *e_availusers;
		QCheckBox *b_notifyall;
		QCheckBox *b_notifyglobal;
		QCheckBox *b_notifydialog;
		QCheckBox *b_notifysound;
		QCheckBox *b_notifyhint;
		QLineEdit *e_soundnotify;

		QCheckBox *b_dccenabled;
		QCheckBox *b_dccip;
		QVGroupBox *g_dccip;
		QLineEdit *e_dccip;
		QCheckBox *b_dccfwd;
		QVGroupBox *g_fwdprop;
		QLineEdit *e_extip;
		QLineEdit *e_extport;
		QCheckBox *b_defserver;
		QCheckBox *b_tls;
		QHBox	*serverbox;
		QLineEdit *e_server;
		QCheckBox *b_useproxy;
		QLineEdit *e_proxyserver;
		QLineEdit *e_proxyport;
		QLineEdit *e_proxyuser;
		QLineEdit *e_proxypassword;
		QHBox	*portbox;
		QComboBox *cb_portselect;

		QComboBox *cb_chatfont;
		QComboBox *cb_chatfontsize;
		QComboBox *cb_userboxfont;
		QComboBox *cb_userboxfontsize;
		QComboBox *cb_chatselect;
		QComboBox *cb_userboxselect;
		QComboBox *cb_otherselect;
		QComboBox *cb_otherfont;
		QComboBox *cb_otherfontsize;
		QPushButton *pb_chatcolor;
		QPushButton *pb_userboxcolor;
		QPushButton *pb_othercolor;
		QLineEdit *e_chatcolor;
		QLineEdit *e_userboxcolor;
		QLineEdit *e_othercolor;
		QLineEdit *e_panelsyntax;
		QLineEdit *e_chatsyntax;
		QLineEdit *e_conferencesyntax;
		QLineEdit *e_conferenceprefix;
		QHBox *chatselectfont;
		QHBox *userboxselectfont;
		QHBox *otherselectfont;
		QValueList<QColor> vl_userboxcolor;
		QValueList<QFont> vl_userboxfont;
		QValueList<QColor> vl_chatcolor;
		QValueList<QFont> vl_chatfont;
		QValueList<QColor> vl_othercolor;
		QValueList<QFont> vl_otherfont;
		
		QSpinBox *s_qcount;
		QSlider *s_qtime;
		QLabel *l_qtimeinfo;

#ifdef HAVE_OPENSSL
		QCheckBox *b_encryption;
		QCheckBox *b_encryptmsg;
		QComboBox *cb_keyslen;
		QPushButton *pb_encryption;
#endif

	protected slots:
		void _Left();
		void _Right();
		void updateConfig();
		void chooseMsgFile();
		void chooseChatFile();
		void choosePlayerFile();
		void chooseNotifyFile();
		void chooseEmoticonsPath();
		void generateMyKeys();
		void chooseChatColorGet();
		void chooseUserboxColorGet();
		void chooseChatFontGet(int index);
		void chooseUserboxFontGet(int index);
		void chooseChatSelect(int index);
		void chooseUserboxSelect(int index);
		void chooseUserboxFontSizeGet(int index);
		void chooseChatFontSizeGet(int index);
		void chooseOtherColorGet();
		void chooseOtherFontGet(int index);
		void chooseOtherSelect(int index);
		void chooseOtherFontSizeGet(int index);
		void chooseChatLine(const QString&);
		void chooseUserboxLine(const QString&);
		void chooseOtherLine(const QString&);
		void chooseMsgTest();
		void chooseChatTest();
		void chooseNotifyTest();
		void chooseEmoticonsStyle(int index);
		void ifDockEnabled(bool);
		void ifNotifyGlobal(bool);
		void ifNotifyAll(bool);
		void ifDccEnabled(bool);
		void ifDccIpEnabled(bool);
		void ifDefServerEnabled(bool);
		void useTlsEnabled(bool);
		void onSmsBuildInCheckToogle(bool);
		void onDefWebBrowserToogle(bool);
		void updateQuoteTimeLabel(int);
};

struct colors {
	QColor userboxBg;
	QColor userboxFg;
	QColor userboxDescBg;
	QColor userboxDescText;
	QColor mychatBg;
	QColor mychatText;
	QColor usrchatBg;
	QColor usrchatText;
	QColor trayhintBg;
	QColor trayhintText;
};

struct fonts {
	QFont userbox;
	QFont userboxDesc;
	QFont chat;
	QFont trayhint;
};

enum EmoticonsStyle
{
	EMOTS_NONE,
	EMOTS_STATIC,
	EMOTS_ANIMATED
};

struct config {
	uin_t uin;
	QString password;
	QString nick;
	QString soundmsg;
	QString soundchat;
	QString soundprog;
	double soundvol;
	bool soundvolctrl;
	bool playartsdsp;
	bool playsound;
	bool logmessages;
	bool savegeometry;
	bool playsoundchat;
	bool playsoundchatinvisible;
	int defaultstatus;
	QStringList defaultdescription;
	int sysmsgidx;
	bool allowdcc;
	QHostAddress dccip;
	QHostAddress extip;
	int extport;
	QValueList<QHostAddress> servers;
	bool default_servers;
	int default_port;
	int tls;

	bool dock;
	bool rundocked;
	
	bool privatestatus;
	bool grouptabs;
	bool checkupdates;
	bool addtodescription;
	bool trayhint;
	bool hinterror;
	bool hintalert;
	bool showdesc;
	bool multicoluserbox;
	bool disconnectwithdesc;
	
	QString panelsyntax;
	QString chatsyntax;
	QString conferenceprefix;
	QString conferencesyntax;
	QString disconnectdesc;

	QRect geometry;
	QString dockwindows;
	QSize splitsize;
	
	bool defaultwebbrowser;
	QString webbrowser;

	bool smsbuildin;
	char * smsapp;
	bool smscustomconf;
	char * smsconf;

	EmoticonsStyle emoticons_style;
	QString emoticons_theme;
	bool autosend;
	bool scrolldown;
	int chatprunelen;
	int chathistorycitation;
	int chathistorycitationtime;
	bool chatprune;
	bool msgacks;
	bool blinkchattitle;
	bool ignoreanonusers;

	bool autoaway;
	int autoawaytime;
	int hinttime;

	QStringList notifies;
	char *soundnotify;
	bool notifyglobal;
	bool notifyall;
	bool notifydialog;
	bool notifysound;
	bool notifyhint;

	bool useproxy;
	QHostAddress proxyaddr;
	unsigned short proxyport;
	QString proxyuser;
	QString proxypassword;

	bool raise;

	struct colors colors;
	struct fonts fonts;
#ifdef HAVE_OPENSSL
	bool encryption;
#endif
};

extern struct config config;

#endif
