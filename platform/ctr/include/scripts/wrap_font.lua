R"luastring"--(
local Font_mt = ...
local Font = Font_mt.__index

local utf8 = require("utf8")

-- Gets formatting information for text, given a wrap limit.
-- This function accounts for newlines correctly (i.e. '\n').
-- @param  text       The text that will be wrapped.
-- @param  wrap_limit The maximum width in pixels of each line that text is allowed before wrapping.
-- @return width      The maximum width of the wrapped text.
-- @return wrapped    A sequence containing each line of text that was wrapped.
-- https://love2d.org/wiki/Font:getWrap
function Font:getWrap(text, wrap_limit)
    local lines = {}
    local line_widths = {}

    local width = 0
    local line_codepoints = {}

    local previous_glyph = 0

    local last_space_index = -1
    local width_before_last_space = 0
    local width_of_trailing_space = 0

    local length = utf8.len(text)
    local codepoints = {}

    -- get the codepoints into a table
    for index = 1, length do
        local codepoint = utf8.codepoint(text, index, index)
        table.insert(codepoints, codepoint)
    end

    local const_codepoints = { newline = 10, carriage = 13, space = 32 }

    local index = 1
    local char_width = 0

    while index < #codepoints + 1 do
        local codepoint = codepoints[index]

        repeat
            -- push the line on newline
            if codepoint == const_codepoints.newline then
                table.insert(lines, line_codepoints)

                -- ignore the width of trailing spaces for individual lines
                table.insert(line_widths, width - width_of_trailing_space)

                width = 0
                width_before_last_space = 0
                width_of_trailing_space = 0

                previous_glyph = 0
                last_space_index = -1
                line_codepoints = {}

                index = index + 1

                break
            end

            -- ignore carriage returns
            if codepoint == const_codepoints.carriage then
                index = index + 1
                break
            end

            char_width = Font:getWidth(codepoint)
            local new_width = width + char_width

            -- push the line after limit is hit
            if codepoint ~= const_codepoints.space and new_width > wrap_limit then
                -- if this is the first character in the line and exceeds the limit
                -- we should skip it completely
                if #line_codepoints == 0 then
                    index = index + 1
                elseif last_space_index ~= -1 then
                    -- rewind to the last seen space, if the line has one
                    while #line_codepoints ~= 0 and line_codepoints[#line_codepoints] ~= const_codepoints.space do
                        table.remove(line_codepoints, #line_codepoints)
                    end

                    -- ignore width of trailing spaces in wrapped lines
                    width = width_before_last_space
                    index = last_space_index

                    -- start next line after the space
                    index = index + 1
                end

                table.insert(lines, line_codepoints)
                table.insert(line_widths, width)

                previous_glyph = 0

                width = 0
                width_before_last_space = 0
                width_of_trailing_space = 0

                line_codepoints = {}
                last_space_index = -1

                break
            end


            if previous_glyph ~= const_codepoints.space and codepoint ~= const_codepoints.space then
                width_before_last_space = width
            end

            width = new_width
            previous_glyph = codepoint

            table.insert(line_codepoints, codepoint)

            -- keep track of the last seen space to rewind to when wrapping
            if codepoint == const_codepoints.space then
                last_space_index = index
                width_of_trailing_space = width_of_trailing_space + char_width
            elseif codepoint ~= const_codepoints.newline then
                width_of_trailing_space = 0
            end
            index = index + 1
        until true
    end

    -- push the last line
    table.insert(lines, line_codepoints)
    table.insert(line_widths, width - width_of_trailing_space)

    local max_length = 0
    for width_index = 1, #line_widths do
        if line_widths[width_index] > max_length then
            max_length = line_widths[width_index]
        end
    end

    for line_index, value in ipairs(lines) do
        lines[line_index] = utf8.char(unpack(value))
    end

    return max_length, lines
end
--)luastring"--"
