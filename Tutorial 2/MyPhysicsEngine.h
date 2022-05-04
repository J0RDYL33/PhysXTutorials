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
		Domino* starterDominos[100];
		CompoundObject* disruptionHammer1;
		CompoundObject* disruptionHammer2;
		CompoundObject* disruptionHammer3;
		CompoundObject* disruptionHammer4;
		CompoundObject* disruptionHammer5;
		RevoluteJoint* disruptionJoint1;
		RevoluteJoint* disruptionJoint2;
		RevoluteJoint* disruptionJoint3;
		RevoluteJoint* disruptionJoint4;
		RevoluteJoint* disruptionJoint5;

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

			startHammer = new CompoundObject(PxTransform(PxVec3(20.f, -3.f, .0f)));
			//startHammer->GetShape(0)->setLocalPose(PxTransform(PxQuat(PxPi/2, PxVec3(1.0f, .0f, .0f).getNormalized())));
			//startHammer->GetShape(1)->setLocalPose(PxTransform(PxQuat(PxPi / 2, PxVec3(1.0f, .0f, .0f).getNormalized())));
			startHammer->Color(PxVec3(0.f / 255.f, 100.f / 255.f, 100.f / 255.f), 0);
			startHammer->Color(PxVec3(50.f / 255.f, 50.f / 255.f, 100.f / 255.f), 1);
			RevoluteJoint* hammerJoint = new RevoluteJoint(nullptr, PxTransform(5.0f, 21.f, 3.f), startHammer, PxTransform(.0f, -10.f, .0f));
			//startHammer->SetKinematic(1);
			Add(startHammer);
			startHammer->SetKinematic(1);


			//RevoluteJoint* hammerJoint = new RevoluteJoint(nullptr, PxTransform(5.0f, 21.f, 3.f),startHammer,PxTransform(.0f,-10.f,.0f));

			//Spawn in the initial 100 dominos
			for (int i = 0; i < 100; i++)
			{
				starterDominos[i] = new Domino(PxTransform(PxVec3(5.f, .5f, i * 2.f)));
				starterDominos[i]->Color(PxVec3(0.f / 255.f, (1.5f * i) / 255.f, (2.f * i) / 255.f));
				Add(starterDominos[i]);
			}

			disruptionHammer1 = new CompoundObject(PxTransform(PxVec3(0.f, 0.f, 0.f)));
			disruptionHammer1->Color(PxVec3(0.f / 255.f, 100.f / 255.f, 100.f / 255.f), 0);
			disruptionHammer1->Color(PxVec3(75.f / 255.f, 200.f / 255.f, 100.f / 255.f), 1);
			disruptionJoint1 = new RevoluteJoint(nullptr, PxTransform(PxVec3(0.f, 21.2f, 30.f),
				PxQuat(PxPi / 2, PxVec3(0.f, 1.f, 0.f))), disruptionHammer1, PxTransform(.0f, -10.f, .0f));
			disruptionJoint1->DriveVelocity(1);
			Add(disruptionHammer1);
			disruptionHammer1->SetKinematic(1);

			disruptionHammer2 = new CompoundObject(PxTransform(PxVec3(0.f, 0.f, 0.f)));
			disruptionHammer2->Color(PxVec3(0.f / 255.f, 100.f / 255.f, 100.f / 255.f), 0);
			disruptionHammer2->Color(PxVec3(75.f / 255.f, 200.f / 255.f, 100.f / 255.f), 1);
			disruptionJoint2 = new RevoluteJoint(nullptr, PxTransform(PxVec3(0.f, 21.2f, 60.f),
				PxQuat(PxPi / 2, PxVec3(0.f, 1.f, 0.f))), disruptionHammer2, PxTransform(.0f, -10.f, .0f));
			disruptionJoint2->DriveVelocity(1);
			Add(disruptionHammer2);
			disruptionHammer2->SetKinematic(1);

			disruptionHammer3 = new CompoundObject(PxTransform(PxVec3(0.f, 0.f, 0.f)));
			disruptionHammer3->Color(PxVec3(0.f / 255.f, 100.f / 255.f, 100.f / 255.f), 0);
			disruptionHammer3->Color(PxVec3(75.f / 255.f, 200.f / 255.f, 100.f / 255.f), 1);
			disruptionJoint3 = new RevoluteJoint(nullptr, PxTransform(PxVec3(0.f, 21.2f, 90.f),
				PxQuat(PxPi / 2, PxVec3(0.f, 1.f, 0.f))), disruptionHammer3, PxTransform(.0f, -10.f, .0f));
			disruptionJoint3->DriveVelocity(1);
			Add(disruptionHammer3);
			disruptionHammer3->SetKinematic(1);

			disruptionHammer4 = new CompoundObject(PxTransform(PxVec3(0.f, 0.f, 0.f)));
			disruptionHammer4->Color(PxVec3(0.f / 255.f, 100.f / 255.f, 100.f / 255.f), 0);
			disruptionHammer4->Color(PxVec3(75.f / 255.f, 200.f / 255.f, 100.f / 255.f), 1);
			disruptionJoint4 = new RevoluteJoint(nullptr, PxTransform(PxVec3(0.f, 21.2f, 120.f),
				PxQuat(PxPi / 2, PxVec3(0.f, 1.f, 0.f))), disruptionHammer4, PxTransform(.0f, -10.f, .0f));
			disruptionJoint4->DriveVelocity(1);
			Add(disruptionHammer4);
			disruptionHammer4->SetKinematic(1);

			disruptionHammer5 = new CompoundObject(PxTransform(PxVec3(0.f, 0.f, 0.f)));
			disruptionHammer5->Color(PxVec3(0.f / 255.f, 100.f / 255.f, 100.f / 255.f), 0);
			disruptionHammer5->Color(PxVec3(75.f / 255.f, 200.f / 255.f, 100.f / 255.f), 1);
			disruptionJoint5 = new RevoluteJoint(nullptr, PxTransform(PxVec3(0.f, 21.2f, 150.f),
				PxQuat(PxPi / 2, PxVec3(0.f, 1.f, 0.f))), disruptionHammer5, PxTransform(.0f, -10.f, .0f));
			disruptionJoint5->DriveVelocity(1);
			Add(disruptionHammer5);
			disruptionHammer5->SetKinematic(1);

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

		virtual void StartTheHammer()
		{
			startHammer->SetKinematic(0);
		}

		virtual void StartRuinerHammer(float disToMove)
		{
			cout << "Moving ruiner hammers" << disToMove << endl;
			if (disToMove == 0)
				disruptionHammer1->SetKinematic(0);
			else if (disToMove == 1)
				disruptionHammer2->SetKinematic(0);
			else if (disToMove == 2)
				disruptionHammer3->SetKinematic(0);
			else if (disToMove == 3)
				disruptionHammer4->SetKinematic(0);
			else if (disToMove == 4)
				disruptionHammer5->SetKinematic(0);

		}
	};
}
