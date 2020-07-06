#pragma once
#include "RenderInstance.h"
#include "Shader.h"
#include "AShaderStorageBufferObject.h"
#include <glm\glm.hpp>

#include <EngineCore\Logger.h>
#include <cstring>

namespace render
{

	class SoftBodyShader : public Shader
	{
	public:

		class SoftBodySSBO : public AShaderStorageBufferObject
		{
		private:
			int count;

		public:
			struct DataStruct
			{
				glm::mat4 TEMP0 = glm::mat4();
				glm::mat4 TEMP1 = glm::mat4();
				glm::vec4 color = glm::vec4(1.0, 1.0, 1.0, 1.0);
				//glm::ivec4 texid = glm::ivec4(0, 0, 0, 0);
			} *data;

			SoftBodySSBO(GLsizei size) : AShaderStorageBufferObject(size * sizeof(DataStruct))
			{
				data = new DataStruct[size];
				count = 0;
			}

			~SoftBodySSBO()
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


		SoftBodyShader(RenderCache *cache, int n, SCInfo fragment_info[]) : Shader(cache)
		{
			ssbo = new SoftBodySSBO(1000);

			initShaderAndFragments(n, fragment_info);

			glUseProgram(program);

			ssbo->attach(program, glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, "ssbo_data"), 2);
			ssbo->bind(2);
		}

		~SoftBodyShader()
		{
			delete ssbo;
		}

		virtual void useProgram() override
		{
			glUseProgram(program);
			ssbo->bind(2);
		}

		SoftBodySSBO* getSSBO() { return (SoftBodySSBO*)ssbo; }

	};
}

