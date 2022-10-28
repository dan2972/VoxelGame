#pragma once

#include <vector>

class Mesh
{
public:
	// Generates a new mesh and returns the id
	// Enter in an array to define the VAO
	// For example, if you want a position, color, and texture, 
	// then the example format would be: [3, 3, 2]
	Mesh(const std::vector<unsigned>& format);
	~Mesh();

	// Returns the index of the vertex added to the mesh
	unsigned addVertex(const std::vector<float>& vertex);

	// Enter the indicies of the verticies to create a triangle
	// for the index buffer
	void addTriangle(const std::vector<unsigned>& vertices);

	void render();

	void start();

	void end();
private:
	unsigned m_vaoID, m_vboID, m_iboID;
	float* m_vbo;
	unsigned* m_ibo;
	unsigned m_vboCounter = 0;
	unsigned m_iboCounter = 0;
};

