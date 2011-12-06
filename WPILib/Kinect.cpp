/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "Kinect.h"

/**
 * Get the number of tracked players on the Kinect
 * @returns the number of players being actively tracked
 */
int Kinect::getNumberOfPlayers()
{
	return m_numberOfPlayers;
}

/**
 * Get the floor clip plane as defined in the Kinect SDK
 * @param point
 */
void Kinect::getFloorClipPlane(Point4 &point)
{
	point = m_floorClipPlane;
}

/**
 * Get the gravity normal from the kinect as defined in the Kinect SDK
 * @param point The gravity normal (w is ignored)
 */
void Kinect::getGravityNormal(Point4 &point)
{
	point = m_gravityNormal;
}

/**
 * Get the skeleton data
 * Returns the detected skeleton data from the kinect as defined in the Kinect SDK
 * @param skeleton The current version of the skeleton object. This is copied to a user supplied object
 * @param index Which of (potentially 2) skeletons to return. This is ignored in this implementation and
 * only a single skeleton is supported for the FRC release default gesture interpretation.
 */
void Kinect::getSkeleton(Skeleton &skeleton, int index)
{
	skeleton = m_skeleton;
}

/**
 * Get the current position data
 * Returns the current position as defined in the Kinect SDK
 * @param point The position data (w is ignored)
 */
void Kinect::getPosition(Point4 &point)
{
	point = m_position;
}

/**
 * Get the quality data
 * @returns the quality value as defined in the Kinect SDK
 */
UINT32 Kinect::getQuality()
{
	return m_quality;
}

/**
 * Accept an update from the Driver Station of new data
 * Called by the DriverStation class when new data is available. This
 * will read the new values and update the data structures in this class.
 */
void Kinect::updateData()
{
	
}
