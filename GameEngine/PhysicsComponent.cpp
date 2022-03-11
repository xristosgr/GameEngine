#include "PhysicsComponent.h"
#include <string>

PhysicsComponent::PhysicsComponent()
{
	verts = nullptr;
	indices = nullptr;

	mass = 0.0f;
	density = 10.0f;
	trans = physx::PxTransform(physx::PxIDENTITY());
	physicsShapeEnum = PhysicsShapeEnum::NONE;
	selectedShape = -1;
	bCreatePhysicsComp = false;

	triangleMeshStride = 3;
	convexMeshDetail = 64;

	physics_rot = physx::PxQuat(0, 0, 0, 1);
	physics_scale = physx::PxVec3(1, 1, 1);

	isCharacter = false;
	hasLineOfSight = false;
}

PhysicsComponent::~PhysicsComponent()
{
}

void PhysicsComponent::CreateCube(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3 _scale, physx::PxVec3 _pos)
{
	current_scale = _scale;
	if (mass > 0.0f)
	{
		aActor = physics.createRigidDynamic(physx::PxTransform(_pos));
		aActor->setMass(mass);
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.1f);
		aShape = physx::PxRigidActorExt::createExclusiveShape(*aActor, physx::PxBoxGeometry(_scale.x, _scale.y, _scale.z), *aMaterial);

		physx::PxRigidBodyExt::updateMassAndInertia(*aActor, density);

		aActor->getShapes(&aShape, aActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

		trans = aActor->getGlobalPose();
		trans.p = _pos;
		aActor->setGlobalPose(trans);

		scene.addActor(*aActor);
	}
	else
	{
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);
		aStaticActor = physx::PxCreateStatic(physics, physx::PxTransform(_pos), physx::PxBoxGeometry(_scale.x, _scale.y, _scale.z), *aMaterial);

		aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

		trans = aStaticActor->getGlobalPose();
		trans.p = _pos;
		aStaticActor->setGlobalPose(trans);

		scene.addActor(*aStaticActor);
	}
}

void PhysicsComponent::CreatePlane(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3 _pos)
{
	aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);

	aStaticActor = physx::PxCreatePlane(physics, physx::PxPlane(_pos, physx::PxVec3(0, 1, 0)), *aMaterial);

	aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
	aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


	trans = aStaticActor->getGlobalPose();
	trans.p = _pos;
	aStaticActor->setGlobalPose(trans);

	scene.addActor(*aStaticActor);
}

void PhysicsComponent::CreateSphere(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxReal radius, physx::PxVec3 _pos)
{

	current_scale.x = radius;
	if (mass > 0.0f)
	{
		aActor = physics.createRigidDynamic(physx::PxTransform(_pos));
		physx::PxTransform relativePose(physx::PxQuat(physx::PxPi, physx::PxVec3(0, 0, 1)));
		aActor->setMass(mass);
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.1f);
		//float radius = 1.0f;
		aShape = physx::PxRigidActorExt::createExclusiveShape(*aActor, physx::PxSphereGeometry(radius), *aMaterial);
		aShape->setLocalPose(relativePose);

		physx::PxRigidBodyExt::updateMassAndInertia(*aActor, density);

		aActor->getShapes(&aShape, aActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


		trans = aActor->getGlobalPose();
		trans.p = _pos;
		aActor->setGlobalPose(trans);

		scene.addActor(*aActor);
	}
	else
	{
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);
		aStaticActor = physx::PxCreateStatic(physics, physx::PxTransform(_pos), physx::PxSphereGeometry(radius), *aMaterial);

		aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


		trans = aStaticActor->getGlobalPose();
		trans.p = _pos;
		aStaticActor->setGlobalPose(trans);

		scene.addActor(*aStaticActor);
	}
}

void PhysicsComponent::CreateCapsule(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3 _scale, physx::PxVec3 _pos)
{
	current_scale = _scale;

	if (mass > 0.0f)
	{
		aActor = physics.createRigidDynamic(physx::PxTransform(_pos));
		physx::PxTransform relativePose(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)));
		
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.0f);
		aActor->setMass(mass);
		aActor->setAngularDamping(1000.0f);
		//aActor->setLinearDamping(1000.0f);
		//float radius = 1.0f;
		aShape = physx::PxRigidActorExt::createExclusiveShape(*aActor,
			physx::PxCapsuleGeometry(_scale.x, _scale.y / 2.0f), *aMaterial);

		
		aShape->setLocalPose(relativePose);
		density = 80.0f;
		physx::PxRigidBodyExt::updateMassAndInertia(*aActor, density);

		aActor->getShapes(&aShape, aActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


		trans = aActor->getGlobalPose();
		trans.p = _pos;
		aActor->setGlobalPose(trans);

		scene.addActor(*aActor);
	}
	else
	{
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);
		aStaticActor = physx::PxCreateStatic(physics, physx::PxTransform(_pos), physx::PxCapsuleGeometry(_scale.x, _scale.y / 2), *aMaterial);

		aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


		trans = aStaticActor->getGlobalPose();
		trans.p = _pos;
		aStaticActor->setGlobalPose(trans);

		scene.addActor(*aStaticActor);
	}
}

