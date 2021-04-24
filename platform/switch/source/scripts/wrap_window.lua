-- set up variables
local textFont = love.graphics.newFont(24)
local glyphFont = nil

local textColors = {}

textColors.buttons = {}

textColors.buttons.light = { 0.16, 0.28, 0.80 }
textColors.buttons.dark  = {}

textColors.buttons.default = textColors.buttons.light

textColors.body = {}

textColors.body.light = { 0.00, 0.00, 0.00 }
textColors.body.dark  = { 1.00, 1.00, 1.00 }

textColors.body.default = textColors.body.light

local dividerColors = {}

dividerColors.light = { 0.82, 0.82, 0.82 }
dividerColors.dark  = {}

dividerColors.default = dividerColors.light

local selectionColors = {}

selectionColors.light = { 0.28, 0.95, 0.84 }
selectionColors.dark  = {}

selectionColors.default = selectionColors.light

g_windowShown = false

local boxPosition = {}
local boxSize = {}

local utf8 = require("utf8")

local function newButton(id, t, x, width)
    local button = {}

    assert(type(t) == "table" or type(tostring(t)) == "string")

    if type(t) == "table" then
        button.text = t[1] or t.text
        button.shortcut = t[2] or t.button
    else
        button.text = t
    end

    button.width = width

    button.x = x
    button.y = boxPosition.y + (boxSize.h - 70)

    button.width = width
    button.height = 70

    button.id = id or 1

    button.textColor = textColors.buttons.default

    function button:draw()
        local x, y = self.x + (self.width - textFont:getWidth(self.text)) / 2, self.y + (self.height - textFont:getHeight()) / 2

        love.graphics.setColor(self.textColor)
        love.graphics.print(self.text, textFont, x, y)
    end

    function button:getID()
        return self.id
    end

    function button:gamepadpressed(_, button)
        if button == self.shortcut then
            return true
        end
    end

    function button:getBounds()
        return self.x, self.y, self.width, self.height
    end

    function button:touchpressed(_, x, y)
        if (x > self.x and x + 1 < self.x + self.width and y > self.y and y + 1 < self.y + self.height) then
            -- self.textColor = textColors.pressed
            return true
        end
    end

    function button:touchreleased()
        -- self.textColor = textColors.default
    end

    function button:mousepressed(x, y)
        return self:touchpressed(nil, x, y)
    end

    return button
end

