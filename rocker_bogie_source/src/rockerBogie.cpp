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
double timestep = .005;

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
   
	//////////////////////////////Create the Robot////////////////////////////////
	//create frame color
	ChSharedPtr<ChColorAsset> mlegColor(new ChColorAsset());
	mlegColor->SetColor(ChColor(0.8, 0.25, 0.25));
	std::vector<ChSharedPtr<ChBody>> bodyParts;

	for (int i = 0; i < 2; i++){
		double xOff;
		if (i == 0) xOff = -tubeWidth;
		else if (i == 1) xOff = tubeWidth;
		double xPos = 0;
		double yPos = 0;
		double zPos = 0;
		//right rear leg
		ChSharedPtr<ChBody> rearLeg(new ChBody());

		double rearMass = tubeDensity * (tubeWidth * tubeWidth * rearLegLength) + 
			tubeDensity * (tubeWidth * tubeWidth * rearBeamLength) + 
			tubeDensity * (tubeWidth * tubeWidth * frontBeamLength);

		rearLeg->SetDensity((float)tubeDensity);
		rearLeg->SetMass(rearMass);
		rearLeg->SetInertiaXX(ChVector<>(0.01, 0.1, 0.1));

		//collision model
		rearLeg->GetCollisionModel()->ClearModel();
		rearLeg->GetCollisionModel()->AddBox(tubeWidth, rearLegLength, tubeWidth, { xOff+i*bodyWidth, rearLegLength, 0 });  // radius x, radius z, height on y

		rearLeg->GetCollisionModel()->AddBox(tubeWidth, rearBeamLength, tubeWidth, 
		{ -xOff, rearLegLength/2.0 + rearBeamLength / 2.0*cos(rearAngVert), rearBeamLength / 2.0*sin(rearAngVert) },
		Q_from_AngAxis(rearAngVert, { 1.0, 0, 0 }));

		rearLeg->GetCollisionModel()->AddBox(tubeWidth, frontBeamLength, tubeWidth,
		{ 0, rearBeamLength/2.0*cos(rearAngVert) - frontBeamLength / 2.0*cos(frontBeamAngVert), rearBeamLength/2.0*sin(rearAngVert) + frontBeamLength / 2.0*sin(frontBeamAngVert) },
		Q_from_AngAxis(frontBeamAngVert, {1.0, 0, 0}));

		rearLeg->GetCollisionModel()->BuildModel();
		rearLeg->SetCollide(true);

		//visualization model
		ChSharedPtr<ChBoxShape> vshape1(new ChBoxShape());
		vshape1->GetBoxGeometry().SetLengths(ChVector<>(tubeWidth, rearLegLength, tubeWidth));
		vshape1->Pos = { xOff + i*bodyWidth, rearLegLength, 0 };
		rearLeg->AddAsset(vshape1);

		ChSharedPtr<ChBoxShape> vshape2(new ChBoxShape());
		vshape2->GetBoxGeometry().SetLengths(ChVector<>(tubeWidth, rearBeamLength, tubeWidth));
		vshape2->Pos = { 0 + i*bodyWidth, 3.0*rearLegLength/2.0 + rearBeamLength / 2.0*cos(rearAngVert), rearBeamLength / 2.0*sin(rearAngVert) };
		vshape2->Rot = Q_from_AngAxis(rearAngVert, { 1.0, 0, 0 });
		rearLeg->AddAsset(vshape2);

		ChSharedPtr<ChBoxShape> vshape3(new ChBoxShape());
		vshape3->GetBoxGeometry().SetLengths(ChVector<>(tubeWidth, frontBeamLength, tubeWidth));
		vshape3->Pos = { 0 + i*bodyWidth, 3.0*rearLegLength/2.0 + rearBeamLength*cos(rearAngVert) + frontBeamLength / 2.0*cos(frontBeamAngVert), rearBeamLength*sin(rearAngVert) + frontBeamLength / 2.0*sin(frontBeamAngVert) };
		vshape3->Rot = Q_from_AngAxis(frontBeamAngVert, { 1.0, 0, 0 });
		rearLeg->AddAsset(vshape3);

		rearLeg->AddAsset(mlegColor);
		rearLeg->SetBodyFixed(false);
		mphysicalSystem.Add(rearLeg);
		//ChSharedPtr<ChMaterialSurfaceBase> mat(new ChMaterialSurfaceBase())
		//rearLeg->SetMaterialSurface

		//ChSharedPtr<ChBodyEasySphere> sph(new ChBodyEasySphere(0.2, 3000.0, true, true));
		//sph->SetPos({ i*2.0, 3.0, 0 });
		//mphysicalSystem.Add(sph);
		bodyParts.push_back(rearLeg);

	}

	/////////////////////////////Create the Physical Environment///////////////////
    ChSharedPtr<ChBodyEasyBox> floorBody(new ChBodyEasyBox(
        10, 2, 10, 3000, true, true));  // to create the floor, false -> doesn't represent a collide's surface
    floorBody->SetPos(ChVector<>(0, -1, 0));
    floorBody->SetBodyFixed(true);
    mphysicalSystem.Add(floorBody);

	//Add a texture for visualization
	ChSharedPtr<ChColorAsset> mcolor(new ChColorAsset());
	mcolor->SetColor(ChColor(0.2, 0.25, 0.25));
	floorBody->AddAsset(mcolor);

	/*
    ChSharedPtr<ChLinkMateGeneric> sphericalLink(
        new ChLinkMateGeneric(true, true, true, false, false, false));  // x,y,z,Rx,Ry,Rz constrains
    ChFrame<> link_position_abs(ChVector<>(0, 4, 0));

    sphericalLink->Initialize(pendulumBody,        // the 1st body to connect
                              floorBody,           // the 2nd body to connect
                              false,               // the two following frames are in absolute, not relative, coords.
                              link_position_abs,   // the link reference attached to 1st body
                              link_position_abs);  // the link reference attached to 2nd body

    mphysicalSystem.Add(sphericalLink);

    // optional, attach a texture to the pendulum, for better visualization
    ChSharedPtr<ChTexture> mtexture(new ChTexture());
    mtexture->SetTextureFilename(GetChronoDataFile("cubetexture_bluwhite.png"));  // texture in ../data
    pendulumBody->AddAsset(mtexture);
	*/


	////////////////////////////Initialize the Simulation////////////////////////

    application.AssetBindAll();
    // Use this function for 'converting' assets into Irrlicht meshes
    application.AssetUpdateAll();
    // Adjust some settings:
	mphysicalSystem.SetIterLCPmaxItersSpeed(iterSpeed);
    application.SetTimestep(timestep);
    //application.SetTryRealtime(true);

	////////////////////////////Simulation Loop//////////////////////////////////

    while (application.GetDevice()->run()) {
        application.BeginScene();

        application.DrawAll();

        // This performs the integration timestep!
        application.DoStep();

		printf("legs are at: %f, %f, %f\n", bodyParts[0]->GetPos().x, bodyParts[0]->GetPos().y, bodyParts[0]->GetPos().z);

        application.EndScene();
    }
    return 0;
}
