/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2011. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef SKELETON_H
#define SKELETON_H

class Skeleton {

public:

	class Joint {
	private:
		float x, y, z;
		unsigned char trackingState;
	public:
		typedef enum {kNotTracked, kInferred, kTracked} JointTrackingState;
		
		float getX() { return x; }
		float getY() { return y; }
		float getZ() { return z; }
		unsigned char getTrackingState() { return trackingState; }
	};

	typedef enum {
        HipCenter = 0,
        Spine = 1,
        ShoulderCenter = 2,
        Head = 3,
        ShoulderLeft = 4,
        ElbowLeft = 5,
        WristLeft = 6,
        HandLeft = 7,
        ShoulderRight = 8,
        ElbowRight = 9,
        WristRight = 10,
        HandRight = 11,
        HipLeft = 12,
        KneeLeft = 13,
        AnkleLeft = 14,
        FootLeft = 15,
        HipRight = 16,
        KneeRight = 17,
        AnkleRight = 18,
        FootRight = 19,
        Count = 20
	} JointTypes;

	void GetHandRight(Joint &value) { value = skeleton[HandRight]; }
	void GetHandLeft(Joint &value) { value = skeleton[HandLeft]; }
	void GetWristRight(Joint &value) {  value = skeleton[WristRight]; }
	void GetWristLeft(Joint &value) {  value = skeleton[WristLeft]; }
	void GetElbowLeft(Joint &value) {  value = skeleton[ElbowLeft]; }
	void GetElbowRight(Joint &value) {  value = skeleton[ElbowRight]; }
	void GetShoulderLeft(Joint &value) {  value = skeleton[ShoulderLeft]; }
	void GetShoulderRight(Joint &value) {  value = skeleton[ShoulderRight]; }
	void GetShoulderCenter(Joint &value) {  value = skeleton[ShoulderCenter]; }
	void GetHead(Joint &value) {  value = skeleton[Head]; }
	void GetSpine(Joint &value) {  value = skeleton[Spine]; }
	void GetHipCenter(Joint &value) {  value = skeleton[HipCenter]; }
	void GetHipRight(Joint &value) {  value = skeleton[HipRight]; }
	void GetHipLeft(Joint &value) {  value = skeleton[HipLeft]; }
	void GetKneeLeft(Joint &value) {  value = skeleton[KneeLeft]; }
	void GetKneeRight(Joint &value) {  value = skeleton[KneeRight]; }
	void GetAnkleLeft(Joint &value) {  value = skeleton[AnkleLeft]; }
	void GetAnkleRight(Joint &value) {  value = skeleton[AnkleRight]; }
	void GetFootLeft(Joint &value) {  value = skeleton[FootLeft]; }
	void GetFootRight(Joint &value) {  value = skeleton[FootRight]; }
	void GetValue(Joint &value, int index) { value = skeleton[index]; }
private:
	Joint skeleton[20];
};

#endif

