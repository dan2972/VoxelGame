#include "chunk_renderer.h"
#include "resource_manager.h"

ChunkRenderer::ChunkRenderer(const ChunkMap& chunkMap) {
	m_chunkMap = &chunkMap;
}

bool ChunkRenderer::shouldRenderLeft(const Chunk& chunk, unsigned worldX, unsigned worldY, unsigned worldZ) {
	return m_chunkMap->getBlockAt(worldX - 1, worldY, worldZ) == Air;
}

bool ChunkRenderer::shouldRenderRight(const Chunk& chunk, unsigned worldX, unsigned worldY, unsigned worldZ) {
	return m_chunkMap->getBlockAt(worldX + 1, worldY, worldZ) == Air;
}

bool ChunkRenderer::shouldRenderDown(const Chunk& chunk, unsigned worldX, unsigned worldY, unsigned worldZ) {
	return worldY == 0 || m_chunkMap->getBlockAt(worldX, worldY - 1, worldZ) == Air;
}

bool ChunkRenderer::shouldRenderUp(const Chunk& chunk, unsigned worldX, unsigned worldY, unsigned worldZ) {
	return worldY == Chunk::CHUNK_SIZE - 1 || m_chunkMap->getBlockAt(worldX, worldY + 1, worldZ) == Air;
}

bool ChunkRenderer::shouldRenderBack(const Chunk& chunk, unsigned worldX, unsigned worldY, unsigned worldZ) {
	return m_chunkMap->getBlockAt(worldX, worldY, worldZ - 1) == Air;
}

bool ChunkRenderer::shouldRenderFront(const Chunk& chunk, unsigned worldX, unsigned worldY, unsigned worldZ) {
	return m_chunkMap->getBlockAt(worldX, worldY, worldZ + 1) == Air;
}

void ChunkRenderer::drawChunk(int chunkX, int chunkZ) {
	Chunk* chunk = m_chunkMap->getChunk(chunkX, chunkZ);
	if (chunk != nullptr) {
		generateLightMap(*chunk);
		if (m_meshMap.find({ chunkX, chunkZ }) == m_meshMap.end()) {
			Mesh* mesh = new Mesh{ {3, 2, 1} };
			m_meshMap.emplace(ChunkMap::ChunkCoord{ chunkX, chunkZ }, std::unique_ptr<Mesh>(mesh));
			generateMesh(*chunk, *mesh);
			mesh->render();
		}
		else {
			if (chunk->requestedUpdate) {
				generateMesh(*chunk, *m_meshMap.at({ chunkX, chunkZ }).get());
				chunk->requestedUpdate = false;
			}
			m_meshMap.at({ chunkX, chunkZ }).get()->render();
		}
	}
}

