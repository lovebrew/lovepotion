local __metatable = {}
local Button = {}

function Button.new(text, index, position, width, scale)
    local button = {}

    button.text = text
    button.index = index

    button.position = position
    button.size = { width = width * scale, height = 70 * scale }

    button.highlights = {}

    button.highlights.light = {0.24, 0.91, 0.82}
    button.highlights.dark = {0.38, 0.80, 0.89}

    button.textColors = {}

    button.textColors.light = {0.21, 0.33, 1.00}
    button.textColors.dark = {0.00, 1.00, 0.80}

    button.selected = false

    function button:getIndex()
        return self.index
    end

    function button:draw(theme, opacity)
        if self.selected then
            local r, g, b = unpack(self.highlights[theme])
            love.graphics.setColor(r, g, b, math.abs(math.sin(love.timer.getTime() / 0.5)))

            love.graphics.rectangle("line", self.position.x, self.position.y, self.size.width, self.size.height, 4, 4)
        end

        local r, g, b = unpack(self.textColors[theme])
        love.graphics.setColor(r, g, b, opacity)

        local font = love.graphics.getFont()
        love.graphics.printf(self.text, self.position.x, self.position.y + (self.size.height - font:getHeight()) * 0.5, self.size.width, "center")
    end

    local function coordsInside(x, y, w, h, mouse_x, mouse_y)
        return (mouse_x > x and mouse_x + 1 < x + w and mouse_y > y and mouse_y + 1 < y + h)
    end

    function button:setSelected(value)
        self.selected = value
    end

    function button:touchpressed(x, y)
        if coordsInside(self.position.x, self.position.y, self.size.width, self.size.height, x, y) then
            -- change text color
            return true
        end
    end

    function button:touchreleased(x, y)
        if coordsInside(self.position.x, self.position.y, self.size.width, self.size.height, x, y) then
            -- change text color
            return true
        end
    end

    return setmetatable(button, __metatable)
end

return setmetatable(Button, {__call = function(_, ...)
    return Button.new(...)
end})
