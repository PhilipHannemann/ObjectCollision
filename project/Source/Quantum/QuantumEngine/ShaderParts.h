#pragma once
#include "RenderInstance.h"
#include "Shader.h"
#include "AShaderStorageBufferObject.h"
#include <glm\glm.hpp>

namespace editor
{

	class ShaderParts : public render::Shader
	{
	public:

		class ShaderPartsSSBO : public render::AShaderStorageBufferObject
		{
		private:
			int count;

		public:
			struct DataStruct
			{
				glm::mat4 model_matrix;
				glm::mat4 normal_matrix;
				glm::vec4 color = glm::vec4(1.0, 1.0, 1.0, 1.0);
			} *data;

			ShaderPartsSSBO(GLsizei size) : render::AShaderStorageBufferObject(size * sizeof(DataStruct))
			{
				data = new DataStruct[size];
				count = 0;
			}

			~ShaderPartsSSBO()
			{
				delete[] data;
			}

			void updateAll() override
			{
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 1000 * sizeof(DataStruct), data);
			}

			void updateRange(GLuint start, GLuint count) override
			{
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, start * sizeof(DataStruct), count * sizeof(DataStruct), data + start);
			}

			int addBlock() override
			{
				if (count * sizeof(DataStruct) > size)
				{
					this->resize(count + 100, true);
				}
				count++;
				return count - 1;
			}

			void removeBlock(int id) override
			{
				/*TODO*/

			}

		};


		ShaderParts(render::RenderCache *cache, int n, render::SCInfo fragment_info[]) : render::Shader(cache)
		{
			ssbo = new ShaderPartsSSBO(1000);

			initShaderAndFragments(n, fragment_info);

			glUseProgram(program);

			ssbo->attach(program, glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "ssbo_data"), 2);
			ssbo->bind(2);
		}

		~ShaderParts()
		{
			delete ssbo;
		}

		virtual void useProgram() override
		{
			glUseProgram(program);
			ssbo->bind(2);
		}

		ShaderParts* getSSBO() { return (ShaderParts*)ssbo; }

	};
}
