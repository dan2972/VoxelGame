#include "chunk_renderer.h"
#include "resource_manager.h"

ChunkRenderer::ChunkRenderer(const ChunkMap& chunkMap) {
	m_chunkMap = &chunkMap;
}

bool shouldRenderLeft(const Chunk& chunk, unsigned x, unsigned y, unsigned z) {
	return x == 0 || chunk.getBlockAt(x - 1, y, z) == Air;
}

bool shouldRenderRight(const Chunk& chunk, unsigned x, unsigned y, unsigned z) {
	return x == Chunk::CHUNK_SIZE - 1 || chunk.getBlockAt(x + 1, y, z) == Air;
}

bool shouldRenderDown(const Chunk& chunk, unsigned x, unsigned y, unsigned z) {
	return y == 0 || chunk.getBlockAt(x, y - 1, z) == Air;
}

bool shouldRenderUp(const Chunk& chunk, unsigned x, unsigned y, unsigned z) {
	return y == Chunk::CHUNK_SIZE - 1 || chunk.getBlockAt(x, y + 1, z) == Air;
}

bool shouldRenderBack(const Chunk& chunk, unsigned x, unsigned y, unsigned z) {
	return z == 0 || chunk.getBlockAt(x, y, z - 1) == Air;
}

bool shouldRenderFront(const Chunk& chunk, unsigned x, unsigned y, unsigned z) {
	return z == Chunk::CHUNK_SIZE - 1 || chunk.getBlockAt(x, y, z + 1) == Air;
}

void ChunkRenderer::drawChunk(int chunkX, int chunkZ) {
	Chunk& chunk = m_chunkMap->getChunk(chunkX, chunkZ);
	if (m_meshMap.find({ chunkX, chunkZ }) == m_meshMap.end()) {
		Mesh* mesh = new Mesh{ {3, 2} };
		m_meshMap.emplace(ChunkCoord{ chunkX, chunkZ }, std::unique_ptr<Mesh>(mesh));
		generateMesh(chunk, *mesh);
		mesh->render();
	}
	else {
		m_meshMap.at({ chunkX, chunkZ }).get()->render();
	}
}

void ChunkRenderer::generateMesh(const Chunk& chunk, Mesh& mesh) {
	mesh.start();
	for (unsigned y = 0; y < Chunk::CHUNK_SIZE; ++y) {
		for (unsigned z = 0; z < Chunk::CHUNK_SIZE; ++z) {
			for (unsigned x = 0; x < Chunk::CHUNK_SIZE; ++x) {

				int shiftX = chunk.getChunkX() * Chunk::CHUNK_SIZE;
				int shiftZ = chunk.getChunkZ() * Chunk::CHUNK_SIZE;

				if (chunk.getBlockAt(x, y, z) != Air) {

					Texture2D texture = ResourceManager::getTexture("grass");
					texture.bind();

					if (shouldRenderFront(chunk, x, y, z)) {
						unsigned int v1 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z + 0.5f, 0.0f, 0.0f });
						unsigned int v2 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z + 0.5f, 1.0f, 0.0f });
						unsigned int v3 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z + 0.5f, 1.0f, 1.0f });
						unsigned int v4 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z + 0.5f, 0.0f, 1.0f });

						//front
						mesh.addTriangle({ v1, v2, v3 });
						mesh.addTriangle({ v1, v3, v4 });
					}

					if (shouldRenderBack(chunk, x, y, z)) {
						unsigned int v5 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z - 0.5f, 1.0f, 0.0f });
						unsigned int v6 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z - 0.5f, 0.0f, 0.0f });
						unsigned int v7 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z - 0.5f, 0.0f, 1.0f });
						unsigned int v8 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z - 0.5f, 1.0f, 1.0f });
						// back
						mesh.addTriangle({ v6, v5, v8 });
						mesh.addTriangle({ v6, v8, v7 });
					}

					if (shouldRenderLeft(chunk, x, y, z)) {
						unsigned int v1 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z + 0.5f, 1.0f, 0.0f });
						unsigned int v4 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z + 0.5f, 1.0f, 1.0f });
						unsigned int v5 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z - 0.5f, 0.0f, 0.0f });
						unsigned int v8 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z - 0.5f, 0.0f, 1.0f });

						//left
						mesh.addTriangle({ v5, v1, v4 });
						mesh.addTriangle({ v5, v4, v8 });
					}
					
					if (shouldRenderRight(chunk, x, y, z)) {
						unsigned int v2 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z + 0.5f, 0.0f, 0.0f });
						unsigned int v3 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z + 0.5f, 0.0f, 1.0f });
						unsigned int v6 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z - 0.5f, 1.0f, 0.0f });
						unsigned int v7 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z - 0.5f, 1.0f, 1.0f });

						//right
						mesh.addTriangle({ v2, v6, v7 });
						mesh.addTriangle({ v2, v7, v3 });
					}

					if (shouldRenderUp(chunk, x, y, z)) {
						unsigned int v3 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z + 0.5f, 1.0f, 0.0f });
						unsigned int v4 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z + 0.5f, 0.0f, 0.0f });
						unsigned int v7 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z - 0.5f, 1.0f, 1.0f });
						unsigned int v8 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z - 0.5f, 0.0f, 1.0f });
						//up
						mesh.addTriangle({ v4, v3, v7 });
						mesh.addTriangle({ v4, v7, v8 });
					}

					if (shouldRenderDown(chunk, x, y, z)) {
						unsigned int v1 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z + 0.5f, 0.0f, 1.0f });
						unsigned int v2 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z + 0.5f, 1.0f, 1.0f });
						unsigned int v5 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z - 0.5f, 0.0f, 0.0f });
						unsigned int v6 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z - 0.5f, 1.0f, 0.0f });
						//down
						mesh.addTriangle({ v5, v6, v2 });
						mesh.addTriangle({ v5, v2, v1 });
					}
				}
			}
		}
	}
	mesh.end();
}