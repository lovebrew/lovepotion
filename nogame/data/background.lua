local BACKGROUND = {}

function BACKGROUND:new(width, height)
    return {{0, height, 0, 0, width, 0}, {0, height, width, height, width, 0}}
end

return BACKGROUND
