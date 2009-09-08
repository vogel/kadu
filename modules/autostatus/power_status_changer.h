#ifndef POWER_STATUS_CHANGER_H
#define POWER_STATUS_CHANGER_H

#include "status_changer.h"

/*!
 * This class is PowerKadu's status changer.
 * \class PowerStatusChanger
 * \brief Main PowerKadu's status changer.
 */
class PowerStatusChanger : public StatusChanger
{
	Q_OBJECT

		/*!
		 * Type of status that is about to be set.
		 * \enum StatusState
		 */
		enum StatusState
		{
			STATUS_NOT_SPECIFIED,
			STATUS_ONLINE,
			STATUS_BUSY,
			STATUS_INVISIBLE,
			STATUS_OFFLINE,
			STATUS_FFC,
			STATUS_DND,
			STATUS_BY_INDEX
		};

	public:
		/*!
		 * \fn PowerStatusChanger(int priority)
		 * The constructor of PowerStatusChanger
		 * See \see StatusChanger for details.
		 */
		PowerStatusChanger();

		/*!
		 * \fn ~PowerStatusChanger()
		 * Default destructor
		 */
		~PowerStatusChanger();

		/*!
		 * This function is called when the statusChanged()
		 * signal is emmited.
		 * \param status An instance of a status that the
		 * function is supposed to change.
		 */
		virtual void changeStatus(UserStatus &status);

	public slots:
		/*!
		 * \fn void setOnline(QString desc)
		 * This function should be called by any PowerKadu component
		 * to change the status to online.
		 * \param desc The description for the status.
		 */
		void setOnline(QString desc);

		/*!
		 * \fn void setBusy(QString desc)
		 * This function should be called by any PowerKadu component
		 * to change the status to busy.
		 * \param desc The description for the status.
		 */
		void setBusy(QString desc);

		/*!
		 * \fn void setInvisible(QString desc)
		 * This function should be called by any PowerKadu component
		 * to change the status to invisible.
		 * \param desc The description for the status.
		 */
		void setInvisible(QString desc);

		/*!
		 * \fn void setOffline(QString desc)
		 * This function should be called by any PowerKadu component
		 * to change the status to offline.
		 * \param desc The description for the status.
		 */
		void setOffline(QString desc);

		/*!
		 * \fn void setTalkWithMe(QString desc)
		 * This function should be called by any PowerKadu component
		 * to change the status to talk with me.
		 * \param desc The description for the status.
		 */
		void setTalkWithMe(QString desc);

		/*!
		 * \fn void setDoNotDisturb(QString desc)
		 * This function should be called by any PowerKadu component
		 * to change the status to DND.
		 * \param desc The description for the status.
		 */
		void setDoNotDisturb(QString desc);

		/*!
		 * \fn void setStatus(StatusState passedState, QString passedDesc)
		 * Sets appropriate status state and description, then emits statusChanged()
		 *
		 * \param passedState State for the status
		 * \param passedDesc Description
		 */
		void setStatus(StatusState passedState, QString passedDesc);

		/*!
		 * \fn setIndex(int currStat, QString currDesc)
		 * This function should be called by any PowerKadu component
		 * to change the status by an index. See
		 * UserStatus::setIndex(int index, const QString& desc = QString::null)
		 * for details.
		 * \param currStat index
		 * \param currDesc description
		 */
		void setIndex(int currStat, QString currDesc);

	private:
		QString description;

		/*!
		 * State that is supposed to be set. If it's equal to 
		 * STATUS_NOT_SPECIFIED, the status will not be changed
		 * by PowerStatusChanger::changeStatus(UserStatus &status)
		 */
		StatusState state;
		int index;
};

#endif
