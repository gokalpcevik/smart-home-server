#pragma once

// Returns the boost::beast::error_code::what()
// with the line and file information.
#ifndef SHM_FMT_SV_ERR
#define SHM_FMT_SV_ERR(ec) fmt::format("Server Error: {0},\n File: {1} at line {2}",(ec).what(), __FILE__, __LINE__)
#endif

#ifndef SHM_SV_ERR
#define SHM_SV_ERR(ec) shm_error(SHM_FMT_SV_ERR((ec)))
#endif