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
				const path exeDir = GetExecutableDirectory();
				if (exeDir.empty())
				{
					return path{};
				}

				// .exe のディレクトリから上方向へ最大4階層まで "Asset" ディレクトリを探索
				path current = exeDir;
				for (int depth = 0; depth <= 4; ++depth)
				{
					std::error_code ec;
					if (std::filesystem::exists(current / L"Asset", ec) && !ec)
					{
						return current;
					}

					const path parent = current.parent_path();
					if (parent == current)
					{
						break;
					}
					current = parent;
				}

				// 見つからない場合のフォールバックとして実行ファイル配置ディレクトリを返す
				return exeDir;
			}

			path GetExecutableDirectory()
			{
				std::wstring buffer(MAX_PATH, L'\0');

				while (true)
				{
					const DWORD length = ::GetModuleFileNameW(
						nullptr,
						buffer.data(),
						static_cast<DWORD>(buffer.size())
					);

					if (length == 0)
					{
						return path{};
					}

					if (length < buffer.size())
					{
						buffer.resize(length);
						return path(buffer).parent_path();
					}

					// 260文字(MAX_PATH)を超える長いパスに対応（無限拡張ガード付き）
					if (buffer.size() >= 32768)
					{
						return path{};
					}
					buffer.resize(buffer.size() * 2);
				}
			}
		} // namespace fs
	} // namespace kit
} // namespace ts