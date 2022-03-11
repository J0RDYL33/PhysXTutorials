#include <iostream> //cout, cerr
#include <iomanip> //stream formatting
#include <windows.h> //delay function
#include <PxPhysicsAPI.h> //PhysX

using namespace std;
using namespace physx;

//PhysX objects
PxPhysics* physics = 0;
PxFoundation* foundation = 0;
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
debugger::comm::PvdConnection* pvd = 0;
#else
PxPvd*  pvd = 0;
#endif

//simulation objects
PxScene* scene;
PxRigidDynamic* box;
PxRigidDynamic* box2;
PxRigidDynamic* box3;
PxRigidStatic* floorBox1;
PxRigidDynamic* floorBox2;
PxRigidDynamic* floorBox3;
PxRigidDynamic* capsule;
PxRigidStatic* plane;

int numberOfLoops = 0;

///Initialise PhysX objects
bool PxInit()
{
	//default error and allocator callbacks
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	//Init PhysX
	//foundation
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
#else
	foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
#endif

	if(!foundation)
		return false;

	//connect to an external visual debugger (if exists)
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
	pvd = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 100,
		PxVisualDebuggerExt::getAllConnectionFlags());
#else
	pvd = PxCreatePvd(*foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
	pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif

	//physics
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());
#else
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
#endif

	if(!physics)
		return false;

	//create a default scene
	PxSceneDesc sceneDesc(physics->getTolerancesScale());

	if(!sceneDesc.cpuDispatcher)
	{
		PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if(!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	scene = physics->createScene(sceneDesc);

	if (!scene)
		return false;

	return true;
}

/// Release all allocated resources
void PxRelease()
{
	if (scene)
		scene->release();
	if (physics)
		physics->release();
	if (pvd)
		pvd->release();
	if (foundation)
		foundation->release();
}

///Initialise the scene
void InitScene()
{
	//default gravity
	scene->setGravity(PxVec3(0.f, -9.81f, 0.f));

	//materials
	//Set dynamic friction to 0.07 for it to move exactly 1 meter
	PxMaterial* default_material = physics->createMaterial(0.f, 0.1f, .7f);   //static friction, dynamic friction, restitution
	PxMaterial* floor_material = physics->createMaterial(0.f, 0.f, .0f);

	//create a static plane (XZ)
	plane = PxCreatePlane(*physics, PxPlane(PxVec3(0.f, 1.f, 0.f), 0.f), *default_material);
	scene->addActor(*plane);

	//Static box creations

	floorBox1 = physics->createRigidStatic(PxTransform(PxVec3(0.f, .5f, 0.f)));
	floorBox2 = physics->createRigidDynamic(PxTransform(PxVec3(5.f, .5f, 0.f)));
	floorBox3 = physics->createRigidDynamic(PxTransform(PxVec3(10.f, .5f, 0.f)));

	capsule = physics->createRigidDynamic(PxTransform(PxVec3(0.f, 15.f, 0.f)));

	floorBox3->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	floorBox1->createShape(PxBoxGeometry(.5f, .5f, .5f), *floor_material);
	floorBox2->createShape(PxBoxGeometry(.5f, .5f, .5f), *floor_material);
	floorBox3->createShape(PxBoxGeometry(.5f, .5f, .5f), *floor_material);

	//create a dynamic actor and place it 10 m above the ground
	box = physics->createRigidDynamic(PxTransform(PxVec3(0.f, 10.f, 0.f)));
	box2 = physics->createRigidDynamic(PxTransform(PxVec3(5.f, 10.f, 0.f)));
	box3 = physics->createRigidDynamic(PxTransform(PxVec3(10.f, 10.f, 0.f)));

	//Capsule creation 15 m above the ground
	
	capsule->createShape(PxCapsuleGeometry(.5f, .5f), *floor_material);
	//create a box shape of 1m x 1m x 1m size (values are provided in halves)
	box->createShape(PxBoxGeometry(.5f, .5f, .5f), *default_material);
	box2->createShape(PxBoxGeometry(.5f, .5f, .5f), *default_material);
	box3->createShape(PxBoxGeometry(.5f, .5f, .5f), *default_material);
	//update the mass of the box
	PxRigidBodyExt::updateMassAndInertia(*box, 1.f);
	PxRigidBodyExt::updateMassAndInertia(*box2, 1.f);
	PxRigidBodyExt::updateMassAndInertia(*box3, 1.f);//density of 1kg/m^3
	PxRigidBodyExt::updateMassAndInertia(*floorBox2, 1.f);
	PxRigidBodyExt::updateMassAndInertia(*floorBox3, 1.f);
	PxRigidBodyExt::updateMassAndInertia(*capsule, .5f);
	scene->addActor(*box);
	scene->addActor(*box2);
	scene->addActor(*box3);
	scene->addActor(*floorBox1);
	scene->addActor(*floorBox2);
	scene->addActor(*floorBox3);
	scene->addActor(*capsule);

	//Add 100N of force on the X
	/*PxVec3 newForce;
	newForce.x = 100;
	newForce.y = 0;
	newForce.z = 0;
	box->addForce(newForce);*/
}

void MoveBox()
{
	PxTransform newPose = box->getGlobalPose();
	PxVec3 pos = newPose.p;

	pos.x += 10.f;
	newPose.p = pos;

	box->setGlobalPose(newPose);
}

void StopBox()
{
	PxVec3 vel = box->getLinearVelocity();

	vel.x = 0.f;
	vel.y = 0.f;
	vel.z = 0.f;

	box->setLinearVelocity(vel);
}

/// Perform a single simulation step
void Update(PxReal delta_time)
{
	scene->simulate(delta_time);
	scene->fetchResults(true);
	if (numberOfLoops == 10)
	{
		//MoveBox();
		//StopBox();
	}
	numberOfLoops++;
}


/// The main function
int main()
{
	//initialise PhysX	
	if (!PxInit())
	{
		cerr << "Could not initialise PhysX." << endl;
		return 0;
	}

	//initialise the scene
	InitScene();

	//set the simulation step to 1/60th of a second
	PxReal delta_time = 1.f/60.f;

	//simulate until the 'Esc' is pressed
	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		if (box->isSleeping() == false)
		{
			//'visualise' position and velocity of the box
			PxVec3 position = box->getGlobalPose().p;
			PxVec3 velocity = box->getLinearVelocity();
			cout << setiosflags(ios::fixed) << setprecision(2) << "x=" << position.x <<
				", y=" << position.y << ", z=" << position.z << ",  ";
			cout << setiosflags(ios::fixed) << setprecision(2) << "vx=" << velocity.x <<
				", vy=" << velocity.y << ", vz=" << velocity.z << ", number of loops = " << numberOfLoops << endl;
		}
		else if (box->isSleeping() == true)
			cout << "Sleeping!" << endl;

		//perform a single simulation step
		Update(delta_time);
		
		//introduce 100ms delay for easier visual analysis of the results
		Sleep(5);
	}

	//Release all resources
	PxRelease();

	return 0;
}
