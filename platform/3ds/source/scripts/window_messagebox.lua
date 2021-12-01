local Button     = require("window_button")
local MessageBox = {}

local __metatable = {}

local _SCREEN_SIZE = { WIDTH = 320, HEIGHT = 240 }

local _MESSGB_SIZE = { WIDTH = 300, HEIGHT = 216 }
local _MESSGB_POSN = { (_SCREEN_SIZE.WIDTH  - _MESSGB_SIZE.WIDTH)  * 0.5,
                       (_SCREEN_SIZE.HEIGHT - _MESSGB_SIZE.HEIGHT) * 0.5 }

MessageBox.States =
{
    SINGLE = "single",
    DOUBLE = "double"
}

-- Pressed Events
MessageBox.PRESSED_EVENTS = {}

MessageBox.PRESSED_EVENTS.mousepressed = true
MessageBox.PRESSED_EVENTS.touchpressed = true

-- Released Events
MessageBox.RELEASED_EVENTS = {}

MessageBox.RELEASED_EVENTS.mousereleased = true
MessageBox.RELEASED_EVENTS.touchreleased = true

MessageBox.Button = 1
MessageBox.Inited = false

MessageBox.Textures = {}

function MessageBox.initTextures()
    if MessageBox.Inited then
        return
    end

    MessageBox.Textures.single =
    {
        love.graphics.newImage("internal/graphics/messagebox_single_none.png"),
        love.graphics.newImage("internal/graphics/messagebox_single_pressed.png")
    }

    MessageBox.Textures.double =
    {
        love.graphics.newImage("internal/graphics/messagebox_two_none.png"),
        love.graphics.newImage("internal/graphics/messagebox_two_pressed_left.png"),
        love.graphics.newImage("internal/graphics/messagebox_two_pressed_right.png")
    }

    MessageBox.Inited = true
end

function MessageBox.new(text, buttons)
    MessageBox.initTextures()

    local messagebox = {}

    messagebox.text = text

    messagebox.state =  #buttons == 1 and MessageBox.States.SINGLE or MessageBox.States.DOUBLE
    messagebox.defaultTexture = MessageBox.Textures[messagebox.state][1]

    messagebox.currentTexture = messagebox.defaultTexture
    messagebox.opacity = 0
    messagebox.released = false

    messagebox.buttons = {}
    for index = 1, #buttons do
        local width = (#buttons == 1 and _MESSGB_SIZE.WIDTH or _MESSGB_SIZE.WIDTH * 0.5)
        local position = (#buttons == 2 and width or 0)

        local button = Button(buttons[index], index, { x = _MESSGB_POSN[1] + (index - 1) * position, y = (_MESSGB_POSN[2] + _MESSGB_SIZE.HEIGHT) - 40 }, width)
        table.insert(messagebox.buttons, button)
    end

    function messagebox:poll(name, ...)
        local args = {...}
        table.remove(args, 1)

        local x, y = args[1], args[2]

        if MessageBox.PRESSED_EVENTS[name] then
            for _, button in ipairs(self.buttons) do
                if button:touchpressed(x, y) then
                    MessageBox.Button = button:getIndex() + 1
                    self.currentTexture = MessageBox.Textures[self.state][MessageBox.Button]
                    break
                end
            end
        elseif MessageBox.RELEASED_EVENTS[name] then
            self.currentTexture = self.defaultTexture
            self.released = true
        end
    end

    function messagebox:getPressedButton()
        return MessageBox.Button
    end

    function messagebox:update(dt)
        if self.released and self.opacity == 0 then
            return true
        end

        if MessageBox.Button ~= 1 and self.released then
            self.opacity = math.max(self.opacity - dt / 0.20, 0)
        else
            self.opacity = math.min(self.opacity + dt / 0.20, 1)
        end
    end

    local font = love.graphics.newFont(20)

    function messagebox:draw()
        love.graphics.push("all")

        love.graphics.setColor(1, 1, 1, self.opacity)
        love.graphics.draw(self.currentTexture, _MESSGB_POSN[1], _MESSGB_POSN[2])

        love.graphics.setFont(font)

        love.graphics.setColor(0.33, 0.33, 0.33)
        love.graphics.printf(self.text, _MESSGB_POSN[1], _MESSGB_POSN[2] + 12, _MESSGB_SIZE.WIDTH, "center")

        for _, button in ipairs(self.buttons) do
            button:draw(self.opacity)
        end

        love.graphics.pop()
    end

    return setmetatable(messagebox, __metatable)
end

__metatable.__call = function(_, ...)
    return MessageBox.new(...)
end

return setmetatable(MessageBox, {__call = function(_, ...)
    return MessageBox.new(...)
end})
