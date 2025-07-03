#include "tools/asset_cache.h"

namespace d2 {

AssetCache::AssetCache(size_t maxMemoryBytes) 
    : maxMemory(maxMemoryBytes) {
}

} // namespace d2