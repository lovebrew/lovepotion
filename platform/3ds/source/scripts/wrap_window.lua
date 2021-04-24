require("love.graphics")

-- set up variables
local textFont = nil
local glyphFont = nil

local textColors = {}

textColors.default = {0.08, 0.08, 0.08}
textColors.pressed = {1.00, 1.00, 1.00}

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
    button.y = boxPosition.y + (boxSize.h - 40)

    button.width = width
    button.height = 40

    button.id = id or 1

    button.textColor = textColors.default

    function button:drawShadowText(x, y)
        love.graphics.setColor(textColors.default)
        love.graphics.print(self.text, textFont, x, y)
    end

    function button:draw()
        local x, y = self.x + (self.width - textFont:getWidth(self.text)) / 2, self.y + (self.height - textFont:getHeight()) / 2

        if self.textColor == textColors.pressed then
            self:drawShadowText(x, y + 1)
        end

        love.graphics.setColor(self.textColor)

        if self.shortcut then
            love.graphics.print(utf8.char("0xE000"), glyphFont, x - 32, y)
        end
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

    function button:touchpressed(_, x, y)
        if (x > self.x and x + 1 < self.x + self.width and y > self.y and y + 1 < self.y + self.height) then
            self.textColor = textColors.pressed
            return true
        end
    end

    function button:touchreleased()
        self.textColor = textColors.default
    end

    function button:mousepressed(x, y)
        return self:touchpressed(nil, x, y)
    end

    return button
end

local function decodeBuffer(buffer)
    return love.data.decode("data", "base64", buffer)
end

