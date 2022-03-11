#include "Entity.h"

Entity::Entity()
{
	offsetPos = DirectX::XMFLOAT3(0, 0, 0);
	pos = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(1, 1, 1);
	rot = DirectX::XMFLOAT3(0, 0, 0);
	modelPos = DirectX::XMFLOAT3(0, 0, 0);
	isAnimated = false;
	bRender = true;
	isDeleted = false;
}

bool Entity::Intitialize(const std::string filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool isAnimated)
{
	this->_filePath = filePath;
	this->isAnimated = isAnimated;
	if (!model.Initialize(this->filePath, device, deviceContext, cb_vs_vertexshader, this->isAnimated))
	{
		return false;
	}


	return true;
}

void Entity::CreatePhysicsComponent(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking)
{
	if (isDeleted)
		return;

	if(physicsComponent.aActor)
		scene.removeActor(*physicsComponent.aActor);
	else if (physicsComponent.aStaticActor)
		scene.removeActor(*physicsComponent.aStaticActor);

	switch (physicsComponent.physicsShapeEnum)
	{
	case CUBE:
		physicsComponent.CreateCube(physics, scene, physx::PxVec3(scale.x, scale.y, scale.z), physx::PxVec3(pos.x, pos.y, pos.z));
		break;
	case PLANE:
		physicsComponent.CreatePlane(physics, scene, physx::PxVec3(pos.x, pos.y, pos.z));
		break;
	case SPHERE:
		physicsComponent.CreateSphere(physics, scene, physicsComponent.physics_scale.x, physx::PxVec3(pos.x, pos.y, pos.z));
		break;
	case CAPSULE:
		physicsComponent.CreateCapsule(physics, scene, physx::PxVec3(scale.x, scale.y, scale.z), physx::PxVec3(pos.x, pos.y, pos.z));
		break;
	case CONVEXMESH:
		physicsComponent.CreateConvex(physics, scene, cooking, model.m_vertices, physx::PxVec3(scale.x, scale.y, scale.z), physx::PxVec3(pos.x, pos.y, pos.z));
		break;
	case TRIANGLEMESH:
		physicsComponent.CreateTriangleMesh(physics, scene, cooking,model.m_vertices,model.m_indices, physx::PxVec3(scale.x, scale.y, scale.z), physx::PxVec3(pos.x, pos.y, pos.z),physx::PxQuat(1,physx::PxVec3(rot.x,rot.y,rot.z)));
		break;
	}
}

void Entity::UpdatePhysics()
{
	if (isDeleted)
		return;

	if (physicsComponent.aActor)
	{
		physicsComponent.aActor->setName(entityName.c_str());
		physicsComponent.trans = physicsComponent.aActor->getGlobalPose();
	}
	else if (physicsComponent.aStaticActor)
	{
		physicsComponent.aStaticActor->setName(entityName.c_str());
		physicsComponent.trans = physicsComponent.aStaticActor->getGlobalPose();
	}

	physicsComponent.trans.p.x += offsetPos.x;
	physicsComponent.trans.p.y += offsetPos.y;
	physicsComponent.trans.p.z += offsetPos.z;
	offsetPos = DirectX::XMFLOAT3(0, 0, 0);

	if (physicsComponent.aActor)
	{
		if (physicsComponent.aActor->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_SIMULATION))
		{
			physicsComponent.trans.q += physx::PxQuat(physicsComponent.physics_rot.w, physx::PxVec3(physicsComponent.physics_rot.x, physicsComponent.physics_rot.y, physicsComponent.physics_rot.z));
		}
	}
	else if (physicsComponent.aStaticActor)
	{
		if (physicsComponent.aStaticActor->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_SIMULATION))
		{
			physicsComponent.trans.q += physx::PxQuat(physicsComponent.physics_rot.w, physx::PxVec3(physicsComponent.physics_rot.x, physicsComponent.physics_rot.y, physicsComponent.physics_rot.z));
		}
	}

	if (physicsComponent.aActor)
	{
		physicsComponent.aActor->setGlobalPose(physicsComponent.trans);
	}
	else if (physicsComponent.aStaticActor)
	{
		physicsComponent.aStaticActor->setGlobalPose(physicsComponent.trans);
	}

}

