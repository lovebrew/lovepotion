function love.graphics.newVideo(file, settings)
    settings = settings == nil and {} or settings

    if type(settings) ~= "table" then
        error("bad argument #2 to newVideo (expected table)", 2)
    end

    local video = love.graphics._newVideo(file, settings.dpiscale)
    local source, success

    if settings.audio ~= false and love.audio then
        success, source = pcall(love.audio.newSource, video:getStream():getFilename(), "stream")
    end

    if success then
        video:setSource(source)
    elseif settings.audio == true then
        if love.audio then
            error("Video had no audio track", 2)
        else
            error("love.audio was not loaded", 2)
        end
    else
        video:getStream():setSync()
    end

    return video
end
