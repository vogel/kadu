#ifndef XOSD_NOTIFY_H
#define XOSD_NOTIFY_H

#include <xosd.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QList>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"
#include "../notify/notify.h"

#include "xlfd_chooser.h"
class QSpinBox;
class ConfigSpinBox;

struct TestConfig
	{
		QString type;
		int position;
		QColor font_color, outline_color, shadow_color;
		QString font;
		int timeout;
		int shadow_offset, outline_offset;

		int font_size;//warto�� pomocnicza
		TestConfig();
	};

class XOSDConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT
		QString currentNotifyEvent;
	
		QMap<QString, TestConfig> configs;
		TestConfig currentConfig;

		/**
			sprawdza rozmiar czcionki wed�ug XLFD,
			je�eli znajdzie odpowiednie pole to zwraca rozmiar czcionki,
			je�eli nie, to zwraca -1
		**/
		int getFontSize(const QString &xlfd);

		ColorButton *fontColor;
		ColorButton *outlineColor;
		ColorButton *shadowColor;
		QSpinBox *shadowOffset;
		QSpinBox *OutlineOffset;
		QSpinBox *timeout;
		QComboBox *position;

	private slots:
		void clicked_ChangeFont();
		void fontSelected(const QString &name);
		void changed_Timeout(int);
		void changed_OutlineOffset(int);
		void changed_ShadowOffset(int);
		void clicked_ChangeFgColor(const QColor &);
		void clicked_ChangeOutlineColor(const QColor &);
		void clicked_ChangeShadowColor(const QColor &);	
		void clicked_Positions(int);
		void test();

	public:
		XOSDConfigurationWidget(QWidget *parent = 0, char *name = 0);
		virtual ~XOSDConfigurationWidget();

		virtual void loadNotifyConfigurations() {}
		virtual void saveNotifyConfigurations();
		virtual void switchToEvent(const QString &event);
};

class XOSDNotify : public Notifier, public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	private:
		void import_0_5_0_configuration();
		void import_0_5_0_ConfigurationFromTo(const QString &from, const QString &to);

		struct OSDLine
		{
			xosd *handle;
			QString text;
			int timeout;
			QColor font_color;
			QColor outline_color;
			QColor shadow_color;
			QString font;

			int font_size;//warto�� pomocnicza
			OSDLine();
		};

		QMap<QString, TestConfig> configs;

		struct offset
		{
			int x,y;
		} offsets[9];

		QList<OSDLine> lines[9];

		QTimer *timer;

		QString currentOptionPrefix;
		QStringList optionPrefixes;

		/**
			przekszta�ca pozycj� - liczb� z zakresu 0..8 na zmienne typ�w
			xosd_pos(XOSD_left, XOSD_center, XOSD_right) i xosd_align(XOSD_top, XOSD_middle, XOSD_bottom)
		**/
		void position2PosAlign(int position, xosd_pos &pos, xosd_align &align);

		/**
			sprawdza rozmiar czcionki wed�ug XLFD,
			je�eli znajdzie odpowiednie pole to zwraca rozmiar czcionki,
			je�eli nie, to zwraca -1
		**/
		int getFontSize(const QString &xlfd);

		xosd *testXOSD;

		ConfigSpinBox *XOffsetSpinBox;
		ConfigSpinBox *YOffsetSpinBox;		
		ConfigComboBox *positionComboBox;

	protected:
		virtual void configurationUpdated();
	private slots:
		void oneSecond();
		/**
			od�wie�a wszystkie dymki na pozycji "position"
		**/
		void refresh(int position);

	public:
		XOSDNotify(QObject *parent=0, const char *name=0);
		~XOSDNotify();
		
		virtual void notify(Notification *notification);

		virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);

		void copyConfiguration(const QString &fromEvent, const QString &toEvent) {}

 		void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	public slots:
		/**
			dodaj� now� lini� tekstu na pozycji "position" (0..8) o czasie znikania "timeout"
			czcionce "font", kolorze "font_color", kolorze cienia "shadow_color" i przesuni�ciu "shadow_offset",
			kolorze obram�wki "outline_color" i przesuni�ciu obram�wki "outline_offset"
		**/
		void addLine(int position, const QString &text, int timeout,
					const QString &font=QString::null, const QColor &font_color=QColor(),
					const QColor &shadow_color=QColor(), int shadow_offset=0,
					const QColor &outline_color=QColor(), int outline_offset=0);
		
		void test(const QString &text, const struct TestConfig &config);
		void destroyTest();

	private slots:
		
		void clicked_Positions2(int);
		void changed_XOffset(int);
		void changed_YOffset(int);

};

extern XLFDChooser *main_xlfd_chooser;
extern XOSDNotify *xosd_notify;
#endif
