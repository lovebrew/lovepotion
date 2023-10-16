R"luastring"--(
local utf8 = require("utf8")

local SCREEN_WIDTH  = 320
local SCREEN_HEIGHT = 240

local colors = {}

colors.header      = { 0.26, 0.26, 0.26, 1.00 }
colors.background  = { 0.93, 0.93, 0.93, 1.00 }
colors.text        = { 0.26, 0.26, 0.26, 1.00 }
colors.scroll_line = { 0.81, 0.81, 0.81, 1.00 }

local glyphs = {}

glyphs.a = utf8.char("0xE04C")
glyphs.b = utf8.char("0xE04D")
glyphs.x = utf8.char("0xE04E")
glyphs.y = utf8.char("0xE04F")

local fonts  = {}
fonts.header = love.graphics.newFont("standard", 20.5)
fonts.body   = love.graphics.newFont("standard", 18)
fonts.button = love.graphics.newFont("standard", 18)

local function checkbounds(x, y, self)
    if x >= self.x and x < (self.x + self.width) and y >= (self.y + self.height * self.value) and
        y < (self.height + self.y + self.height * self.value) then
        return true
    end
    return false
end

-- button class

local button = {}
button.__index = button

function button.new(id, text, x)
    local instance = setmetatable({}, button)

    instance.text = love.graphics.newTextBatch(fonts.button, text)

    instance.x = x
    instance.y = SCREEN_HEIGHT * 0.915

    return instance
end

function button:draw()
    love.graphics.draw(self.text, self.x, self.y)
end

function button:getWidth()
    return self.text:getWidth()
end

-- header class

local header = {}
header.__index = header

function header.new(title)
    local instance = setmetatable({}, header)

    instance.title = love.graphics.newTextBatch(fonts.header, title)
    instance.height = 28

    return instance
end

function header:draw()
    love.graphics.setColor(colors.header)
    love.graphics.rectangle("fill", 0, 0, SCREEN_WIDTH, self.height);

    love.graphics.setColor(colors.background)
    love.graphics.draw(self.title, 8, (self.height - fonts.header:getHeight()) / 2)
end

-- body class

local body = {}
body.__index = body

function body.new(text, x, y, width, height)
    local instance = setmetatable({}, body)

    instance.x      = x
    instance.y      = y
    instance.width  = width
    instance.height = height

    instance.text = text

    return instance
end

function body:draw(value)
    love.graphics.setScissor(self.x, self.y, self.width, self.height)
    love.graphics.push()

    love.graphics.translate(0, -value)

    love.graphics.setColor(colors.text)
    love.graphics.printf(self.text, fonts.body, self.x, self.y, self.width, "left")

    love.graphics.pop()
    love.graphics.setScissor()
end

-- scrollbar class

local scrollbar = {}
scrollbar.__index = scrollbar

function scrollbar.new(bodyObject, wrap_count, total_height)
    local instance = setmetatable({}, scrollbar)

    instance.x = SCREEN_WIDTH * 0.95
    instance.width = 8
    instance.height = bodyObject.height * 0.90
    instance.y = bodyObject.y + (bodyObject.height - instance.height) * 0.5

    instance.bar_width = instance.width * 2

    instance.value = 0
    instance.held = false
    instance.dragPosition = 0
    instance.touchId = nil
    instance.items = wrap_count
    instance.heightValue = total_height

    instance.barHeight = total_height / wrap_count

    return instance
end

function scrollbar:draw()
    love.graphics.setColor(colors.scroll_line)
    love.graphics.rectangle("fill", self.x, self.y, self.width, self.height, 4, 4)

    love.graphics.setColor(colors.text)
    love.graphics.rectangle("fill", self.x, self.y + math.min((self.height * self.value), self.height - self.barHeight), self.width, self.barHeight, 4, 4)
end

function scrollbar:update(dt)
    if not self.touchId then
        return
    end

    local _, y = love.touch.getPosition(self.touchId)
    local scroll_y = ((y - self.dragPosition) - self.y)
    local actual_range = self.height

    self.value = scroll_y / actual_range
    self.value = math.min(math.max(0, self.value), 1)
end

function scrollbar:touchpressed(id, x, y)
    if checkbounds(x, y, self) then
        self.held = true
        self.touchId = id
        self.dragPosition = y - (self.y + self.height * self.value)
    end
end

function scrollbar:touchreleased()
    if self.held then
        self.held = false
        self.touchId = nil
    end
end

function scrollbar:getValue()
    return self.value * ((self.items - 10) * fonts.body:getHeight())
end

-- messagebox class

local messagebox = {}
messagebox.__index = messagebox

local function create_button(id, text, offset)
    local padding = 8
    local value = string.format("%s %s", glyphs[text[id].button], text[id].text)
    local x = (SCREEN_WIDTH - (padding * 2)) - fonts.button:getWidth(value)

    if id == 2 then
        x = x - (offset + padding * 2)
    end

    return button.new(id, value, x)
end

function messagebox.new(title, text, buttons)
    local instance = setmetatable({}, messagebox)

    instance.header = header.new(title)

    local body_text = text:sub(1, 0x1FD) .. "..."
    instance.body = body.new(body_text, 8, instance.header.height + 8, SCREEN_WIDTH * 0.80, SCREEN_HEIGHT * 0.75)

    local first_button = create_button(1, buttons)
    instance.buttons = { first_button }

    if #buttons == 2 then
        table.insert(instance.buttons, create_button(2, buttons, first_button:getWidth()))
    end

    local width, wrapped = fonts.body:getWrap(body_text, SCREEN_WIDTH * 0.8)
    if #wrapped > 10 then
        instance.scrollbar = scrollbar.new(instance.body, #wrapped, #wrapped * fonts.body:getHeight())
    end

    instance.height = SCREEN_HEIGHT

    return instance
end

function messagebox:poll(name, ...)
    if name == "gamepadpressed" then
        local button_name = select(2, ...)

        if button_name == "a" or button_name == "b" then
            return button_name == "a" and 1 or 2
        end
    end

    if self.scrollbar then
        if name == "touchpressed" then
            local id, x, y = select(1, ...), select(2, ...), select(3, ...)
            self.scrollbar:touchpressed(id, x, y)
        elseif name == "touchreleased" then
            self.scrollbar:touchreleased()
        end
    end
end

function messagebox:update(dt)
    if self.scrollbar then
        self.scrollbar:update(dt)
    end
end

function messagebox:draw(screen)
    if screen ~= "bottom" then
        return
    end

    love.graphics.setColor(colors.background)
    love.graphics.rectangle("fill", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)

    self.body:draw(self.scrollbar:getValue())

    for index = 1, #self.buttons do
        self.buttons[index]:draw()
    end

    if self.scrollbar then
        self.scrollbar:draw()
    end

    self.header:draw()
end

return messagebox
-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
