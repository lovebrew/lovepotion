local Button     = require("window_button")
local MessageBox = {}

local __metatable = {}

local _MESSGB_SIZE = { WIDTH = 770, HEIGHT = 292 }

MessageBox.States =
{
    SINGLE = "single",
    DOUBLE = "double"
}

MessageBox.Button = 1
MessageBox.Inited = false

MessageBox.Textures = {}
MessageBox.Textures.light = {}
MessageBox.Textures.dark  = {}

local function _getColorTheme()
    if love.system.getOS() == "Horizon" then
        return love.system.getColorTheme()
    end
    return "light"
end

function MessageBox.initTextures()
    if MessageBox.Inited then
        return
    end

    MessageBox.Textures.light.single =
    {
        love.graphics.newImage("internal/graphics/messagebox_light.png"),
        love.graphics.newImage("internal/graphics/messagebox_light_pressed.png")
    }

    MessageBox.Textures.light.double =
    {
        love.graphics.newImage("internal/graphics/messagebox_light_two_none.png"),
        love.graphics.newImage("internal/graphics/messagebox_light_two_left.png"),
        love.graphics.newImage("internal/graphics/messagebox_light_two_left_pressed.png"),
        love.graphics.newImage("internal/graphics/messagebox_light_two_right.png"),
        love.graphics.newImage("internal/graphics/messagebox_light_two_right_pressed.png")
    }

    MessageBox.Textures.dark.single =
    {
        love.graphics.newImage("internal/graphics/messagebox_dark.png"),
        love.graphics.newImage("internal/graphics/messagebox_dark_pressed.png")
    }

    MessageBox.Textures.dark.double =
    {
        love.graphics.newImage("internal/graphics/messagebox_dark_two_none.png"),
        love.graphics.newImage("internal/graphics/messagebox_dark_two_left.png"),
        love.graphics.newImage("internal/graphics/messagebox_dark_two_left_pressed.png"),
        love.graphics.newImage("internal/graphics/messagebox_dark_two_right.png"),
        love.graphics.newImage("internal/graphics/messagebox_dark_two_right_pressed.png")
    }

    MessageBox.Inited = true
end

