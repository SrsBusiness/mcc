#include <math.h>
#include <stdio.h>
#include "dig.h"

/* Would rather use arrays, but Minecraft data values are not consecutive. */

/* Returns infinity for unbreakable, NAN for unrecognized block_ids */
double block_hardness(int block_data) {
    enum MINECRAFT_BLOCK block_id = block_data >> 4;
    switch(block_id) {
        case MINECRAFT_BLOCK_BARRIER:
        case MINECRAFT_BLOCK_BEDROCK:
        case MINECRAFT_BLOCK_COMMAND_BLOCK:
        case MINECRAFT_BLOCK_REPEATING_COMMAND_BLOCK:
        case MINECRAFT_BLOCK_CHAIN_COMMAND_BLOCK:
        case MINECRAFT_BLOCK_END_PORTAL:
        case MINECRAFT_BLOCK_END_PORTAL_FRAME:
        case MINECRAFT_BLOCK_PORTAL:
        case MINECRAFT_BLOCK_STRUCTURE_BLOCK:
        case MINECRAFT_BLOCK_STRUCTURE_VOID:
        case MINECRAFT_BLOCK_PISTON_EXTENSION:
            return INFINITY;
        case MINECRAFT_BLOCK_WATER: /* unbreakable */
        case MINECRAFT_BLOCK_FLOWING_WATER:
        case MINECRAFT_BLOCK_LAVA:  /* unbreakable */
        case MINECRAFT_BLOCK_FLOWING_LAVA:
            return 100.0;
        case MINECRAFT_BLOCK_OBSIDIAN:
            return 50.0;
        case MINECRAFT_BLOCK_ENDER_CHEST:
            return 22.5;
        case MINECRAFT_BLOCK_ANVIL:
        case MINECRAFT_BLOCK_COAL_BLOCK:
        case MINECRAFT_BLOCK_DIAMOND_BLOCK:
        case MINECRAFT_BLOCK_EMERALD_BLOCK:
        case MINECRAFT_BLOCK_IRON_BLOCK:
        case MINECRAFT_BLOCK_REDSTONE_BLOCK:
        case MINECRAFT_BLOCK_ENCHANTING_TABLE:
        case MINECRAFT_BLOCK_IRON_BARS:
        case MINECRAFT_BLOCK_IRON_DOOR:
        case MINECRAFT_BLOCK_IRON_TRAPDOOR:
        case MINECRAFT_BLOCK_MOB_SPAWNER:
            return 5.0;
        case MINECRAFT_BLOCK_WEB:
            return 4.0;
        case MINECRAFT_BLOCK_DISPENSER:
        case MINECRAFT_BLOCK_DROPPER:
        case MINECRAFT_BLOCK_FURNACE:
        case MINECRAFT_BLOCK_LIT_FURNACE:
            return 3.5;
        case MINECRAFT_BLOCK_BEACON:
        case MINECRAFT_BLOCK_GOLD_BLOCK:
        case MINECRAFT_BLOCK_COAL_ORE:
        case MINECRAFT_BLOCK_DRAGON_EGG:
        case MINECRAFT_BLOCK_DIAMOND_ORE:
        case MINECRAFT_BLOCK_EMERALD_ORE:
        case MINECRAFT_BLOCK_END_STONE:
        case MINECRAFT_BLOCK_GOLD_ORE:
        case MINECRAFT_BLOCK_HOPPER:
        case MINECRAFT_BLOCK_IRON_ORE:
        case MINECRAFT_BLOCK_LAPIS_BLOCK:
        case MINECRAFT_BLOCK_LAPIS_ORE:
        case MINECRAFT_BLOCK_QUARTZ_ORE:
        case MINECRAFT_BLOCK_REDSTONE_ORE:
        case MINECRAFT_BLOCK_LIT_REDSTONE_ORE:
        case MINECRAFT_BLOCK_TRAPDOOR:
        case MINECRAFT_BLOCK_WOODEN_DOOR:
        case MINECRAFT_BLOCK_SPRUCE_DOOR:
        case MINECRAFT_BLOCK_BIRCH_DOOR:
        case MINECRAFT_BLOCK_JUNGLE_DOOR:
        case MINECRAFT_BLOCK_ACACIA_DOOR:
        case MINECRAFT_BLOCK_DARK_OAK_DOOR:
            return 3.0;
        case MINECRAFT_BLOCK_CHEST:
        case MINECRAFT_BLOCK_TRAPPED_CHEST:
        case MINECRAFT_BLOCK_CRAFTING_TABLE:
            return 2.5;
        case MINECRAFT_BLOCK_BONE_BLOCK:
        case MINECRAFT_BLOCK_BRICK_STAIRS:
        case MINECRAFT_BLOCK_BRICK_BLOCK:
        case MINECRAFT_BLOCK_CAULDRON:
        case MINECRAFT_BLOCK_COBBLESTONE:
        case MINECRAFT_BLOCK_STONE_STAIRS:
        case MINECRAFT_BLOCK_COBBLESTONE_WALL:
        case MINECRAFT_BLOCK_FENCE:
        case MINECRAFT_BLOCK_SPRUCE_FENCE:
        case MINECRAFT_BLOCK_BIRCH_FENCE:
        case MINECRAFT_BLOCK_JUNGLE_FENCE:
        case MINECRAFT_BLOCK_DARK_OAK_FENCE:
        case MINECRAFT_BLOCK_ACACIA_FENCE:
        case MINECRAFT_BLOCK_FENCE_GATE:
        case MINECRAFT_BLOCK_SPRUCE_FENCE_GATE:
        case MINECRAFT_BLOCK_BIRCH_FENCE_GATE:
        case MINECRAFT_BLOCK_JUNGLE_FENCE_GATE:
        case MINECRAFT_BLOCK_DARK_OAK_FENCE_GATE:
        case MINECRAFT_BLOCK_ACACIA_FENCE_GATE:
        case MINECRAFT_BLOCK_JUKEBOX:
        case MINECRAFT_BLOCK_MOSSY_COBBLESTONE:
        case MINECRAFT_BLOCK_NETHER_BRICK:
        case MINECRAFT_BLOCK_RED_NETHER_BRICK:
        case MINECRAFT_BLOCK_NETHER_BRICK_FENCE:
        case MINECRAFT_BLOCK_NETHER_BRICK_STAIRS:
        case MINECRAFT_BLOCK_STONE_SLAB:
        case MINECRAFT_BLOCK_DOUBLE_STONE_SLAB:
        case MINECRAFT_BLOCK_STONE_SLAB2:
        case MINECRAFT_BLOCK_DOUBLE_STONE_SLAB2:
        case MINECRAFT_BLOCK_PURPUR_SLAB:
        case MINECRAFT_BLOCK_PURPUR_DOUBLE_SLAB:
        case MINECRAFT_BLOCK_LOG:
        case MINECRAFT_BLOCK_LOG2:
        case MINECRAFT_BLOCK_PLANKS:
        case MINECRAFT_BLOCK_WOODEN_SLAB:
        case MINECRAFT_BLOCK_DOUBLE_WOODEN_SLAB:
        case MINECRAFT_BLOCK_OAK_STAIRS:
        case MINECRAFT_BLOCK_SPRUCE_STAIRS:
        case MINECRAFT_BLOCK_BIRCH_STAIRS:
        case MINECRAFT_BLOCK_JUNGLE_STAIRS:
        case MINECRAFT_BLOCK_ACACIA_STAIRS:
        case MINECRAFT_BLOCK_DARK_OAK_STAIRS:
            return 2.0;
        case MINECRAFT_BLOCK_STONE:
        case MINECRAFT_BLOCK_BOOKSHELF:
        case MINECRAFT_BLOCK_PRISMARINE:
        case MINECRAFT_BLOCK_STONE_BRICK:
        case MINECRAFT_BLOCK_STONE_BRICK_STAIRS:
        case MINECRAFT_BLOCK_PURPUR_BLOCK:
        case MINECRAFT_BLOCK_PURPUR_PILLAR:
        case MINECRAFT_BLOCK_PURPUR_STAIRS:
            return 1.5;
        case MINECRAFT_BLOCK_HARDENED_CLAY:
        case MINECRAFT_BLOCK_STAINED_HARDENED_CLAY:
            return 1.25;
        case MINECRAFT_BLOCK_STANDING_BANNER:
        case MINECRAFT_BLOCK_WALL_BANNER:
        case MINECRAFT_BLOCK_LIT_PUMPKIN:
        case MINECRAFT_BLOCK_MELON_BLOCK:
        case MINECRAFT_BLOCK_SKULL:
        case MINECRAFT_BLOCK_NETHER_WART_BLOCK:
        case MINECRAFT_BLOCK_PUMPKIN:
        case MINECRAFT_BLOCK_STANDING_SIGN:
        case MINECRAFT_BLOCK_WALL_SIGN:
            return 1.0;
        case MINECRAFT_BLOCK_QUARTZ_BLOCK:
        case MINECRAFT_BLOCK_NOTE_BLOCK:
        case MINECRAFT_BLOCK_QUARTZ_STAIRS:
        case MINECRAFT_BLOCK_RED_SANDSTONE:
        case MINECRAFT_BLOCK_RED_SANDSTONE_STAIRS:
        case MINECRAFT_BLOCK_SANDSTONE:
        case MINECRAFT_BLOCK_SANDSTONE_STAIRS:
        case MINECRAFT_BLOCK_WOOL:
        case MINECRAFT_BLOCK_END_BRICKS:
            return 0.8;
        case MINECRAFT_BLOCK_MONSTER_EGG:
            return 0.75;
        case MINECRAFT_BLOCK_RAIL:
        case MINECRAFT_BLOCK_GOLDEN_RAIL:
        case MINECRAFT_BLOCK_DETECTOR_RAIL:
        case MINECRAFT_BLOCK_ACTIVATOR_RAIL:
            return 0.7;
        case MINECRAFT_BLOCK_CLAY:
        case MINECRAFT_BLOCK_FARMLAND:
        case MINECRAFT_BLOCK_GRASS:
        case MINECRAFT_BLOCK_GRASS_PATH:
        case MINECRAFT_BLOCK_GRAVEL:
        case MINECRAFT_BLOCK_MYCELIUM:
        case MINECRAFT_BLOCK_SPONGE:
            return 0.6;
        case MINECRAFT_BLOCK_BREWING_STAND:
        case MINECRAFT_BLOCK_STONE_BUTTON:
        case MINECRAFT_BLOCK_WOODEN_BUTTON:
        case MINECRAFT_BLOCK_CAKE:
        case MINECRAFT_BLOCK_DIRT:
        case MINECRAFT_BLOCK_FROSTED_ICE:
        case MINECRAFT_BLOCK_HAY_BLOCK:
        case MINECRAFT_BLOCK_ICE:
        case MINECRAFT_BLOCK_LEVER:
        case MINECRAFT_BLOCK_MAGMA:
        case MINECRAFT_BLOCK_PACKED_ICE:
        case MINECRAFT_BLOCK_PISTON:
        case MINECRAFT_BLOCK_STICKY_PISTON:
        case MINECRAFT_BLOCK_PISTON_HEAD:
        case MINECRAFT_BLOCK_SAND:
        case MINECRAFT_BLOCK_SOUL_SAND:
        case MINECRAFT_BLOCK_STONE_PRESSURE_PLATE:
        case MINECRAFT_BLOCK_HEAVY_WEIGHTED_PRESSURE_PLATE:
        case MINECRAFT_BLOCK_LIGHT_WEIGHTED_PRESSURE_PLATE:
        case MINECRAFT_BLOCK_WOODEN_PRESSURE_PLATE:
            return 0.5;
        case MINECRAFT_BLOCK_CACTUS:
        case MINECRAFT_BLOCK_LADDER:
        case MINECRAFT_BLOCK_NETHERRACK:
        case MINECRAFT_BLOCK_CHORUS_PLANT:
        case MINECRAFT_BLOCK_CHORUS_FLOWER:
            return 0.4;
        case MINECRAFT_BLOCK_GLASS:
        case MINECRAFT_BLOCK_GLASS_PANE:
        case MINECRAFT_BLOCK_GLOWSTONE:
        case MINECRAFT_BLOCK_REDSTONE_LAMP:
        case MINECRAFT_BLOCK_LIT_REDSTONE_LAMP:
        case MINECRAFT_BLOCK_SEA_LANTERN:
        case MINECRAFT_BLOCK_STAINED_GLASS:
        case MINECRAFT_BLOCK_STAINED_GLASS_PANE:
            return 0.3;
        case MINECRAFT_BLOCK_BED:
        case MINECRAFT_BLOCK_COCOA:
        case MINECRAFT_BLOCK_DAYLIGHT_DETECTOR:
        case MINECRAFT_BLOCK_DAYLIGHT_DETECTOR_INVERTED:
        case MINECRAFT_BLOCK_RED_MUSHROOM_BLOCK:
        case MINECRAFT_BLOCK_BROWN_MUSHROOM_BLOCK:
        case MINECRAFT_BLOCK_LEAVES:
        case MINECRAFT_BLOCK_LEAVES2:
        case MINECRAFT_BLOCK_SNOW:
        case MINECRAFT_BLOCK_VINE:
            return 0.2;
        case MINECRAFT_BLOCK_CARPET:
        case MINECRAFT_BLOCK_SNOW_LAYER:
            return 0.1;
        case MINECRAFT_BLOCK_WATERLILY:
        case MINECRAFT_BLOCK_AIR:
        case MINECRAFT_BLOCK_CARROTS:
        case MINECRAFT_BLOCK_DEADBUSH:
        case MINECRAFT_BLOCK_FIRE:
        case MINECRAFT_BLOCK_FLOWER_POT:
        case MINECRAFT_BLOCK_RED_FLOWER:
        case MINECRAFT_BLOCK_YELLOW_FLOWER:
        case MINECRAFT_BLOCK_DOUBLE_PLANT:
        case MINECRAFT_BLOCK_TALLGRASS:
        case MINECRAFT_BLOCK_MELON_STEM:
        case MINECRAFT_BLOCK_RED_MUSHROOM:
        case MINECRAFT_BLOCK_BROWN_MUSHROOM:
        case MINECRAFT_BLOCK_NETHER_WART:
        case MINECRAFT_BLOCK_POTATOES:
        case MINECRAFT_BLOCK_PUMPKIN_STEM:
        case MINECRAFT_BLOCK_POWERED_COMPARATOR:
        case MINECRAFT_BLOCK_UNPOWERED_COMPARATOR:
        case MINECRAFT_BLOCK_POWERED_REPEATER:
        case MINECRAFT_BLOCK_UNPOWERED_REPEATER:
        case MINECRAFT_BLOCK_REDSTONE_TORCH:
        case MINECRAFT_BLOCK_UNLIT_REDSTONE_TORCH:
        case MINECRAFT_BLOCK_REDSTONE_WIRE:
        case MINECRAFT_BLOCK_SAPLING:
        case MINECRAFT_BLOCK_SLIME:
        case MINECRAFT_BLOCK_REEDS:
        case MINECRAFT_BLOCK_TNT:
        case MINECRAFT_BLOCK_TORCH:
        case MINECRAFT_BLOCK_TRIPWIRE:
        case MINECRAFT_BLOCK_TRIPWIRE_HOOK:
        case MINECRAFT_BLOCK_WHEAT:
        case MINECRAFT_BLOCK_END_ROD: /* This is a guess */
        case MINECRAFT_BLOCK_BEETROOTS: /* Guess */
            return 0.0;
        case MINECRAFT_BLOCK_END_GATEWAY: /* Investigate */
            return -1.0;
        default:
            return NAN;
    }
}