void Entity::Draw(Camera& camera, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix)
{
	if (isDeleted)
		return;

	DirectX::XMMATRIX matrix_scale;
	DirectX::XMMATRIX matrix_rotate;
	DirectX::XMMATRIX matrix_translate;
	DirectX::XMMATRIX worldMatrix;

	//if (model.isAnimated && bRender)
	//{
	//	model.SetAnimIndex(0);
	//	model.Update();
	//}
	
	if (physicsComponent.aActor || physicsComponent.aStaticActor)
	{
		if(physicsComponent.aActor)
			physicsComponent.trans = physicsComponent.aActor->getGlobalPose();
		else if (physicsComponent.aStaticActor)
			physicsComponent.trans = physicsComponent.aStaticActor->getGlobalPose();

		matrix_scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
		matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		matrix_rotate *= DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ physicsComponent.trans.q.x,physicsComponent.trans.q.y, physicsComponent.trans.q.z }, physicsComponent.trans.q.getAngle());
		matrix_translate = DirectX::XMMatrixTranslation(physicsComponent.trans.p.x + modelPos.x, physicsComponent.trans.p.y + modelPos.y, physicsComponent.trans.p.z + modelPos.z);
		worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
	}
	else
	{
		matrix_scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
		matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		matrix_translate = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		worldMatrix = matrix_scale * matrix_rotate * matrix_translate;

	}
	if (bRender)
	{
		DirectX::XMMATRIX view = viewMatrix;
		DirectX::XMMATRIX proj = projectionMatrix;
		frustum.ConstructFrustum(100, view, proj);

		if (isfrustumEnabled)
		{
			if (physicsComponent.aActor || physicsComponent.aStaticActor)
				frustum.checkFrustum = frustum.CheckRect(physicsComponent.trans.p.x, physicsComponent.trans.p.y, physicsComponent.trans.p.z, scale.x, scale.y, scale.z);
			else
				frustum.checkFrustum = frustum.CheckRect(pos.x, pos.y, pos.z, scale.x, scale.y, scale.z);

			if (frustum.checkFrustum)
			{
				model.Draw(worldMatrix, viewMatrix, projectionMatrix);
			}
		}
		else
		{
			model.Draw(worldMatrix, viewMatrix, projectionMatrix);
		}
	}

}


void Entity::AttachController(physx::PxController& characterController, bool& runPhysics)
{
	if (isDeleted)
		return;

	physicsComponent.trans = characterController.getActor()->getGlobalPose();
	//pos = DirectX::XMFLOAT3(characterController.getPosition().x, characterController.getPosition().y, characterController.getPosition().z);


	if (!runPhysics)
	{
		characterController.setPosition(physx::PxExtendedVec3(offsetPos.x + pos.x, offsetPos.y + pos.y, offsetPos.z + pos.z));
		offsetPos = DirectX::XMFLOAT3(0, 0, 0);
	}
	
	physicsComponent.trans.q = physx::PxQuat(physicsComponent.physics_rot.w, physx::PxVec3(physicsComponent.physics_rot.x, physicsComponent.physics_rot.y, physicsComponent.physics_rot.z));


	physicsComponent.trans.q = physicsComponent.trans.q.getNormalized();


	characterController.getActor()->setGlobalPose(physicsComponent.trans);




	matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	matrix_rotate *= DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ physicsComponent.trans.q.x,physicsComponent.trans.q.y, physicsComponent.trans.q.z }, physicsComponent.trans.q.getAngle());

}

