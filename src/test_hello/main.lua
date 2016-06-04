LoadLibrary("Renderer")

gRenderer = Renderer:Create()
gRenderer:AlignText("center", "center")

function update()
    gRenderer:DrawText2d(0, 0, "Hello\n World")
    -- print("")
end

-- Looks like it's crashing in LuaJIT?
-- Maybe get a proper co