local function newMessageBox(text, buttons)
    local messagebox = {}

    local currentFont = love.graphics.getFont()

    messagebox.buttons = {}

    if buttons then
        assert(#buttons < 3, "cannot have more than two buttons")
    end

    -- textures
    local textures = {}

    textures.single = {}

    -- single up
    textures.single[0] = love.graphics.newImage(decodeBuffer("\
    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\
    tDi7vAIwPx//8AAVifAAAAAA"))

    -- single down
    textures.single[1] = love.graphics.newImage(decodeBuffer("\
    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"))

    textures.double = {}

    -- double up
    textures.double[0] = love.graphics.newImage(decodeBuffer("\
    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\
    vAIwPx//8AAVifAAAAAA"))

    -- double left
    textures.double[1] = love.graphics.newImage(decodeBuffer("\
    AQAuAAAsAdgAAAAABFgCoAARAAAIQAAGIAD/kqOm/8UC09b/7/z+AkA/xADS1f+QoqT/7oBQP//X\
    5ef/2eZj6TBPQAPw/f+AA/APxOALwJeisrSAo+PxRPNAT+bz9QDAQ5qqBK3/x9XYgHPJ1wbZ/+36\
    /ACwcwMAG++NYNfv+/0AUOtgG+4gH7ExE+4wCyCT7vn8QAcT7fn7UAP4+gDxHwBBS8EAcH+xd+77\
    /v/sIGefcZPu+lCDcJdQh/APIJdk+qCjIJ/s+GAH9/kM/+v2+VAHQAv4/xXq9vdAy+wwA/dQI7xQ\
    M/pQKyAjMCdwK+n1ZfZAM0AL9PYgA/WCK0bpIF/p9fiCOzH35Tvy9EFDMc9wZ+5gn3CXQO5hX+Pw\
    8v/i71DyQfvrIV/g7vD/H+Dt70DjQANgl3F7UK/+cANQz0CrYOeApzCzILfosSCr6CKLcLfo8/RA\
    A49w4+n190ADUEMhGyAP9zA3QCswOyDL5yDPMEMwRx/n8vNAAzCrQK8gqzCv9/AP8K/wvwMAP/dw\
    r1Dn8A/x8K8AAL8C8D8RsyD/7fr8X/8A7vD/7vv9/+4A/P7/4u/y/+Mp8PIAL//8QAPs+C9LVfgD\
    b//2f//3QAvsIA9w6SALMA8AD//09f/pRPQ///T/6CAD8P1W/0AD5SAP5i9rcA/pQ/UgQ/f5/+4g\
    2zADmzCr7PhQ6zDj7G/XcD+OMA/u+/6AT3BXMQ/v7iETMPcwS+8hGzADUFf9oCC7+S//+f/q9ve1\
    QAPsIHNPV/ZQ1+lAr7c/x/RAAz/n6X/XIMtxR/8w8zBLcJdwR3BXcEeAVz9v7yBz8K/wv+9wBzDH\
    IdeAF8RgCzHrytjagUfI1wfZ/52trwEAV4EfMVvtYV8/a5Ev+0F7MRvsYR/3Medxh0DrIZ/wcIdh\
    Z/Cf93BHwF8xf1GPAGAAQgch/42AD6W1uIAX8dPnIY+hYjcAAOAA/5Slp4BfHMjW2QAAHwJxfwBx\
    t9roReqAz93q7ADx3wAI4AAI/8fV2IGfk6SmQAARlKAA/5ysrv/kAPHz/+/8/v/uAvv+//D9/0AX\
    5iPz9UAf7vv9QBsgF9gCwAMwX+8wByBPorK0Iv/uIG/E0tUAQFvgI+3vQG/r+PoBUFcwuyP6/ECr\
    7vn8UG8gt1HwQNP9QBvt+ftQA/ZgU3A3MI8wk+0wP2An7+JwM+A3MDvr9/lAS+zkIJtAAyAP7Pcg\
    F/b5rCAD+CAH90AjUAv4/2DqIA8wA+n19v/qjiAD6PT1AAA/8C8wU+q0QEv5gD8wO+lgP+r14FBP\
    cA8wU+fz9P/ojyAD5/LzUBMwCzAPIduIMAPm8PJAA+Xv8bqQl/VwV2BfQDf1gAfn5XBPYFdAR/Hy\
    ICf2QCdq6DBzQB/1IAf0gB/mnHAH8PFAb0B3IAvk7wfw/+Xu8ECjUCdQI/dwHzCbMB8wo+QgJ0AD\
    IguJUAfu/+MwA+ztgEfKMBswE+TuMAfvQBvjKuvtICPuQAfiMAPqUOwgB+xAB+Hq6/8e4enrISNx\
    F1CjUNfz3VCLMIPvUKswVzB/5WCLa+NgbzA352FL5SDTkVv/MLtgNzCTMIcgi1CnkA9Qx3vuYJ8w\
    ryCrINfhQKOQpwHh6er/4OjqwAsZ4OjpUNcw3+nsQM+2MOfhIONQM+tgJyA753fpUANgNzAP3yAT\
    8K/wv3/jAACv0L8C8D9Aq6Cv8A9Aq37oUKswr/APAvA/EE8Q/ziT8f+m/1ijR/MNhv9YuzmTcQ8B\
    8P/VcS8Ld//kAyj/5A2o/+QK6f/9AnK/8e8B8j8QF3L/8r8C/f/kED4t/90QK/f/Ap//8QpM/wO0\
    /wnf//EPD//q/v9/Dw0a//EDQP/kEAqL//GlA83/5y//5/F///FP/0bkr//k7u9AAwO//+I/6+2P\
    /zALUA8gFyADX/9c6l/751ALQANgD+bxRvMgr/P/5iCrNrfnljAP8vRgC/Igry/X7t0/x1/75GDX\
    MNMwy+ggKxbo8/UgR/JARzAL6TH09iAXMAv0/+VxX3Tuax8hGzBz5WBv4uxm7S9bIL/q7EADX2Pu\
    Ff/i6nDH6q/H6l/nLerrYAvqUV8xT+tfn8pwZ3F34uoxH+yAN+PgISs/y1AX7f/q9fYI/+n192C3\
    9f/qAvb3/+v2+EAX6rUgF+dgx9Df9UAX5yDfAev2+f/s9/ogM4BQN+z4+v/t+furIBP5QAvpMEf2\
    IF8wW98gazBb6yAbcGexH0ETMUf/ITtElzF/IXcxgzEjMYNBj72Bf/BAgzBbcKdAb/VQb9Ywez/f\
    54Fv8kGTQC/xgFC/7fr8/+/7/aBAs+4wC/z+//D9bv9AEzAL7WDPMLMwC+7/oC8wOzArALADwE8A\
    ICvxD/E/euqhP1FLIWOBP/mAs+4P+fz/7iDH8Hdw13DHdu9gI0ArYOvnIq9gxwCGYAD/na2vQAfw\
    3+Y28/VAHyBrAAMgAAABP+6IIX+js7VBh8XT1bAAQV/gI5Mxo+z5+wCEEZLgAP/w/f8AgAPv/FL+\
    AIAX7zAf+/1AA+4I+vz/7iAL7fn7EP/s+CAD9/n/7QD4+v/r9vj/7aMgG+wgF+75/EATMBsQ6/X4\
    IBv5/+r2RfcAQHPu+v1gB/wAEHeyMG/5kG9Ac/f6QHfrxDBbIHPq9fdAe+n1XPZgh/lAI0AbIHPp\
    9Hf1QCMwB1An91AXIC8wGwLn8/T/6PMgB/Jd80BD6CAvQAcgE+ggGxHn8fNAG+bx8kALEObw8kAL\
    5u/x/0TlIAPk7vBQA+3viEAb5e/wQBfl7u+CIBvv/+Pt7iAb7gr/4+ztQIvooG/mtEBP8UBbMEvn\
    YG/o8nP0UGMgazBf5fAwT5BnQOVgV+Ps7v/i6xHt/+MgA+Lq7ECD4DBnQBswE+vs/+HqR+tAG+Hp\
    62AfUDdAJ2/sUCNQJ+wAAA8wOzA3MDsU4OjqQCPisA/p6oBAF+Do6f/g5+mSwAvf52AH5uhAA949\
    5+dQUyCHMENQN+pgN/NQL1BPYEcAID/f51BLUFNQ50AD3kAD5v/d5V3mYA/mQBMwD1AD5VADD+Tl\
    /9+gmzCXUKcwm/9QnzAHsFOwW1BfkHtAd3A733CHoHPcIG+AgzALsA9gj6VAI+RgC+PkYBPkQAug\
    cAPbQAPj/9zi4/JgR9AfkCNQJ9vikAPcgGAD2+Li/9vh4v8AEK/gvwCAr2C/AvA/8K8AAA9wr3jh\
    UK8AAA8C4D8Rs/D/3+bo60ADUAsAX//eL//cT0sEH/8a3eTkz1fP/+IwA+O0L2vjUAcvW98v/+Dn\
    cekv91/7MAvf6OlQx54wF+bmz8cw01/vUBfgAujq/+Hp6kBD4IAgN+Lp6//h6uy/YBfrQQ9wX0AL\
    IFswMzBb/jB3TwNvBwAPXyEfj9Ngt9z/4N+xZzCrULcxg1GLMWOwT8BQI3Av5//i6+z/GOPs7UCz\
    UAvu/+Q17e9AEzAL4UDP64DPgPAv5O7w/+Xv8cBAMzAL5vDy/+fxEPP/5iAT5vHy/xrj7e4gI1BP\
    5CAL5UfvMCPx/+UgEzA3UTfWYQ8hd+GhP+IAYT+Rb+rPIL9Qp+LqIMcgC3CHMHf/MGMwuzDjQLsw\
    I0CHIN8wN4Agm+fz9P/p9PUA/+fy8//o8/UA/+n19//r9viCIAf2/+r290DL6PwgIzDLQCMgKzAb\
    UCv2/wHs9/n/7Pj7QDMA7fj6/+76/P8A7/v9/+35+/9G7iAH6/X4QCMwS+ts9iArIBftICNAK/n8\
    q0EL5iEv5WEv6OCPcVN85TFnITcxWzCzQCvy9DH/6qCvMLfs9/pAe33rYJcw2zDHMONQ2/lAp4hw\
    K/D9/0AD7/z+9YALsAdwFzDT7wAQHwAC4AC/IS/9QHtBNwAggzEjsU/gr0AAEZLgAP/w/f//7yj8\
    /kAH8CAH7vr9AP/t+fv/7fr8Cf/s+PtQH/v9gAdU7kAX+kAH7SAH7PcB+f/r9vj/6yAHAOr29//p\
    9fb/Aujz9f/q9DAH9PggG1AXMCNAH2AX6fT1Av/o8vT/7jBT+/kgc3AHMHsgXzCD7PcgX9cgizAj\
    60Bz/YAXcB8wezrp9VBfMHtQV/NAg+f8IAdAdyCTcAdwF0Af8fOIQCPm8fJAC+bw8QD/5e/x/+Tt\
    7wr/5O/wQAfmICPl4CAXMCMwG+Xu8P/jyDAfMAfs7kAD4uvtgCAL7f/i6uz/4QLq6//h6eogA+sC\
    /+Do6v/jIBvh+CAXMCNQG1AXMCPg6OmVgFfm8DBf8CBX70BX/nAHMBcwizCbcJNAdzAH6/gweyBb\
    IGtAe2Bz4Ofp0IAHcBfhAAAf6f/f59AgA2AH4DAL5uj/3krnIAPm5yAL5yAH5spASzAT3+cwG+dA\
    G9065eZAGzAHUAPlUAvkQOVAA9zj5P/c5FjkYAfl0BcgG9vj488gAyAj4uNAczBrQHswc/ygTzA7\
    cI9wl4Bn4G/d5LygW+KQA1BvcHdQc9viTuJQB+HiUI+wL4A74vVQH3ALYC8wM9qAA+EAIBOJkBva\
    4OFAI9ngUAN/2qAHcANwT5BX0C+ASzB30wAgP9AL4IAD2d9QA3AL/7ATsAeAy8BvoA8A8AcC8D8A\
    IK/wAHCrAEC/AvA/EbOw/+Hh/9vrL/8wA79X2a//2WAnkBfYA///v1fhAI8zf2vb4uMy/9wgA0C3\
    4uJQD+Nv5EAXMBvagM9f6zAjkC+JUCvd5OVAM9zkIAss5eUgA+ZAEzAH2+P+UE9AY3ArYC8wNwAB\
    D6E/2/4AIT9wh3+rQKcgY3CvsI/d+CCPsK9wg0DfYAfe5ucI/97n6CAH5v/fgCAL3+fp/9/o6Y8g\
    C+j/4CALMMswI1AHZOZAKzAX3ucwK+j/AOHp6v/h6uv/EODo6iAL6//i6wDt/+Ps7v/i6lfsIAft\
    gBfgIFNQKyAX9SAjMDMwKzEv3iEv3GEv3kCDIHveYAdxR3FPkK/n64CLMHtwB+JAg+xQiyCD/HAf\
    MNdQg1AXMKswi+TtBO//5e/xQAfk7wDw/+bw8v/n8kDzIAfx/+fx8/9K4zAf7vBA0+QgB+WwIBfm\
    IBcwM+bx8v8B6PP1/+n19iAHAPT/6vT2/+v2APj/7Pf5/+r2Gff/6yAHMBfp9CAfV/IwB/VQF/Zg\
    HyArUFds74AHcFflYItAd+3vuiEz7kCLcKNQq/EgV/P/QFcws1CLUFcwkzCLcBcwy7Awq+pgF3AH\
    7fn7/wDu+v3/7Pj7/wDt+vz/8Pz+/wTw/f//72AH7PhE+iAf/P/tYAfv+279UB9QBwAC4ABAVzCD\
    7Dv3+lCLIF9wA+0ga3AXobAf7GCz6/j6ABGS4AAA/+v4+v/u+v2AUAf7/f/t+fv/HOz3+oAHcBcA\
    gB/2+Aj/6vX2gAfo8/UP/+fx84AHAQAfAPBfBOB/Aebw8f/k7/CABwHj7e//4+vugAeAAPAf4ers\
    /+Hp64CAB+Do6f/f5+nggAcA8B8G8H/e5+j/3iDm54AH3eXm/90w5OWABwDwH9zj5P9g2yADgAfi\
    4//b4XTikAcA4B8G8H/aMAPg4eCQB/ADAPAf4OD/2d984IAH8AMBAB8G4H8RxDAT2uDi0AcB8B8F\
    sH/a4OEAEAPhaOLQBwCgH9sgK9zi44eQB+Pk/9wgA4AHAOAfgAbwf93k5f/d5eaAgAfe5uf/3ufo\
    wIAHAPAf3+fp/+DoQOmAB+Hp6//h6nDsgAcA8B8G8H/j6+7/EOPt74AH5O/w/xjm8PGABwDwH+fx\
    8wj/6PP1gAfq9fYO/+v2+IAHAPAfBvB/7AL3+v/t+fuAB+4D+v3/6/j6gAcA4B9wAALgAAbwfxGO\
    8D//6/j6CP/u+/2AB+35+wz/7Pf6gAcBAB/2+Aj/6vX2gAfo8/UO/+fx84AHAQAfBuB/5gLw8f/k\
    7/CAB+MD7e//4+vugAcA8B8B4ers/+Hp64AHAeDo6f/f5+mAB8AA8B8G8H/e5+j/3uZA54AH3eXm\
    /93kYOWABwDwH9zj5P/bwCADgAfi4//b4eLpkAcA4B8G8H/aMAPg4ZAHwPADAPAf4OD/2d/g/oAH\
    8AMBAB8CYH8BgF8AYJcRPzAT1iDc3AmP/9LY2f/VJ9vcEGlP/9rg0AcB8B8FsH8T2uDhABAD4eLQ\
    BwCgH0TbICvc4uOQB+PkP//cIAOABwDgHwGwf1FbUJ/sT/8guwFwf9pgK/Cf3eQE5f/d5eaAB97m\
    Buf/3ufogAcA8B/fAufp/+Do6YAH4QPp6//h6uyABwDwH4AG8H/j6+7/4+3vgIAH5O/w/+bw8cCA\
    BwDwH+fx8//o80D1gAfq9fb/6/Zw+IAHAPAfBvB/7Pf6/xDt+fuAB+76/f8b6/j6gAcA4B8AAuAA\
    BvB/gBGO8D//6/j6/+77QP2AB+35+//s92D6gAcBAB/2+P/q9UD2gAfo8/X/5/Fw84AHAQAfBuB/\
    5vDx/xDk7/CAB+Pt7/8Y4+vugAcA8B/h6uwI/+Hp64AH4OjpDv/f5+mABwDwHwbwf94C5+j/3ubn\
    gAfdA+Xm/93k5YAHAPAfB9zj5P/bIANwB/AD4ADwHwbwf/Cz2uHi/9k+3+CABwDwHwLwAwbwfxHT\
    MEPaIODhgAfb4+T/3PAgA3AHATAfBrB/3eTl/xDd5eaAB97m5/8Y3ufogAcA8B/f5+kI/+Do6YAH\
    4enrDv/h6uyABwDwHwbwf+MC6+7/4+3vgAfkA+/w/+bw8YAHAPAfAefx8//o8/WABwHq9fb/6/b4\
    gAfAAPAfBvB/7Pf6/+35QPuAB+76/f/r+G76gAcA4B8AAuAABvB/EY7wP/8B6/j6/+77/YAHAe35\
    +//s9/qAB4EBAB/2+P/q9faABwHo8/X/5/HzgAfAAQAfBuB/5vDx/+TvQPCAB+Pt7//j62DugAcA\
    8B/h6uz/4SDp64AH4Ojp/9845+mABwDwHwbwf97n6Aj/3ubngAfd5eYM/93k5YAHAPAf3OMf5P/b\
    IANwB/ADAPAfBvB/gPCz2uHi/9nf4PiABwDwHwLwAwbwfxHTMEPa4OGDgAfb4+T/3CADcAfAATAf\
    BrB/3eTl/93lQOaAB97m5//e52DogAcA8B/f5+n/4CDo6YAH4enr/+E46uyABwDwHwbwf+Pr7gj/\
    4+3vgAfk7/AM/+bw8YAHAPAf5/EE8//o8/WAB+r1B/b/6/b4gAcA8B8G8H8B7Pf6/+35+4AHAe76\
    /f/r+PqAB7gA4B8AAuAABvB/EY7wP//r+AT6/+77/YAH7fkG+//s9/qABwEAH/YE+P/q9faAB+jz\
    B/X/5/HzgAcBAB8G4H8B5vDx/+Tv8IAHAePt7//j6+6AB4AA8B/h6uz/4enrgIAH4Ojp/9/n6eCA\
    BwDwHwbwf97n6P/eIObngAfd5eb/3TDk5YAHAPAf3OPk/2jbIANwB9twD+Lj/x/b4eIAAB+AF2Af\
    A/B/8F/RAPB/cLPaIKva4OFABx/Z3+CAA7AXQB/gA7ArnwAwC9rg0AcAAB8A4F8BAB8A8F/PAOB/\
    EcSwE9rgANAXAHAfA3B3AbB/Gtrg4QAf/yAT2gAwA+F04oAHcDswP9sgF9vjHOT/3CADgAcgK9zi\
    fuOQB+AfAPBfAPB/APBfAPB/3QLk5f/d5eaAB94D5uf/3ufogAcA8B8B3+fp/+Do6YAHAeHp6//h\
    6uyAB8AA8B8G8H/j6+7/4+1A74AH5O/w/+bwYPGABwDwH+fx8//oIPP1gAfq9fb/6zj2+IAHAPAf\
    BvB/7Pf6CP/t+fuAB+76/Q3/6/j6gAcA4B8AAuAAwAbwfxGO8D//6/j6/+4g+/2AB+35+//sMPf6\
    gAcBAB/2+P/qIPX2gAfo8/X/5zjx84AHAQAfBuB/5vDxCP/k7/CAB+Pt7wz/4+vugAcA8B/h6gTs\
    /+Hp64AH4OgH6f/f5+mABwDwHwbwfwHe5+j/3ubngAcB3eXm/93k5YAHgwDwH9zj5P/bIAOABwPi\
    4//b4eKQBwDgH6cG8H/aMAPg4ZAH8AMA8B8D4OD/2d/ggAfwA+cBAB8G4H8RxDAT2uDQBwHwHwWw\
    fxPa4OEAEAPh4tAHAKAfRNsgK9zi45AH4+Q8/9wgA4AHAOAfBvB/3eQE5f/d5eaAB97mBuf/3ufo\
    gAcA8B/fAufp/+Do6YAH4QPp6//h6uyABwDwH4AG8H/j6+7/4+3vgIAH5O/w/+bw8cCABwDwH+fx\
    8//o80D1gAfq9fb/6/Zw+IAHAPAfBvB/7Pf6/xDt+fuAB+76/f8b6/j6gAcA4B8AAuAABvB/gBGO\
    8D//6/j6/+77QP2AB+35+//s92D6gAcBAB/2+P/q9UD2gAfo8/X/5/Fw84AHAQAfBuB/5vDx/xDk\
    7/CAB+Pt7/8Y4+vugAcA8B/h6uwI/+Hp64AH4OjpDv/f5+mABwDwHwbwf94C5+j/3ubngAfdA+Xm\
    /93k5YAHAPAfBtzj5P/bIAOAB+IH4//b4eKQBwDgHwbwf07aMAPg4ZAH8AMA8B/gB+D/2d/ggAfw\
    AwEAH84G4H8RxDAT2uDQBwHwHwWwf9om4OEAEAPh4tAHAKAf24ggK9zi45AH4+T/eNwgA4AHAOAf\
    BvB/3eTlCP/d5eaAB97m5wz/3ufogAcA8B/f5wTp/+Do6YAH4ekH6//h6uyABwDwHwbwfwHj6+7/\
    4+3vgAcB5O/w/+bw8YAHgADwH+fx8//o8/WAgAfq9fb/6/b44IAHAPAfBvB/7Pf6/+0g+fuAB+76\
    /f/rN/j6gAcA4B8AAuAABvB/EY7wPwD/6/j6/+77/YCAB+35+//s9/rAgAcBAB/2+P/q9faAgAfo\
    8/X/5/Hz4IAHAQAfBuB/5vDx/+Qg7/CAB+Pt7//jMOvugAcA8B/h6uz/EOHp64AH4Ojp/xzf5+mA\
    BwDwHwbwf97nBOj/3ubngAfd5Qbm/93k5YAHAPAf3Azj5P/bIAOAB+LjDv/b4eKQBwDgHwbwf9qc\
    MAPg4ZAH8AMA8B/g4A//2d/ggAfwAwEAHwbgf5wRxDAT2uDQBwHwHwWwf9rgTeEAEAPh4tAHAKAf\
    2yArENzi45AH4+T/3PAgA4AHAOAfBvB/3eTl/xDd5eaAB97m5/8Y3ufogAcA8B/f5+kI/+Do6YAH\
    4enrDv/h6uyABwDwHwbwf+MC6+7/4+3vgAfkA+/w/+bw8YAHAPAfAefx8//o8/WABwHq9fb/6/b4\
    gAfAAPAfBvB/7Pf6/+35QPuAB+76/f/r+G76gAcA4B8AAuAABvB/EY7wP/8B6/j6/+77/YAHAe35\
    +//s9/qAB4EBAB/2+P/q9faABwHo8/X/5/HzgAfAAQAfBuB/5vDx/+TvQPCAB+Pt7//j62DugAcA\
    8B/h6uz/4SDp64AH4Ojp/9845+mABwDwHwbwf97n6Aj/3ubngAfd5eYM/93k5YAHAPAf3OMY5P/b\
    IAOAB+Lj/x3b4eKQBwDgHwbwf9owAzjg4ZAH8AMA8B/g4P8f2d/ggAfwAwEAHwbgfxHEMBM42uDQ\
    BwHwHwWwf9rg4ZoAEAPh4tAHAKAf2yAr3CHi45AH4+T/3CAD4IAHAOAfBvB/3eTl/90g5eaAB97m\
    5//eMOfogAcA8B/f5+n/EODo6YAH4enr/xzh6uyABwDwHwbwf+PrBO7/4+3vgAfk7wbw/+bw8YAH\
    APAf5wLx8//o8/WAB+oD9fb/6/b4gAcA8B+ABvB/7Pf6/+35+4CAB+76/f/r+PrcgAcA4B8AAuAA\
    BvB/EY7wP//rAvj6/+77/YAH7QP5+//s9/qABwEAHwL2+P/q9faAB+gD8/X/5/HzgAcBAB+ABuB/\
    5vDx/+Tv8ICAB+Pt7//j6+7AgAcA8B/h6uz/4elA64AH4Ojp/9/ncOmABwDwHwbwf97n6P8Q3ubn\
    gAfd5eb/GN3k5YAHAPAf3OPkMP/bIAOAB+Lj/9s64eKQBwDgHwbwf9owA+Bw4ZAH8AMA8B/g4P/Z\
    Pt/ggAfwAwEAHwbgfxHEMBPaceDQBwHwHwWwf9rg4QAQAzTh4tAHAKAf2yAr3OJD45AH4+T/3CAD\
    gAfAAOAfBvB/3eTl/93lQOaAB97m5//e52DogAcA8B/f5+n/4CDo6YAH4enr/+E46uyABwDwHwbw\
    f+Pr7gj/4+3vgAfk7/AM/+bw8YAHAPAf5/EE8//o8/WAB+r1B/b/6/b4gAcA8B8G8H8B7Pf6/+35\
    +4AHAe76/f/r+PqAB7gA4B8AAuAABvB/EY7wP//r+AT6/+77/YAH7fkG+//s9/qABwEAH/YE+P/q\
    9faAB+jzB/X/5/HzgAcBAB8G4H8B5vDx/+Tv8IAHAePt7//j6+6AB4AA8B/h6uz/4enrgIAH4Ojp\
    /9/n6eCABwDwHwbwf97n6P/eIObngAfd5eb/3TDk5YAHAPAf3OPk/2DbIAOAB+Lj/9vhdOKQBwDg\
    Hwbwf9owA+Dh4JAH8AMA8B/g4P/Z33zggAfwAwEAHwbgfxHEMBPa4OLQBwHwHwWwf9rg4QAQA+Fo\
    4tAHAKAf2yAr3OLjh5AH4+T/3CADgAcA4B+ABvB/3eTl/93l5oCAB97m5//e5+jAgAcA8B/f5+n/\
    4OhA6YAH4enr/+HqcOyABwDwHwbwf+Pr7v8Q4+3vgAfk7/D/GObw8YAHAPAf5/HzCP/o8/WAB+r1\
    9g7/6/b4gAcA8B8G8H/sAvf6/+35+4AH7gP6/f/r+PqABwDgH3AAAuAABvB/EY7wP//r+PoI/+77\
    /YAH7fn7DP/s9/qABwEAH/b4CP/q9faAB+jz9Q7/5/HzgAcBAB8G4H/mAvDx/+Tv8IAH4wPt7//j\
    6+6ABwDwHwHh6uz/4enrgAcB4Ojp/9/n6YAHwADwHwbwf97n6P/e5kDngAfd5eb/3eRg5YAHAPAf\
    3OPk/9vAIAOAB+Lj/9vh4umQBwDgHwbwf9owA+DhkAfA8AMA8B/g4P/Z3+D4gAfwAwEAHwbgfxE7\
    cBPW3N2MAE//1NnaAMAfEHQ//9rg4tAHAfAfBbB/2uDhABAD4Wji0AcAoB/bICvc4uOHkAfj5P/c\
    IAOABwDgH4AG8H/d5OX/3eXmgIAH3ubn/97n6MCABwDwH9/n6f/g6EDpgAfh6ev/4epw7IAHAPAf\
    BvB/4+vu/xDj7e+AB+Tv8P8Y5vDxgAcA8B/n8fMI/+jz9YAH6vX2Dv/r9viABwDwHwbwf+wC9/r/\
    7fn7gAfuA/r9/+v4+oAHAOAfcAAC4AAG8H8RjvA//+v4+gj/7vv9gAft+fsM/+z3+oAHAQAf9vgI\
    /+r19oAH6PP1Dv/n8fOABwEAHwbgf+YC8PH/5O/wgAfjA+3v/+Pr7oAHAPAfAeHq7P/h6euABwHg\
    6On/3+fpgAfAAPAfBvB/3ufo/97mQOeAB93l5v/d5GDlgAcA8B/c4+T/28AgA4AH4uP/2+Hi6ZAH\
    AOAfBvB/2jAD4OGQB8DwAwDwH+Dg/9nf4PmAB/ADAQAfBuB/EcQwE9rg0AfEAfAfBbB/2uDhABAD\
    4eLR0AcAoB/bICvc4uOQBw/j5P/cIAOABwDgHwbwfwHd5OX/3eXmgAcB3ubn/97n6IAHgADwH9/n\
    6f/g6OmAgAfh6ev/4ers4IAHAPAfBvB/4+vu/+Mg7e+AB+Tv8P/mMPDxgAcA8B/n8fP/EOjz9YAH\
    6vX2/xzr9viABwDwHwbwf+z3BPr/7fn7gAfu+gb9/+v4+oAHAOAfAOAC4AAG8H8RjvA//+v4+v8Q\
    7vv9gAft+fv/GOz3+oAHAQAf9vj/EOr19oAH6PP1/xzn8fOABwEAHwbgf+bwBPH/5O/wgAfj7Qbv\
    /+Pr7oAHAPAf4QLq7P/h6euAB+AD6On/3+fpgAcA8B+ABvB/3ufo/97m54CAB93l5v/d5OXBgAcA\
    8B/c4+T/2yADgYAH4uP/2+HikAfTAOAfBvB/2jAD4OGQB/ADgQDwH+Dg/9nf4IAH8/ADAQAfBuB/\
    EcQwE9rg0AcB8B+JBbB/2uDhABAD4eLQB6IAoB/bICvc4uOQB+Me5P/cIAOABwDgHwbwf90C5OX/\
    3eXmgAfeA+bn/97n6IAHAPAfAd/n6f/g6OmABwHh6ev/4ersgAfAAPAfBvB/4+vu/+PtQO+AB+Tv\
    8P/m8GDxgAcA8B/n8fP/6CDz9YAH6vX2/+s49viABwDwHwbwf+z3+gj/7fn7gAfu+v0N/+v4+oAH\
    AOAfAALgAMAG8H8RjvA//+v4+v/uIPv9gAft+fv/7DD3+oAHAQAf9vj/6iD19oAH6PP1/+c48fOA\
    BwEAHwbgf+bw8Qj/5O/wgAfj7e8M/+Pr7oAHAPAf4eoE7P/h6euAB+DoB+n/3+fpgAcA8B8G8H8B\
    3ufo/97m54AHAd3l5v/d5OWAB4MA8B/c4+T/2yADgAcD4uP/2+HikAcA4B+nBvB/2jAD4OGQB/AD\
    APAfA+Dg/9nf4IAH8APnAQAfBuB/EcQwE9rg0AcB8B8FsH8T2uDhABAD4eLQBwCgH0TbICvc4uOQ\
    B+PkPP/cIAOABwDgHwbwf93kBOX/3eXmgAfe5gbn/97n6IAHAPAf3wLn6f/g6OmAB+ED6ev/4ers\
    gAcA8B+ABvB/4+vu/+Pt74CAB+Tv8P/m8PHAgAcA8B/n8fP/6PNA9YAH6vX2/+v2cPiABwDwHwbw\
    f+z3+v8Q7fn7gAfu+v3/G+v4+oAHAOAfAALgAAbwf4ARjvA//+v4+v/u+0D9gAft+fv/7Pdg+oAH\
    AQAf9vj/6vVA9oAH6PP1/+fxcPOABwEAHwbgf+bw8f8Q5O/wgAfj7e//GOPr7oAHAPAf4ersCP/h\
    6euAB+Do6Q7/3+fpgAcA8B8G8H/eAufo/97m54AH3QPl5v/d5OWABwDwHwbc4+T/2yADgAfiB+P/\
    2+HikAcA4B8G8H9O2jAD4OGQB/ADAPAf4Afg/9nf4IAH8AMBAB/OBuB/EcQwE9rg0AcB8B8FsH/a\
    JuDhABAD4eLQBwCgH9uIICvc4uOQB+Pk/3jcIAOABwDgHwbwf93k5Qj/3eXmgAfe5ucM/97n6IAH\
    APAf3+cE6f/g6OmAB+HpB+v/4ersgAcA8B8G8H8B4+vu/+Pt74AHAeTv8P/m8PGAB4AA8B/n8fP/\
    6PP1gIAH6vX2/+v2+OCABwDwHwbwf+z3+v/tIPn7gAfu+v3/6zf4+oAHAOAfAALgAAbwfxGO8D8A\
    /+v4+v/u+/2AgAft+fv/7Pf6wIAHAQAf9vj/6vX2gIAH6PP1/+fx8+CABwEAHwbgf+bw8f/kIO/w\
    gAfj7e//4zDr7oAHAPAf4ers/xDh6euAB+Do6f8c3+fpgAcA8B8G8H/e5wTo/97m54AH3eUG5v/d\
    5OWABwDwH9wM4+T/2yADgAfi4w7/2+HikAcA4B8G8H/anDAD4OGQB/ADAPAf4OAP/9nf4IAH8AMB\
    AB8G4H+cEcQwE9rg0AcB8B8FsH/a4E3hABAD4eLQBwCgH9sgKxDc4uOQB+Pk/9zwIAOABwDgHwbw\
    f93k5f8Q3eXmgAfe5uf/GN7n6IAHAPAf3+fpCP/g6OmAB+Hp6w7/4ersgAcA8B8G8H/jAuvu/+Pt\
    74AH5APv8P/m8PGABwDwHwHn8fP/6PP1gAcB6vX2/+v2+IAH0ADwHwWwf+cAIJ/s9/r/EO35+4AH\
    7vr9/xvr+PqABwDgHwAC4AAG8H+AEY7wP//r+Pr/7vtA/YAH7fn7/+z3YPqABwEAH/b4/+r1QPaA\
    B+fz9f/n8XDzgAcBAB8A4H/G0dP/AMnU1f9ZXl//AFtfYP/I0tT/AMfQ0/9aX1//YFogEwDwf8bP\
    0f/FIM/PID9e/1ldXgD/w83P/8PLzQj/WFxdIANc/+YC8PH/5O/wgAfjA+3v/+Pr7oAHAPAfAeHq\
    7P/h6euABwHg6On/3+fpgAfAAPAfAPB/wsrL/8DKBMr/V1tcIANb/wC/yMr/v8bJ/wFWWlv/Vllb\
    AQB/Ab7Fx/++xMYgK0BaIC9Z/73ExP8BvMPE/1VYWUADAd7n6P/e5ueABwHd5eb/3eTlgAeDAPAf\
    3OPk/9sgA4AHA+Lj/9vh4pAHAOAfoADwf7sgq7vCw/9UAFhY/1RXWP+6AsHC/7rAwSALV8RAAwDw\
    f7m/wEADVFYQV/9TIAO5v7//FLm+vyAPViAPVv9H2iCr2uDhkAfwAwDwHwPg4P/Z3+CAB/AD0QEA\
    HwDgf7ggq7i9vkCrG1NVVkALQA9VUA8A8H8BuL29/7e8vSA7f1VAAzALsA8AMLMHsBPwrwHwv88C\
    8D8RsjD/2uDQBwHwHwL//7B/kQA//7i9n//a4OEAEAM04eLQBwCgH9sgK9ziQ+OQB+Pk/9wgA4AH\
    xADgHwDwf7i9vkADU1VkVkADQA++v1APVlaiAQB/uSArub+/QCtUiCAvub/AQANTVleAIA9X/93k\
    5f/dIOXmgAfe5uf/3jDn6IAHAPAf3+fp/xDg6OmAB+Hp6/8c4ersgAcA8B8A8H+6wADB/7rBwv9U\
    V0BXQAO7wsP/u8NBxCAPWP9UWFgBAH9AvCArvcTE/1VYQFlAA77Exv++xQTH/1ZZWSADWv8B4+vu\
    /+Pt74AHAeTv8P/m8PGAB4AA8B/n8fP/5/P1gIAH6vX2/+v2+OCABwDwHwDwf7/Gyf+/IMjKIK9b\
    /1ZaWwD/wMrK/8LKywr/V1tbIANcAQB/wwDLzf/Dzc//WCBcXCADXf/Fz88A/8bP0f9ZXV4A/1le\
    Xv/s9/oI/+35+4AH7vr9Df/r+PqABwDgHwAC4ACAAQB/x9DT/8jS1AD/Wl5f/1pfXwD/ydPV/8bR\
    0wn/W19gIL9fABGS4AAA/+fr7P+7ztMA/8/Y2v9tmKII/7TKzkADXI2YgEADx9LU/1GEkYFQB4OR\
    /z13hUADHz54hkAD8C/wP/Av8D+AQF+Ckf/I09X/EFKDkiBXiP8/eQCJ/0B7i/9BfUCNgBfJ1Nb/\
    UoQAkv9Cfo//Q4BAkUADRIOV/z96QYkgA4r/QX6OQAPAMAuwD0OBk/9EggCU/0WFl/9Ghn+ZQAsw\
    DzALMA8C8L8C8P/wr/jwv/Cv8L8C8D8Q8zD/U1VVxE//MAfv+vxAA+D6fP9AAz//MB8//zAn2vlP\
    /0AD1vhQAwM//zB/P/+JMIfN9v5AA8LzUAPxP/8wnz//MKez7/5AAz6o7FAD8K/wv/CvsL/V/yDD\
    AvA/8K/wv/Cv8L8C8D8CsP9b1gKg/9Vh/w1w/9ViLwqx/3rVA6H/c/8Ocv8QOfD/1RAwJP/VrQ9p\
    /+4QEWr/1Q+n/wS//84A7/8ltfBf/6rtAB//8C//APn7+//k+///Ffj8/QAP/94v//cgF6BgBwAC\
    4AD/z/b//wDR9///xfT+/wTK9f7/3WBf3fiAUGe68f7/wfL+Av+x7v7/uiC33JhgF9v3A0B/EY7w\
    P//K1ADW/1aFlf/M1kDYQAdHg5X/SYcAmf9Ig5b/SocBm//M2Nn/VyAfGM7Z2iAHIBOFmP8CTIic\
    /0qEIAeHCZz/S4kgA4qeQAMRTIufwAtNi6AgAwmf/06MIAeKnyADaKBAC0ADi1AD0NrcAP9Yhpb/\
    0dvdAP9Zh5j/S4WZgEBXTIWa/06JnoBAF1uJmf/S3N4C/1yJmv9PIJNQqiBLUCB7UUBTn0BLTwKL\
    of9RjaKAV1AojKJAA1EgB1KNoy//UiAPUyAHcBcwCzAPH0uLnkADcJdwDzC7MM/f8K8w40xgczDP\
    cD9AF7D//yDvATAD8K/wv/Cv8L9xRzD/S04hA0+MUANOYA8AsD/6MRuAu2CvQQ8AcL+J4AuJ3wAQ\
    7wCA/4kAUD9QqzIPcLNyG7/Qv1Fhy/AP8C8AMD8xF0A7f4tQC3APQP8BYL9CuzIT4bf/AXD/gj/h\
    97DvcGNyW1Bz0K/dQeMx14twz/Av0D9P4C/68A8C8L8DcP8CcL8QBkD/ixAB4f+LvRAKov+LD+T/\
    4Q8C8T8OBf+LB1b/93kPCXf/eb8GR/+LCFj/uv8Ix/9/iwdQ/z4LBgT/AOk/AvO/ArM/AYz//wKy\
    /28XAzP/ADG/AvA/Drz/Ab7/AL3/GFNVVU//MAee5/ueQAOa5VADcBc/4zAnmifk+1AD4vpAAwJx\
    /7//5DB/P59AX+H6QAOZ4PhQAz8rMJ8//zCnmN/5i0ADl9z3QAOf4K/wD+vwr/C/MO+foO+eAeA/\
    kK9/+UCz8L/wr/C/8O8B8D8DMP/9ArE/BvD/8q/xzwHw/wKR//oHQf/WAfK/A9D/+QZB/58Cof8A\
    ND+f3wqg/wH0/58K4v8C878K8/8DNP8Csj/7BzT/ArP/A5b/EAeS/wL4/58J5P8C9f/8Arc/D9j/\
    Clb/CrH/AvO/Dsb/4/u7AZz/42AD8A8Abr+YLw9AA+QBbv9wL0/74fkAwD+X3n/4Ahn/AGC/AY4/\
    AKD/sK/wv0Cr+KCv8A8w6/A/sT+X3veJgO+W2/ZQ+932QAvXMA8Ai//kP+v60b9B82ADkwCl/6Lo\
    b0vm+wAALzA/KJ7kYjvjn/+X3/jHQAPwD5fd90ADcK/wD9nwPzH7ni+rMPea4AAQPwWW3Pb/mWD/\
    mSALAKnp/P+z7Pz/QKUgs7Dr/P/Z9QX9//X6+1AH+yAXQeYgF+n7/6TlIB+QINfX9CAf/Pz/1wHz\
    /f/2+/wAAuAAIP+kL1+u5/r/ogDi+f+t5vn/1gDy/P/2/P3/1CDx/EB/oN/3/6sE4/j/oN4gB+H3\
    Av/T8Pv/9CCf0ijv+jAHABGS4AD/090E3/9eipxAB2CLAJ3/UYac/1OKAKD/VIie/1aMAKL/1d7g\
    /2ONQZ5AB2WOoP9XIBsAWI2j/1qNov8AW4+l/1SMo/8AVY2k/1eOpf8eWI+mQAswDzALMA9ZgCAT\
    WpCn/1yRqA7/XZKpQAtwDzAT1gDf4f9pkaL/1wDg4v9skqT/XoAgU1+Rp/9ikKYI/2STq0AXb5Wm\
    AP/Y4eP/dpmqAP9lkqj/aJatAP9qlar/bJivAP9gk6r/YZSrAP9klaz/ZZWt8EAPMBMwDzATaJiu\
    /0BnICdtmrH/bZxuskALMA9sIA8wA/CvVL0gy1QwwyDPMNewr1sgwx1Zj6dAA3C/MC9TMQPmMC8h\
    AzALU4xgP2ETWD6OpoDvMAtwDzBPML9fcJKQrzALMA9jlKz/aGIgA4Cvm1CvZ5euIP9mIANrmbH/\
    aiuasIAvYmAvXiFjMAOjcA9mYC9qmbBAA/AP/zCrsK/wD3CvcbfwDwAQPzD/8VITMQfwPzInVo2l\
    ggeO8K9dkalAA0DPIf9lDZau/2Qx7yC/aSHvvTALZCHnMAswD3AvYSIrzzADMD9ckCFXIVMwAzAr\
    8LAv8A9wr3EHUouj/x9Ri6KCBzCrcK8x+3AP/3LfMCuwL/AP8C/wPzCrsK/P8A8w62OVIgchBzH7\
    MAvlgA8hG7A/X5FQP1sg//8wCzAPMDtCR2AvcidwTwLwv78C8P9b4K/wD8Cv8L8iGwHAP2qXABA/\
    DcD/lg+g/5cH0f9evQAi/14Cov9wT/DvCLL/XgCwv9UM4v8C8f9eACH/Xgz0/5cNpP97l+a/YA8B\
    wD/0/5cK4P8Bt//qAGA/EArB/wHy/5cMEP9cACs/XNcJ4P+wr1wCIP9cDuH/ArE/Az3/GFNVVU8v\
    MAeX2/aAIAP3/5XY9P+W+CADP+swHz8vMCeU1vOPQAOT1PFAAwM2/zB/PvuIMIeS0e9AA5DO7fhA\
    Az77MJ8/OzCnjszqAP+Oy+r/i8fmD/+LxuaArzCrMLMwu79QD/VAv/Cv8L+wL1A/0P/7APA/8K/w\
    vzCrQK/GUK9Av33MkA8BAD/g7/BPsP+WwP999EEDAZD/UC8B8D8BQL/MAuD/trA/y9E/ce+VAmC/\
    AFA/9v+CTwQA/+IPAUI/AHD/IwOwr7H/VZUCwf/2Q/+VBDL/ywKh/33HADL/AFD/AdL/ke+R//UD\
    kP//AHL/JMMC9D+xr5P/kA8Bkv80//+x/wFVPwGEvwJy/wA0PwAh/3KvAfC/9/Hv9g8ERf8BI/+P\
    AYX/sb+z//sBUr+X77b/Uf8B9P+PAbG/Z7/XsO/wP48BQv/24v+QDwFQ/+uS7wARPzc/9Og/9EAT\
    9/9djwPk/48BZj+5r/O/lYf/bvQAgL/Y//UAIT/Q7wFx/4//AeT/ADH/ATY/ABC/ADG/ALD/Uu/6\
    P/tU/wA5PwBR/wJ2vwKHP8d3/wAx/1f3AgH/lQHB//QCGv9gr8AP/wGw/wAwP8j/ABu/AFO/0w/z\
    L9UP2QGT/wFA/8YAEL8AzD/N7AEAP//07wBZv/P/ALf/lf+cPwFT/wLB//8g6wKR//b/tr8A8/+c\
    PwCUPwFj/9EhrwDBv8cAUf+R0O6O78QwC3APj8zrAEM/jcpN6cP/ltpZu0mz2jAPmCu/2fVAAwB0\
    v5LT8PFAA5AvbENgP5TX8yADkWbr1fOAL5PV8UAD/3A/cK8wqzCv8A8wq7CvMLsejMnohL8A8D8w\
    K0AvxnvlQAPwDzD7UK/0QK9QD0f1gA+T1fJAA9C/kA+XsO+V12D/2VBLMQOAPzTS70ADUE/xgA+Q\
    z1/tYP/qQAMwDzATcA8AQL8iyOdAz4rG5AAAL5A/zuxAA4A/ICswLzArMC+28A+WIecwr5Vg/323\
    k80/8zGn1PBQA2C/kj+zRdRQv5HR7i8380ADM5TWUScwD5jZL+8gDy6W1nA/8UArMC8wT5Vc1VAP\
    lDD/4K9wv47LZOlAA8C/xeSGt4nFYONAA/Avj83r/5NGz1A/kczpAAAvTgPIgCA/xOL/jMbj/wCf\
    3PT/q+D2/wCe2vP/p970/wDQ7fn/8/n6/wDO7Pj/8ff4/wCc2PL/p9vy/wCa1O//pNjw/xDN6/dA\
    F8vp9v8I7/b3AALgAP+Z0gDt/6PW7v+XzwDq/6HT7P/K5wD0/+709f/H5Ajz/+zyIlPM5/8An9Dp\
    /5LI4/8Anc7m/8Xi8f8A6vDx/8Tg7v8I5+7vABGS4AD/1+AA4P+BobD/z9gA2v+Srbr/cZkArv9x\
    nLH/eZ8Asv94oLT/vsgAyv+lvMb/prOAIAvO1P+DpLb/AH2it/+Mqrn/AIOou/9ynrT/BHSftf94\
    IBN4oni4QA8wEzALMA9+pboA/3+mu/+Fqr4K/4arv0ALfiAPhCCpvkADhpSW/9og4eRAi9vi4/+Z\
    ALK//4ytv/+3AsfP/5q0xECjqSC0toCv2eHj/6sAvsn/3ePk/8UA0tj/jK/B/40AsMP/k7PF/5QC\
    tMf/i67CQAOUILTGIBPG/5y5yoBAA63Czv+jvs4i/5sgD5q4yiALzzT/oiAPcK93INN3oQC3/3Gd\
    tP9wnWuzQAswD30gv3wgw3CvRnxgD4KovUADMCtvR5xQL3ahtkALMA8wCwB1oLb/fKS5/2h7IANw\
    L3ugD4GnvLGAr5IgvzADia3BQAMRkbLFQAOZuMlAAwGivc7/oLzNgA8cob3NQAPwLzA/iKxQwEA/\
    jyEXmbfJ/xSYt8hAO58gP5e2YshAAzALnrvMgK90wSCrMANum7L/bSADwDHTMdd6o7n/eqIEuf+A\
    prxAA3qjdrhAA3APMCtuYC8yC27WYA9yB3oiE3kiFzIDfyOlu0ALeaG4QAswD3+HIKswAzCrMK9A\
    DyInMLsRjq/DQK+WtcdAq+IwrzALMA+duswgA8vfQlMyV4wh9zAD8A8wOzIXH5y5y0ADMhcyGzAP\
    MgsZbJqxQANyt2uZkA/tMgtyBzC7fiC/ghekIhf9IANwPzI3MjvwD3JHfGAv1PAPMxOEIxuMMret\
    wv+DFzALMA9yxzKzMrdyJzLD7zHb8C+wP4oiS3JXMgcyC+0yZ0JXQhfJAwC/AkD/oJEP+3QHMKsw\
    r/APcweYYK/wD/UC8D8C8L8C8f8JQP+gAuH/oATg/16o4j+oBOH/AvL/BPD/BLL/oN0IIP8Es/+g\
    A+T/4T8KRP+hEAGk/1WgBOb/qAkm/6ALp/+gAyD/xhAJ8/8QIOD/U1VVTyswB4kAxOP/icPj/4g+\
    wOBAAz8/MB8/KzAnhQi72/+EIAOCuNj4QAMDPf8wfz7rMId/s9OPQAN8rs5AAz7rMJ8+64gwp3mp\
    yUADdaTE/kADMKsws3CvQL+gDzCrheqgr/C/gO+/YD/DANA/hP/g//Cv8L/wr/C/AvA/gO8gvx+H\
    v98AAA/xrwBA/yHzMDvuMQPx/wBA/7oIUP8AMb8AAq+6/dIP8i8AMj+w7/JPBwH/w9Ov/wBAv+L/\
    AID/Mf/gTwAwPwhC/3C/67LPASO/AAQ/w6JPv2I/CLH/9+WvAEL/AKK/AAM/xAEh/wez/wA1/+ql\
    zwD0vwD1/4UCNf+yAeX/sv8DZP8AJb8AB68A8//lTwkG/wC0/yfT/zar8P8ANv/D/wC2/+Y/CTT/\
    AMD//wLhPwfF/wE3/wCx//j/df8IuP8ApP/6Og8AMr8BCf8A6P8Ayv+y27941+uvMA94Aer/eAG2\
    /wEwvwA1/68BOf+3q/+3UA8A+//w7wH7/+8AMT8Awv/7v7prr2Cvi79gv+8BSv+g7wHw/3kD4P/y\
    DwLwvwUw/48A/f9+stKO7zALsA8Ajj85o8NAA77vjf/D4k4LEYa/3sKvgbbXjr8XgbXWIA/WQCsw\
    LzArwDAvwA++3f+DutrxQANALyAzsA+AtNVAu8C/r0C/sdH/e63NCf97rMxTv6jIgK+PMAt4p8eA\
    vzArQC8gK/EwL/APMCswL3SiwkADgDA7d6bG/3OhwfBAA4C/IKswr4jC4f8eiMHggA9wvzCrMK+D\
    PLrZQANwD0/nYC+FvX3cQDswPzALMA/QL9RAPxiCudhACzAPfbDQx0ADMOt6q8uADzALMA/TMKuQ\
    r8BQD6XFQAswD5NAP6/PUC+qykALQA/xIAswDzArcC9yoL9AOxF2pcRAC3KfvpCvI7zbQL+HwN9A\
    A0C/+CATMKs/+zC7P6uBt9cM/4G414JncC+Eu2jaQANCA8FQD4e92/FQLyIncpcyM4S510HjEYGz\
    0kC7fK/OQLsBearJ/3uuzUADH3ipyEADMKtAryCrMK+MMLt1pMOAD8AvqMeGIjfM/3+wL/cgC3xg\
    qz/3MufC/3GevqBAA3YgC36oxP9zgCD7mbfK/5DE4QD/nszj/47B3QD/osrh/8Pf7AD/4+rr/8bd\
    6gD/2+Lk/4u82RD/p8sv27fV/64AzN//ydzn/8oA09T/z93j/7MQvb8AAuAA/4m10QD/t8/f/5y+\
    0wH/w9Pd/9bdLncClZf/0tjZQB+yAMnX/83W2P+/AM3U/8nP0P+mELGzABGUoAD/prGyCP/h5udA\
    C9DX2IABAC/D0Nf/sMfUAP/g5Ob/yNTaAP+rxNP/qsPSAP+2y9f/tMrXAP/b4eL/3uTlwEBbMAvL\
    1tv/u85C2TBj/9Pc4AQAv9MC2dr/5OjpQMusPLe4AQDvMKswrzCrs8lF10C7qcLSQAuyIA8Qus3a\
    QAO8zdf/HLnM2YAPcBcwK6jCBNH/ssjWIQfV/xanwdEgA9BACzAPubQgP7mgMzAPuKBH3eQB5f/F\
    0dn/4iEPAeTp6v+1yNRAZ3HfIU8gF0GTusPDgZ9y4VAnga9wn7bHIMPHXNOAr7chezADMFPY3wTj\
    /7O9viEb2/8AytXa/73K0/8H1Nrb/+UgbzCrMK9AryC7r8bV/6bAEdD/pSADrsbUQAPkMKuwrzC7\
    t8uRB6W/Ys9AAzAvrcXUQA+k7yATMAswD7YgKzADgKfgD4AA8Ae3xc//ssLNCP/l6utAA6u8yb0g\
    A8iADwDwRzArUC/qQAOM8A+jvs9AAzC7q8NE00APor3Oghe1yn7YQANyB/APMCuwLzA7oQa9zf+q\
    wlJHMD+yP8jXgfswCzAP8hcBEL9w/3zIAlD/IlsyX3EPIyP/4zfo6ECrMK+pYK/wD/Cv//C/AvA/\
    AXG/MKswr/APAvA/EFFQ/1jWA+b/1hBZ5v8A3f9TVVXETuswB3KevkADb5l8ukADPvswHz77MCdr\
    k0GzQANnjq7/aCAD4gM9/zB/cAdnja6gA63xT/8wnz7rMKeYscggAwfH/77FxkAD8K/wv++wrzCz\
    cL9oYA8BsD8w7/BP73Cn0LcxG69BH/CvMLsww3+/IL8wA/Dv8EcAED8CUP9w7/7xDwLxP9Dz0P8x\
    v1DvUa+/v6C/v2G/ADE7sUOwP5E/AlH/v/G/aARA/9ILAZD/ADJD0D+yP+8C8v8C8f+S/62h7wCQ\
    /3Lv8c+30vutwkuR/8gDQf8C8/8AsP//ABLDUM8A8b8AEPuRBwPy/wO0/9P//1G7ADIDAPO/AFP/\
    kE8Dc/8D9P8Alf99rQHg/wBQQ1L/ARE/BxL/ruH739cTAVD/rQADA3RPBHL/Afb/ADc/+wA17wIS\
    /wAxQwUT/wIX/60BIv8AEsP9AVP/Uu/V/wAYPwLz/9m/rQKh/32yak/597LHALX/APb/rQQA//8A\
    cb8BWz+Q7wCW/wAzwwGz//k/AVE//Qd4//i/AJm/3L8AsT8D8v9nBOr/+gBQvwA8/wCw/93/A9f/\
    rQDM/24DmLn/cZ2+QAMwCxFumLgATj9pkbGj//gAMAMBkr8AUz8AFD/7P8DGyNhAA5CvuECvsA9t\
    l7f4QKtwrz+vsA8/w3GcveNAAzArMC9wnLxAAzA7H2yWtoA/cT/wPwAcvwBTvxjAx8hAA34PwsjI\
    7kADADAz2D/IwC/wPzCvbzybu0CrMK8wC4APlbUP/2mQsEADf//wD4AvxCArMC9vmrpAA2uUR7RA\
    A2iQr0ADf+9ADzqPrgCg/9cPP/uZYP/BgSEDnbTI/6G2UPvAMA9//3GUsv9xlUCzj/99nbj/j6oA\
    wP+luMj/r76AMevH/7C3uP+5QMMwR8n/rra4/wGSnZ3/bpm5QAMda5SzQANyBzAPaian6DCrsK9w\
    D2kg14imu7BC428i5zArdZq2/wB+o73/tsbQ/wCuv8n/w8vN/wB5mrX/rr3H/yi1wjGvyiHHy/+8\
    AcPE/7rBwgAgAAD/ao+w/36euRD/qrsuk8bJ/6gCucb/vcbKId/JAv++xMX/wSAHvTHDxTArQC+Z\
    o6VwC8BAPzAnsLi5/5GcIJ4AAiAA/8nP0f8BmKOl/7S7vAIwP8Af//AAFYnwAAAA"))

    -- double right
    textures.double[2] = love.graphics.newImage(decodeBuffer("\
    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"))

    messagebox.texture = textures.single[0]

    if not buttons or #buttons == 0 then
        table.insert(messagebox.buttons, newButton(1, "OK", boxPosition.x, boxSize.w))
    else
        if #buttons == 2 then
            messagebox.texture = textures.double[0]
        end

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

    local releasedEvents = {}

    releasedEvents.gamepadreleased = true
    releasedEvents.touchreleased = true
    releasedEvents.mousereleased = true

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
            end

            if not buttonID then
                return
            end

            self.texture = textures[self.mode][buttonID]
        elseif releasedEvents[event] then
            self.texture = self.baseTexture

            if buttonID then
                self.buttons[buttonID]:touchreleased()
            end
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
        love.graphics.draw(self.texture, boxPosition.x, boxPosition.y)

        love.graphics.setColor(textColors.default)
        love.graphics.printf(text, textFont, 24, ((boxPosition.y + boxSize.h) - 40) / 3, boxSize.w - 24, "center")

        for _, value in ipairs(self.buttons) do
            value:draw()
        end

        love.graphics.setFont(currentFont)
        love.graphics.setColor(1, 1, 1, 1)
    end

    return messagebox
end

local box

local function main()
    local normalScreens = love.graphics.getScreens()
    local plainScreens
    if love._console_name == "3DS" then
        plainScreens = {"top", "bottom"}
    end

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

        if love.graphics then
            love.graphics.origin()

            love.graphics.setActiveScreen("bottom")
            love.graphics.clear(love.graphics.getBackgroundColor())

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
    textFont = love.graphics.newFont(16)

    boxPosition = { x = 10, y = 12}
    boxSize = { w = (love.graphics.getWidth("bottom") or 320) - (boxPosition.x * 2), h = love.graphics.getHeight() - (boxPosition.y * 2)}

    box = newMessageBox(text, buttons)

    return main()
end
