/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef KINNECT_H
#define KINNECT_H

#include "ErrorBase.h"
#include "Skeleton.h"

class Kinect: public ErrorBase {
public:

	class Point4 {
	public:
		Point4() { w = x = y = z = 0.0; }
		float x, y, z, w;
		float getX() { return x; }
		float getY() { return y; }
		float getZ() { return z; }
		float getW() { return w; }
	};
	
	typedef enum {kNotTracked, kPositionOnly, kTracked} SkeletonTrackingState;
	typedef enum {kClippedRight = 1, kClippedLeft = 2, kClippedTop = 4, kClippedBottom = 8} SkeletonQuality;
	
	int getNumberOfPlayers();
	void getFloorClipPlane(Point4 &point);
	void getGravityNormal(Point4 &point);
	void getSkeleton(Skeleton &skeleton, int index = 1);
	void getPosition(Point4 &point);
	SkeletonTrackingState getTrackingState();
	UINT32 getQuality();

private:
	void updateData();
	Skeleton m_skeleton;
	int m_numberOfPlayers;
	UINT32 m_quality;
	Point4 m_floorClipPlane;
	Point4 m_gravityNormal;
	Point4 m_position;
};

#endif

