/***************************************************************************
                          config.h  -  description
                             -------------------
    begin                : Thu Feb 14 2002
    copyright            : (C) 2002 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <qtabdialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qslider.h>
#include <qvgroupbox.h>
#include "../libgadu/lib/libgadu.h"

void loadKaduConfig(void);
void saveKaduConfig(void);

class ConfigDialog : public QTabDialog	{
	Q_OBJECT

	public:
		ConfigDialog(QWidget *parent=0, const char *name=0);

	protected:
		void setupTab1();
		void setupTab2();
		void setupTab3();
		void setupTab4();
		void setupTab5();
		void setupTab6();

		QComboBox *cb_defstatus;
		QCheckBox *b_geometry;
		QCheckBox *b_logging;
		QLineEdit *e_password;
		QLineEdit *e_uin;
		QLineEdit *e_nick;
		QLineEdit *e_smsapp;
		QLineEdit *e_smsconf;
		QCheckBox *b_smscustomconf;
		QCheckBox *b_autoaway;
		QLineEdit *e_autoawaytime;
		QCheckBox *b_dock;
		QCheckBox *b_private;
		QCheckBox *b_rdocked;
		QCheckBox *b_grptabs;

		QLineEdit *e_soundprog;
		QCheckBox *b_playsound;
		QCheckBox *b_playartsdsp;
		QLineEdit *e_msgfile;
		QLineEdit *e_chatfile;
		QCheckBox *b_playchat;
		QCheckBox *b_playchatinvisible;
		QSlider *s_volume;
		QCheckBox *b_soundvolctrl;

		QCheckBox *b_emoticons;
		QCheckBox *b_autosend;
		QCheckBox *b_scrolldown;
		QLineEdit *e_emoticonspath;
		QCheckBox *b_chatprune;
		QLineEdit *e_chatprunelen;
		QCheckBox *b_msgacks;
		
		QListBox *e_notifies;
		QListBox *e_availusers;
		QCheckBox *b_notifyglobal;
		QCheckBox *b_notifydialog;
		QCheckBox *b_notifysound;
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
		QVGroupBox *g_server;
		QLineEdit *e_server;

		QVGroupBox *bgcolor;
		QLineEdit *e_chatusrbgcolor;
		QLineEdit *e_chatmybgcolor;
		QLineEdit *e_userboxbgcolor;
		QLineEdit *e_userboxfgcolor;
		QLineEdit *e_chatusrfontcolor;
    QLineEdit *e_chatmyfontcolor;
		QPushButton *pb_chatmybgcolor;
		QPushButton *pb_chatusrbgcolor;
		QPushButton *pb_chatmyfontcolor;
		QPushButton *pb_chatusrfontcolor;
		QPushButton *pb_userboxbgcolor;
		QPushButton *pb_userboxfgcolor;

	protected slots:
		void _Left();
		void _Right();
		void updateConfig();
		void chooseMsgFile();
		void chooseChatFile();
		void choosePlayerFile();
		void chooseNotifyFile();
		void chooseEmoticonsPath();
		void chooseChatMyBgColorGet();
		void chooseChatUsrBgColorGet();
		void chooseChatMyFontColorGet();
		void chooseChatUsrFontColorGet();
		void chooseUserboxBgColorGet();
		void chooseUserboxFgColorGet();
		void emoticonsEnabled(bool);
		void ifDccEnabled(bool);
		void ifDccIpEnabled(bool);
		void ifDefServerEnabled(bool);
};

#endif