enum MINECRAFT_TOOL_TYPE {
    MINECRAFT_TOOL_AXE,
    MINECRAFT_TOOL_PICKAXE,
    MINECRAFT_TOOL_SHEARS,
    MINECRAFT_TOOL_SHOVEL,
    MINECRAFT_TOOL_SWORD,
    MINECRAFT_TOOL_OTHER
};

//enum MINECRAFT_TOOL_TYPE tool_type(enum MINECRAFT_ITEM item) {
//    switch(item) {
//        /* AXE */
//        case MINECRAFT_ITEM_IRON_AXE:
//        case MINECRAFT_ITEM_WOODEN_AXE:
//        case MINECRAFT_ITEM_STONE_AXE:
//        case MINECRAFT_ITEM_DIAMOND_AXE:
//        case MINECRAFT_ITEM_GOLDEN_AXE:
//            return MINECRAFT_TOOL_AXE;
//        /* PICKAXE */
//        case MINECRAFT_ITEM_IRON_PICKAXE:
//        case MINECRAFT_ITEM_WOODEN_PICKAXE:
//        case MINECRAFT_ITEM_STONE_PICKAXE:
//        case MINECRAFT_ITEM_DIAMOND_PICKAXE:
//        case MINECRAFT_ITEM_GOLDEN_PICKAXE:
//            return MINECRAFT_TOOL_PICKAXE;
//        /* SHEARS */
//        case MINECRAFT_ITEM_SHEARS:
//            return MINECRAFT_TOOL_SHEARS;
//        /* SHOVEL */ 
//        case MINECRAFT_ITEM_IRON_SHOVEL:
//        case MINECRAFT_ITEM_WOODEN_SHOVEL:
//        case MINECRAFT_ITEM_STONE_SHOVEL:
//        case MINECRAFT_ITEM_DIAMOND_SHOVEL:
//        case MINECRAFT_ITEM_GOLDEN_SHOVEL:
//            return MINECRAFT_TOOL_SHOVEL;
//        /* SWORD */
//        case MINECRAFT_ITEM_IRON_SWORD:
//        case MINECRAFT_ITEM_WOODEN_SWORD:
//        case MINECRAFT_ITEM_STONE_SWORD:
//        case MINECRAFT_ITEM_DIAMOND_SWORD:
//        case MINECRAFT_ITEM_GOLDEN_SWORD:
//            return MINECRAFT_TOOL_SWORD;
//        default:
//            return MINECRAFT_TOOL_OTHER;
//    }
//}

