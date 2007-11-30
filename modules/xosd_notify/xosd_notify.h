#ifndef XOSD_NOTIFY_H
#define XOSD_NOTIFY_H

#include <xosd.h>

#include <qobject.h>
#include <qtimer.h>
#include <qvaluelist.h>

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

		int font_size;//warto¶æ pomocnicza
		TestConfig();
	};

class XOSDConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT
		QString currentNotifyEvent;
	
		QMap<QString, TestConfig> configs;
		TestConfig currentConfig;

		/**
			sprawdza rozmiar czcionki wed³ug XLFD,
			je¿eli znajdzie odpowiednie pole to zwraca rozmiar czcionki,
			je¿eli nie, to zwraca -1
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

class XOSDNotify : public Notifier, public ConfigurationUiHandler, public ConfigurationAwareObject
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

			int font_size;//warto¶æ pomocnicza
			OSDLine();
		};

		QMap<QString, TestConfig> configs;

		struct offset
		{
			int x,y;
		} offsets[9];

		QValueList<OSDLine> lines[9];

		QTimer *timer;

		QString currentOptionPrefix;
		QStringList optionPrefixes;

		/**
			przekszta³ca pozycjê - liczbê z zakresu 0..8 na zmienne typów
			xosd_pos(XOSD_left, XOSD_center, XOSD_right) i xosd_align(XOSD_top, XOSD_middle, XOSD_bottom)
		**/
		void position2PosAlign(int position, xosd_pos &pos, xosd_align &align);

		/**
			sprawdza rozmiar czcionki wed³ug XLFD,
			je¿eli znajdzie odpowiednie pole to zwraca rozmiar czcionki,
			je¿eli nie, to zwraca -1
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
			od¶wie¿a wszystkie dymki na pozycji "position"
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
			dodajê now± liniê tekstu na pozycji "position" (0..8) o czasie znikania "timeout"
			czcionce "font", kolorze "font_color", kolorze cienia "shadow_color" i przesuniêciu "shadow_offset",
			kolorze obramówki "outline_color" i przesuniêciu obramówki "outline_offset"
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
