local Video_mt = ...
local Video = Video_mt.__index

function Video:setSource(source)
    self:_setSource(source)
    self:getStream():setSync(source)
end

function Video:play()
    return self:getStream():play()
end

function Video:pause()
    return self:getStream():pause()
end

function Video:seek(offset)
    return self:getStream():seek(offset)
end

function Video:rewind()
    return self:getStream():rewind()
end

function Video:tell()
    return self:getStream():tell()
end

function Video:isPlaying()
    return self:getStream():isPlaying()
end
