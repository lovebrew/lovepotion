R"luastring"--(
-- DO NOT REMOVE THE ABOVE LINE. It is used to load this file as a C++ string.
-- There is a matching delimiter at the bottom of the file.
local love = require("love")

function love.nogame()
    local function require_30log()
        local next, assert, pairs, type, tostring, setmetatable, baseMt, _instances, _classes, _class = next, assert, pairs, type, tostring, setmetatable, {}, setmetatable({},{__mode = 'k'}), setmetatable({},{__mode = 'k'})
        local function assert_call_from_class(class, method) assert(_classes[class], ('Wrong method call. Expected class:%s.'):format(method)) end; local function assert_call_from_instance(instance, method) assert(_instances[instance], ('Wrong method call. Expected instance:%s.'):format(method)) end
        local function bind(f, v) return function(...) return f(v, ...) end end
        local default_filter = function() return true end
        local function deep_copy(t, dest, aType) t = t or {}; local r = dest or {}; for k,v in pairs(t) do if aType ~= nil and type(v) == aType then r[k] = (type(v) == 'table') and ((_classes[v] or _instances[v]) and v or deep_copy(v)) or v elseif aType == nil then r[k] = (type(v) == 'table') and k ~= '__index' and ((_classes[v] or _instances[v]) and v or deep_copy(v)) or v end; end return r end
        local function instantiate(call_init,self,...) assert_call_from_class(self, 'new(...) or class(...)'); local instance = {class = self}; _instances[instance] = tostring(instance); deep_copy(self, instance, 'table')
            instance.__index, instance.__subclasses, instance.__instances, instance.mixins = nil, nil, nil, nil; setmetatable(instance,self); if call_init and self.init then if type(self.init) == 'table' then deep_copy(self.init, instance) else self.init(instance, ...) end end; return instance
        end
        local function extend(self, name, extra_params)
            assert_call_from_class(self, 'extend(...)'); local heir = {}; _classes[heir] = tostring(heir); self.__subclasses[heir] = true; deep_copy(extra_params, deep_copy(self, heir))
            heir.name, heir.__index, heir.super, heir.mixins = extra_params and extra_params.name or name, heir, self, {}; return setmetatable(heir,self)
        end
        baseMt = { __call = function (self,...) return self:new(...) end, __tostring = function(self,...)
            if _instances[self] then return ("instance of '%s' (%s)"):format(rawget(self.class,'name') or '?', _instances[self]) end; return _classes[self] and ("class '%s' (%s)"):format(rawget(self,'name') or '?', _classes[self]) or self end
        }; _classes[baseMt] = tostring(baseMt); setmetatable(baseMt, {__tostring = baseMt.__tostring})
        local class = {isClass = function(t) return not not _classes[t] end, isInstance = function(t) return not not _instances[t] end}
        _class = function(name, attr) local c = deep_copy(attr); _classes[c] = tostring(c)
        c.name, c.__tostring, c.__call, c.new, c.create, c.extend, c.__index, c.mixins, c.__instances, c.__subclasses = name or c.name, baseMt.__tostring, baseMt.__call, bind(instantiate, true), bind(instantiate, false), extend, c, setmetatable({}, {__mode = 'k'}), setmetatable({}, {__mode = 'k'}), setmetatable({}, {__mode = 'k'})
        c.subclasses = function(self, filter, ...) assert_call_from_class(self, 'subclasses(class)'); filter = filter or default_filter; local subclasses = {}; for class in pairs(_classes) do if class ~= baseMt and class:subclassOf(self) and filter(class,...) then subclasses[#subclasses + 1] = class end end; return subclasses end
        c.instances = function(self, filter, ...) assert_call_from_class(self, 'instances(class)'); filter = filter or default_filter; local instances = {}; for instance in pairs(_instances) do if instance:instanceOf(self) and filter(instance, ...) then instances[#instances + 1] = instance end end; return instances end
        c.subclassOf = function(self, superclass) assert_call_from_class(self, 'subclassOf(superclass)'); assert(class.isClass(superclass), 'Wrong argument given to method "subclassOf()". Expected a class.'); local super = self.super; while super do if super == superclass then return true end; super = super.super end; return false end
        c.classOf = function(self, subclass) assert_call_from_class(self, 'classOf(subclass)'); assert(class.isClass(subclass), 'Wrong argument given to method "classOf()". Expected a class.'); return subclass:subclassOf(self) end
        c.instanceOf = function(self, fromclass) assert_call_from_instance(self, 'instanceOf(class)'); assert(class.isClass(fromclass), 'Wrong argument given to method "instanceOf()". Expected a class.'); return ((self.class == fromclass) or (self.class:subclassOf(fromclass))) end
        c.cast = function(self, toclass) assert_call_from_instance(self, 'instanceOf(class)'); assert(class.isClass(toclass), 'Wrong argument given to method "cast()". Expected a class.'); setmetatable(self, toclass); self.class = toclass; return self end
        c.with = function(self,...) assert_call_from_class(self, 'with(mixin)'); for _, mixin in ipairs({...}) do assert(self.mixins[mixin] ~= true, ('Attempted to include a mixin which was already included in %s'):format(tostring(self))); self.mixins[mixin] = true; deep_copy(mixin, self, 'function') end return self end
        c.includes = function(self, mixin) assert_call_from_class(self,'includes(mixin)'); return not not (self.mixins[mixin] or (self.super and self.super:includes(mixin))) end
        c.without = function(self, ...) assert_call_from_class(self, 'without(mixin)'); for _, mixin in ipairs({...}) do
		assert(self.mixins[mixin] == true, ('Attempted to remove a mixin which is not included in %s'):format(tostring(self))); local classes = self:subclasses(); classes[#classes + 1] = self
		for _, class in ipairs(classes) do for method_name, method in pairs(mixin) do if type(method) == 'function' then class[method_name] = nil end end end; self.mixins[mixin] = nil end; return self end; return setmetatable(c, baseMt) end
        class._DESCRIPTION = '30 lines library for object orientation in Lua'; class._VERSION = '30log v1.2.0'; class._URL = 'http://github.com/Yonaba/30log'; class._LICENSE = 'MIT LICENSE <http://www.opensource.org/licenses/mit-license.php>'
        return setmetatable(class,{__call = function(_,...) return _class(...) end })
    end

    local class = require_30log()

    local DEBUG        = false
    local g_Time       = 0
    local g_frameCount = 0

    local DEBUG_INFO = [[
FPS: %d
Time: %.2f
Frame: %d
    ]]

    local drive = "romfs:/"
    if love._os == "cafe" then
        drive = "/vol/content/"
    end

    local success = love.filesystem.mountFullPath(drive, "assets", "read", true)
    assert(success, "Failed to mount nogame content!")

    local CONFIG = require("assets.nogame.config")

    local HeartTrack = class("HeartTrack")

    local function allowed_screen(t, screen)
        for _, value in pairs(t) do
            if screen == value then
                return true
            end
        end
        return false
    end

    function HeartTrack:init(x, y, offset, speed)
        self.x = x
        self.y = y
        self.offset = offset
        self.spacing = 50
        self.imageWidth = heart_image:getWidth()
        self.width = self.spacing + self.imageWidth
        self.speed = speed
        self.count = love.graphics.getWidth() / self.width + 2
        self.image = heart_image
    end

    function HeartTrack:draw(screen, depth)
        local speed, alpha = self.speed, 0.35
        if screen == CONFIG.hearts.reflect then
            speed, alpha = self.speed * 0.4, 0.20
        end

        local absolute_offset = (self.offset + (speed * g_Time))
        local offset = absolute_offset % self.width

        love.graphics.setColor(1, 1, 1, alpha)
        for index = 1, self.count do
            local x = self.x + (index - 1) * (self.imageWidth + self.spacing) + offset - self.width
            love.graphics.draw(self.image, x + depth * 1.5, self.y, -0.05)
        end
    end

    local Hearts = class("Hearts")

    function Hearts:init()
        local layer_height = 50

        self.tracks = {}
        local max = (love.graphics.getHeight() / layer_height) + 1
        for index = 1, max do
            local width = heart_image:getWidth()
            self.tracks[#self.tracks + 1] = HeartTrack(0, 20 + (index - 1) * layer_height, width / 2 * index, 40, heart_image)
        end
    end

    function Hearts:draw(screen, depth)
        for _, track in ipairs(self.tracks) do
            track:draw(screen, depth)
        end
    end

    local Cartridge = class("Cartridge")

    function Cartridge:init()
        self.width  = cartridge_image:getWidth()
        self.height = cartridge_image:getHeight()
        self.speed = 0.25
    end

    function Cartridge:draw(screen, depth)
        if not allowed_screen(CONFIG.cartridge.screens, screen) then
            return
        end
        local dx, dy = love.graphics.getDimensions()
        local x = (dx - self.width)  / 2
        local y = (dy - self.height) / 2
        love.graphics.setColor(1, 1, 1, 1)
        love.graphics.draw(cartridge_image, x + depth * 3, y + math.sin(g_Time  / self.speed) * 3)
    end

    local world = {}

    local function create_world()
        world.Hearts    = Hearts()
        world.Cartridge = Cartridge()
    end

    function love.load()
        love.graphics.setBackgroundColor(0.1686, 0.6471, 0.8745)
        heart_image     = love.graphics.newImage("assets/nogame/heart.png")
        cartridge_image = love.graphics.newImage("assets/nogame/cartridge.png")

        create_world()
    end

    function love.update(dt)
        g_Time = g_Time + dt
    end

    function love.draw(screen, depth)
        world.Hearts:draw(screen, depth)
        world.Cartridge:draw(screen, depth)

        if DEBUG then
            love.graphics.setColor(0, 0, 0, 0.5)
            love.graphics.print(DEBUG_INFO:format(love.timer.getFPS(), g_Time, g_frameCount), 0, 0)
        end
        g_frameCount = g_frameCount + 1
    end

    function love.keypressed(key)
        if key == "escape" then
            love.event.quit()
        elseif key == "space" then
            print("ligma")
            DEBUG = not DEBUG
        end
    end

    function love.gamepadpressed(_, button)
        if button == "start" then
            love.event.quit()
        elseif button == "back" then
            DEBUG = not DEBUG
        end
    end

    function love.touchpressed(_, x, y)
    end

    function love.touchreleased(_, x, y)
    end
end

return love.nogame
-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
