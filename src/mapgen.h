/*
Minetest-c55
Copyright (C) 2010-2011 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef MAPGEN_HEADER
#define MAPGEN_HEADER

#include "irrlichttypes_extrabloated.h"
#include "util/container.h" // UniqueQueue
#include "gamedef.h"
#include "mapnode.h"
#include "noise.h"

class BiomeDefManager;
class Biome;

//struct BlockMakeData;
class MapBlock;
class ManualMapVoxelManipulator;
class VoxelManipulator;
class INodeDefManager;


enum BiomeType
{
	BT_NORMAL,
	BT_DESERT
};


struct BlockMakeData {
	bool no_op;
	ManualMapVoxelManipulator *vmanip;
	u64 seed;
	v3s16 blockpos_min;
	v3s16 blockpos_max;
	v3s16 blockpos_requested;
	UniqueQueue<v3s16> transforming_liquid;
	INodeDefManager *nodedef;

	BlockMakeData();
	~BlockMakeData();
};


/////////////////// Mapgen flags
#define MG_TREES         0x01
#define MG_CAVES         0x02
#define MG_DUNGEONS      0x04
#define MGV6_FORESTS     0x08
#define MGV6_BIOME_BLEND 0x10

#define AVERAGE_MUD_AMOUNT 4

/////////////////// Mapgen V6 perlin noise default values
NoiseParams nparams_v6_def_terrain_base =
	{-AVERAGE_MUD_AMOUNT, 20.0, v3f(250.0, 250.0, 250.0), 82341, 5, 0.6};
NoiseParams nparams_v6_def_terrain_higher =
	{20.0, 16.0, v3f(500.0, 500.0, 500.0), 85309, 5, 0.6};
NoiseParams nparams_v6_def_steepness =
	{0.85, 0.5, v3f(125.0, 125.0, 125.0), -932, 5, 0.7};
NoiseParams nparams_v6_def_height_select =
	{0.5, 1.0, v3f(250.0, 250.0, 250.0), 4213, 5, 0.69};
NoiseParams nparams_v6_def_trees =
	{0.0, 1.0, v3f(125.0, 125.0, 125.0), 2, 4, 0.66};
NoiseParams nparams_v6_def_mud =
	{AVERAGE_MUD_AMOUNT, 2.0, v3f(200.0, 200.0, 200.0), 91013, 3, 0.55};
NoiseParams nparams_v6_def_beach =
	{0.0, 1.0, v3f(250.0, 250.0, 250.0), 59420, 3, 0.50};
NoiseParams nparams_v6_def_biome =
	{0.0, 1.0, v3f(250.0, 250.0, 250.0), 9130, 3, 0.50};
NoiseParams nparams_v6_def_cave =
	{6.0, 6.0, v3f(250.0, 250.0, 250.0), 34329, 3, 0.50};

/////////////////// Mapgen V7 perlin noise default values
NoiseParams nparams_v7_def_terrain =
	{10.0, 12.0, v3f(350., 350., 350.), 82341, 5, 0.6}; //terrain
NoiseParams nparams_v7_def_bgroup =
	{0.5, 1/(2*1.6), v3f(350., 350., 350.), 5923, 2, 0.60}; //0 to 1
NoiseParams nparams_v7_def_heat =
	{25.0, 50.0, v3f(500., 500., 500.), 35293, 1, 0.00}; //-25 to 75
NoiseParams nparams_v7_def_humidity =
	{50, 100/(2*1.6), v3f(750., 750., 750.), 12094, 2, 0.60}; //0 to 100

struct MapgenParams {
	int seed;
	int water_level;
	int chunksize;
	u32 flags;

	MapgenParams() {
		seed        = 0;
		water_level = 1;
		chunksize   = 5;
		flags       = MG_TREES | MG_CAVES | MGV6_BIOME_BLEND;
	}

};

struct MapgenV6Params : public MapgenParams {
	float freq_desert;
	float freq_beach;
	NoiseParams *np_terrain_base;
	NoiseParams *np_terrain_higher;
	NoiseParams *np_steepness;
	NoiseParams *np_height_select;
	NoiseParams *np_trees;
	NoiseParams *np_mud;
	NoiseParams *np_beach;
	NoiseParams *np_biome;
	NoiseParams *np_cave;

	MapgenV6Params() {
		freq_desert       = 0.45;
		freq_beach        = 0.15;
		np_terrain_base   = &nparams_v6_def_terrain_base;
		np_terrain_higher = &nparams_v6_def_terrain_higher;
		np_steepness      = &nparams_v6_def_steepness;
		np_height_select  = &nparams_v6_def_height_select;
		np_trees          = &nparams_v6_def_trees;
		np_mud            = &nparams_v6_def_mud;
		np_beach          = &nparams_v6_def_beach;
		np_biome          = &nparams_v6_def_biome;
		np_cave           = &nparams_v6_def_cave;
	}
};

struct MapgenV7Params : public MapgenParams {
	NoiseParams *np_terrain;
	NoiseParams *np_bgroup;
	NoiseParams *np_heat;
	NoiseParams *np_humidity;

	MapgenV7Params() {
		np_terrain  = &nparams_v7_def_terrain;
		np_bgroup   = &nparams_v7_def_bgroup;
		np_heat     = &nparams_v7_def_heat;
		np_humidity = &nparams_v7_def_humidity;
	}
};


class Mapgen {
public:
	int seed;
	int water_level;
	bool generating;
	int id;

	virtual void makeChunk(BlockMakeData *data) {};

	//Legacy functions for Farmesh (pending removal)
	static bool get_have_beach(u64 seed, v2s16 p2d);
	static double tree_amount_2d(u64 seed, v2s16 p);
	static s16 find_ground_level_from_noise(u64 seed, v2s16 p2d, s16 precision);
};


class MapgenV6 : public Mapgen {
public:
	//ManualMapVoxelManipulator &vmanip;

	int ystride;
	v3s16 csize;

	v3s16 node_min;
	v3s16 node_max;

	Noise *noise_terrain_base;
	Noise *noise_terrain_higher;
	Noise *noise_steepness;
	Noise *noise_height_select;
	Noise *noise_trees;
	Noise *noise_mud;
	Noise *noise_beach;
	Noise *noise_biome;
	Noise *noise_cave;

	float *map_terrain_base;
	float *map_terrain_higher;
	float *map_steepness;
	float *map_height_select;
	float *map_trees;
	float *map_mud;
	float *map_beach;
	float *map_biome;
	float *map_cave;

	u32 flags;
	float freq_desert;
	float freq_beach;

	MapgenV6(int mapgenid, MapgenV6Params *params);
	~MapgenV6();

	void makeChunk(BlockMakeData *data);


	static s16 find_ground_level(VoxelManipulator &vmanip, v2s16 p2d, INodeDefManager *ndef);
	static s16 find_stone_level(VoxelManipulator &vmanip, v2s16 p2d, INodeDefManager *ndef);
	void make_tree(ManualMapVoxelManipulator &vmanip, v3s16 p0, bool is_apple_tree, INodeDefManager *ndef);
	double tree_amount_2d(u64 seed, v2s16 p);
	bool block_is_underground(u64 seed, v3s16 blockpos);
	double base_rock_level_2d(u64 seed, v2s16 p);
	s16 find_ground_level_from_noise(u64 seed, v2s16 p2d, s16 precision);
	double get_mud_add_amount(u64 seed, v2s16 p);
	bool get_have_beach(u64 seed, v2s16 p2d);
	BiomeType get_biome(u64 seed, v2s16 p2d);
	u32 get_blockseed(u64 seed, v3s16 p);
};


class MapgenV7 : public Mapgen {
public:
	BlockMakeData *data;
	ManualMapVoxelManipulator *vmanip;
	INodeDefManager *ndef;
	BiomeDefManager *biomedef;

	int ystride;
	int zstride;

	v3s16 csize;

	Noise *noise_terrain;
	Noise *noise_bgroup;
	Noise *noise_heat;
	Noise *noise_humidity;

	v3s16 node_min;
	v3s16 node_max;

	float *map_terrain;
	float *map_bgroup;
	float *map_heat;
	float *map_humidity;

	bool generating;
	int id;
	u32 flags;

/*
	NoiseParams *np_terrain;
	NoiseParams *np_bgroup;
	NoiseParams *np_heat;
	NoiseParams *np_humidity;*/

	//should these be broken off into a "commonly used nodes" class?
	MapNode n_air;
	MapNode n_water;
	MapNode n_lava;

	MapgenV7(BiomeDefManager *biomedef, int mapgenid, MapgenV7Params *params);
	~MapgenV7();

	void makeChunk(BlockMakeData *data);
	void updateLiquid(v3s16 node_min, v3s16 node_max);
	void updateLighting(v3s16 node_min, v3s16 node_max);

	//Legacy functions for Farmesh (pending removal)
