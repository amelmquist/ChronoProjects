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

#include "physics/ChSystem.h"
#include "physics/ChBodyEasy.h"
#include "physics/ChLinkMate.h"
#include "assets/ChTexture.h"
#include "assets/ChColorAsset.h"
#include "unit_IRRLICHT/ChIrrApp.h"

// Use the namespace of Chrono

using namespace chrono;

// Use the main namespaces of Irrlicht

using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::io;
using namespace irr::gui;


///////////////////////////setup variables////////////////////////////////////

int iterSpeed = 1000;
double timestep = .0001;

//////////////////////////Robot Variables////////////////////////////////////
double wheelRadius = 0.175;
double wheelWidth = 0.175;
double wheelDensity = 0.1;
double wheelFriction = 0.1;

double bodyWidth = 0.75;
double tubeWidth = .03;
double rearLegLength = 0.2;
double rearBeamLength = 0.5;
double frontBeamLength = 0.5;
double frontArmLength = 0.3;
double middleLegLength = 0.3;
double frontLegLength = 0.2;
double tubeDensity = 1000;

double rearAngVert = CH_C_PI / 2.5;
double frontBeamAngVert = CH_C_PI / 1.8;

bool fixed = false;




int main(int argc, char* argv[]) {


	/////////////////////////Setup of the System/////////////////////////////////

    // Create a ChronoENGINE physical system
    ChSystem mphysicalSystem;
	//Create the Irrlicht visualization system
    ChIrrApp application(&mphysicalSystem, L"Rocker Bogie", core::dimension2d<u32>(1300, 900), false);  // screen dimensions
    application.AddTypicalLogo();
    application.AddTypicalSky();
    application.AddTypicalLights();
    application.AddTypicalCamera(core::vector3df(2, 2, -2), core::vector3df(0, 0.5, 0));  // to change the position of camera
   
	//////////////////////////////Create the Robot//////////////////////////////////(Just one side)/////////////
	//create the wheels
	ChSharedPtr<ChBodyEasyCylinder> rearWheel(new ChBodyEasyCylinder(0.1, .15, 100, true, true));
	rearWheel->SetPos({0.2, 0.5, 0.0});
	rearWheel->SetRot(Q_from_AngAxis(CH_C_PI / 2.0, { 0, 0, 1.0 }));
	//rearWheel->SetBodyFixed(fixed);
	mphysicalSystem.Add(rearWheel);

	ChSharedPtr<ChBodyEasyCylinder> middleWheel(new ChBodyEasyCylinder(0.1, .15, 100, true, true));
	middleWheel->SetPos({ 0.2, 0.5, 0.5 });
	middleWheel->SetRot(Q_from_AngAxis(CH_C_PI / 2.0, { 0, 0, 1.0 }));
	//middleWheel->SetBodyFixed(fixed);
	mphysicalSystem.Add(middleWheel);

	ChSharedPtr<ChBodyEasyCylinder> frontWheel(new ChBodyEasyCylinder(0.1, .15, 100, true, true));
	frontWheel->SetPos({ 0.2, 0.5, 1.0 });
	frontWheel->SetRot(Q_from_AngAxis(CH_C_PI / 2.0, { 0, 0, 1.0 }));
	//frontWheel->SetBodyFixed(fixed);
	mphysicalSystem.Add(frontWheel);

	//rear leg
	ChSharedPtr<ChBodyEasyBox> rearLeg(new ChBodyEasyBox(0.02, 0.2, 0.02, tubeDensity, false, true));
	rearLeg->SetPos({ 0.2, 0.6, 0 });
	//rearLeg->SetBodyFixed(fixed);
	mphysicalSystem.Add(rearLeg);

	//rear beam
	ChSharedPtr<ChBody> rearBeam(new ChBody());
	rearBeam->SetDensity(2000.0);
	rearBeam->SetMass(0.5);

	ChSharedPtr<ChBoxShape> vshape1(new ChBoxShape());
	vshape1->GetBoxGeometry().SetLengths({ 0.02, 0.02, 0.548 });
	vshape1->GetBoxGeometry().Pos = { 0.1, 0.75, 0.25 };
	vshape1->GetBoxGeometry().Rot = Q_from_AngAxis(-.197, { 1.0, 0, 0 })*Q_from_AngAxis(-.381, {0, 1.0, 0 });
	rearBeam->AddAsset(vshape1);

	ChSharedPtr<ChBoxShape> vshape2(new ChBoxShape());
	vshape2->GetBoxGeometry().SetLengths({ 0.02, 0.02, 0.269 });
	vshape2->GetBoxGeometry().Pos = { 0, 0.75, 0.625 };
	vshape2->GetBoxGeometry().Rot = Q_from_AngAxis(.381, { 1.0, 0, 0 });
	rearBeam->AddAsset(vshape2);

	rearBeam->SetBodyFixed(fixed);
	mphysicalSystem.Add(rearBeam);

	//front beam
	ChSharedPtr<ChBody> frontBeam(new ChBody());
	frontBeam->SetDensity(2000.0);
	frontBeam->SetMass(0.5);

	ChSharedPtr<ChBoxShape> vshape3(new ChBoxShape());
	vshape3->GetBoxGeometry().SetLengths({ 0.02, 0.02, 0.320 });
	vshape3->GetBoxGeometry().Pos = { 0.05, 0.6, 0.625 };
	vshape3->GetBoxGeometry().Rot = Q_from_AngAxis(-.675, { 1.0, 0, 0 });
	frontBeam->AddAsset(vshape3);

	ChSharedPtr<ChBoxShape> vshape4(new ChBoxShape());
	vshape4->GetBoxGeometry().SetLengths({ 0.02, 0.02, 0.292 });
	vshape4->GetBoxGeometry().Pos = { 0.125, 0.7, 0.875 };
	vshape4->GetBoxGeometry().Rot = Q_from_AngAxis(.540, { 0, 1.0, 0 });
	frontBeam->AddAsset(vshape4);

	frontBeam->SetBodyFixed(fixed);
	mphysicalSystem.Add(frontBeam);

	//front leg
	ChSharedPtr<ChBodyEasyBox> frontLeg(new ChBodyEasyBox(0.02, 0.2, 0.02, tubeDensity, false, true));
	frontLeg->SetPos({ 0.2, 0.6, 1.0 });
	frontLeg->SetBodyFixed(fixed);
	mphysicalSystem.Add(frontLeg);








	/////////////////////////Left Side//////////////////////////////////////////////
	//create the wheels
	ChSharedPtr<ChBodyEasyCylinder> rearWheelL(new ChBodyEasyCylinder(0.1, .15, 100, true, true));
	rearWheelL->SetPos({ -.7, 0.5, 0.0 });
	rearWheelL->SetRot(Q_from_AngAxis(CH_C_PI / 2.0, { 0, 0, 1.0 }));
	rearWheelL->SetBodyFixed(fixed);
	mphysicalSystem.Add(rearWheelL);

	ChSharedPtr<ChBodyEasyCylinder> middleWheelL(new ChBodyEasyCylinder(0.1, .15, 100, true, true));
	middleWheelL->SetPos({ -.7, 0.5, 0.5 });
	middleWheelL->SetRot(Q_from_AngAxis(CH_C_PI / 2.0, { 0, 0, 1.0 }));
	middleWheelL->SetBodyFixed(fixed);
	mphysicalSystem.Add(middleWheelL);

	ChSharedPtr<ChBodyEasyCylinder> frontWheelL(new ChBodyEasyCylinder(0.1, .15, 100, true, true));
	frontWheelL->SetPos({ -.7, 0.5, 1.0 });
	frontWheelL->SetRot(Q_from_AngAxis(CH_C_PI / 2.0, { 0, 0, 1.0 }));
	frontWheelL->SetBodyFixed(fixed);
	mphysicalSystem.Add(frontWheelL);

	//rear leg
	ChSharedPtr<ChBodyEasyBox> rearLegL(new ChBodyEasyBox(0.02, 0.2, 0.02, tubeDensity, false, true));
	rearLegL->SetPos({ -.7, 0.6, 0 });
	rearLegL->SetBodyFixed(fixed);
	mphysicalSystem.Add(rearLegL);

	//rear beam
	ChSharedPtr<ChBody> rearBeamL(new ChBody());
	rearBeamL->SetDensity(2000.0);
	rearBeamL->SetMass(0.5);

	ChSharedPtr<ChBoxShape> vshape1L(new ChBoxShape());
	vshape1L->GetBoxGeometry().SetLengths({ 0.02, 0.02, 0.548 });
	vshape1L->GetBoxGeometry().Pos = { -.6, 0.75, 0.25 };
	vshape1L->GetBoxGeometry().Rot = Q_from_AngAxis(-.197, { 1.0, 0, 0 })*Q_from_AngAxis(.381, { 0, 1.0, 0 });
	rearBeamL->AddAsset(vshape1L);

	ChSharedPtr<ChBoxShape> vshape2L(new ChBoxShape());
	vshape2L->GetBoxGeometry().SetLengths({ 0.02, 0.02, 0.269 });
	vshape2L->GetBoxGeometry().Pos = { -.5, 0.75, 0.625 };
	vshape2L->GetBoxGeometry().Rot = Q_from_AngAxis(.381, { 1.0, 0, 0 });
	rearBeamL->AddAsset(vshape2L);

	rearBeamL->SetBodyFixed(fixed);
	mphysicalSystem.Add(rearBeamL);

	//front beam
	ChSharedPtr<ChBody> frontBeamL(new ChBody());
	frontBeamL->SetDensity(2000.0);
	frontBeamL->SetMass(0.5);

	ChSharedPtr<ChBoxShape> vshape3L(new ChBoxShape());
	vshape3L->GetBoxGeometry().SetLengths({ 0.02, 0.02, 0.320 });
	vshape3L->GetBoxGeometry().Pos = { -.55, 0.6, 0.625 };
	vshape3L->GetBoxGeometry().Rot = Q_from_AngAxis(-.675, { 1.0, 0, 0 });
	frontBeam->AddAsset(vshape3L);

	ChSharedPtr<ChBoxShape> vshape4L(new ChBoxShape());
	vshape4L->GetBoxGeometry().SetLengths({ 0.02, 0.02, 0.292 });
	vshape4L->GetBoxGeometry().Pos = { -.625, 0.7, 0.875 };
	vshape4L->GetBoxGeometry().Rot = Q_from_AngAxis(-.540, { 0, 1.0, 0 });
	frontBeam->AddAsset(vshape4L);

	frontBeam->SetBodyFixed(fixed);
	mphysicalSystem.Add(frontBeamL);

	//front leg
	ChSharedPtr<ChBodyEasyBox> frontLegL(new ChBodyEasyBox(0.02, 0.2, 0.02, tubeDensity, false, true));
	frontLegL->SetPos({ -.7, 0.6, 1.0 });
	frontLegL->SetBodyFixed(fixed);
	mphysicalSystem.Add(frontLegL);


	/////////////////////////////////////center body//////////////////////////////
	ChSharedPtr<ChBodyEasyCylinder> centerRod(new ChBodyEasyCylinder(0.01, 0.5, tubeDensity, false, true));
	centerRod->SetPos({ -0.25, 0.8, 0.5 });
	centerRod->SetRot(Q_from_AngAxis(CH_C_PI/2.0, { 0, 0, 1.0 }));
	centerRod->SetBodyFixed(fixed);
	mphysicalSystem.Add(centerRod);





	////////////////////////////setup links//////////////////////////////////////
	ChSharedPtr<ChLinkLockRevolute> link1(new ChLinkLockRevolute());
	link1->Initialize(rearWheel, rearLeg, ChCoordsys<>({ 0.2, 0.5, 0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	mphysicalSystem.AddLink(link1);

	ChSharedPtr<ChLinkLockRevolute> link2(new ChLinkLockRevolute());
	link2->Initialize(middleWheel, frontBeam, ChCoordsys<>({ 0.2, 0.5, 0.5 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	mphysicalSystem.AddLink(link2);

	ChSharedPtr<ChLinkLockRevolute> link3(new ChLinkLockRevolute());
	link3->Initialize(frontWheel, frontLeg, ChCoordsys<>({ 0.2, 0.5, 1.0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	mphysicalSystem.AddLink(link3);

	ChSharedPtr<ChLinkLockRevolute> link4(new ChLinkLockRevolute());
	link4->Initialize(rearLeg, rearBeam, ChCoordsys<>({ 0.2, 0.7, 0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 1.0, 0, 0 })));
	mphysicalSystem.AddLink(link4);

	ChSharedPtr<ChLinkLockRevolute> link5(new ChLinkLockRevolute());
	link5->Initialize(rearBeam, frontBeam, ChCoordsys<>({ 0.025, 0.7, .75 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	mphysicalSystem.AddLink(link5);

	ChSharedPtr<ChLinkLockRevolute> link6(new ChLinkLockRevolute());
	link6->Initialize(frontLeg, frontBeam, ChCoordsys<>({ 0.2, 0.7, 1.0 }, Q_from_AngAxis(CH_C_PI/2.0, { 1.0, 0, 0 })));
	mphysicalSystem.AddLink(link6);



	ChSharedPtr<ChLinkLockRevolute> link7(new ChLinkLockRevolute());
	link7->Initialize(rearWheelL, rearLegL, ChCoordsys<>({ -.7, 0.5, 0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	mphysicalSystem.AddLink(link7);

	ChSharedPtr<ChLinkLockRevolute> link8(new ChLinkLockRevolute());
	link8->Initialize(middleWheelL, frontBeamL, ChCoordsys<>({ -.7, 0.5, 0.5 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	mphysicalSystem.AddLink(link8);

	ChSharedPtr<ChLinkLockRevolute> link9(new ChLinkLockRevolute());
	link9->Initialize(frontWheelL, frontLegL, ChCoordsys<>({ -.7, 0.5, 1.0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	mphysicalSystem.AddLink(link9);

	ChSharedPtr<ChLinkLockRevolute> link10(new ChLinkLockRevolute());
	link10->Initialize(rearLegL, rearBeamL, ChCoordsys<>({ -.7, 0.7, 0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 1.0, 0, 0 })));
	mphysicalSystem.AddLink(link10);

	ChSharedPtr<ChLinkLockRevolute> link11(new ChLinkLockRevolute());
	link11->Initialize(rearBeamL, frontBeamL, ChCoordsys<>({ -.525, 0.7, .75 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	mphysicalSystem.AddLink(link11);

	ChSharedPtr<ChLinkLockRevolute> link12(new ChLinkLockRevolute());
	link12->Initialize(frontLegL, frontBeamL, ChCoordsys<>({ -.7, 0.7, 1.0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 1.0, 0, 0 })));
	mphysicalSystem.AddLink(link12);


	ChSharedPtr<ChLinkLockRevolute> link13(new ChLinkLockRevolute());
	link13->Initialize(rearBeam, centerRod, ChCoordsys<>({ 0, 0.8, 0.5 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	mphysicalSystem.AddLink(link13);

	ChSharedPtr<ChLinkLockRevolute> link14(new ChLinkLockRevolute());
	link14->Initialize(rearBeamL, centerRod, ChCoordsys<>({ -.5, 0.8, 0.5 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	mphysicalSystem.AddLink(link14);

	
	///////////////////////////////////Motors/////////////////////////////////////
	ChSharedPtr<ChLinkEngine> motor1 (new ChLinkEngine);
	motor1->Initialize(rearWheel, rearLeg,
		ChCoordsys<>({ 0.2, 0.5, 0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	motor1->Set_eng_mode(ChLinkEngine::ENG_MODE_SPEED);
	if (ChSharedPtr<ChFunction_Const> mfun1 = motor1->Get_spe_funct().DynamicCastTo<ChFunction_Const>())
		mfun1->Set_yconst(0);  // speed w=90°/s
	mphysicalSystem.AddLink(motor1);
	
	ChSharedPtr<ChLinkEngine> motor2(new ChLinkEngine);
	motor2->Initialize(middleWheel, frontBeam,
		ChCoordsys<>({ 0.2, 0.5, 0.5 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	motor1->Set_eng_mode(ChLinkEngine::ENG_MODE_SPEED);
	if (ChSharedPtr<ChFunction_Const> mfun2 = motor2->Get_spe_funct().DynamicCastTo<ChFunction_Const>())
		mfun2->Set_yconst(0);  // speed w=90°/s
	mphysicalSystem.AddLink(motor2);

	ChSharedPtr<ChLinkEngine> motor3(new ChLinkEngine);
	motor3->Initialize(frontWheel, frontLeg,
		ChCoordsys<>({ 0.2, 0.5, 1.0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	motor3->Set_eng_mode(ChLinkEngine::ENG_MODE_SPEED);
	if (ChSharedPtr<ChFunction_Const> mfun3 = motor3->Get_spe_funct().DynamicCastTo<ChFunction_Const>())
		mfun3->Set_yconst(0);  // speed w=90°/s
	mphysicalSystem.AddLink(motor3);

	ChSharedPtr<ChLinkEngine> motor4(new ChLinkEngine);
	motor4->Initialize(rearWheelL, rearLegL,
		ChCoordsys<>({ -.7, 0.5, 0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	motor4->Set_eng_mode(ChLinkEngine::ENG_MODE_SPEED);
	if (ChSharedPtr<ChFunction_Const> mfun4 = motor4->Get_spe_funct().DynamicCastTo<ChFunction_Const>())
		mfun4->Set_yconst(0);  // speed w=90°/s
	mphysicalSystem.AddLink(motor4);

	ChSharedPtr<ChLinkEngine> motor5(new ChLinkEngine);
	motor5->Initialize(middleWheelL, frontBeamL,
		ChCoordsys<>({ -.7, 0.5, 0.5 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	motor5->Set_eng_mode(ChLinkEngine::ENG_MODE_SPEED);
	if (ChSharedPtr<ChFunction_Const> mfun5 = motor5->Get_spe_funct().DynamicCastTo<ChFunction_Const>())
		mfun5->Set_yconst(0);  // speed w=90°/s
	mphysicalSystem.AddLink(motor5);

	ChSharedPtr<ChLinkEngine> motor6(new ChLinkEngine);
	motor6->Initialize(frontWheelL, frontLegL,
		ChCoordsys<>({ -.7, 0.5, 1.0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 0, 1.0, 0 })));
	motor6->Set_eng_mode(ChLinkEngine::ENG_MODE_SPEED);
	if (ChSharedPtr<ChFunction_Const> mfun6 = motor1->Get_spe_funct().DynamicCastTo<ChFunction_Const>())
		mfun6->Set_yconst(0);  // speed w=90°/s
	mphysicalSystem.AddLink(motor6);
	
	ChSharedPtr<ChLinkEngine> motor7(new ChLinkEngine);
	motor7->Initialize(rearLeg, rearBeam,
		ChCoordsys<>({ 0.2, 0.7, 0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 1.0, 0, 0 })));
	motor7->Set_eng_mode(ChLinkEngine::ENG_MODE_SPEED);
	if (ChSharedPtr<ChFunction_Const> mfun7 = motor7->Get_spe_funct().DynamicCastTo<ChFunction_Const>())
		mfun7->Set_yconst(0);  // speed w=90°/s
	mphysicalSystem.AddLink(motor7);

	ChSharedPtr<ChLinkEngine> motor8(new ChLinkEngine);
	motor8->Initialize(frontLeg, frontBeam,
		ChCoordsys<>({ 0.2, 0.7, 1.0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 1.0, 0, 0 })));
	motor8->Set_eng_mode(ChLinkEngine::ENG_MODE_SPEED);
	if (ChSharedPtr<ChFunction_Const> mfun8 = motor8->Get_spe_funct().DynamicCastTo<ChFunction_Const>())
		mfun8->Set_yconst(0);  // speed w=90°/s
	mphysicalSystem.AddLink(motor8);

	ChSharedPtr<ChLinkEngine> motor9(new ChLinkEngine);
	motor9->Initialize(rearLegL, rearBeamL,
		ChCoordsys<>({ -.7, 0.7, 0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 1.0, 0, 0 })));
	motor9->Set_eng_mode(ChLinkEngine::ENG_MODE_SPEED);
	if (ChSharedPtr<ChFunction_Const> mfun9 = motor9->Get_spe_funct().DynamicCastTo<ChFunction_Const>())
		mfun9->Set_yconst(0);  // speed w=90°/s
	mphysicalSystem.AddLink(motor9);

	ChSharedPtr<ChLinkEngine> motor10(new ChLinkEngine);
	motor10->Initialize(frontLegL, frontBeamL,
		ChCoordsys<>({ -.7, 0.7, 1.0 }, Q_from_AngAxis(CH_C_PI / 2.0, { 1.0, 0, 0 })));
	motor10->Set_eng_mode(ChLinkEngine::ENG_MODE_SPEED);
	if (ChSharedPtr<ChFunction_Const> mfun10 = motor10->Get_spe_funct().DynamicCastTo<ChFunction_Const>())
		mfun10->Set_yconst(0);  // speed w=90°/s
	mphysicalSystem.AddLink(motor10);
	






	////////////////////////////Setup floor and Obstacles////////////////////////
	ChSharedPtr<ChBodyEasyBox> ground(new ChBodyEasyBox(50.0, 6.0, 50, 5000.0, true, true));
	ground->SetPos({ 0.0, -3.0, 0.0 });
	ground->SetBodyFixed(true);
	mphysicalSystem.Add(ground);

	ChSharedPtr<ChColorAsset> mcolor(new ChColorAsset());
	mcolor->SetColor(ChColor(0.2, 0.25, 0.25));
	ground->AddAsset(mcolor);

	////////////////////////////Initialize the Simulation////////////////////////

    application.AssetBindAll();
    // Use this function for 'converting' assets into Irrlicht meshes
    application.AssetUpdateAll();
    // Adjust some settings:
	mphysicalSystem.SetIterLCPmaxItersSpeed(iterSpeed);
	mphysicalSystem.SetIterLCPmaxItersStab(100);
    application.SetTimestep(timestep);
    //application.SetTryRealtime(true);
	float time = 0.0;
	long step_number = 0;
	
	////////////////////////////Simulation Loop//////////////////////////////////

    while (application.GetDevice()->run()) {
        application.BeginScene();

        application.DrawAll();

        // This performs the integration timestep!
        application.DoStep();

		if (step_number % 100 == 0){
			std::cout << "Time:   " << time << std::endl;
		}
		//printf("leg is at: %f, %f, %f\n", rearWheel->GetPos().x, rearWheel->GetPos().y, rearWheel->GetPos().z);
		//for (long j = 0; j < 200; j++){
		//	printf("waiting\n");
		//}
        application.EndScene();
		time += timestep;
		step_number++;
    }
    return 0;
}
