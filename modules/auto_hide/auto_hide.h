#ifndef AUTO_HIDE_H
#define AUTO_HIDE_H

#include <QtCore/QTimer>
#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"

/*!
 * This class provides autohiding Kadu's main window after preset time.
 * \brief This class provides autohiding Kadu's main window after preset time
 */
class AutoHide : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	public:
		/*!
		 * \brief Default constructor
		 * \param *parent parent object
		 * \param *name object's name
		 */
		AutoHide(QObject *parent = 0);

		//! Default destructor
		~AutoHide();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
		
	protected:
		virtual void configurationUpdated();
	
	private slots:
		void timerTimeoutSlot();
		
	private:
		QTimer timer;
		int idleTime;
};

extern AutoHide *autoHide;

#endif
