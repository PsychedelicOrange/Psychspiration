#include<Renderer.h>
// insert functions

void Renderer::setUpEvents()
{
	input.registerKeyCallback("RELOAD_SHADER_KEY", 1, [this]() {
		updateShaders();
		});
}

void Renderer::insertObjects(vector<RObject*>& objects)
{
	for (auto obj : objects)
	{
		if (obj->dynamic)
			this->dynamicObjects.push_back(obj);
		else
			this->staticObjects.push_back(obj);
	}

}
 void Renderer::updateShaders()
 {
	 std::cout << "Updating Shaders...";
	 delete *shaders.begin();
	 shaders.pop_back();
	 Shader* shader = new Shader("pbr_baked_diffuse.vs", "pbr_baked_diffuse.fs", "0");
	 shaders.push_back(shader);
 }
 void Renderer::innit()
{
	 //setUpEvents
	setUpEvents();
	// opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);

	// compile shaders variations
	//Shader* shader= new Shader("pbr_new.vs", "pbr_new.fs", "0");
	Shader* shader = new Shader("pbr_baked_diffuse.vs", "pbr_baked_diffuse.fs", "0");
	shaders.push_back(shader);

	// init all framebuffers and renderbuffers, shadow maps, HDR maps etc. 
	plightBuffer.innit();
	plightBuffer.updateBuffer(pLightsLive);

	prepareMeshes();
	// init VAOs and VBOs
	initVABOs();
}

 void Renderer::prepareMeshes()
{
	// using staticObjects, populate staticMeshes vector and set transforms for each mesh 
	for (auto object : staticObjects)
	{
		for (auto mesh : object->model->meshes)
		{
			mesh->transform = &object->transform;
			staticMeshes.push_back(mesh);
		}
	}
}

 void Renderer::initVABOs()
{
	glGenVertexArrays(5, &vaos[0]);
	glGenBuffers(5, &vbos[0]);

	glBindVertexArray(vaos[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);

	int total_verts = 0;
	for (auto mesh : staticMeshes)
	{
		total_verts += mesh->vertices.size();
	}
	glBufferData(GL_ARRAY_BUFFER, total_verts * sizeof(Vertex), 0, GL_STATIC_DRAW);
	for (int i = 0; i < staticMeshes.size(); i++)
	{
		if (i == 0)
		{
			//std::cout << "count of mesh 0 verts : " << meshes[0]->vertices.size();
			staticMeshes[0]->VBOoffset = 0;
			staticMeshes[0]->baseVertex = 0;
			glBufferSubData(GL_ARRAY_BUFFER, staticMeshes[0]->VBOoffset, staticMeshes[0]->vertices.size() * sizeof(Vertex), &(staticMeshes[0]->vertices[0]));
		}
		else
		{
			int prev_vert_count = staticMeshes[i - 1]->vertices.size();
			int prev_size = prev_vert_count * sizeof(Vertex);
			staticMeshes[i]->baseVertex = staticMeshes[i - 1]->baseVertex + prev_vert_count;
			staticMeshes[i]->VBOoffset = staticMeshes[i - 1]->VBOoffset + prev_size;
			glBufferSubData(GL_ARRAY_BUFFER, staticMeshes[i]->VBOoffset, staticMeshes[i]->vertices.size() * sizeof(Vertex), &(staticMeshes[i]->vertices[0]));
		}
	}
	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex texture coords2
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords2));
	// vertex tangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	glBindVertexArray(0);
}

 void Renderer::drawStatic()
{
	auto shader = shaders[0];
	shader->use();
	//doIfDirty();
	setGlobalUniforms(shader); // view, projection, lights
	glBindVertexArray(vaos[0]);
	for (auto obj : staticObjects)
	{
		for (auto mesh : obj->model->meshes)
		{
			// set transform
			shader->setMat4("model", *(mesh->transform));
			setMeshUniforms(mesh, shader); // transforms and materials
										   //glBindVertexArray(mesh->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
			glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0, mesh->baseVertex);
			//glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
			glActiveTexture(GL_TEXTURE0);
		}
	}

	//for (auto mesh : staticMeshes)
	//{
	//	// set transform
	//	shader->setMat4("model", *(mesh->transform));
	//	setMeshUniforms(mesh, shader); // transforms and materials
	//	//glBindVertexArray(mesh->VAO);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
	//	glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0, mesh->baseVertex);
	//	//glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	//	glActiveTexture(GL_TEXTURE0);
	//}
	glBindVertexArray(0);
}

 void Renderer::draw(RObject* obj)
{
	auto shader = shaders[0];
	shader->setMat4("model", obj->transform);
	for (auto mesh : obj->model->meshes)
	{
		setMeshUniforms(mesh, shader);
		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
		// always good practice to set everything back to defaults once configured.
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
	}
}

 void Renderer::draw(vector<RObject*> objects)
{
	auto shader = shaders[0];
	shader->use();
	setGlobalUniforms(shader);
	for (auto obj : objects)
		draw(obj);
}

 void Renderer::drawInstanced()
{

	//glDrawElementsInstancedBaseVertex(GL_TRIANGLES,mesh->indices.size(),GL_UNSIGNED_INT,0,10,mesh->baseVertex)
}

//void draw()
//{
//	for (int i =0 ; i < 3; i++)
//	{
//		shaders[i].use();
//		setGlobalUniforms(shaders[i]);
//		for (auto vao : vaos[shaders[i]])
//		{
//			glBindVertexArray(vao);
//			for (RMesh mesh : vao)
//			{
//				glDrawElementsBaseVertex(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, mesh.offsetIntoVBO);
//			}
//			
//		}
//	}
//}

 void Renderer::setGlobalUniforms(Shader* shader)
{
	// set lights buffer, skymaps , shadow maps , etc.
	// set camera uniforms like veiw projections
	shader->setInt("numPLights", pLightsLive.size());

	shader->setVec3("viewPos", camerasLive[0]->Position);
	shader->setMat4("view", camerasLive[0]->getView());
	shader->setMat4("projection", camerasLive[0]->projection);
}

 void Renderer::setMeshUniforms(RMesh* mesh, Shader* shader)
{

	// set materials and textures
	auto material = mesh->material;
	if (material.TexAlbedo.empty())
	{
		shader->setBool("existdiffuse", false);
		shader->setVec3("u_albedo", material.Albedo);
	}
	else
	{
		shader->setBool("existdiffuse", true);
		shader->setInt("texture_diffuse", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.TexAlbedo[0]->ID);
	}
	if (material.TexNormal.empty())
	{
		shader->setBool("existnormals", false);
	}
	else
	{
		shader->setBool("existnormals", true);
		shader->setInt("texture_normal", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.TexNormal[0]->ID);
	}
	if (material.TexRoughMetal.empty())
	{
		shader->setBool("existroughmetal", false);
		shader->setFloat("u_metallic", material.Metallic);
		shader->setFloat("u_roughness", material.Roughness);
	}
	else
	{
		shader->setBool("existroughmetal", true);
		shader->setInt("texture_roughmetal", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material.TexRoughMetal[0]->ID);
	}
	if (material.TexEmmisive.empty())
	{
		shader->setBool("existemmisive", false);
		shader->setVec3("u_emmisive", material.Emmisive);
	}
	else
	{
		shader->setBool("existemmisive", true);
		shader->setInt("texture_emmisive", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, material.TexEmmisive[0]->ID);
	}
	shader->setBool("existOpacity", (material.aplhaType != AlphaType::OPAQUE));
	shader->setBool("existClip", (material.aplhaType == AlphaType::MASK));
}