local function newMessageBox(text, buttons)
    local messagebox = {}

    local currentLineWidth = love.graphics.getLineWidth()
    local currentFont = love.graphics.getFont()

    messagebox.buttons = {}

    if buttons then
        assert(#buttons < 3, "cannot have more than two buttons")
    end

    if not buttons or #buttons == 0 then
        table.insert(messagebox.buttons, newButton(1, "OK", boxPosition.x, boxSize.w))
    else
        local width = (boxSize.w / #buttons)
        for i = 1, #buttons do
            table.insert(messagebox.buttons, newButton(i, buttons[i], boxPosition.x + (i - 1) * width, width))
        end
    end

    messagebox.baseTexture = messagebox.texture
    messagebox.mode = #messagebox.buttons == 1 and "single" or "double"
    messagebox.opacity = 0

    local buttonID = nil

    local pressedEvents = {}

    pressedEvents.gamepadpressed = true
    pressedEvents.mousepressed = true
    pressedEvents.touchpressed = true
    pressedEvents.gamepadaxis = true

    local releasedEvents = {}

    releasedEvents.gamepadreleased = true
    releasedEvents.touchreleased = true
    releasedEvents.mousereleased = true

    messagebox.selection = 1

    function messagebox:poll(event, ...)
        local args = {...}

        if pressedEvents[event] then
            if event == "touchpressed" then
                for _, button in ipairs(self.buttons) do
                    if button:touchpressed(unpack(args)) then
                        buttonID = button:getID()
                    end
                end
            elseif event == "mousepressed" then
                for _, button in ipairs(self.buttons) do
                    if button:mousepressed(unpack(args)) then
                        buttonID = button:getID()
                    end
                end
            elseif event == "gamepadpressed" then
                for _, button in ipairs(self.buttons) do
                    if button:gamepadpressed(unpack(args)) then
                        buttonID = button:getID()
                    end
                end
                table.remove(args, 1)

                if args[1] == "dpright" then
                    self.selection = math.min(self.selection + 1, #self.buttons)
                elseif args[1] == "dpleft" then
                    self.selection = math.max(1, self.selection - 1)
                end
            elseif event == "gamepadaxis" then
                table.remove(args, 1)

                if args[1] == "leftx" then
                    if args[2] > 0.5 then
                        self.selection = math.min(self.selection + 1, #self.buttons)
                    elseif args[2] < -0.5 then
                        self.selection = math.max(1, self.selection - 1)
                    end
                end

                buttonID = self.selection
            end
        elseif releasedEvents[event] then
            return self.mode == "double" and buttonID or buttonID and true
        end
    end

    function messagebox:update(dt)
        self.opacity = math.min(self.opacity + dt / 0.1, 1)
    end

    function messagebox:draw()
        love.graphics.setColor(0, 0, 0, 0.5)
        love.graphics.rectangle("fill", 0, 0, love.graphics.getWidth("bottom"), love.graphics.getHeight())

        love.graphics.setColor(1, 1, 1, self.opacity)
        love.graphics.rectangle("fill", boxPosition.x, boxPosition.y, boxSize.w, boxSize.h, 4, 4)

        love.graphics.setColor(textColors.body.default)
        love.graphics.printf(text, textFont, boxPosition.x + 72, ((boxPosition.y + boxSize.h) - 70) / 2, boxSize.w - 144, "left")

        love.graphics.setColor(dividerColors.default)
        love.graphics.line(boxPosition.x, boxPosition.y + (boxSize.h - 70), boxPosition.x + boxSize.w, boxPosition.y + (boxSize.h - 70))

        if self.mode == "double" then
            love.graphics.line(boxPosition.x + boxSize.w / 2, boxPosition.y + (boxSize.h - 70), boxPosition.x + boxSize.w / 2, boxPosition.y + boxSize.h)
        end

        for _, value in ipairs(self.buttons) do
            value:draw()

            if self.selection == value:getID() then
                local r, g, b = unpack(selectionColors.default)
                love.graphics.setColor(r, g, b, math.abs(math.sin(love.timer.getTime() / 0.5)))

                local x, y, w ,h = value:getBounds()

                love.graphics.setLineWidth(5)
                love.graphics.rectangle("line", x, y, w, h, 8, 8)
                love.graphics.setLineWidth(currentLineWidth)
            end
        end


        love.graphics.setFont(currentFont)
        love.graphics.setColor(1, 1, 1, 1)
    end

    return messagebox
end

local box

local function main()
    while true do
        if love.event and love.event.pump then
            love.event.pump()

            for name, a, b, c, d, e, f in love.event.poll() do
                local value = box:poll(name, a, b, c, d, e, f)

                if value then
                    g_windowShown = false
                    return value
                end
            end
        end

        if love.timer then
            dt = love.timer.step()
        end

        -- Call update
        box:update(dt)

        if love.graphics and love.graphics.isActive() then
            love.graphics.origin()
            love.graphics.clear(love.graphics.getBackgroundColor())

            if love.draw then
                love.draw()
            end

            box:draw()

            love.graphics.present()
        end

        if love.timer then
            love.timer.sleep(0.001)
        end
    end
end

function love.window.showMessageBox(_, text, buttons)
    g_windowShown = true

    boxSize = { w = 770, h = 352}
    boxPosition = { x = (love.graphics.getWidth() - boxSize.w) / 2, y = (love.graphics.getHeight() - boxSize.h) / 2}

    box = newMessageBox(text, buttons)

    return main()
end
