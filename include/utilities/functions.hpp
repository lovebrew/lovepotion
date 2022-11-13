#pragma once

#include <algorithm>
#include <filesystem>

namespace love
{
    template<std::ranges::range Range>
    requires(std::is_pointer_v<std::ranges::range_value_t<Range>>)
    inline bool is_nullptr_empty(const Range& range)
    {
        if (range.empty())
            return true;

        const auto lambda = [](const auto& v) { return v == nullptr; };
        return std::ranges::all_of(range.begin(), range.end(), lambda);
    }

    inline bool has_file_extension(const std::filesystem::path& path)
    {
        return path.has_extension();
    }
} // namespace love
