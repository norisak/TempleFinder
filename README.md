# TempleFinder

Used to find Minecraft world seeds with certain loot in the desert temple chests. 

Minecraft will attempt to create one temple per 512x512 block region, and if it's not a desert biome, it won't be there. 

If the temple doesn't spawn because it's not the right biome, add 2<sup>48</sup> to the seed. This will leave the loot and temple location unchanged, but the biomes will be different. Keep doing this until you find a seed with the right biome.

Sometimes the loot generated in minecraft will be different than what is predicted by this program. This can happen if there is a mineshaft, village or stronghold in the same chunk. This should be pretty rare if searching in the 0, 0 region, but mineshafts are more common farther away from spawn, so you'll have a higher chance of getting false results.

The program is by default configured to search for 3 or more mending books in the temple in the 0, 0 region. This temple can be found between 0-384 on both the x and z axis. Feel free to modify it to suit your needs.
