#pragma once

// Wrapper header to provide consistent interface for MPQ loading
// This maps the StormLib implementation to the expected interface

#include "stormlib_mpq_loader.h"

namespace d2 {

// Alias the types from d2portable namespace
using MPQFileInfo = d2portable::utils::StormMPQFileInfo;
using MPQLoader = d2portable::utils::StormLibMPQLoader;

} // namespace d2