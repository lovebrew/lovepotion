R"luastring"--(
-- DO NOT REMOVE THE ABOVE LINE. It is used to load this file as a C++ string.
-- There is a matching delimiter at the bottom of the file.
local love = require("love")

function love.nogame()
    local function require_30log()
        local assert, pairs, type, tostring, setmetatable = assert, pairs, type, tostring, setmetatable
        local baseMt, _instances, _classes, _class = {}, setmetatable({}, { __mode = 'k' }),
            setmetatable({}, { __mode = 'k' })
        local function assert_class(class, method) assert(_classes[class],
                ('Wrong method call. Expected class:%s.'):format(method)) end
        local function deep_copy(t, dest, aType)
            t = t or {}; local r = dest or {}
            for k, v in pairs(t) do
                if aType and type(v) == aType then
                    r[k] = v
                elseif not aType then
                    if type(v) == 'table' and k ~= "__index" then r[k] = deep_copy(v) else r[k] = v end
                end
            end; return r
        end
        local function instantiate(self, ...)
            assert_class(self, 'new(...) or class(...)'); local instance = { class = self }; _instances[instance] =
            tostring(instance); setmetatable(instance, self)
            if self.init then if type(self.init) == 'table' then deep_copy(self.init, instance) else self.init(instance,
                        ...) end; end; return instance
        end
        local function extend(self, name, extra_params)
            assert_class(self, 'extend(...)'); local heir = {}; _classes[heir] = tostring(heir); deep_copy(extra_params,
                deep_copy(self, heir));
            heir.name, heir.__index, heir.super = extra_params and extra_params.name or name, heir, self; return
            setmetatable(heir, self)
        end
        baseMt = {
            __call = function(self, ...) return self:new(...) end,
            __tostring = function(self, ...)
                if _instances[self] then return ("instance of '%s' (%s)"):format(rawget(self.class, 'name') or '?',
                        _instances[self]) end
                return _classes[self] and ("class '%s' (%s)"):format(rawget(self, 'name') or '?', _classes[self]) or self
            end
        }; _classes[baseMt] = tostring(baseMt); setmetatable(baseMt, { __tostring = baseMt.__tostring })
        local class = {
            isClass = function(class, ofsuper)
                local isclass = not not _classes[class]; if ofsuper then return isclass and (class.super == ofsuper) end; return
                isclass
            end,
            isInstance = function(instance, ofclass)
                local isinstance = not not _instances[instance]; if ofclass then return isinstance and
                    (instance.class == ofclass) end; return isinstance
            end
        }; _class = function(name, attr)
            local c = deep_copy(attr); c.mixins = setmetatable({}, { __mode = 'k' }); _classes[c] = tostring(c); c.name, c.__tostring, c.__call =
            name or c.name, baseMt.__tostring, baseMt.__call
            c.include = function(self, mixin)
                assert_class(self, 'include(mixin)'); self.mixins[mixin] = true; return deep_copy(mixin, self, 'function')
            end
            c.new, c.extend, c.__index, c.includes = instantiate, extend, c,
                function(self, mixin)
                    assert_class(self, 'includes(mixin)')
                    return not not (self.mixins[mixin] or (self.super and self.super:includes(mixin)))
                end
            c.extends = function(self, class)
                assert_class(self, 'extends(class)')
                local super = self; repeat super = super.super until (super == class or super == nil); return class and
                (super == class)
            end
            return setmetatable(c, baseMt)
        end; class._DESCRIPTION = '30 lines library for object orientation in Lua'; class._VERSION = '30log v1.0.0'; class._URL =
        'http://github.com/Yonaba/30log'; class._LICENSE =
        'MIT LICENSE <http://www.opensource.org/licenses/mit-license.php>'
        return setmetatable(class, { __call = function(_, ...) return _class(...) end })
    end

    local class = require_30log()

    local mount_path = "assets"
    local path = "romfs:/"
    if love._console == "Wii U" then
        path = "/vol/content/"
    end

    if not love.filesystem.mountFullPath(path, mount_path, "read", true) then
        print("Failed to mount nogame content!")
    end

    local CONFIG =
    {
        ["3DS"] =
        {
            dimensions = { x = 400, y = 240 },
            steps  = 100,
            wave_screens =
            {
                ["left"]  = true,
                ["right"] = true
            },
            cartridge_screens =
            {
                ["left"]  = true,
                ["right"] = true
            },
            nogame_screens =
            {
                ["bottom"] = true
            }
        },
        ["Wii U"] =
        {
            dimensions = {},
            steps = 100,
            cartridge_screens = { ["tv"] = true },
            nogame_screens = { ["gamepad"] = true }
        },
        ["Switch"] =
        {
            dimensions = {},
            steps = 100,
            cartridge_screens = { ["default"] = true },
            nogame_screens = { ["default"] = true }
        }
    }

    local Wave = class("Wave")
    local WAVE_COLORS = {
        { 0.81, 0.31, 0.56, 1 },
        { 0.28, 0.65, 0.93, 1 }
    }
    local WAVE_PHASES  = { 0.0, 2.0 }
    local WAVE_SPEEDS  = { 3.0, 3.5 }
    local WAVE_HEIGHTS = { 0.35, 0.25 }

    local waves = {}

    -- #region Wave
    function Wave:init(id)
        self.id = id
        self.color = WAVE_COLORS[id]

        self.precomputed_frames = {}
        self.total_frames = 0
        self.time = 0
    end

    function Wave:precomputeFrames(steps, width, height, duration, fps)
        local frames = {}
        local total_frames = math.floor(duration * fps)
        local wave_height = height - (height * WAVE_HEIGHTS[self.id])
        local wave_speed = WAVE_SPEEDS[self.id]
        local wave_phase = WAVE_PHASES[self.id]

        -- Use a consistent sine cycle that loops seamlessly
        for frame = 1, total_frames do
            local time = (frame - 1) / total_frames * (2 * math.pi) -- Normalized to one full cycle
            local points = {}

            for x = 0, width, steps do
                local sine = math.sin(x * wave_speed / width + time + wave_phase)
                local y = sine * 10 + wave_height
                points[#points + 1] = x
                points[#points + 1] = y
            end

            points[#points + 1] = width
            points[#points + 1] = height
            points[#points + 1] = 0
            points[#points + 1] = height

            frames[frame] = points
        end

        self.precomputed_frames = frames
        self.total_frames = total_frames
    end


    function Wave:update(dt)
        self.time = (self.time or 0) + dt
    end

    function Wave:draw(screen)
        if not CONFIG[love._console].wave_screens[screen] then
            return
        end

        local frame = math.floor((self.time * 60) % self.total_frames) + 1 -- 60 FPS
        local points = self.precomputed_frames[frame]

        if points then
            love.graphics.setColor(self.color);
            love.graphics.polygon("fill", points)
        end
    end
    -- #endregion

    -- #region Cartridge
    local Cartridge = class("Cartridge")
    local cartridge = nil
    local tex_cartridge = nil

    function Cartridge:init()
        self.width, self.height = 150, 150
        if love._console ~= "3DS" then
            self.width, self.height = 600, 600
        end

        self.quads = {}
        for index = 1, 2 do
            self.quads[index] = love.graphics.newQuad((index - 1) * self.width, 0, self.width, self.height, tex_cartridge)
        end

        self.angle = 0
        self.max_angle = math.pi / 8
        self.rotation_speed = 0.75
        self.rotation_direction = 1
    end

    function Cartridge:update(dt)
        -- Update the rotation angle
        self.angle = self.angle + self.rotation_direction * self.rotation_speed * dt

        -- Check for overshooting and reverse direction
        if math.abs(self.angle) > self.max_angle then
            self.angle = self.max_angle * self.rotation_direction -- Clamp to max_angle
            self.rotation_direction = -self.rotation_direction    -- Reverse direction
        end
    end


    function Cartridge:draw(screen, iod)
        if not CONFIG[love._console].cartridge_screens[screen] then
            return
        end

        love.graphics.setColor(1, 1, 1, 1)

        local x = (CONFIG[love._console].dimensions.x - self.width) * 0.5
        local y = (CONFIG[love._console].dimensions.y - self.height) * 0.5

        for index = 1, #self.quads do
            love.graphics.draw(tex_cartridge, self.quads[index], (x + self.width / 2) + iod * 3, y + self.height / 2, self.angle, 1, 1, self.width / 2, self.height / 2)
        end
    end
    -- #endregion

    -- #region NoGame
    local NoGame = class("NoGame")
    local nogame = nil
    local tex_nogame = nil

    function NoGame:draw(screen)
        if not CONFIG[love._console].nogame_screens[screen] then
            return
        end

        local width = love._console == "Switch" and CONFIG[love._console].dimensions.x or love.graphics.getWidth()

        if love._console ~= "Switch" then
            love.graphics.setColor(WAVE_COLORS[2])
            love.graphics.rectangle("fill", 0, 0, love.graphics.getDimensions())
        end

        local x = (width - tex_nogame:getWidth()) * 0.5
        local y = (CONFIG[love._console].dimensions.y - tex_nogame:getHeight()) * 0.5

        love.graphics.setColor(1, 1, 1, 1)
        love.graphics.draw(tex_nogame, x, y)
    end
    -- #endregion

    function love.load()
        love.graphics.setBackgroundColor(0.93, 0.93, 0.93)

        for index = 1, 2 do
            waves[index] = Wave(index)


            waves[index]:precomputeFrames(10, 400, 240, 5, 60)
        end

        tex_cartridge = love.graphics.newImage(mount_path .. "/nogame/cartridge.png")
        tex_nogame    = love.graphics.newImage(mount_path .. "/nogame/nogame.png")

        cartridge = Cartridge()
        nogame = NoGame()
    end

    function love.update(dt)
        for _, wave in ipairs(waves) do
            wave:update(dt)
        end

        cartridge:update(dt)
    end

    function love.draw(screen, iod)
        for _, wave in ipairs(waves) do
            wave:draw(screen)
        end

        cartridge:draw(screen, iod)
        nogame:draw(screen)

        love.graphics.setColor(0, 0, 0, 0.5)
        love.graphics.print(("FPS: %d"):format(love.timer.getFPS()))
    end

    function love.gamepadpressed()
        love.event.quit()
    end

    function love.conf(t)
        t.console = true
    end
end

return love.nogame
-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
