/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/namespace_doc.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef NAMESPACE_DOC_HPP_
#define NAMESPACE_DOC_HPP_

/**
 * \file
 * Contains the API documentation for the namespaces.
 */

/// Represents the root namespace for the multicore engine (mce) project.
namespace mce {

/// Contains the asset management code.
/**
 * Assets in this context are data resources that are used at runtime by the engine or games using it.
 * The mce asset management layer supports asynchronous loading of assets from disk either from native OS
 * files (see #mce::asset::native_file_reader) or from package files (see #mce::asset::pack_file_reader).
 * The assets can be loaded individually (see #mce::asset::file_asset_loader) or bundled into load units that
 * are loaded into memory as a unit (see #mce::asset::load_unit_asset_loader).
 */
namespace asset {}

/// Contains code for the asset generator pipeline tools.
/**
 * The code in this namespace provides functionality used for the tools to prepare assets (see #mce::asset)
 * for the engine. This includes the generator tools that bundle assets into pack files and load units. Tool
 * functionality for converting individual assets to the correct format is also contained here.
 */
namespace asset_gen {

/// Contains asset generator ASTs.
/**
 * These abstract syntax tree node structures are used for parsing asset description files (see
 * #mce::asset_gen::parser).
 */
namespace ast {}

/// Contains the parser code for asset description files.
namespace parser {}

} // namespace asset_gen

/// Contains the binary data streams used for (de-)serialization in mce.
namespace bstream {}

/// Contains the global runtime configuration system for mce.
namespace config {}

/// Contains container data structures used in mce.
namespace containers {}

/// Contains central code of the engine.
namespace core {}

/// Contains the management code for entities (aka. game objects) and their components.
namespace entity {

/// Contains the parser code for entity description files to define entities in a data-driven way.
namespace parser {}

} // namespace entity

/// Contains the memory management code of mce.
namespace memory {}

/// Contains the loading code for polygonal and collision aspects of 3D models on top of the asset system.
namespace model {}

/// Contains reflection code to introspect into object properties which is used for data-driven functionality.
namespace reflection {}

/// Contains various utility functionalities used across the mce code base.
namespace util {}

} // namespace mce

#endif /* NAMESPACE_DOC_HPP_ */
