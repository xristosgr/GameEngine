#include "Entity.h"

Entity::Entity()
{
	offsetPos = DirectX::XMFLOAT3(0, 0, 0);
	pos = DirectX::XMFLOAT3(0, 0, 0);
	scale = DirectX::XMFLOAT3(1, 1, 1);
	rot = DirectX::XMFLOAT3(0, 0, 0);
	modelPos = DirectX::XMFLOAT3(0, 0, 0);
	frustumScale = DirectX::XMFLOAT3(0, 0, 0);
	isAnimated = false;
	bRender = true;
	isDeleted = false;
	bFlagForDeletion = false;
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

void Entity::Update()
{
	UpdatePhysics();
}

void Entity::Draw(Camera& camera, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix, Texture* text,bool bCheckFrustum)
{

	DirectX::XMMATRIX matrix_scale;
	DirectX::XMMATRIX matrix_rotate;
	DirectX::XMMATRIX matrix_translate;

	if (physicsComponent.aActor || physicsComponent.aStaticActor)
	{
		if(physicsComponent.aActor)
			physicsComponent.trans = physicsComponent.aActor->getGlobalPose();
		else if (physicsComponent.aStaticActor)
			physicsComponent.trans = physicsComponent.aStaticActor->getGlobalPose();

		if (!model.isAttached)
		{
			matrix_scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
			matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
			matrix_rotate *= DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ physicsComponent.trans.q.x,physicsComponent.trans.q.y, physicsComponent.trans.q.z }, physicsComponent.trans.q.getAngle());

			matrix_translate = DirectX::XMMatrixTranslation(physicsComponent.trans.p.x + modelPos.x, physicsComponent.trans.p.y + modelPos.y, physicsComponent.trans.p.z + modelPos.z);
			worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
		}
		else
		{
			if (parent)
			{

				DirectX::XMFLOAT3 floatPos;
				DirectX::XMFLOAT4 floatRot;
				DirectX::XMStoreFloat3(&floatPos, _pos);
				DirectX::XMStoreFloat4(&floatRot, _rot);


				physicsComponent.trans.p = physx::PxVec3(floatPos.x, floatPos.y, floatPos.z);
				physicsComponent.trans.q = physx::PxQuat(floatRot.w, physx::PxVec3(floatRot.x, floatRot.y, floatRot.z));

				if (physicsComponent.aActor)
				{
					physicsComponent.aActor->setGlobalPose(physicsComponent.trans);
					//physicsComponent.aActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true)
					physicsComponent.aActor->getShapes(&physicsComponent.aShape, physicsComponent.aActor->getNbShapes());
					physicsComponent.aShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
					physicsComponent.aShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
				}
				else if (physicsComponent.aStaticActor)
				{
					physicsComponent.aStaticActor->setGlobalPose(physicsComponent.trans);
					//physicsComponent.aStaticActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
					physicsComponent.aStaticActor->getShapes(&physicsComponent.aShape, physicsComponent.aStaticActor->getNbShapes());
					physicsComponent.aShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
					physicsComponent.aShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
				}
		

				matrix_scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
				matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
				matrix_rotate *= DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ physicsComponent.trans.q.x,physicsComponent.trans.q.y, physicsComponent.trans.q.z }, physicsComponent.trans.q.getAngle());

				matrix_translate = DirectX::XMMatrixTranslation(physicsComponent.trans.p.x + modelPos.x, physicsComponent.trans.p.y + modelPos.y, physicsComponent.trans.p.z + modelPos.z);
				worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
			}

		}
	}
	else
	{
		
		matrix_scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
		matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		matrix_translate = DirectX::XMMatrixTranslation(modelPos.x, modelPos.y, modelPos.z);

		worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
		if (!model.isAttached)
		{
			matrix_scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
			matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
			matrix_translate = DirectX::XMMatrixTranslation(pos.x + modelPos.x, pos.y + modelPos.y, pos.z + modelPos.z);



			worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
		}
		else
		{
			matrix_scale = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
			matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
			matrix_rotate *= DirectX::XMMatrixRotationQuaternion(_rot);
			matrix_translate = DirectX::XMMatrixTranslationFromVector(_pos);
			worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
		}
	}

	if (bRender)
	{
		DirectX::XMMATRIX view = viewMatrix;
		DirectX::XMMATRIX proj = projectionMatrix;
		frustum.ConstructFrustum(100, view, proj);

		if (!bCheckFrustum)
		{
			model.Draw(worldMatrix, viewMatrix, projectionMatrix, text);
		}
		else
		{
			if (isfrustumEnabled)
			{
				if (physicsComponent.aActor || physicsComponent.aStaticActor)
					frustum.checkFrustum = frustum.CheckRect(physicsComponent.trans.p.x, physicsComponent.trans.p.y, physicsComponent.trans.p.z, scale.x + frustumScale.x, scale.y + frustumScale.x, scale.z + frustumScale.z);
				else
					frustum.checkFrustum = frustum.CheckRect(pos.x, pos.y, pos.z, scale.x + frustumScale.x, scale.y + frustumScale.y, scale.z + frustumScale.z);

				if (frustum.checkFrustum)
				{
					model.Draw(worldMatrix, viewMatrix, projectionMatrix, text);
				}
			}
			else
			{
				model.Draw(worldMatrix, viewMatrix, projectionMatrix, text);
			}
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

void Entity::MouseMove(Mouse& mouse, Keyboard& keyboard, Camera& camera)
{
	if (physicsComponent.aActor)
	{
		physicsComponent.aActor->getShapes(&physicsComponent.aShape, physicsComponent.aActor->getNbShapes());
		if (physicsComponent.aShape->getFlags().isSet(physx::PxShapeFlag::eVISUALIZATION))
		{

			float pointX, pointY;
			DirectX::XMMATRIX viewMatrix, inverseViewMatrix;
			DirectX::XMFLOAT3 direction = DirectX::XMFLOAT3(0, 0, 0);

			pointX = ((2.0f * (float)mouse.GetPosX()) / (float)1280) - 1.0f;
			pointY = (((2.0f * (float)mouse.GetPosY()) / (float)720) - 1.0f) * -1.0f;


			DirectX::XMFLOAT4X4 projection;
			DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
			pointX = pointX / projection._11;
			pointY = pointY / projection._22;

			viewMatrix = camera.GetViewMatrix();
			inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);


			DirectX::XMFLOAT4X4 invView;
			DirectX::XMStoreFloat4x4(&invView, inverseViewMatrix);

			direction.x = (pointX * invView._11) + (pointY * invView._21) + invView._31;
			direction.y = (pointX * invView._12) + (pointY * invView._22) + invView._32;
			direction.z = (pointX * invView._13) + (pointY * invView._23) + invView._33;
		   
			if (keyboard.KeyIsPressed('X'))
			{
				
				physicsComponent.trans = physicsComponent.aActor->getGlobalPose();
				physicsComponent.trans.p.x += direction.x * 0.3f;
				physicsComponent.aActor->setGlobalPose(physicsComponent.trans);
			}
			if (keyboard.KeyIsPressed('Y'))
			{

				physicsComponent.trans = physicsComponent.aActor->getGlobalPose();
				physicsComponent.trans.p.y += direction.y * 0.3f;
				physicsComponent.aActor->setGlobalPose(physicsComponent.trans);
			}
			if (keyboard.KeyIsPressed('Z'))
			{

				physicsComponent.trans = physicsComponent.aActor->getGlobalPose();
				physicsComponent.trans.p.z += direction.z * 0.3f;
				physicsComponent.aActor->setGlobalPose(physicsComponent.trans);
			}

		}
	}
}

void Entity::SetupAttachment(Entity* entity, std::string boneName)
{
	//parent = entity;
	//if (model.isAttached)
	//{
	//	if (parent)
	//	{
	//		parent->model.attachedBone = boneName;
	//
	//		parent->model.FinalBoneTrans = parent->model.boneTrans * parent->model._worldMatrix;
	//
	//
	//		DirectX::XMMatrixDecompose(&_scale, &_rot, &_pos, parent->model.FinalBoneTrans);
	//
	//		DirectX::XMStoreFloat3(&parent->model.worldPos, _pos);
	//		DirectX::XMStoreFloat3(&parent->model.worldScale, _scale);
	//		DirectX::XMStoreFloat4(&parent->model.worldRot, _rot);
	//
	//
	//		pos = XMFLOAT3(parent->model.worldPos.x, parent->model.worldPos.y, parent->model.worldPos.z);
	//	}
	//}
}
void Entity::SetupAttachment(Entity* entity)
{

	if (entity)
		parent = entity;
	else
		return;

	if (model.isAttached && parent)
	{
		DirectX::XMMATRIX boneTrans;
		parent->model.AttachTo(attachedBone, boneTrans);
		boneTrans = boneTrans * parent->worldMatrix;
	
	
		DirectX::XMMatrixDecompose(&_scale, &_rot, &_pos, boneTrans);
	
		DirectX::XMStoreFloat3(&parent->model.worldPos, _pos);
		DirectX::XMStoreFloat3(&parent->model.worldScale, _scale);
		DirectX::XMStoreFloat4(&parent->model.worldRot, _rot);
	
		pos = DirectX::XMFLOAT3(parent->model.worldPos.x, parent->model.worldPos.y, parent->model.worldPos.z);
	}
}
void Entity::DrawGui(physx::PxScene& scene,std::vector<Entity>& entities)
{
	if (isDeleted)
		return;


	ImGui::Text(entityName.c_str());
	ImGui::SameLine();

	if (ImGui::Button("Delete"))
	{
		bFlagForDeletion = true;
	}
	ImGui::Text(("X: " + std::to_string(physicsComponent.trans.p.x)).c_str());
	ImGui::SameLine();
	ImGui::Text((" Y: " + std::to_string(physicsComponent.trans.p.y)).c_str());
	ImGui::SameLine();
	ImGui::Text((" Z: " + std::to_string(physicsComponent.trans.p.z)).c_str());


	if(physicsComponent.aActor || physicsComponent.aStaticActor || physicsComponent.isCharacter)
		ImGui::DragFloat3("pos", &offsetPos.x, 0.01f);
	else
		ImGui::DragFloat3("pos", &pos.x, 0.01f);

	ImGui::DragFloat("rotDir", &rotationDir, 0.01f);
	ImGui::DragFloat3("rot", &rot.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);

	static bool showHidden = false;
	ImGui::Checkbox("Show hidden", &showHidden);

	if (showHidden)
	{
		ImGui::Checkbox("isCharacter", &physicsComponent.isCharacter);
		ImGui::Checkbox("isPlayer", &isPlayer);
		ImGui::Checkbox("isAI", &isAI);
		ImGui::Checkbox("isWalkable", &isWalkable);
		ImGui::Checkbox("isObstacle", &isObstacle);
		ImGui::Checkbox("isAttached", &model.isAttached);
		ImGui::Checkbox("isEmissive", &isEmissive);
		if (ImGui::Button("Create Controller"))
		{
			bCreateController = true;
		}
		ImGui::Checkbox("Render", &bRender);

		ImGui::DragFloat3("modelPos", &modelPos.x, 0.01f);


		ImGui::DragFloat4("physics_rot", &physicsComponent.physics_rot.x, 0.01f);
		ImGui::DragFloat3("physics_scale", &physicsComponent.physics_scale.x, 0.01f);
		ImGui::DragFloat3("frustumScale", &frustumScale.x, 0.01f);
		ImGui::DragFloat3("emissiveColor", &emissiveColor.x, 0.01f);
		ImGui::DragFloat3("BoneRot", &model.BoneRot.x, 0.01f);

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
		


		if (isAnimated)
		{
			std::fstream f;

			bool open = false, save = false;

			//ImGui::SameLine();
			if (ImGui::BeginMenu("Anim Files"))
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
			}


		}
		if (model.isAttached)
		{
			if (ImGui::CollapsingHeader("Show entities"))
			{
				std::vector<const char*> _entitiesData;
				for (int i = 0; i < entities.size(); ++i)
				{
					if(entities[i].isAnimated)
						_entitiesData.push_back(entities[i].entityName.c_str());
				}
				static int listbox_current = -1;
				ImGui::ListBox("Entities", &listbox_current, _entitiesData.data(), _entitiesData.size());
			
				if (listbox_current > -1)
				{
					parentName = _entitiesData[listbox_current];
					for (int i = 0; i < entities.size(); ++i)
					{
						if (entities[i].entityName == _entitiesData[listbox_current])
						{
							parent = &entities[i];
						}
					}
				}
					
				
			}

			if (ImGui::CollapsingHeader("Show skeleton"))
			{
				if (parent)
				{
					std::vector<const char*> _bonesData;
					for (int i = 0; i < parent->model.boneNames.size(); ++i)
					{
						_bonesData.push_back(parent->model.boneNames[i].c_str());
					}
					static int listbox_current = 0;
					ImGui::ListBox("Skeleton", &listbox_current, _bonesData.data(), _bonesData.size());

					if(listbox_current > -1)
						attachedBone = _bonesData[listbox_current];
				}
			}
		}

		if (isAnimated)
		{
			if (ImGui::CollapsingHeader("Show skeleton"))
			{
				std::vector<const char*> _bonesData;
				for (int i = 0; i < model.boneNames.size(); ++i)
				{
					_bonesData.push_back(model.boneNames[i].c_str());
				}
				static int listbox_current = 0;
				ImGui::ListBox("Skeleton", &listbox_current, _bonesData.data(), _bonesData.size());
			}
		}
	}
}

void Entity::Input(Mouse& mouse, Keyboard& keyboard)
{
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
