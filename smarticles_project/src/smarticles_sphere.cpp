//
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2010-2011 Alessandro Tasora
// Copyright (c) 2013 Project Chrono
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file at the top level of the distribution
// and at http://projectchrono.org/license-chrono.txt.
//

// A very simple example that can be used as template project for
// a Chrono::Engine simulator with 3D view.
#include <ostream>
#include <fstream>


#include "physics/ChSystem.h"
#include "physics/ChBodyEasy.h"
#include "physics/ChLinkMate.h"
#include "assets/ChTexture.h"
#include "assets/ChColorAsset.h"
#include "unit_IRRLICHT/ChIrrApp.h"
#include "core/ChFileutils.h"
#include "core/ChStream.h"
#include "core/ChRealtimeStep.h"
#include "unit_POSTPROCESS/ChPovRay.h"
#include "unit_POSTPROCESS/ChPovRayAssetCustom.h"
#include "utils/ChUtilsInputOutput.h"



#define USE_IRRLICHT/////////////////////////////////////**************************************///////////////////////////////////
// Use the namespace of Chrono

using namespace chrono;
//using namespace postprocess;

// Use the main namespaces of Irrlicht

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::io;
using namespace irr::gui;

ChSharedPtr<ChFunction> mfun0(new ChFunction_Const(-0.05));
ChSharedPtr<ChFunction> mfun1(new ChFunction_Const(-0.05));
ChSharedPtr<ChFunction> mfun2(new ChFunction_Const(0.05));

ChSharedPtr<ChFunction> mfunRamp1(new ChFunction_Ramp(-0.05, 2.0));
ChSharedPtr<ChFunction> mfunRamp2(new ChFunction_Ramp(-0.05, 5.5));

ChSharedPtr<ChFunction> mfun = mfun0;
bool changed = false;
bool out = true;
bool allowed = true;

bool left = false;
bool right = false;
bool forward = false;
bool back = false;

class MyEventReceiver : public IEventReceiver {
public:
	MyEventReceiver(ChIrrAppInterface* myapp) {
		// store pointer applicaiton
		application = myapp;
	}

	bool OnEvent(const SEvent& event) {
		// check if user presses keys
		if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown && allowed) {
			switch (event.KeyInput.Key) {
			case irr::KEY_KEY_A:
				allowed = false;
				left = true;
				right = false;
				forward = false;
				back = false;
				return true;
			case irr::KEY_KEY_D:
				allowed = false;
				left = false;
				right = true;
				forward = false;
				back = false;
				return true;
			case irr::KEY_KEY_W:
				allowed = false;
				left = false;
				right = false;
				forward = true;
				back = false;
				return true;
			case irr::KEY_KEY_S:
				allowed = false;
				left = false;
				right = false;
				forward = false;
				back = true;
				return true;
			case irr::KEY_SPACE:
				allowed = false;
				left = false;
				right = false;
				forward = false;
				back = false;
				return true;
			}
			return false;
		}
	}
private:
	ChIrrAppInterface* application;
};

