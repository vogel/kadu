#ifndef HINT_MANAGER_SLOTS_H
#define HINT_MANAGER_SLOTS_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qcolor.h>
#include <qfont.h>

class HintProperties
{
	public:
		static HintProperties fromString(const QString &oldstring);
	
		QFont font;
		QColor fgcolor;
		QColor bgcolor;
		unsigned int timeout;
};

class HintManagerSlots : public QObject
{
	Q_OBJECT
	public:
		HintManagerSlots(QObject *parent=0, const char *name=0);
		~HintManagerSlots();

	public slots:
		void onCreateConfigDialog();
		void onApplyConfigDialog();
		void onCloseConfigDialog();
		
		void toggled_ShowMessageContent(bool);
		void toggled_UseNotifySyntax(bool);
		void toggled_UseOwnPosition(bool);
		void toggled_SetAll(bool);
		
		void clicked_HintType(int id);
		void clicked_ChangeFont();
		void clicked_ChangeFgColor();
		void clicked_ChangeBgColor();
		
		void changed_Timeout(int value);

	private:
		QStringList config_opts_prefixes;
		QString currentOptionPrefix;
		QMap<QString, HintProperties> config_hint_properties;
};

#endif
