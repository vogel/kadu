#ifndef KADU_ICONS_MANAGER_H
#define KADU_ICONS_MANAGER_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <qmenudata.h>
#include <qmovie.h>
#include <qicon.h>
#include <qpixmap.h>
#include <qstring.h>
#include <q3valuelist.h>

#include "configuration_aware_object.h"
#include "themes.h"

class IconsManager : public Themes, ConfigurationAwareObject
{
	Q_OBJECT

	QMap<QString, QPixmap> icons;
	QMap<QString, QIcon> iconSets;
// 	QMap<QString, QMovie> animatedIcons;

//	Q3ValueList<QPair<QMenuData *, Q3ValueList<QPair<QString, QString> > > > menus;

public:
	IconsManager(const QString& name, const QString& configname);

	/**
		Zwraca pe³n± ¶cie¿kê do ikony z aktualnego zestawu
		lub bezposrednio name je¶li jest to pe³na ¶cie¿ka.
		@param name nazwa ikony z zestawu lub sciezka do pliku
		(jesli zawiera znak '/' to jest interpretowana jako
		sciezka).
	**/
	QString iconPath(const QString &name) const;

	/**
		£aduje ikonê z aktualnego zestawu lub z podanego pliku.
		@param name nazwa ikony z zestawu lub ¶cie¿ka do pliku
		(je¶li zawiera znak '/' to jest interpretowana jako
		¶cie¿ka).
	**/
	const QPixmap &loadIcon(const QString &name);
// 	const QMovie &loadAnimatedIcon(const QString &name);
	const QIcon &loadIconSet(const QString &name);

	static void initModule();
	static void closeModule();

// 	void registerMenu(QMenuData *menu);
// 	void unregisterMenu(QMenuData *menu);

// 	void registerMenuItem(QMenuData *menu, const QString &caption, const QString &iconName);
// 	void unregisterMenuItem(QMenuData *menu, const QString &caption);

public: // TODO: fix, see Kadu::Kadu
	virtual void configurationUpdated();

public slots:
	void clear();
	void refreshMenus();

signals:
	void themeChanged();

};

extern IconsManager *icons_manager;

#endif