//int best_tool(int block_data) {
//    enum MINECRAFT_BLOCK block_id = block_data >> 4;
//    switch(block_id) {
//        /* Axe */
//        case MINECRAFT_BLOCK_TRAPDOOR:
//        case MINECRAFT_BLOCK_WOODEN_DOOR:
//        case MINECRAFT_BLOCK_SPRUCE_DOOR:
//        case MINECRAFT_BLOCK_BIRCH_DOOR:
//        case MINECRAFT_BLOCK_JUNGLE_DOOR:
//        case MINECRAFT_BLOCK_ACACIA_DOOR:
//        case MINECRAFT_BLOCK_DARK_OAK_DOOR:
//        case MINECRAFT_BLOCK_CHEST:
//        case MINECRAFT_BLOCK_TRAPPED_CHEST:
//        case MINECRAFT_BLOCK_CRAFTING_TABLE:
//        case MINECRAFT_BLOCK_FENCE:
//        case MINECRAFT_BLOCK_SPRUCE_FENCE:
//        case MINECRAFT_BLOCK_BIRCH_FENCE:
//        case MINECRAFT_BLOCK_JUNGLE_FENCE:
//        case MINECRAFT_BLOCK_DARK_OAK_FENCE:
//        case MINECRAFT_BLOCK_ACACIA_FENCE:
//        case MINECRAFT_BLOCK_FENCE_GATE:
//        case MINECRAFT_BLOCK_SPRUCE_FENCE_GATE:
//        case MINECRAFT_BLOCK_BIRCH_FENCE_GATE:
//        case MINECRAFT_BLOCK_JUNGLE_FENCE_GATE:
//        case MINECRAFT_BLOCK_DARK_OAK_FENCE_GATE:
//        case MINECRAFT_BLOCK_ACACIA_FENCE_GATE:
//        case MINECRAFT_BLOCK_JUKEBOX:
//        case MINECRAFT_BLOCK_LOG:
//        case MINECRAFT_BLOCK_LOG2:
//        case MINECRAFT_BLOCK_PLANKS:
//        case MINECRAFT_BLOCK_WOODEN_SLAB:
//        case MINECRAFT_BLOCK_DOUBLE_WOODEN_SLAB:
//        case MINECRAFT_BLOCK_OAK_STAIRS:
//        case MINECRAFT_BLOCK_SPRUCE_STAIRS:
//        case MINECRAFT_BLOCK_BIRCH_STAIRS:
//        case MINECRAFT_BLOCK_JUNGLE_STAIRS:
//        case MINECRAFT_BLOCK_ACACIA_STAIRS:
//        case MINECRAFT_BLOCK_DARK_OAK_STAIRS:
//        case MINECRAFT_BLOCK_BOOKSHELF:
//        case MINECRAFT_BLOCK_STANDING_BANNER:
//        case MINECRAFT_BLOCK_WALL_BANNER:
//        case MINECRAFT_BLOCK_LIT_PUMPKIN:      
//        case MINECRAFT_BLOCK_PUMPKIN:
//        case MINECRAFT_BLOCK_MELON_BLOCK:
//        case MINECRAFT_BLOCK_STANDING_SIGN:
//        case MINECRAFT_BLOCK_WALL_SIGN:
//        case MINECRAFT_BLOCK_NOTE_BLOCK:
//        case MINECRAFT_BLOCK_WOODEN_PRESSURE_PLATE:
//        case MINECRAFT_BLOCK_LADDER:
//        case MINECRAFT_BLOCK_COCOA:
//        case MINECRAFT_BLOCK_DAYLIGHT_DETECTOR:
//        case MINECRAFT_BLOCK_DAYLIGHT_DETECTOR_INVERTED:
//        case MINECRAFT_BLOCK_RED_MUSHROOM_BLOCK:
//        case MINECRAFT_BLOCK_BROWN_MUSHROOM_BLOCK:
//        case MINECRAFT_BLOCK_VINE:
//            return MINECRAFT_TOOL_AXE;
//        
//
//        case MINECRAFT_BLOCK_STONE_BUTTON:
//
//        case MINECRAFT_BLOCK_ICE:
//        case MINECRAFT_BLOCK_PACKED_ICE:
//        
//        case MINECRAFT_BLOCK_ANVIL:
//
//        case MINECRAFT_BLOCK_REDSTONE_BLOCK:
//
//        case MINECRAFT_BLOCK_BREWING_STAND:
//
//        case MINECRAFT_BLOCK_CAULDRON:
//
//        case MINECRAFT_BLOCK_IRON_BARS:
//
//        case MINECRAFT_BLOCK_IRON_DOOR:
//
//        case MINECRAFT_BLOCK_IRON_TRAPDOOR:
//
//        case MINECRAFT_BLOCK_HOPPER:
//
//        case MINECRAFT_BLOCK_HEAVY_WEIGHTED_PRESSURE_PLATE:
//        case MINECRAFT_BLOCK_LIGHT_WEIGHTED_PRESSURE_PLATE:
//
//        case MINECRAFT_BLOCK_IRON_BLOCK:
//
//        case MINECRAFT_BLOCK_LAPIS_BLOCK:
//
//        case MINECRAFT_BLOCK_DIAMOND_BLOCK:
//
//        case MINECRAFT_BLOCK_EMERALD_BLOCK:
//
//        case MINECRAFT_BLOCK_GOLD_BLOCK:
//
//
//
//    }
//}

/*
double break_time(int block_data, int item, int item_enchantment, int status_effects) {
    enum MINECRAFT_BLOCK block_id = block_data >> 4;
    switch(block_id) {
    }
}
*/
