#pragma once
#include "chunk.h"
#include "mesh.h"
class ChunkRenderer
{
public:
	ChunkRenderer(const Chunk& chunk);

	void generateMesh(Mesh& mesh);
private:
	const Chunk* m_chunk = nullptr;
};

