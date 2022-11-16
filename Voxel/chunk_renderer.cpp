#include "chunk_renderer.h"

ChunkRenderer::ChunkRenderer(const Chunk& chunk) {
	m_chunk = &chunk;
}

bool shouldRender(const Chunk& chunk, unsigned x, unsigned y, unsigned z) {
	if (x == 0 || y == 0 || z == 0 || x == Chunk::CHUNK_SIZE - 1 || y == Chunk::CHUNK_SIZE - 1 || z == Chunk::CHUNK_SIZE - 1)
		return true;
	if (chunk.getBlockAt(x - 1, y, z) == Air ||
		chunk.getBlockAt(x + 1, y, z) == Air ||
		chunk.getBlockAt(x, y - 1, z) == Air ||
		chunk.getBlockAt(x, y + 1, z) == Air ||
		chunk.getBlockAt(x, y, z - 1) == Air ||
		chunk.getBlockAt(x, y, z + 1) == Air)
		return true;
	return false;
}

void ChunkRenderer::generateMesh(Mesh& mesh) {
	mesh.start();
	for (unsigned y = 0; y < Chunk::CHUNK_SIZE; ++y) {
		for (unsigned z = 0; z < Chunk::CHUNK_SIZE; ++z) {
			for (unsigned x = 0; x < Chunk::CHUNK_SIZE; ++x) {

				if (m_chunk->getBlockAt(x, y, z) != Air && shouldRender(*m_chunk, x, y, z)) {
					unsigned int v1 = mesh.addVertex({ x-0.5f, y-0.5f, z+0.5f,  1.0f, 0.0f, 0.0f });
					unsigned int v2 = mesh.addVertex({ x+0.5f, y-0.5f, z+0.5f,   0.0f, 1.0f, 0.0f });
					unsigned int v3 = mesh.addVertex({ x+0.5f, y+0.5f, z+0.5f,    0.0f, 0.0f, 1.0f });
					unsigned int v4 = mesh.addVertex({ x-0.5f, y+0.5f, z+0.5f,   0.0f, 1.0f, 1.0f });
					unsigned int v5 = mesh.addVertex({ x-0.5f, y-0.5f, z-0.5f, 1.0f, 0.0f, 0.0f });
					unsigned int v6 = mesh.addVertex({ x+0.5f, y-0.5f, z-0.5f,  0.0f, 1.0f, 0.0f });
					unsigned int v7 = mesh.addVertex({ x+0.5f, y+0.5f, z-0.5f,   0.0f, 0.0f, 1.0f });
					unsigned int v8 = mesh.addVertex({ x-0.5f, y+0.5f, z-0.5f,  0.0f, 1.0f, 1.0f });
					//front
					mesh.addTriangle({ v1, v2, v3 });
					mesh.addTriangle({ v1, v3, v4 });
					//left
					mesh.addTriangle({ v5, v1, v4 });
					mesh.addTriangle({ v5, v4, v8 });
					//right
					mesh.addTriangle({ v2, v6, v7 });
					mesh.addTriangle({ v2, v7, v3 });
					//back
					mesh.addTriangle({ v6, v5, v8 });
					mesh.addTriangle({ v6, v8, v7 });
					//up
					mesh.addTriangle({ v4, v3, v7 });
					mesh.addTriangle({ v4, v7, v8 });
					//down
					mesh.addTriangle({ v5, v6, v2 });
					mesh.addTriangle({ v5, v2, v1 });
				}
			}
		}
	}
	mesh.end();
}