//	static bool get_have_beach(u64 seed, v2s16 p2d);
//	static double tree_amount_2d(u64 seed, v2s16 p);
//	static s16 find_ground_level_from_noise(u64 seed, v2s16 p2d, s16 precision);
};

class EmergeManager {
public:
	//settings
	int mg_version;
	MapgenParams *params;

	//mapgen objects here
	Mapgen *mapgen;

	//biome manager
	BiomeDefManager *biomedef;

	EmergeManager(IGameDef *gamedef, int mg_version=6);
	~EmergeManager();

	Mapgen *getMapgen();
	void setMapgenParams();
	void addBlockToQueue();

	//mapgen helper methods
	Biome *getBiomeAtPoint(v3s16 p);
	int getGroundLevelAtPoint(v2s16 p);
	bool isBlockUnderground(v3s16 blockpos);
	u32 getBlockSeed(v3s16 p);
};


/*
namespace mapgen
{
	// Finds precise ground level at any position
	s16 find_ground_level_from_noise(u64 seed, v2s16 p2d, s16 precision);

	// Find out if block is completely underground
	bool block_is_underground(u64 seed, v3s16 blockpos);

	// Get a pseudorandom seed for a position on the map
	u32 get_blockseed(u64 seed, v3s16 p);

	// Main map generation routine
	void make_block(BlockMakeData *data);


	//These are used by FarMesh
	bool get_have_beach(u64 seed, v2s16 p2d);
	double tree_amount_2d(u64 seed, v2s16 p);

	struct BlockMakeData
	{
		bool no_op;
		ManualMapVoxelManipulator *vmanip; // Destructor deletes
		u64 seed;
		v3s16 blockpos_min;
		v3s16 blockpos_max;
		v3s16 blockpos_requested;
		UniqueQueue<v3s16> transforming_liquid;
		INodeDefManager *nodedef;

		BlockMakeData();
		~BlockMakeData();
	};

}; // namespace mapgen
*/
#endif