void PhysicsComponent::CreateConvex(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking, std::vector<Vertex>& vertices, physx::PxVec3 _scale, physx::PxVec3 _pos)
{
	current_scale = _scale;
	verts = &vertices;
	if (mass > 0)
	{
		aActor = physics.createRigidDynamic(physx::PxTransform(_pos));
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.1f);

		std::vector< physx::PxVec3> convexVerts;

		//if (vertices.size() <= 0)
			//return;
		for (int i = 0; i < vertices.size(); ++i)
		{
			convexVerts.push_back(physx::PxVec3(vertices[i].pos.x * _scale.x, vertices[i].pos.y * _scale.y, vertices[i].pos.z * _scale.z));
		}


		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count = convexVerts.size();
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data = convexVerts.data();
		convexDesc.vertexLimit = convexMeshDetail;
		convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
		physx::PxDefaultMemoryOutputStream buf;
		physx::PxConvexMeshCookingResult::Enum result;
		if (!cooking->cookConvexMesh(convexDesc, buf, &result))
		{
			return;
		}
		//PX_ASSERT(result);

		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxConvexMesh* convexMesh = physics.createConvexMesh(input);

		aShape = physx::PxRigidActorExt::createExclusiveShape(*aActor, physx::PxConvexMeshGeometry(convexMesh), *aMaterial);

		aActor->setMass(mass);

		physx::PxRigidBodyExt::updateMassAndInertia(*aActor, density);


		trans = aActor->getGlobalPose();
		trans.p = _pos;
		aActor->setGlobalPose(trans);

		scene.addActor(*aActor);

		aActor->getShapes(&aShape, aActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
	}
	else
	{
		aStaticActor = physics.createRigidStatic(physx::PxTransform(_pos));
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);

		std::vector< physx::PxVec3> convexVerts;

		if (vertices.size() <= 0)
			return;
		for (int i = 0; i < vertices.size(); ++i)
		{
			convexVerts.push_back(physx::PxVec3(vertices[i].pos.x * _scale.x, vertices[i].pos.y * _scale.y, vertices[i].pos.z * _scale.z));
		}
		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count = convexVerts.size();
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data = convexVerts.data();
		convexDesc.vertexLimit = convexMeshDetail;

		convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxDefaultMemoryOutputStream buf;
		physx::PxConvexMeshCookingResult::Enum result;
		if (!cooking->cookConvexMesh(convexDesc, buf, &result))
		{
			return;
		}
		//PX_ASSERT(result);
		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxConvexMesh* convexMesh = physics.createConvexMesh(input);

		aShape = physx::PxRigidActorExt::createExclusiveShape(*aStaticActor, physx::PxConvexMeshGeometry(convexMesh), *aMaterial);


		trans = aStaticActor->getGlobalPose();
		trans.p = _pos;
		aStaticActor->setGlobalPose(trans);

		scene.addActor(*aStaticActor);

		aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
	}
}

void PhysicsComponent::CreateTriangleMesh(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, physx::PxVec3 _scale, physx::PxVec3 _pos, physx::PxQuat _rot)
{
	//OutputDebugStringA(("POS = " + std::to_string(_pos.x) + "\n").c_str());
	//scale = _scale;
	//rot = _rot;

	current_scale = physics_scale;
	verts = &vertices;
	this->indices = &indices;
	if (vertices.size() <= 0)
		return;
	aStaticActor = physics.createRigidStatic(physx::PxTransform(_pos, physics_rot));
	aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);

	std::vector< physx::PxU32>tris;
	std::vector<physx::PxVec3>convexVerts;

	for (int i = 0; i < vertices.size(); ++i)
	{
		convexVerts.push_back(physx::PxVec3(vertices[i].pos.x * physics_scale.x, vertices[i].pos.y * physics_scale.y, vertices[i].pos.z * physics_scale.z));
	}


	for (int i = 0; i < indices.size(); ++i)
	{
		tris.push_back(indices[i]);
	}

	physx::PxTriangleMeshDesc meshDesc;

	meshDesc.points.count = convexVerts.size();
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.points.data = convexVerts.data();

	meshDesc.triangles.count = tris.size();
	meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
	meshDesc.triangles.data = tris.data();

	physx::PxDefaultMemoryOutputStream writeBuffer;
	physx::PxTriangleMeshCookingResult::Enum result;
	bool status;
	if(meshDesc.isValid())
		status = cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);



	PX_ASSERT(result);
	PX_ASSERT(status);

	physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

	physx::PxTriangleMesh* triMesh = physics.createTriangleMesh(readBuffer);

	aShape = physx::PxRigidActorExt::createExclusiveShape(*aStaticActor, physx::PxTriangleMeshGeometry(triMesh), *aMaterial);


	trans = aStaticActor->getGlobalPose();
	trans.p = _pos;
	aStaticActor->setGlobalPose(trans);

	scene.addActor(*aStaticActor);

	aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
	aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


}