void ChunkRenderer::generateMesh(const Chunk& chunk, Mesh& mesh) {
	mesh.start();
	for (unsigned y = 0; y < Chunk::CHUNK_SIZE; ++y) {
		for (unsigned z = 0; z < Chunk::CHUNK_SIZE; ++z) {
			for (unsigned x = 0; x < Chunk::CHUNK_SIZE; ++x) {

				int shiftX = chunk.getChunkX() * Chunk::CHUNK_SIZE;
				int shiftZ = chunk.getChunkZ() * Chunk::CHUNK_SIZE;
				int worldX = chunk.getChunkX() * Chunk::CHUNK_SIZE + x;
				int worldZ = chunk.getChunkZ() * Chunk::CHUNK_SIZE + z;

				if (chunk.getBlockAt(x, y, z) != Air) {

					Texture2D texture = ResourceManager::getTexture("grass");
					texture.bind();

					if (shouldRenderFront(chunk, worldX, y, worldZ)) {
						float light = m_chunkMap->getSunLightAt(worldX, y, worldZ + 1) / 15.0f;
						light *= 0.8;
						unsigned int v1 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z + 0.5f, 0.0f, 0.0f, light });
						unsigned int v2 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z + 0.5f, 1.0f, 0.0f, light });
						unsigned int v3 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z + 0.5f, 1.0f, 1.0f, light });
						unsigned int v4 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z + 0.5f, 0.0f, 1.0f, light });

						//front
						mesh.addTriangle({ v1, v2, v3 });
						mesh.addTriangle({ v1, v3, v4 });
					}

					if (shouldRenderBack(chunk, worldX, y, worldZ)) {
						float light = m_chunkMap->getSunLightAt(worldX, y, worldZ - 1) / 15.0f;
						light *= 0.8;
						unsigned int v5 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z - 0.5f, 1.0f, 0.0f, light });
						unsigned int v6 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z - 0.5f, 0.0f, 0.0f, light });
						unsigned int v7 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z - 0.5f, 0.0f, 1.0f, light });
						unsigned int v8 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z - 0.5f, 1.0f, 1.0f, light });
						// back
						mesh.addTriangle({ v6, v5, v8 });
						mesh.addTriangle({ v6, v8, v7 });
					}

					if (shouldRenderLeft(chunk, worldX, y, worldZ)) {
						float light = m_chunkMap->getSunLightAt(worldX - 1, y, worldZ) / 15.0f;
						light *= 0.8;
						unsigned int v1 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z + 0.5f, 1.0f, 0.0f, light });
						unsigned int v4 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z + 0.5f, 1.0f, 1.0f, light });
						unsigned int v5 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z - 0.5f, 0.0f, 0.0f, light });
						unsigned int v8 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z - 0.5f, 0.0f, 1.0f, light });

						//left
						mesh.addTriangle({ v5, v1, v4 });
						mesh.addTriangle({ v5, v4, v8 });
					}
					
					if (shouldRenderRight(chunk, worldX, y, worldZ)) {
						float light = m_chunkMap->getSunLightAt(worldX + 1, y, worldZ) / 15.0f;
						light *= 0.8;
						unsigned int v2 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z + 0.5f, 0.0f, 0.0f, light });
						unsigned int v3 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z + 0.5f, 0.0f, 1.0f, light });
						unsigned int v6 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z - 0.5f, 1.0f, 0.0f, light });
						unsigned int v7 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z - 0.5f, 1.0f, 1.0f, light });

						//right
						mesh.addTriangle({ v2, v6, v7 });
						mesh.addTriangle({ v2, v7, v3 });
					}

					if (shouldRenderUp(chunk, worldX, y, worldZ)) {
						float light = y == Chunk::CHUNK_SIZE - 1 ? m_sunlightLevel : m_chunkMap->getSunLightAt(worldX, y + 1, worldZ) / 15.0f;
						unsigned int v3 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z + 0.5f, 1.0f, 0.0f, light });
						unsigned int v4 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z + 0.5f, 0.0f, 0.0f, light });
						unsigned int v7 = mesh.addVertex({ shiftX + x + 0.5f, y + 0.5f, shiftZ + z - 0.5f, 1.0f, 1.0f, light });
						unsigned int v8 = mesh.addVertex({ shiftX + x - 0.5f, y + 0.5f, shiftZ + z - 0.5f, 0.0f, 1.0f, light });
						//up
						mesh.addTriangle({ v4, v3, v7 });
						mesh.addTriangle({ v4, v7, v8 });
					}

					if (shouldRenderDown(chunk, worldX, y, worldZ)) {
						float light = y == 0 ? 0 : m_chunkMap->getSunLightAt(worldX, y - 1, worldZ) / 15.0f;
						unsigned int v1 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z + 0.5f, 0.0f, 1.0f, light });
						unsigned int v2 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z + 0.5f, 1.0f, 1.0f, light });
						unsigned int v5 = mesh.addVertex({ shiftX + x - 0.5f, y - 0.5f, shiftZ + z - 0.5f, 0.0f, 0.0f, light });
						unsigned int v6 = mesh.addVertex({ shiftX + x + 0.5f, y - 0.5f, shiftZ + z - 0.5f, 1.0f, 0.0f, light });
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

void ChunkRenderer::generateLightMap(Chunk& chunk) {
	for (int y = Chunk::CHUNK_SIZE - 1; y >= 0; --y) {
		for (unsigned z = 0; z < Chunk::CHUNK_SIZE; ++z) {
			for (unsigned x = 0; x < Chunk::CHUNK_SIZE; ++x) {
				if (y == Chunk::CHUNK_SIZE - 1 && chunk.getBlockAt(x, y, z) == Air) {
					chunk.setSunLight(x, y, z, m_sunlightLevel);
					continue;
				}
				if (chunk.getBlockAt(x, y, z) == Air) {
					if (chunk.getSunLight(x, y + 1, z) != chunk.getSunLight(x, y, z)) {
						chunk.setSunLight(x, y, z, chunk.getSunLight(x, y + 1, z));
						chunk.requestedUpdate = true;
					}
				}
			}
		}
	}
}