function MessageBox.new(text, buttons)
    local messagebox = {}

    MessageBox.initTextures()

    messagebox.text = text
    messagebox.buttons = {}

    messagebox.theme = _getColorTheme()
    messagebox.state = #buttons == 1 and MessageBox.States.SINGLE or MessageBox.States.DOUBLE

    messagebox.defaultTexture = MessageBox.Textures[messagebox.theme][messagebox.state][1]
    messagebox.currentTexture = messagebox.defaultTexture

    messagebox.opacity = 0
    messagebox.selection = 1
    messagebox.submitted = false

    messagebox.textColors = {}

    messagebox.textColors.light = {0, 0, 0}
    messagebox.textColors.dark = {1, 1, 1}

    messagebox.oldFont = love.graphics.getFont()

    function messagebox:initButtons()
        for index = 1, #buttons do
            local scale = self:getScaleValue()
            local width = (#buttons == 1 and _MESSGB_SIZE.WIDTH or (_MESSGB_SIZE.WIDTH * 0.5))
            local offset = (#buttons == 2 and width or 0)
            local x, y = self:getPosition()

            local position = { x = x + (index - 1) * offset,
                               y = (y + _MESSGB_SIZE.HEIGHT * scale) - 70 * scale }

            local button = Button(buttons[index], index, position, width, scale)
            table.insert(self.buttons, button)
        end

        self.buttons[self.selection]:setSelected(true)

        self:updateTexture()
    end

    function messagebox:updateTexture()
        -- press
        if self.state == "single" then
            self.currentTexture = MessageBox.Textures[self.theme][self.state][MessageBox.Button]
        else
            local id = 3
            -- press
            if self.selection == 0 then
                if MessageBox.Button == 2 then
                    id = 3
                elseif MessageBox.Button == 3 then
                    id = 5
                end
            else -- gamepadaxis
                if self.selection == 1 then
                    id = 2
                elseif self.selection == 2 then
                    id = 4
                end
            end
            self.currentTexture = MessageBox.Textures[self.theme][self.state][id]
        end
    end

    function messagebox:poll(name, ...)
        local args = {...}
        table.remove(args, 1)

        local x, y = args[1], args[2]

        if name == "resize" then
            self:initButtons()
        end

        if self.opacity < 1 then
            return
        end

        -- touch events
        local pass = false
        if name == "touchpressed" or name == "mousepressed" then
            for _, button in ipairs(self.buttons) do
                if button:touchpressed(x, y) then
                    MessageBox.Button = button:getIndex() + 1
                    self:updateTexture()
                    pass = true
                    break
                end
            end

            if not pass then
                MessageBox.Button = 1
                self.selection = 0

                for _, button in ipairs(self.buttons) do
                    button:setSelected(false)
                end
            end
        elseif name == "touchreleased" or name == "mousereleased" then
            if pass then
                self.currentTexture = self.defaultTexture
                self.submitted = true
            end
        end

        -- gamepadaxis
        if name == "gamepadaxis" and not self.submitted then
            local axis, value = args[1], args[2]

            if axis == "leftx" then
                self.buttons[self.selection]:setSelected(false)

                if value > 0.5 then
                    self.selection = math.min(self.selection + 1, #self.buttons)
                elseif value < -0.5 then
                    self.selection = math.max(self.selection - 1, 1)
                end

                self.buttons[self.selection]:setSelected(true)
                MessageBox.Button = self.selection + 1

                self:updateTexture()
            end
        end

        -- gamepadpressed
        if name == "gamepadpressed" then
            if self.selection > 0 and args[1] == "a" then
                self.buttons[self.selection]:setSelected(false)
                MessageBox.Button = self.selection + 1
                self:updateTexture()
                self.submitted = true
            end
        elseif name == "gamepadreleased" then
            self.currentTexture = self.defaultTexture
        end
    end

    function messagebox:update(dt)
        if self.submitted and self.opacity == 0 then
            return true
        end

        if MessageBox.Button ~= 1 and self.submitted then
            self.opacity = math.max(self.opacity - dt / 0.10, 0)
        else
            self.opacity = math.min(self.opacity + dt / 0.20, 1)
        end
    end


    function messagebox:getScaleValue()
        local _, screenHeight = love.graphics.getDimensions()

        if screenHeight == 720 then
            return 1
        end
        return 1.5
    end

    function messagebox:getPosition()
        local screenWidth, screenHeight = love.graphics.getDimensions()
        local scale = self:getScaleValue()
        local width, height = _MESSGB_SIZE.WIDTH * scale, _MESSGB_SIZE.HEIGHT * scale

        return (screenWidth - width) * 0.5, (screenHeight - height) * 0.5
    end

    local font = love.graphics.newFont(24 * messagebox:getScaleValue())

    function messagebox:draw()
        love.graphics.push("all")

        love.graphics.setColor(1, 1, 1, self.opacity)

        local x, y = self:getPosition()
        local scale = self:getScaleValue()

        love.graphics.draw(self.currentTexture, x, y, 0, scale, scale)

        love.graphics.setFont(font)

        local r, g, b = unpack(self.textColors[_getColorTheme()])
        love.graphics.setColor(r, g, b, self.opacity)
        love.graphics.printf(self.text, x, y + 12, _MESSGB_SIZE.WIDTH * scale, "center")

        for _, button in ipairs(self.buttons) do
            button:draw(self.theme, self.opacity)
        end

        love.graphics.pop()
    end

    function messagebox:getPressedButton()
        if not self.submitted then
            return
        end
        return MessageBox.Button
    end

    messagebox:initButtons()

    return setmetatable(messagebox, __metatable)
end

__metatable.__call = function(_, ...)
    return MessageBox.new(...)
end

return setmetatable(MessageBox, {__call = function(_, ...)
    return MessageBox.new(...)
end})
