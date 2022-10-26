#pragma once

#include <vector>

class Renderer
{
public:
	Renderer() = default;
	~Renderer();

	// Generates a new mesh and returns the id
	// Enter in an array to define the VAO
	// For example, if you want a position, color, and texture, 
	// then the example format would be: [3, 3, 2]
	unsigned createMesh(const std::vector<unsigned>& format);

	// Returns the index of the vertex added to the mesh
	unsigned addVertexToMesh(int meshID, const std::vector<float>& vertex);

	// Enter the indicies of the verticies to create a triangle
	// for the index buffer
	void addTriangleToMesh(int meshID, const std::vector<unsigned>& vertices);

	void renderMesh(int meshID);

	void startMesh(int meshID);

	void endMesh(int meshID);

	void print(int meshID);
private:
	unsigned meshIDCounter = 0;
	std::vector<unsigned> m_vaoIDList;
	std::vector<unsigned> m_vboIDList;
	std::vector<unsigned> m_iboIDList;
	std::vector<std::vector<float>> m_VBO;
	std::vector<std::vector<unsigned>> m_IBO;
};