/**
Will modify this to read in the arrays from a text file. For now it will just be input here
*/
std::vector<std::vector<double>> getVertices(){
	double phi = (1 + std::sqrt(5)) / 2.0;
	std::vector<std::vector<double>> vertices{
		{ 3.0*phi, 0, 1.0 }, { 3.0*phi, 0, -1.0 }, { -3.0*phi, 0, 1.0 }, { -3.0*phi, 0, -1.0 },
		{ (1.0 + 2.0*phi), phi, 2.0 }, { (1.0 + 2.0*phi), phi, -2.0 }, { (1.0 + 2.0*phi), -phi, 2.0 }, { (1.0 + 2.0*phi), -phi, -2.0 },
		{ -(1.0 + 2.0*phi), phi, 2.0 }, { -(1.0 + 2.0*phi), phi, -2.0 }, { -(1.0 + 2.0*phi), -phi, 2.0 }, { -(1.0 + 2.0*phi), -phi, -2.0 },
		{ (2.0 + phi), 2.0*phi, 1.0 }, { (2.0 + phi), 2.0*phi, -1.0 }, { (2.0 + phi), -2.0*phi, 1.0 }, { (2.0 + phi), -2.0*phi, -1.0 },
		{ -(2.0 + phi), 2.0*phi, 1.0 }, { -(2.0 + phi), 2.0*phi, -1.0 }, { -(2.0 + phi), -2.0*phi, 1.0 }, { -(2.0 + phi), -2.0*phi, -1.0 },

		{ 1.0, 3.0*phi, 0 }, { 1.0, -3.0*phi, 0 }, { -1.0, 3.0*phi, 0 }, { -1.0, -3.0*phi, 0 },
		{ 2.0, (1.0 + 2.0*phi), phi }, { 2.0, (1.0 + 2.0*phi), -phi }, { 2.0, -(1.0 + 2.0*phi), phi }, { 2.0, -(1.0 + 2.0*phi), -phi },
		{ -2.0, (1.0 + 2.0*phi), phi }, { -2.0, (1.0 + 2.0*phi), -phi }, { -2.0, -(1.0 + 2.0*phi), phi }, { -2.0, -(1.0 + 2.0*phi), -phi },
		{ 1.0, (2.0 + phi), 2.0*phi }, { 1.0, (2.0 + phi), -2.0*phi }, { 1.0, -(2.0 + phi), 2.0*phi }, { 1.0, -(2.0 + phi), -2.0*phi },
		{ -1.0, (2.0 + phi), 2.0*phi }, { -1.0, (2.0 + phi), -2.0*phi }, { -1.0, -(2.0 + phi), 2.0*phi }, { -1.0, -(2.0 + phi), -2.0*phi },

		{ 0, 1.0, 3.0*phi }, { 0, 1.0, -3.0*phi }, { 0, -1.0, 3.0*phi }, { 0, -1.0, -3.0*phi },
		{ phi, 2.0, (1 + 2.0*phi) }, { phi, 2.0, -(1 + 2.0*phi) }, { phi, -2.0, (1 + 2.0*phi) }, { phi, -2.0, -(1 + 2.0*phi) },
		{ -phi, 2.0, (1 + 2.0*phi) }, { -phi, 2.0, -(1 + 2.0*phi) }, { -phi, -2.0, (1 + 2.0*phi) }, { -phi, -2.0, -(1 + 2.0*phi) },
		{ 2.0*phi, 1.0, (2.0 + phi) }, { 2.0*phi, 1.0, -(2.0 + phi) }, { 2.0*phi, -1.0, (2.0 + phi) }, { 2.0*phi, -1.0, -(2.0 + phi) },
		{ -2.0*phi, 1.0, (2.0 + phi) }, { -2.0*phi, 1.0, -(2.0 + phi) }, { -2.0*phi, -1.0, (2.0 + phi) }, { -2.0*phi, -1.0, -(2.0 + phi) }
	};
	return vertices;
}

