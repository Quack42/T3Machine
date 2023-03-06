/*
 * T3Machine.h
 *
 *  Created on: Feb 26, 2023
 *      Author: quack
 */

#ifndef INC_T3MACHINE_H_
#define INC_T3MACHINE_H_

template<typename Platform, typename DriverX, /*DriverY, DriverZ,*/ typename SensorX>//, SensorY, SensorZ>
class T3Machine {
private:
	DriverX & driverX;
	/*DriverY & driverY;
	DriverZ & driverZ;*/
	SensorX & sensorX;
//	SensorY & sensorY;
//	SensorZ & sensorZ;

	
public:
	T3Machine(	DriverX & driverX,
				/*DriverY & driverY,
				DriverZ & driverZ,*/
				SensorX & sensorX//,
//				SensorY & sensorY,
//				SensorZ & sensorZ
	) :
			driverX(driverX),
			/*driverY(driverY),
			driverZ(driverZ),*/
			sensorX(sensorX)//,
//			sensorY(sensorY),
//			sensorZ(sensorZ)
	{

	}

	void tick() {
		if (sensorX.isInterruptFlagged()) {
			//halt
		}
	}

private:
	void emergencyStop() {
		//TODO: halt behaviors
		//TODO: indicate issue to user
	}
};



#endif /* INC_T3MACHINE_H_ */
