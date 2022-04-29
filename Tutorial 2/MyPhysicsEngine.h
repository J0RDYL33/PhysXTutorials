#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	//a list of colours: Circus Palette
	static const PxVec3 color_palette[] = {PxVec3(46.f/255.f,9.f/255.f,39.f/255.f),PxVec3(217.f/255.f,0.f/255.f,0.f/255.f),
		PxVec3(255.f/255.f,45.f/255.f,0.f/255.f),PxVec3(255.f/255.f,140.f/255.f,54.f/255.f),PxVec3(4.f/255.f,117.f/255.f,111.f/255.f)};

	//pyramid vertices
	static PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
	//pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	//vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = {1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1};

	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose=PxTransform(PxIdentity), PxReal density=1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose=PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs),end(pyramid_trigs)), pose)
		{
		}
	};

	///Custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		Box* box;
		Box* box2;
		Box* boxArr[10];
		CompoundObject* startHammer;
		Box* boxAngle;
		float boxRotator = .1f;
		Domino* starterDominos[10];

	public:
		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(1.f);

			plane = new Plane();
			plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
			Add(plane);


			/*box = new Box();
			box->GetShape()->setLocalPose(PxTransform(PxVec3(5.0f, 3.f, .0f)));
			box->Color(color_palette[0]);
			Add(box);*/
			/*
			box2 = new Box(PxTransform(PxVec3(.0f, 10.f, 5.f)));
			box2->Color(PxVec3(0.f/255.f, 100.f / 255.f, 100.f / 255.f));
			Add(box2);

			box3 = new CompoundObject(PxTransform(PxVec3(10.f,10.f,10.f)));
			box3->Color(PxVec3(0.f / 255.f, 100.f / 255.f, 100.f / 255.f), 0);
			box3->Color(PxVec3(100.f / 255.f, 0.f / 255.f, 100.f / 255.f), 1);
			Add(box3);
			*/
			/*for (int i = 0; i < 10; i++)
			{
				boxArr[i] = new Box(PxTransform(PxVec3(.0f, i * 1.5f, .0f)));
				boxArr[i]->GetShape()->setLocalPose(PxTransform(PxQuat(.2f*i, PxVec3(.0f, 1.0f, .0f))));
				//boxArr[i]->GetShape()->setLocalPose(PxTransform(PxVec3(.0f, i*3.f, .0f)));
				boxArr[i]->Color(PxVec3(0.f / 255.f, (20.f *i) / 255.f, (20.f * i) / 255.f));
				Add(boxArr[i]);
			}*/

			startHammer = new CompoundObject(PxTransform(PxVec3(20.f, 3.f, .0f)));
			//startHammer->GetShape(0)->setLocalPose(PxTransform(PxQuat(PxPi/2, PxVec3(1.0f, .0f, .0f).getNormalized())));
			//startHammer->GetShape(1)->setLocalPose(PxTransform(PxQuat(PxPi / 2, PxVec3(1.0f, .0f, .0f).getNormalized())));
			startHammer->Color(PxVec3(0.f / 255.f, 100.f / 255.f, 100.f / 255.f), 0);
			startHammer->Color(PxVec3(50.f / 255.f, 50.f / 255.f, 100.f / 255.f), 1);
			Add(startHammer);


			RevoluteJoint* hammerJoint = new RevoluteJoint(nullptr, PxTransform(5.0f, 22.f, -6.f),startHammer,PxTransform(.0f,-10.f,.0f));

			//Spawn in the initial 10 dominos for a pass
			for (int i = 0; i < 10; i++)
			{
				starterDominos[i] = new Domino(PxTransform(PxVec3(5.f, .5f, i * 1.f)));
				starterDominos[i]->Color(PxVec3(0.f / 255.f, (20.f * i) / 255.f, (20.f * i) / 255.f));
				Add(starterDominos[i]);
			}

			/*boxAngle = new Box(PxTransform(PxVec3(5.5f, .5f, .5f), PxQuat(1.f, PxVec3(1.f, .0f, .0f))));
			Add(boxAngle);*/
		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{
			
			/*box->GetShape()->setLocalPose(PxTransform(PxQuat(boxRotator, PxVec3(.0f, 1.f, .0f))));
			boxRotator += 0.1f;*/
			/*PxTransform newPose = ((PxRigidBody*)box3->Get())->getGlobalPose();
			PxTransform newerPose = ((PxRigidActor*)box3->Get())->getGlobalPose();
			//PxTransform newPose = box3->getGlobalPose();
			PxVec3 pos = newPose.p;

			pos.x += .1f;
			newerPose.p = pos;

			((PxRigidActor*)box3->Get())->setGlobalPose(newerPose);
			//box3->GetShape()->setLocalPose(newPose);*/
		}
	};
}