int main(int argc, char* argv[]) {
	//setup variables
	double sphereRadius = 0.185;
	double sphereDensity = 25.0;
	double legRadius = 0.005;
	double legLength = 0.15;
	double legDensity = 5000.0;
	//int numLegs = 5;
	bool actuator = true;
	bool receive = false;

	// Create a ChronoENGINE physical system
	ChSystem mphysicalSystem;
	ChSharedPtr<ChCamera> mcamera(new ChCamera);


	const std::string out_dir = "../VEHICLE";
	const std::string pov_dir = out_dir + "/POVRAY";

	// Create the Irrlicht visualization (open the Irrlicht device,
	// bind a simple user interface, etc. etc.)
#ifdef USE_IRRLICHT

	ChIrrApp application(&mphysicalSystem, L"Spherical Smarticle", core::dimension2d<u32>(1000, 800), false);  // screen dimensions

	// Easy shortcuts to add camera, lights, logo and sky in Irrlicht scene:
	application.AddTypicalLogo();
	application.AddTypicalSky();
	application.AddTypicalLights();

	application.AddTypicalCamera(core::vector3df(3, 1.75, -5),
		core::vector3df(2, -.3, 0));  // to change the position of camera
	// application.AddLightWithShadow(vector3df(1,25,-5), vector3df(0,0,0), 35, 0.2,35, 55, 512, video::SColorf(1,1,1));
#else
#endif

	IAnimatedMesh* tireMesh =
		application.GetSceneManager()->getMesh(GetChronoDataFile("SBEL.obj").c_str());

	ChBodySceneNode* wheel = (ChBodySceneNode*)addChBodySceneNode(application.GetSystem(), application.GetSceneManager(),
		tireMesh,  // this mesh only for visualization
		-2.0, { 0, 2, 0 });
	

	//CREATE FLOOR
	ChSharedPtr<ChBodyEasyBox> floorBody(new ChBodyEasyBox(
		20, 2, 20, 3000, true, true));
	floorBody->SetPos(ChVector<>(0, -1.5, 0));
	floorBody->SetBodyFixed(true);
	mphysicalSystem.Add(floorBody);

	//set color for floor for visualization
	ChSharedPtr<ChColorAsset> mcolor(new ChColorAsset());
	mcolor->SetColor(ChColor(0.2, 0.25, 0.25));
	floorBody->AddAsset(mcolor);

	//CREATE CENTER SPHERE
	ChSharedPtr<ChBodyEasySphere> mSphere(new ChBodyEasySphere(sphereRadius, sphereDensity, false, true));
	mSphere->SetPos(ChVector<>(0, 0, 0));
	//mSphere->SetBodyFixed(true);	//**will remove once the sphere has all its legs and ready to move**************************************************************
	mphysicalSystem.Add(mSphere);

	//add texture to the sphere for visualization
	ChSharedPtr<ChTexture> mtexture(new ChTexture());
	mtexture->SetTextureFilename(GetChronoDataFile("wood01.jpg"));  // texture in ../data
	mSphere->AddAsset(mtexture);

	double xPos = 0;
	double yPos = 0;
	double zPos = 0;

	//CREATE LEGS
	std::vector<std::vector<double>> vertices = getVertices();
	//array of linear actuators
	std::vector<ChSharedPtr<ChLinkLinActuator>> actuators;
	std::vector<ChSharedPtr<ChLinkLockPrismatic>> prisms;
	std::vector<ChSharedPtr<ChLinkDistance>> dists;
	std::vector<ChSharedPtr<ChBody>> legs;




	for (int i = 0; i < vertices.size(); i++)
	{
		xPos = 0.02*vertices[i][0];
		yPos = 0.02*vertices[i][1];
		zPos = 0.02*vertices[i][2];

		ChSharedPtr<ChBodyEasyCylinder> mleg(new ChBodyEasyCylinder(legRadius, legLength, legDensity, true, true));

		//find axis of rotation
		ChVector<double> axis{ 0.0, 0.0, 0.0 };
		ChVector<double> start{ 0.0, 1.0, 0.0 };
		double posLength = std::sqrt(xPos*xPos + yPos*yPos + zPos*zPos);

		ChVector<double> posVec{ xPos / posLength, yPos / posLength, zPos / posLength };
		axis.x = (start.y * posVec.z) - (start.z * posVec.y);
		axis.y = (start.z * posVec.x) - (start.x * posVec.z);
		axis.z = (start.x * posVec.y) - (start.y * posVec.x);
		//normalize
		double length = std::sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);

		//calculate angle
		double dist = std::sqrt((posVec.x*posVec.x) + (posVec.y - 1.0)*(posVec.y - 1.0) + (posVec.z*posVec.z));
		double ang = 2.0* std::asin(dist / (2.0));

		//normalize axis vector
		axis.x = axis.x / length;
		axis.y = axis.y / length;
		axis.z = axis.z / length;

		mleg->SetRot(Q_from_AngAxis(ang, axis));
		mleg->SetPos(ChVector<>(xPos, yPos, zPos));
		mphysicalSystem.Add(mleg);

		//add the prismatic contraint
		ChSharedPtr<ChLinkLockPrismatic> legLink = ChSharedPtr<ChLinkLockPrismatic>(new ChLinkLockPrismatic);
		legLink->Initialize(mSphere, mleg, ChCoordsys<>({ xPos, yPos, zPos }, Q_from_AngAxis(ang, axis)*Q_from_AngAxis(CH_C_PI / 2.0, { -1.0, 0, 0.0 })));
		//legLink->GetLinkAbsoluteCoords();		//just for checking coordinates
		prisms.push_back(legLink);
		mphysicalSystem.AddLink(legLink);

		if (actuator){
			//set up linear actuator
			ChSharedPtr<ChLinkLinActuator> legAct = ChSharedPtr<ChLinkLinActuator>(new ChLinkLinActuator);
			legAct->Initialize(mleg, mSphere, ChCoordsys<>({ 0, 0, 0 }, Q_from_AngAxis(ang, axis)*Q_from_AngAxis(CH_C_PI / 2.0, { 1.0, 0, 0 })));
			legAct->Set_dist_funct(mfun0);
			actuators.push_back(legAct);
			mphysicalSystem.AddLink(legAct);
		}
		else{
			ChSharedPtr<ChLinkDistance> legDis = ChSharedPtr<ChLinkDistance>(new ChLinkDistance);
			legDis->Initialize(mleg, mSphere, false, { xPos, yPos, zPos }, { 0, 0, 0 }, false, 0.125);
			dists.push_back(legDis);
			mphysicalSystem.AddLink(legDis);
		}

		//add a textrue to the legs
		ChSharedPtr<ChTexture> mlegTexture(new ChTexture());
		mlegTexture->SetTextureFilename(GetChronoDataFile("redwhite.png"));
		mleg->AddAsset(mlegTexture);

		legs.push_back(mleg);
	}

	/****** CREATE SOME OBSTACLES *****/

	if (actuator){

		ChSharedPtr<ChBodyEasyBox> mob(new ChBodyEasyBox(
			.2, 2, 5, 3000, true, true));
		mob->SetPos(ChVector<>(3.00, 0, 0));
		mob->SetBodyFixed(true);
		mphysicalSystem.Add(mob);

		ChSharedPtr<ChTexture> mobTexture(new ChTexture());
		mobTexture->SetTextureFilename(GetChronoDataFile("cubetexture_bluwhite.png"));
		mob->AddAsset(mobTexture);

		ChSharedPtr<ChBodyEasyBox> mob1(new ChBodyEasyBox(
			5, .2, 5, 3000, true, true));
		mob1->SetPos(ChVector<>(4.00, -.5, 0));
		mob1->SetRot(Q_from_AngAxis(CH_C_PI / 12, { 0, 0, 1.0 }));
		mob1->SetBodyFixed(true);
		mphysicalSystem.Add(mob1);

		ChSharedPtr<ChTexture> mob1Texture(new ChTexture());
		mob1Texture->SetTextureFilename(GetChronoDataFile("cubetexture_bluwhite.png"));
		mob1->AddAsset(mob1Texture);

		ChSharedPtr<ChBodyEasyBox> mob2(new ChBodyEasyBox(
			3.0, 0.5, 0.1, 3000, true, true));
		mob2->SetPos(ChVector<>(4.00, -.250, -1.1));
		mob2->SetBodyFixed(true);
		mphysicalSystem.Add(mob2);

		ChSharedPtr<ChTexture> mobTexture2(new ChTexture());
		mobTexture2->SetTextureFilename(GetChronoDataFile("cubetexture_bluwhite.png"));
		mob2->AddAsset(mobTexture2);

		ChSharedPtr<ChBodyEasyBox> mob3(new ChBodyEasyBox(
			3.0, 0.5, 0.1, 3000, true, true));
		mob3->SetPos(ChVector<>(4.00, -.250, 1.1));
		mob3->SetBodyFixed(true);
		mphysicalSystem.Add(mob3);

		ChSharedPtr<ChTexture> mobTexture3(new ChTexture());
		mobTexture3->SetTextureFilename(GetChronoDataFile("cubetexture_bluwhite.png"));
		mob3->AddAsset(mobTexture3);


	}
	/*
	//create some rolling spheres
	int numSpheres = 100;
	for (int i = 0; i < numSpheres; i++){
	ChSharedPtr<ChBodyEasySphere> mSphs(new ChBodyEasySphere(0.03, 800000.0, true, true));
	double x1 = rand() % 100;
	double y1 = rand() % 100;
	double z1 = rand() % 100;
	mSphs->SetPos(ChVector<>(1.25 + x1/100.0, .25 + y1/100.0, -1.0 + z1/50.0));
	//mSphere->SetBodyFixed(true);	//**will remove once the sphere has all its legs and ready to move**************************************************************
	mphysicalSystem.Add(mSphs);

	//add texture to the sphere for visualization
	ChSharedPtr<ChTexture> mtextureS(new ChTexture());
	mtextureS->SetTextureFilename(GetChronoDataFile("bluwhite.png"));  // texture in ../data
	mSphs->AddAsset(mtextureS);


	}
	*/


	/*
	//add a slanted obstacle
	ChSharedPtr<ChBodyEasyBox> mob(new ChBodyEasyBox(
	5, .5, 5, 3000, true, true));  // to create the floor, false -> doesn't represent a collide's surface
	mob->SetPos(ChVector<>(-1.00, -.5, 0));
	mob->SetRot(Q_from_AngAxis(-CH_C_PI / 12, { 0, 0, 1.0 }));
	mob->SetBodyFixed(true);
	mphysicalSystem.Add(mob);

	ChSharedPtr<ChTexture> mobTexture(new ChTexture());
	mobTexture->SetTextureFilename(GetChronoDataFile("cubetexture_bluwhite.png"));
	mob->AddAsset(mobTexture);

	//add a slanted obstacle
	ChSharedPtr<ChBodyEasyBox> mob1(new ChBodyEasyBox(
	.5, .5, .5, 3000, true, true));  // to create the floor, false -> doesn't represent a collide's surface
	mob1->SetPos(ChVector<>(-.2, -.625, 0));
	mob1->SetBodyFixed(true);
	mphysicalSystem.Add(mob1);

	ChSharedPtr<ChTexture> mobTexture1(new ChTexture());
	mobTexture1->SetTextureFilename(GetChronoDataFile("cubetexture_bluwhite.png"));
	mob1->AddAsset(mobTexture1);

	//add a slanted obstacle
	ChSharedPtr<ChBodyEasyBox> mob2(new ChBodyEasyBox(
	.1, .5, 5, 3000, true, true));  // to create the floor, false -> doesn't represent a collide's surface
	mob2->SetPos(ChVector<>(.5, -.7, 0));
	mob2->SetBodyFixed(true);
	mphysicalSystem.Add(mob2);

	ChSharedPtr<ChTexture> mobTexture2(new ChTexture());
	mobTexture2->SetTextureFilename(GetChronoDataFile("cubetexture_bluwhite.png"));
	mob2->AddAsset(mobTexture2);

	//add a slanted obstacle
	ChSharedPtr<ChBodyEasyBox> mob3(new ChBodyEasyBox(
	.2, .5, 1.0, 3000, true, true));  // to create the floor, false -> doesn't represent a collide's surface
	mob3->SetPos(ChVector<>(1.00, -.65, 0));
	mob3->SetRot(Q_from_AngAxis(-CH_C_PI / 12, { 0, 1.0, 0 }));
	mob3->SetBodyFixed(true);
	mphysicalSystem.Add(mob3);

	ChSharedPtr<ChTexture> mobTexture3(new ChTexture());
	mobTexture3->SetTextureFilename(GetChronoDataFile("cubetexture_bluwhite.png"));
	mob3->AddAsset(mobTexture3);
	*/



	//======================================================================

	// Use this function for adding a ChIrrNodeAsset to all items
	// Otherwise use application.AssetBind(myitem); on a per-item basis.



	// Adjust some settings:
	double timestep = 0.002;
	double render_step_size = 1.0 / 200;   // FPS = 50
	double output_step_size = 1.0 / 1;    // once a second

	mphysicalSystem.SetIterLCPmaxItersSpeed(200);
	mphysicalSystem.SetIterLCPmaxItersStab(100);
	//mphysicalSystem.SetLcpSolverType(ChSystem::LCP_ITERATIVE_BARZILAIBORWEIN);

