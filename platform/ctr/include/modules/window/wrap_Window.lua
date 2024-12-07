R"luastring"--(
-- DO NOT REMOVE THE ABOVE LINE. It is used to load this file as a C++ string.
-- There is a matching delimiter at the bottom of the file.
local title, message, buttons = ...
local result                  = nil

local header_background_color = { 0.34, 0.34, 0.33 }
local header_text_color       = { 0.96, 0.96, 0.96 }
local body_text_color         = { 0.20, 0.20, 0.20 }
local body_background_color   = { 0.88, 0.88, 0.90 }
local divider_bar_color       = { 0.48, 0.48, 0.46 }
local background_bar_color    = { 0.82, 0.82, 0.78 }


local header_font = love.graphics.newFont(14)
local button_font = love.graphics.newFont(14)
local body_font   = love.graphics.newFont(12)

local function aabb(x, y, width, height, other_x, other_y, other_width, other_height)
    return x < other_x + other_width and other_x < x + width and
        y < other_y + other_height and other_y < y + height
end

local ScrollBar = {}
ScrollBar.__index = ScrollBar

function ScrollBar.new(x, y, height)
    local instance = setmetatable({}, ScrollBar)

    instance.x = x
    instance.y = y
    instance.width = 20
    instance.height = height

    instance.bar_x = x
    instance.bar_y = y
    instance.bar_width = 20
    instance.bar_height = 20

    instance.held = false
    instance.drag_pos = 0
    instance.value = 0

    instance.touch_id = nil

    return instance
end

function ScrollBar:update(dt)
    if not self.held then
        return
    end

    local _, y = nil, 0

    if self.touch_id then
        _, y = love.touch.getPosition(self.touch_id)
    end

    -- Calculate the relative scroll position
    local scroll_y = (y - self.drag_pos) - self.y

    -- Adjust to ensure the value hits 1 at the correct position
    self.value = math.min(math.max(0, scroll_y / (self.height - self.bar_height)), 1)
end

function ScrollBar:getValue()
    return self.value
end

function ScrollBar:draw()
    -- Draw the scrollbar track
    love.graphics.setColor(0.84, 0.84, 0.84)
    love.graphics.rectangle("fill", self.x + (self.width - 8) / 2, self.y, 8, self.height, 2, 8)

    -- Draw the inner track
    love.graphics.setColor(0.48, 0.48, 0.48)
    love.graphics.rectangle("fill", self.x + (self.width - 2) / 2, self.y + 8, 2, self.height - 16)

    -- Draw the bar itself using the corrected position
    local bar_position_y = self.bar_y + (self.height - self.bar_height) * self.value
    love.graphics.setColor(0.48, 0.48, 0.48)
    love.graphics.rectangle("line", self.bar_x, bar_position_y, self.bar_width, self.bar_height, 2, 2)

    -- Fill the bar with the background color
    love.graphics.setColor(body_background_color)
    love.graphics.rectangle("fill", self.bar_x + 1, bar_position_y + 1, self.bar_width - 2, self.bar_height - 2, 2, 2)

    -- Add decorative lines on the bar
    love.graphics.setColor(0.48, 0.48, 0.48)
    love.graphics.rectangle("fill", self.bar_x + 3, bar_position_y + (self.bar_height * 0.35), self.bar_width - 6, 1)
    love.graphics.rectangle("fill", self.bar_x + 3, bar_position_y + (self.bar_height * 0.55), self.bar_width - 6, 1)
end

function ScrollBar:mousepressed(x, y)
    if not aabb(self.x, self.bar_y + (self.height - self.bar_height) * self.value, self.bar_width, self.bar_height, x, y, 1, 1) then
        return
    end

    if not self.held then
        self.drag_pos = y - (self.bar_y + (self.height - self.bar_height) * self.value)
        self.held = true
    end
end

function ScrollBar:mousereleased(x, y)
    if self.held then
        self.held = false
    end
end

function ScrollBar:touchpressed(id, x, y)
    if not aabb(self.x, self.bar_y + (self.height - self.bar_height) * self.value, self.bar_width, self.bar_height, x, y, 1, 1) then
        return
    end

    if not self.held then
        self.touch_id = id
        self.drag_pos = y - (self.bar_y + (self.height - self.bar_height) * self.value)
        self.held = true
    end
end

function ScrollBar:touchreleased()
    if self.held then
        self.touch_id = nil
        self.held = false
    end
end

local Button = {}
Button.__index = Button

function Button.new(text, x, y, width, height)
    local instance = setmetatable({}, Button)

    instance.x = x
    instance.y = y
    instance.width = width
    instance.height = height

    instance.pressed = false
    instance.text = text

    return instance
end

function Button:inside(x, y)
    return aabb(self.x, self.y, self.width, self.height, x, y, 1, 1)
end

function Button:press(pressed)
    self.pressed = pressed
end

function Button:draw()
    if self.pressed then
        love.graphics.setColor(0.71, 0.71, 0.68)
        love.graphics.rectangle("fill", self.x, self.y, self.width, self.height)
    end

    local y = self.y + (self.height - button_font:getHeight()) * 0.5

    love.graphics.setColor(1, 1, 1)
    love.graphics.printf(self.text, button_font, self.x, y + 2, self.width, "center")

    love.graphics.setColor(body_text_color)
    love.graphics.printf(self.text, button_font, self.x, y, self.width, "center")
end

local MessageBox = {}
MessageBox.__index = MessageBox

function MessageBox.new()
    local instance = setmetatable({}, MessageBox)

    instance.width = 320
    instance.height = 240

    instance.header_height = 28
    instance.header_divider_height = 2

    instance.body_content_height = 184
    instance.body_content_offset_x = 6
    instance.body_content_offset_y = 37
    instance.content_wrap_width = instance.width - 36

    instance.button_height = 28
    instance.button_y = 212

    instance.buttons = {}
    if not buttons then
        table.insert(instance.buttons, Button.new("OK", 0, instance.button_y, instance.width, instance.button_height))
    else
        local button_width = instance.width / #buttons
        for index = 1, #buttons do
            instance.buttons[index] = Button.new(buttons[index], 0 + (index - 1) * button_width, instance.button_y,
                button_width, instance.button_height)
        end
    end

    local _, lines = body_font:getWrap(message, instance.content_wrap_width)
    instance.line_size = #lines * body_font:getHeight()

    if instance.line_size > instance.body_content_height then
        instance.scroll_bar = ScrollBar.new(297, 39, 160)
    end

    return instance
end

function MessageBox:poll(event, a, b, c)
    if event == "keypressed" and a == "space" then
        return true
    elseif event == "gamepadpressed" then
        if #self.buttons == 1 and b == "a" then
            self.buttons[1]:press(true)
        elseif #self.buttons == 2 then
            if b == "a" then
                self.buttons[2]:press(true)
            elseif b == "b" then
                self.buttons[1]:press(true)
            end
        end
    elseif event == "gamepadreleased" then
        if #self.buttons == 1 and b == "a" then
            self.buttons[1]:press(false)
            return true
        elseif #self.buttons == 2 then
            if b == "a" then
                self.buttons[2]:press(false)
                return 2
            elseif b == "b" then
                self.buttons[1]:press(false)
                return 1
            end
        end
    end

    if event == "touchpressed" then
        for _, button in ipairs(self.buttons) do
            if button:inside(b, c) then
                button:press(true)
            end
        end
    elseif event == "touchreleased" then
        for index, button in ipairs(self.buttons) do
            if button:inside(b, c) then
                button:press(false)
                return #buttons == 1 and true or index
            end
        end
    end

    if not self.scroll_bar then
        return
    end

    if event == "mousepressed" then
        self.scroll_bar:mousepressed(a, b)
    elseif event == "mousereleased" then
        self.scroll_bar:mousereleased(a, b)
    elseif event == "touchpressed" then
        self.scroll_bar:touchpressed(a, b, c)
    elseif event == "touchreleased" then
        self.scroll_bar:touchreleased()
    end
end

function MessageBox:update(dt)
    if self.scroll_bar then
        self.scroll_bar:update(dt)
    end
end

function MessageBox:draw(screen, iod)
    if screen ~= "bottom" then return end
    love.graphics.setColor(body_background_color)
    love.graphics.rectangle("fill", 0, 0, self.width, self.height)

    love.graphics.setColor(background_bar_color)
    for i = 1, self.body_content_height / 4 do
        love.graphics.rectangle("fill", 6, 27 + (i - 1) * 4, 308, 1)
    end

    love.graphics.setScissor(self.body_content_offset_x, 37, self.width,
        self.body_content_height - self.button_height / 2)
    love.graphics.push()

    if self.scroll_bar then
        local max_scroll = math.max(0, self.line_size - (self.body_content_height - 37 / 2))
        local scroll_offset = self.scroll_bar:getValue() * max_scroll

        love.graphics.translate(0, -scroll_offset)
    end

    love.graphics.setColor(body_text_color)
    love.graphics.printf(message, body_font, self.body_content_offset_x, self.body_content_offset_y,
        self.content_wrap_width, "left")

    love.graphics.pop()

    love.graphics.setScissor()

    love.graphics.setColor(header_background_color)
    love.graphics.rectangle("fill", 0, 0, self.width, self.header_height)

    love.graphics.setColor(header_text_color)
    love.graphics.print(title, header_font, 7, (self.header_height - header_font:getHeight()) * 0.5)

    love.graphics.setColor(0, 0, 0, 0.075)
    love.graphics.rectangle("fill", 0, 0, self.width, 6)

    love.graphics.setColor(divider_bar_color)
    love.graphics.rectangle("fill", 0, self.header_height - (self.header_divider_height * 2), self.width,
        self.header_divider_height)

    love.graphics.setColor(divider_bar_color)
    love.graphics.rectangle("fill", 0, self.button_y, self.width, 1)

    for _, button in ipairs(self.buttons) do
        button:draw()
    end

    if self.scroll_bar then
        self.scroll_bar:draw()
    end
end

local function get_3d_depth(screen)
    if love._console ~= "3DS" then
        return nil
    end

    local depth = love.graphics.getDepth()
    if screen == "bottom" then
        depth = -depth
    elseif screen == "bottom" then
        return nil
    end

    return depth
end

MESSAGEBOX = MessageBox.new()

local dt = 0

while MESSAGEBOX do
    if result then break end

    if love.event and love.event.pump then
        love.event.pump()

        for name, a, b, c in love.event.poll() do
            result = MESSAGEBOX:poll(name, a, b, c)
            if result then break end
        end
    end

    if love.timer then
        dt = love.timer.step()
    end

    MESSAGEBOX:update(dt)

    if love.graphics and love.graphics.isActive() then
        local screens = love.graphics.getScreens()

        for _, screen in ipairs(screens) do
            love.graphics.setActiveScreen(screen)

            love.graphics.origin()
            love.graphics.clear(love.graphics.getBackgroundColor())

            MESSAGEBOX:draw(screen, get_3d_depth(screen))

            love.graphics.copyCurrentScanBuffer()
        end

        love.graphics.present()
    end

    if love.timer then
        love.timer.sleep(0.001)
    end
end

MESSAGEBOX = nil
return result
-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
