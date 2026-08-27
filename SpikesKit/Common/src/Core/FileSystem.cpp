// SPDX - License - Identifier: MIT
// Copyright(c) 2024 - 2026 naoki
// Licensed under the MIT License.See the LICENSE file in the project root,
// or visit https://opensource.org/licenses/MIT for details

#include "Core/FileSystem.hpp"

namespace ts
{
	namespace kit
	{
		namespace fs
		{
			path GetRootDirectory()
			{
#ifdef TS_PROJECT_ROOT
				return path(TS_PROJECT_ROOT);
#else // TS_PROJECT_ROOT
#error "TS_PROJECT_ROOT is not defined in CMakeLists.txt!"
				return path("");
#endif //! TS_PROJECT_ROOT
			}

			path GetExecutableDirectory()
			{
				char path[MAX_PATH];
				GetModuleFileNameA(NULL, path, MAX_PATH);
				return fs::path(path).parent_path();
			}
		} // namespace fs
	} // namespace kit
} // namespace ts