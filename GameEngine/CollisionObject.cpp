#include "CollisionObject.h"

CollisionObject::CollisionObject()
{
	offsetPos = DirectX::XMFLOAT3(0, 0, 0);
	bRender = true;
}

void CollisionObject::Initialize(ID3D11Device* device)
{
	cube.Initialize(device);

}

void CollisionObject::CreatePhysicsComponent(physx::PxPhysics& physics, physx::PxScene& scene)
{
	physicsComponent.mass = 0.0f;
	physicsComponent.CreateCube(physics, scene, physx::PxVec3(cube.scale.x, cube.scale.y, cube.scale.z), physx::PxVec3(cube.pos.x, cube.pos.y, cube.pos.z));

	physicsComponent.aStaticActor->setName(entityName.c_str());
}

void CollisionObject::Draw(ID3D11DeviceContext* deviceContext, Camera& camera, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
		if (cube.vertexBuffer.Get() && cube.indexBuffer.Get())
		{
			DirectX::XMMATRIX matrix_scale;
			DirectX::XMMATRIX matrix_rotate;
			DirectX::XMMATRIX matrix_translate;
			DirectX::XMMATRIX worldMatrix;

			if (physicsComponent.aStaticActor)
			{
				physicsComponent.trans = physicsComponent.aStaticActor->getGlobalPose();

				physicsComponent.trans.p.x += offsetPos.x;
				physicsComponent.trans.p.y += offsetPos.y;
				physicsComponent.trans.p.z += offsetPos.z;
				offsetPos = DirectX::XMFLOAT3(0, 0, 0);

				if (bRender)
				{
					//matrix_scale = DirectX::XMMatrixScaling(cube.scale.x, cube.scale.y, cube.scale.z);
					matrix_scale = DirectX::XMMatrixScaling(physicsComponent.physics_scale.x, physicsComponent.physics_scale.y, physicsComponent.physics_scale.z);
					matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(cube.rot.x, cube.rot.y, cube.rot.z);
					matrix_rotate *= DirectX::XMMatrixRotationAxis(DirectX::XMVECTOR{ physicsComponent.trans.q.x,physicsComponent.trans.q.y, physicsComponent.trans.q.z }, physicsComponent.trans.q.getAngle());
					matrix_translate = DirectX::XMMatrixTranslation(physicsComponent.trans.p.x, physicsComponent.trans.p.y, physicsComponent.trans.p.z);
					worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
				}
				

				physicsComponent.trans.q += physx::PxQuat(physicsComponent.physics_rot.w, physx::PxVec3(physicsComponent.physics_rot.x, physicsComponent.physics_rot.y, physicsComponent.physics_rot.z));
				physicsComponent.aStaticActor->setGlobalPose(physicsComponent.trans);
				
			}
			else
			{
				if (bRender)
				{
					matrix_scale = DirectX::XMMatrixScaling(cube.scale.x, cube.scale.y, cube.scale.z);
					matrix_rotate = DirectX::XMMatrixRotationRollPitchYaw(cube.rot.x, cube.rot.y, cube.rot.z);
					matrix_translate = DirectX::XMMatrixTranslation(cube.pos.x, cube.pos.y, cube.pos.z);

					worldMatrix = matrix_scale * matrix_rotate * matrix_translate;
				}
			}
			
			if (bRender)
			{
				deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				UINT stride = sizeof(Vertex);
				UINT offset = 0;
				deviceContext->IASetVertexBuffers(0, 1, cube.vertexBuffer.GetAddressOf(), &stride, &offset);
				deviceContext->IASetIndexBuffer(cube.indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

				deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetBuffer().GetAddressOf());
				cb_vs_vertexshader.data.projectionMatrix = DirectX::XMMatrixTranspose(camera.GetProjectionMatrix());
				cb_vs_vertexshader.data.viewMatrix = DirectX::XMMatrixTranspose(camera.GetViewMatrix());
				cb_vs_vertexshader.data.worldMatrix = matrix_rotate * matrix_scale * matrix_translate;
				cb_vs_vertexshader.data.worldMatrix = DirectX::XMMatrixTranspose(cb_vs_vertexshader.data.worldMatrix);

				cb_vs_vertexshader.UpdateBuffer();

				deviceContext->DrawIndexed(cube.indexBuffer.IndexCount(), 0, 0);
			}
			
		}
}

void CollisionObject::DrawGUI(std::string name)
{
	ImGui::Checkbox("Render", &bRender);
	cube.DrawGUI(name);
	ImGui::DragFloat3("offsetPos", &offsetPos.x, 0.01);
	ImGui::DragFloat4("physics_rot", &physicsComponent.physics_rot.x, 0.01f);
	ImGui::DragFloat3("physics_scale", &physicsComponent.physics_scale.x, 0.01f);
}
