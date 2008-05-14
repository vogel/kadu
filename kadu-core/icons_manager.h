#ifndef KADU_ICONS_MANAGER_H
#define KADU_ICONS_MANAGER_H

#include <QIcon>
#include <QMap>
#include <QPixmap>

#include "configuration_aware_object.h"
#include "themes.h"

class IconsManager : public Themes, ConfigurationAwareObject
{
	Q_OBJECT

	QMap<QString, QPixmap> pixmaps;
	QMap<QString, QIcon> icons;
// 	QMap<QString, QMovie> animatedIcons;

//	QList<QPair<QMenuData *, QList<QPair<QString, QString> > > > menus;

public:
	IconsManager(const QString &name, const QString &configname);

	/**
		Zwraca pe�n� �cie�k� do ikony z aktualnego zestawu
		lub bezposrednio name je�li jest to pe�na �cie�ka.
		@param name nazwa ikony z zestawu lub sciezka do pliku
		(jesli zawiera znak '/' to jest interpretowana jako
		sciezka).
	**/
	QString iconPath(const QString &name) const;

	/**
		�aduje ikon� z aktualnego zestawu lub z podanego pliku.
		@param name nazwa ikony z zestawu lub �cie�ka do pliku
		(je�li zawiera znak '/' to jest interpretowana jako
		�cie�ka).
	**/
	const QPixmap & loadPixmap(const QString &name);
// 	const QMovie &loadAnimatedIcon(const QString &name);
	const QIcon & loadIcon(const QString &name);

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
