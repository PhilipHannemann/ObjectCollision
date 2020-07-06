#include "stdafx.h"
#include "SoftBodyCompactNode.h"
#include <PhysicEngine\PhysicsWorld.h>
#include <EngineCore\BaseExceptions.h>
#include "RenderCache.h"
#include "SceneGraph.h"

namespace scenegraph
{

	SoftBodyCompactNode::SoftBodyCompactNode(physics::DynamicsWorld *world, loader::SoftBodyModelWrapper * ci, float mass, glm::mat4 transform, physics::deformation_mode dmode)
	{
		added = false;

		sbmw = ci;

		bool isDynamic = (mass != 0.f);
		
		std::vector<glm::vec3> mesh(ci->data);

		//make transform
		for (int i = 0; i < mesh.size(); i++)
		{
			mesh[i] = glm::vec3(transform * glm::vec4(mesh[i], 1.0));
		}

		softbody = new physics::SoftBody(mass, mesh, dmode, ci->hull->copyAndTransform(transform), ci->clusters);
		softbody->userptr = this;

		cache_world = world;

		renderinstance = new render::RenderInstance<render::SoftBodyShader::SoftBodySSBO>(ci->stdro);
	}


	SoftBodyCompactNode::~SoftBodyCompactNode()
	{
		sbmw->vao->dequeInstance(softbody);
		cache_world->removeCollisionBody(softbody);
		//delete softbody;
		delete renderinstance;
	}

	void SoftBodyCompactNode::addedToSceneGraph()
	{
		if (added)
		{
			throw new SceneGraphConstructingException("PhysicTransformationNode can't be added multiple time! Copy it before adding!");
		}

		cache_world->addCollisionBody(softbody);

		sbdid = sbmw->vao->enqueInstance(softbody);
		
		added = true;
	}

	void SoftBodyCompactNode::update(StateBuffer * state)
	{
		SceneNode::update(state);
		//sbmw->update(softbody);
		sbmw->vao->updateSoftBodyDataID(softbody, sbdid);
		//TODO
		renderinstance->indizes.z = sbdid;
		state->rcache->enqueueInstance(renderinstance);
		//((loader::SoftBodyVertexData *)ro->vertexData)->update((physics::SoftBody *) rbody);
		//ro->modelMatrix = this->transformationMatrix;
		//SceneNode::update(state);	
	}

	void SoftBodyCompactNode::setColor(glm::vec4 color)
	{
		auto t = renderinstance->getSSBO();
		t->data[renderinstance->ssboid].color = color;
	}

	glm::vec4 SoftBodyCompactNode::getColor()
	{
		auto t = renderinstance->getSSBO();
		return t->data[renderinstance->ssboid].color;
	}

	/*void SoftBodyCompactNode::render(render::DrawPass * pass)
	{
		pass->renderObject(ro);
		SceneNode::render(pass);

		MySceneRenderer* render = ((MySceneRenderer*)ResourceManager::getInstance()->getSDLCore()->getScene());


		bool usePoints = render->debug->showPoints();
		bool useLines = render->debug->showLines();
		bool noDepth = render->debug->ignoreDepth();

		if(noDepth)
			glDisable(GL_DEPTH_TEST);

		if (usePoints)
		{
			int numpoints = ((physics::SoftBodyChildShape*)this->getShape())->getNumPoints();

			float* data = new float[numpoints * 3];

			for (int i = 0; i < numpoints; i++)
			{
				auto vec = ((physics::SoftBodyChildShape*)this->getShape())->getScaledPoint(i);
				data[i * 3 + 0] = vec.getX();
				data[i * 3 + 1] = vec.getY();
				data[i * 3 + 2] = vec.getZ();
			}

			glEnable(GL_POINT_SMOOTH);
			glPointSize(6.0f);
			ResourceManager::getInstance()->getSDLCore()->getDebugRender()->drawData(data, numpoints, GL_POINTS, 0, numpoints, glm::vec4(0.0,1.0,1.0,1.0));

			delete[] data;

		}
		if(useLines)
		{
			int edges = ((physics::SoftBodyChildShape*)this->getShape())->getNumEdges() ;
			float* data = new float[edges * 3 * 2];
			for (int i = 0; i < edges; i++)
			{
				btVector3 vecA, vecB;
				((physics::SoftBodyChildShape*)this->getShape())->getEdge(i, vecA, vecB);
				data[i * 6 + 0] = vecA.getX();
				data[i * 6 + 1] = vecA.getY();
				data[i * 6 + 2] = vecA.getZ();
				data[i * 6 + 3] = vecB.getX();
				data[i * 6 + 4] = vecB.getY();
				data[i * 6 + 5] = vecB.getZ();
			}

			glLineWidth(3.0F);
			ResourceManager::getInstance()->getSDLCore()->getDebugRender()->drawData(data, edges * 2, GL_LINES, 0, edges * 2, glm::vec4(0.0, 1.0, 1.0, 1.0));

			delete[] data;

		}
	}*/

}