void Entity::DrawGui(physx::PxScene& scene)
{
	if (isDeleted)
		return;

	if (isAnimated)
	{
		std::fstream f;

		bool open = false, save = false;

		//ImGui::SameLine();
		if (ImGui::BeginMenu("Files"))
		{
			if (ImGui::MenuItem("Open", NULL))
				open = true;

			ImGui::EndMenu();
		}

		//Remember the name to ImGui::OpenPopup() and showFileDialog() must be same...
		if (open)
			ImGui::OpenPopup("Open File");

		if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(400, 200), "*.*,.obj,.dae,.gltf,.glb,.fbx"))
		{
			std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
			std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
			f = std::fstream(file_dialog.selected_path.c_str());
			if (f.good())
				inName = file_dialog.selected_path;

	
		}
		if (ImGui::Button("AddAnim"))
		{
			model.LoadAnimation(inName);
			model.animFiles.push_back(inName);
			for (int i = 0; i < model.animFiles.size(); ++i)
			{
				OutputDebugStringA(("NAME = " + model.animFiles[i] + "\n").c_str());
			}
		}
		
		
	}

	ImGui::Checkbox("isCharacter", &physicsComponent.isCharacter);
	ImGui::Checkbox("isPlayer", &isPlayer);
	ImGui::Checkbox("isAI", &isAI);
	ImGui::Checkbox("isWalkable", &isWalkable);
	ImGui::Checkbox("isObstacle", &isObstacle);
	if (ImGui::Button("Create Controller"))
	{
		bCreateController = true;
	}
	ImGui::Checkbox("Render", &bRender);

	ImGui::Text(("X: " + std::to_string(physicsComponent.trans.p.x)).c_str());
	ImGui::SameLine();
	ImGui::Text((" Y: " + std::to_string(physicsComponent.trans.p.y)).c_str());
	ImGui::SameLine();
	ImGui::Text((" Z: " + std::to_string(physicsComponent.trans.p.z)).c_str());

	//ImGui::DragFloat("offsetY", &offsetY);
	//ImGui::DragFloat("dirY", &dirY);
	//ImGui::DragFloat("maxDist", &maxDist);
	if(physicsComponent.aActor || physicsComponent.aStaticActor || physicsComponent.isCharacter)
		ImGui::DragFloat3("offsetPos", &offsetPos.x, 0.01f);
	else
		ImGui::DragFloat3("pos", &pos.x, 0.01f);

	ImGui::DragFloat3("rot", &rot.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::DragFloat3("modelPos", &modelPos.x, 0.01f);

	ImGui::DragFloat4("physics_rot", &physicsComponent.physics_rot.x, 0.01f);
	ImGui::DragFloat3("physics_scale", &physicsComponent.physics_scale.x, 0.01f);

	ImGui::Checkbox("isTransparent", &model.isTransparent);
	ImGui::Checkbox("Frustum", &isfrustumEnabled);

	ImGui::DragFloat("Mass", &physicsComponent.mass);
	ImGui::InputInt("triangleMeshStride", &physicsComponent.triangleMeshStride);
	ImGui::InputInt("convexMeshDetail", &physicsComponent.convexMeshDetail);
	ImGui::InputInt("Physics Shape", &physicsComponent.selectedShape);

	if (ImGui::Button("Apply"))
	{
		physicsComponent.physicsShapeEnum = static_cast<PhysicsShapeEnum>(physicsComponent.selectedShape);
		physicsComponent.bCreatePhysicsComp = true;
	}

	if (ImGui::Button("Delete"))
	{
		Clear(scene);
	}
}

void Entity::Clear(physx::PxScene& scene)
{
	if (isDeleted)
		return;

	isDeleted = true;
	model.Clear();
	if (physicsComponent.aStaticActor)
	{

		scene.removeActor(*physicsComponent.aStaticActor);
		physicsComponent.aStaticActor->release();
	}
	else if (physicsComponent.aActor)
	{
		scene.removeActor(*physicsComponent.aActor);
		physicsComponent.aActor->release();
	}
}
