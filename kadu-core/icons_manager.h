#ifndef KADU_ICONS_MANAGER_H
#define KADU_ICONS_MANAGER_H

#include <qmenudata.h>
#include <qmovie.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qvaluelist.h>

#include "configuration_aware_object.h"
#include "themes.h"

class IconsManager : public Themes, ConfigurationAwareObject
{
	Q_OBJECT
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
		const QMovie &loadAnimatedIcon(const QString &name);
		const QIconSet &loadIconSet(const QString &name);

		static void initModule();
		static void closeModule();

		void registerMenu(QMenuData *menu);
		void unregisterMenu(QMenuData *menu);

		void registerMenuItem(QMenuData *menu, const QString &caption, const QString &iconName);
		void unregisterMenuItem(QMenuData *menu, const QString &caption);

	public: // TODO: fix, see Kadu::Kadu
		virtual void configurationUpdated();

	public slots:
		void clear();
		void refreshMenus();

	private:
		QMap<QString, QPixmap> icons;
		QMap<QString, QIconSet> iconSets;
		QMap<QString, QMovie> animatedIcons;

		QValueList<QPair<QMenuData *, QValueList<QPair<QString, QString> > > > menus;

};

extern IconsManager *icons_manager;

#endif
