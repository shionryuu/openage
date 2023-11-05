// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "terrain_factory.h"

#include <mutex>

#include <nyan/nyan.h>

#include "gamestate/api/terrain.h"
#include "gamestate/terrain.h"
#include "gamestate/terrain_chunk.h"
#include "renderer/render_factory.h"
#include "renderer/stages/terrain/terrain_render_entity.h"
#include "time/time.h"

#include "assets/mod_manager.h"
#include "gamestate/game_state.h"


namespace openage::gamestate {

static const std::vector<nyan::fqon_t> aoe1_test_terrain = {};
static const std::vector<nyan::fqon_t> de1_test_terrain = {};
static const std::vector<nyan::fqon_t> aoe2_test_terrain = {
	"aoe2_base.data.terrain.foundation.Foundation",
	"aoe2_base.data.terrain.grass.Grass",
	"aoe2_base.data.terrain.dirt.Dirt",
};
static const std::vector<nyan::fqon_t> de2_test_terrain = {};
static const std::vector<nyan::fqon_t> hd_test_terrain = {
	"hd_base.data.terrain.foundation.Foundation",
	"hd_base.data.terrain.grass.Grass",
	"hd_base.data.terrain.dirt.Dirt",
};
static const std::vector<nyan::fqon_t> swgb_test_terrain = {
	"swgb_base.data.terrain.desert0.Desert0",
	"swgb_base.data.terrain.grass2.Grass2",
	"swgb_base.data.terrain.foundation.Foundation",
};
static const std::vector<nyan::fqon_t> trial_test_terrain = {};

std::shared_ptr<Terrain> TerrainFactory::add_terrain() {
	// TODO: Replace this with a proper terrain generator.
	auto terrain = std::make_shared<Terrain>();

	return terrain;
}

// TODO: Remove hardcoded test texture references
static std::vector<nyan::fqon_t> test_terrains; // declare static so we only have to do this once

void build_test_terrains(const std::shared_ptr<GameState> &gstate) {
	auto modpack_ids = gstate->get_mod_manager()->get_load_order();
	for (auto &modpack_id : modpack_ids) {
		if (modpack_id == "aoe1_base") {
			test_terrains.insert(test_terrains.end(),
			                     aoe1_test_terrain.begin(),
			                     aoe1_test_terrain.end());
		}
		else if (modpack_id == "de1_base") {
			test_terrains.insert(test_terrains.end(),
			                     de1_test_terrain.begin(),
			                     de1_test_terrain.end());
		}
		else if (modpack_id == "aoe2_base") {
			test_terrains.insert(test_terrains.end(),
			                     aoe2_test_terrain.begin(),
			                     aoe2_test_terrain.end());
		}
		else if (modpack_id == "de2_base") {
			test_terrains.insert(test_terrains.end(),
			                     de2_test_terrain.begin(),
			                     de2_test_terrain.end());
		}
		else if (modpack_id == "hd_base") {
			test_terrains.insert(test_terrains.end(),
			                     hd_test_terrain.begin(),
			                     hd_test_terrain.end());
		}
		else if (modpack_id == "swgb_base") {
			test_terrains.insert(test_terrains.end(),
			                     swgb_test_terrain.begin(),
			                     swgb_test_terrain.end());
		}
		else if (modpack_id == "trial_base") {
			test_terrains.insert(test_terrains.end(),
			                     trial_test_terrain.begin(),
			                     trial_test_terrain.end());
		}
	}
}


std::shared_ptr<TerrainChunk> TerrainFactory::add_chunk(const std::shared_ptr<GameState> &gstate,
                                                        const util::Vector2s size,
                                                        const coord::tile_delta offset) {
	auto chunk = std::make_shared<TerrainChunk>(size, offset);

	if (this->render_factory) {
		auto render_entity = this->render_factory->add_terrain_render_entity(size, offset);
		chunk->set_render_entity(render_entity);

		std::string test_texture_path = "../test/textures/test_terrain.terrain";

		// TODO: Remove test texture references
		static size_t test_terrain_index = 0;
		if (test_terrains.empty()) {
			build_test_terrains(gstate);

			// use one of the modpack terrain textures
			if (not test_terrains.empty()) {
				if (test_terrain_index >= test_terrains.size()) {
					test_terrain_index = 0;
				}
				auto terrain_obj = gstate->get_db_view()->get_object(test_terrains[test_terrain_index]);
				test_texture_path = api::APITerrain::get_terrain_path(terrain_obj);

				test_terrain_index += 1;
			}
		}

		chunk->render_update(time::time_t::zero(),
		                     test_texture_path);
	}

	return chunk;
}

void TerrainFactory::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	std::unique_lock lock{this->mutex};

	this->render_factory = render_factory;
}

} // namespace openage::gamestate
