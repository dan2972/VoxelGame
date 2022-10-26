#include "renderer.h"

#include <glad/glad.h>
#include <iostream>

static const unsigned MAX_VERTICES = 40000;

Renderer::~Renderer() {
	for (unsigned id : m_vaoIDList) {
		glDeleteVertexArrays(1, &id);
	}
	for (unsigned id : m_vboIDList) {
		glDeleteBuffers(1, &id);
	}
	for (unsigned id : m_iboIDList) {
		glDeleteBuffers(1, &id);
	}
}

unsigned Renderer::createMesh(const std::vector<unsigned>& format) {
	m_vaoIDList.push_back(0);
	m_vboIDList.push_back(0);
	m_iboIDList.push_back(0);
	m_VBO.push_back(std::vector<float>());
	m_IBO.push_back(std::vector<unsigned>());
	glGenVertexArrays(1, &m_vaoIDList[meshIDCounter]);
	glGenBuffers(1, &m_vboIDList[meshIDCounter]);
	glGenBuffers(1, &m_iboIDList[meshIDCounter]);

	glBindVertexArray(m_vaoIDList[meshIDCounter]);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboIDList[meshIDCounter]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_VERTICES, nullptr, GL_DYNAMIC_DRAW);

	unsigned sum = 0;
	for (int i = 0; i < format.size(); ++i) {
		sum += format[i];
	}
	for (int i = 0; i < format.size(); ++i) {
		if (i == 0) {
			std::cout << "glVertexAttribPointer(" << i << ", " << format[i] << ", GL_FLOAT, GL_FALSE, " << sum << " * sizeof(float), (void*)0)";
			glVertexAttribPointer(i, format[i], GL_FLOAT, GL_FALSE, sum * sizeof(float), (void*)0);
		}
		else {
			std::cout << "glVertexAttribPointer(" << i << ", " << format[i] << ", GL_FLOAT, GL_FALSE, " << sum << " * sizeof(float), " << format[i - 1] << ")";
			glVertexAttribPointer(i, format[i], GL_FLOAT, GL_FALSE, sum * sizeof(float), (void*)(format[i - 1] * sizeof(float)));
		}
		std::cout << std::endl;
		glEnableVertexAttribArray(i);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboIDList[meshIDCounter]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * MAX_VERTICES * 3, nullptr, GL_DYNAMIC_DRAW);

	return meshIDCounter++;
}

void Renderer::print(int meshID) {
	for (float i : m_VBO[meshID]) {
		std::cout << i << ", ";
	}
	std::cout << std::endl;
	for (unsigned i : m_IBO[meshID]) {
		std::cout << i << ", ";
	}
	std::cout << std::endl;
}

void Renderer::startMesh(int meshID) {
	m_VBO[meshID].clear();
	m_IBO[meshID].clear();
}

void Renderer::endMesh(int meshID) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboIDList[meshID]);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned) * m_IBO[meshID].size(), m_IBO[meshID].data());

	glBindBuffer(GL_ARRAY_BUFFER, m_vboIDList[meshID]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_VBO[meshID].size(), m_VBO[meshID].data());
}

void Renderer::renderMesh(int meshID) {
	glBindVertexArray(m_vaoIDList[meshID]);
	glDrawElements(GL_TRIANGLES, m_IBO[meshID].size(), GL_UNSIGNED_INT, nullptr);
}

unsigned Renderer::addVertexToMesh(int meshID, const std::vector<float>& vertex) {
	for (float value : vertex)
		m_VBO[meshID].push_back(value);
	return m_VBO[meshID].size() / vertex.size() - 1;
}

void Renderer::addTriangleToMesh(int meshID, const std::vector<unsigned>& vertices) {
	for (unsigned vertex : vertices)
		m_IBO[meshID].push_back(vertex);
}