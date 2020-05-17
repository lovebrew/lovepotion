return
{
    colors =
    {
        heart = "FFFFFF",
        text  = "f06292",
        background = "64b5f6",
        bottleFill = "FFFFFF",
        clouds = "EEEEEE"
    },

    ["3DS"] =
    {
        cloudSize = 64,
        noGame =
        {
            x = 160,
            y = 145,
            scale = 0.5,
            shouldRender =
            function (s)
                return s == "bottom"
            end
        },
        bottle =
        {
            x = 400,
            y = 140,
            scale = 0.5,
            shouldRender =
            function (s)
                return s == "top"
            end
        },
        box = function (screen)
            if screen == "bottom" then
                love.graphics.setColor(love.math.colorFromBytes(238, 238, 238))
                love.graphics.rectangle("fill", 0, 0, 320, 240)
            end
        end
    },

    ["Switch"] =
    {
        cloudSize = 128,
        noGame =
        {
            x = 640,
            y = 620,
            scale = 1.0,
            shouldRender = function ()
                return true
            end
        },
        bottle =
        {
            x = 1280,
            y = 325,
            scale = 1.0,
            shouldRender = function ()
                return true
            end
        }
    }
}
