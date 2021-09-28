local __metatable = {}
local Button = {}

function Button.new(text, index, position, width)
    local button = {}

    button.text = text
    button.index = index

    button.position = position
    button.size = { width = width, height = 40 }

    button.textColorNormal = {0.33, 0.33, 0.33}
    button.textColorPressed = {0.99, 0.99, 0.98}
    button.textColorCurrent = button.textColorNormal

    function button:getIndex()
        return self.index
    end

    function button:draw(opacity)
        local r, g, b = unpack(button.textColorCurrent)
        love.graphics.setColor(r, g, b, opacity)

        local font = love.graphics.getFont()
        love.graphics.printf(self.text, self.position.x, self.position.y + (self.size.height - font:getHeight()) * 0.5, self.size.width, "center")
    end

    local function coordsInside(x, y, w, h, mouse_x, mouse_y)
        return (mouse_x > x and mouse_x + 1 < x + w and mouse_y > y and mouse_y + 1 < y + h)
    end

    function button:touchpressed(x, y)
        if coordsInside(self.position.x, self.position.y, self.size.width, self.size.height, x, y) then
            self.textColorCurrent = self.textColorPressed
            return true
        end
    end

    function button:touchreleased(x, y)
        if coordsInside(self.position.x, self.position.y, self.size.width, self.size.height, x, y) then
            self.textColorCurrent = self.textColorNormal
            return true
        end
    end

    return setmetatable(button, __metatable)
end

return setmetatable(Button, {__call = function(_, ...)
    return Button.new(...)
end})
