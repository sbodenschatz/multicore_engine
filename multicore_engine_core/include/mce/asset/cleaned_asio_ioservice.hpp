/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/asset/asio_macro_cleanup.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_ASSET_CLEANED_ASIO_IOSERVICE_HPP_
#define MCE_ASSET_CLEANED_ASIO_IOSERVICE_HPP_

/**
 * \file
 * Includes boost/asio/io_service.hpp and undefines problematic macros (transitively) introduced by it.
 */

#include <boost/asio/io_service.hpp>

#ifdef MemoryBarrier
#undef MemoryBarrier
#endif

#endif /* MCE_ASSET_CLEANED_ASIO_IOSERVICE_HPP_ */
