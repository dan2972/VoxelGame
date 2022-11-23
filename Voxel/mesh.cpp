#include "mesh.h"

#include <glad/glad.h>
#include <iostream>

static const unsigned MAX_VERTICES = 50000;

Mesh::Mesh(const std::vector<unsigned>& format) {
	int vertexSize = 0;
	for (unsigned i : format)
		vertexSize += i;
	m_vbo = new float[MAX_VERTICES * vertexSize];
	m_ibo = new unsigned[MAX_VERTICES * 3];
	glGenVertexArrays(1, &m_vaoID);
	glGenBuffers(1, &m_vboID);
	glGenBuffers(1, &m_iboID);

	glBindVertexArray(m_vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_VERTICES * vertexSize, nullptr, GL_DYNAMIC_DRAW);

	unsigned sum = 0;
	for (int i = 0; i < format.size(); ++i) {
		sum += format[i];
	}
	for (int i = 0; i < format.size(); ++i) {
		if (i == 0) {
			//std::cout << "glVertexAttribPointer(" << i << ", " << format[i] << ", GL_FLOAT, GL_FALSE, " << sum << " * sizeof(float), (void*)0)";
			glVertexAttribPointer(i, format[i], GL_FLOAT, GL_FALSE, sum * sizeof(float), (void*)0);
		}
		else {
			int s = 0;
			for (int j = i - 1; j >= 0; --j) {
				s += format[j];
			}
			//std::cout << "glVertexAttribPointer(" << i << ", " << format[i] << ", GL_FLOAT, GL_FALSE, " << sum << " * sizeof(float), (void*)(" << s << " * sizeof(float)))";
			glVertexAttribPointer(i, format[i], GL_FLOAT, GL_FALSE, sum * sizeof(float), (void*)(s * sizeof(float)));
		}
		//std::cout << std::endl;
		glEnableVertexAttribArray(i);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * MAX_VERTICES * 3, nullptr, GL_DYNAMIC_DRAW);
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &m_vaoID);
	glDeleteBuffers(1, &m_vboID);
	glDeleteBuffers(1, &m_iboID);
	delete[] m_vbo;
	delete[] m_ibo;
}

void Mesh::start() {
	m_vboCounter = 0;
	m_iboCounter = 0;
	glBindVertexArray(m_vaoID);
}

void Mesh::end() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned) * (m_iboCounter + 1), m_ibo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * (m_vboCounter + 1), m_vbo);
}

void Mesh::render() {
	glBindVertexArray(m_vaoID);
	glDrawElements(GL_TRIANGLES, m_iboCounter + 1, GL_UNSIGNED_INT, nullptr);
}

unsigned Mesh::addVertex(const std::vector<float>& vertex) {
	for (float value : vertex)
		m_vbo[m_vboCounter++] = value;
	return (m_vboCounter + 1) / vertex.size() - 1;
}

void Mesh::addTriangle(const std::vector<unsigned>& vertices) {
	for (unsigned vertex : vertices)
		m_ibo[m_iboCounter++] = vertex;
}