void PhysicsComponent::CreateController(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3 _pos, std::string& name)
{
	mass = 80.0f;
	//physx::PxVec3 _pos;
	//if (aActor)
	//{
	//	_pos = aActor->getGlobalPose().p;
	//	scene.removeActor(*aActor);
	//}
	//else
	//{
	//	_pos = aStaticActor->getGlobalPose().p;
	//	scene.removeActor(*aStaticActor);
	//}
	//physx::PxExtendedVec3 extPos = physx::PxExtendedVec3(_pos.x, _pos.y, _pos.z);
	//characterController.CreateCharacterController(physics, scene, manager, extPos, name.c_str());

	CreateCapsule(physics, scene, physics_scale, _pos);
}

void PhysicsComponent::UpdatePhysics(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking)
{
	if (aActor || aStaticActor)
	{
		physx::PxVec3 _pos;

		if(aActor)
			aActor->getShapes(&aShape, aActor->getNbShapes());
		else
			aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());

		if (!aShape)
			return;

		if (aShape->getGeometryType() == physx::PxGeometryType::eBOX)
		{
			if (physics_scale.x != current_scale.x || physics_scale.y != current_scale.y || physics_scale.z != current_scale.z)
			{
				aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
				
				if (aActor)
				{
					_pos = aActor->getGlobalPose().p;
					scene.removeActor(*aActor);
				}
				else
				{
					_pos = aStaticActor->getGlobalPose().p;
					scene.removeActor(*aStaticActor);
				}

				CreateCube(physics, scene, physics_scale, _pos);
			}
		}
		else if (aShape->getGeometryType() == physx::PxGeometryType::eSPHERE)
		{
			if (physics_scale.x != current_scale.x)
			{
				aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);

				if (aActor)
				{
					_pos = aActor->getGlobalPose().p;
					scene.removeActor(*aActor);
				}
				else
				{
					_pos = aStaticActor->getGlobalPose().p;
					scene.removeActor(*aStaticActor);
				}
				CreateSphere(physics, scene, physics_scale.x, _pos);
			}
		}
		else if (aShape->getGeometryType() == physx::PxGeometryType::eCONVEXMESH)
		{
			if (physics_scale.x != current_scale.x || physics_scale.y != current_scale.y || physics_scale.z != current_scale.z)
			{
				aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);

				if (aActor)
				{

					_pos = aActor->getGlobalPose().p;
					scene.removeActor(*aActor);
				}
				else
				{
					_pos = aStaticActor->getGlobalPose().p;
					scene.removeActor(*aStaticActor);
				}
				CreateConvex(physics, scene, cooking, *verts, physics_scale, _pos);
			}
		}
		else if (aShape->getGeometryType() == physx::PxGeometryType::eCAPSULE)
		{
			if (physics_scale.x != current_scale.x || physics_scale.y != current_scale.y || physics_scale.z != current_scale.z)
			{
				aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);

				if (aActor)
				{

					_pos = aActor->getGlobalPose().p;
					scene.removeActor(*aActor);
				}
				else
				{
					_pos = aStaticActor->getGlobalPose().p;
					scene.removeActor(*aStaticActor);
				}
				CreateCapsule(physics, scene, physics_scale, _pos);
			}
		}
		else if (aShape->getGeometryType() == physx::PxGeometryType::eTRIANGLEMESH)
		{
			if (physics_scale.x != current_scale.x || physics_scale.y != current_scale.y || physics_scale.z != current_scale.z)
			{
				aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
		
				if (aActor)
				{
		
					_pos = aActor->getGlobalPose().p;
					scene.removeActor(*aActor);
				}
				else
				{
					_pos = aStaticActor->getGlobalPose().p;
					scene.removeActor(*aStaticActor);
				}
				CreateTriangleMesh(physics, scene,cooking,*verts,*indices, physics_scale, _pos, physics_rot);
			}
		}
	}
}