#ifdef USE_IRRLICHT

	application.AssetBindAll();

	// Use this function for 'converting' assets into Irrlicht meshes
	application.AssetUpdateAll();

	//if (receive){
	//MyEventReceiver receiver(&application);
	// note how to add the custom event receiver to the default interface:
	//application.SetUserEventReceiver(&receiver);
	//}
	application.SetTimestep(timestep);
	//mphysicalSystem.SetLcpSolverType(ChSystem::LCP_ITERATIVE_BARZILAIBORWEIN);


	//application.SetTryRealtime(true);
	//
	// THE SOFT-REAL-TIME CYCLE
	//
	long tim = 0;
	bool test = true;
	double differ = 0;

	double tend = 20.0;	//simulation length
	int step_number = 0;
	double time = 0;
	int render_steps = (int)std::ceil(render_step_size / timestep);
	int render_frame = 0;

	if (ChFileutils::MakeDirectory(out_dir.c_str()) < 0) {
		std::cout << "Error creating directory " << out_dir << std::endl;
		return 1;
	}
	if (ChFileutils::MakeDirectory(pov_dir.c_str()) < 0) {
		std::cout << "Error creating directory " << pov_dir << std::endl;
		return 1;
	}

	char filename[100];

	while (true){//application.GetDevice()->run()) {
		//application.AddTypicalCamera(core::vector3df(mSphere->GetPos().x - 3.0, mSphere->GetPos().y + 3.25, mSphere->GetPos().z),
		//	core::vector3df(0, 0, 0));
		application.BeginScene();
		application.DrawAll();
		// This performs the integration timestep!
		application.DoStep();

		//trying to make the camera follow the sphere but it does not work properly yet
		mcamera->SetPosition(ChVector<>(mSphere->GetPos().x, mSphere->GetPos().y, mSphere->GetPos().z));
		mcamera->SetAimPoint(ChVector<>(0, 0, 0));

		//application.AddTypicalCamera(core::vector3df(mSphere->GetPos().x - 3.0, mSphere->GetPos().y + 0.25, mSphere->GetPos().z),
		//	core::vector3df(mSphere->GetPos().x, mSphere->GetPos().y, mSphere->GetPos().z));

		//mphysicalSystem.DoStepDynamics(timestep);

		//Get velocity
		if ((tim % (int)((1 / timestep) / 5) == 0)){
			printf("Position: \t %f, %f, %f\n", mSphere->GetPos().x, mSphere->GetPos().y, mSphere->GetPos().z);
			printf("Velocity:\t %f, %f, %f\n", mSphere->GetPos_dt().x, mSphere->GetPos_dt().y, mSphere->GetPos_dt().z);
			printf("Acceleration:\t %f, %f, %f\n", mSphere->GetPos_dtdt().x, mSphere->GetPos_dtdt().y, mSphere->GetPos_dtdt().z);
			printf("Tim is: %ld\n", tim);

			//printf("forward: %d\n", forward);
			//printf("back:\t %d\n", back);
			//printf("left:\t %d\n", left);
			//printf("right:\t %d\n", right);
		}
		//if (tim < 3800 && tim > 3000 && tim % 20 == 0){
		//	printf("differ: %f\n", differ);
		//	//printf("")
		//}

		if (step_number % render_steps == 0) {

			// Output render data
			sprintf(filename, "%s/data_%03d.dat", pov_dir.c_str(), render_frame + 1);
			utils::WriteShapesPovray(&mphysicalSystem, filename);
			std::cout << "Output frame:   " << render_frame << std::endl;
			std::cout << "Sim frame:      " << step_number << std::endl;
			std::cout << "Time:           " << time << std::endl;
			std::cout << std::endl;
			render_frame++;
		}

		if (!actuator){

			if (left){
				for (int k = 0; k < prisms.size(); k++){
					if (mSphere->GetPos_dt().z > 1.0){
						differ = 0.1;
					}
					else if (mSphere->GetPos_dt().z > 0){
						differ = 0.05;
					}
					else if (mSphere->GetPos_dt().z < -2.5){
						differ = -0.05;
					}
					else if (mSphere->GetPos_dt().z < -1.8){
						differ = 0.00;
					}
					else if (mSphere->GetPos_dt().z < -0.75){
						differ = 0.01;
					}
					else{
						differ = 0.02;
					}
					if (mSphere->GetPos().z - prisms[k]->GetLinkAbsoluteCoords().pos.z < -differ){
						dists[k]->SetImposedDistance(0.25);
					}
					else if (mSphere->GetPos().z - prisms[k]->GetLinkAbsoluteCoords().pos.z < -differ / 2.0){
						dists[k]->SetImposedDistance(0.1875);
					}
					else{
						dists[k]->SetImposedDistance(0.125);
					}
				}

			}
			else if (right){
				for (int k = 0; k < prisms.size(); k++){
					if (mSphere->GetPos_dt().z < -1.0){
						differ = 0.1;
					}
					else if (mSphere->GetPos_dt().z < 0){
						differ = 0.05;
					}
					else if (mSphere->GetPos_dt().z > 2.5){
						differ = -0.05;
					}
					else if (mSphere->GetPos_dt().z > 1.8){
						differ = 0.00;
					}
					else if (mSphere->GetPos_dt().z > 0.75){
						differ = 0.01;
					}
					else{
						differ = 0.02;
					}
					if (mSphere->GetPos().z - prisms[k]->GetLinkAbsoluteCoords().pos.z > differ){
						dists[k]->SetImposedDistance(0.25);
					}
					else if (mSphere->GetPos().z - prisms[k]->GetLinkAbsoluteCoords().pos.z > differ / 2.0){
						dists[k]->SetImposedDistance(0.1875);
					}
					else{
						dists[k]->SetImposedDistance(0.125);
					}
				}
			}
			else if (forward){
				for (int k = 0; k < prisms.size(); k++){
					if (mSphere->GetPos_dt().x > 1.0){
						differ = 0.1;
					}
					else if (mSphere->GetPos_dt().x > 0){
						differ = 0.05;
					}
					else if (mSphere->GetPos_dt().x < -2.5){
						differ = -0.05;
					}
					else if (mSphere->GetPos_dt().x < -1.8){
						differ = 0.00;
					}
					else if (mSphere->GetPos_dt().x < -0.75){
						differ = 0.01;
					}
					else{
						differ = 0.02;
					}
					if (mSphere->GetPos().x - prisms[k]->GetLinkAbsoluteCoords().pos.x < -differ){
						dists[k]->SetImposedDistance(0.25);
					}
					else if (mSphere->GetPos().x - prisms[k]->GetLinkAbsoluteCoords().pos.x < -differ / 2.0){
						dists[k]->SetImposedDistance(0.1875);
					}
					else{
						dists[k]->SetImposedDistance(0.125);
					}
				}
			}
			else if (back){
				for (int k = 0; k < prisms.size(); k++){
					if (mSphere->GetPos_dt().x < -1.0){
						differ = 0.1;
					}
					else if (mSphere->GetPos_dt().x < 0){
						differ = 0.05;
					}
					else if (mSphere->GetPos_dt().x > 2.5){
						differ = -0.05;
					}
					else if (mSphere->GetPos_dt().x > 1.8){
						differ = 0.00;
					}
					else if (mSphere->GetPos_dt().x > 0.75){
						differ = 0.01;
					}
					else{
						differ = 0.02;
					}
					if (mSphere->GetPos().x - prisms[k]->GetLinkAbsoluteCoords().pos.x > differ){
						dists[k]->SetImposedDistance(0.25);
					}
					else if (mSphere->GetPos().x - prisms[k]->GetLinkAbsoluteCoords().pos.x > differ / 2.0){
						dists[k]->SetImposedDistance(0.1875);
					}
					else{
						dists[k]->SetImposedDistance(0.125);
					}
				}
			}
			else{
				for (int k = 0; k < prisms.size(); k++){
					dists[k]->SetImposedDistance(0.25);
				}
			}
		}
		//run a demo course
		if (actuator){
			if (tim < (int)((1.0 / timestep) *1.2)){
				for (int k = 0; k < prisms.size(); k++){
					if (mSphere->GetPos().x - prisms[k]->GetLinkAbsoluteCoords().pos.x > 0.02){
						actuators[k]->Set_dist_funct(mfun2);
					}
					else{
						actuators[k]->Set_dist_funct(mfun0);
					}
				}
			}
			else if (tim == (int)((1.0 / timestep)*1.6)){

				for (int k = 0; k < prisms.size(); k++){
					if (mSphere->GetPos().x - prisms[k]->GetLinkAbsoluteCoords().pos.x > 0.01){
						actuators[k]->Set_dist_funct(mfunRamp2);
					}
				}
			}
			else if (tim == (int)((1.0 / timestep)*1.6) + (int)((1 / timestep) / (int)(1.0 / timestep / 5.0))){
				for (int k = 0; k < prisms.size(); k++){
					actuators[k]->Set_dist_funct(mfun0);
				}
			}
			else if (tim >(int)((1.0 / timestep) * 3.5)){
				for (int k = 0; k < prisms.size(); k++){
					if (mSphere->GetPos().x - prisms[k]->GetLinkAbsoluteCoords().pos.x > 0.02){
						actuators[k]->Set_dist_funct(mfun2);
					}
					else{
						actuators[k]->Set_dist_funct(mfun0);
					}
				}
			}
			else if (tim < (int)((1.0 / timestep) *1.6)){
				for (int k = 0; k < prisms.size(); k++){
					actuators[k]->Set_dist_funct(mfun0);
				}
			}
		}

		if (!receive){

			if (tim < 1000) forward = true;
			else if (tim < (int)((1 / timestep)) * 4) {
				back = true;
				forward = false;
			}
			else if (tim < (int)((1 / timestep)) * 10) {
				forward = true;
				back = false;
			}
			else if (tim < (int)((1 / timestep)) * 14){
				back = true;
				forward = false;
			}
			else if (tim < (int)((1 / timestep)) * 18){
				forward = true;
				back = false;
			}
			else if (tim < (int)((1 / timestep)) * 22){
				back = true;
				forward = false;
			}
			else if (tim < (int)((1 / timestep)) * 26){
				forward = true;
				back = false;
			}
			else{
				forward = false;
			}
		}

		application.EndScene();
		allowed = true;
		tim++;
		step_number++;
		time += timestep;
	}
	return 0;
}

#else
	// Output directories (Povray only)


	while (time < tend)
	{



		mphysicalSystem.DoStepDynamics(timestep);



		//printf("number of legs: %d, %d\n", vertices.size(), prisms.size());
		tim++;

	}
	return 0;
}
#endif