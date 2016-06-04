print('[LUA:test/main.lua] the main file! This is getting called from Lua.')

LoadLibrary("Asset")
LoadLibrary("Vector")
LoadLibrary("Renderer")
LoadLibrary("Texture")
LoadLibrary("Sprite")
LoadLibrary("Matrix")
LoadLibrary("System")
LoadLibrary("Touch")
LoadLibrary("Mouse")
LoadLibrary("Keyboard")
LoadLibrary("Sound")

Asset.Run('Dependancies.lua')
-- tes

print("HELLO")
local n = Sound.Play("treasure_chest.wav")
print("SOUND ID", n, type(n))

gRenderer = Renderer.Create()

texture = Texture.Find("basketball.png")
gBackground = Sprite.Create()
gBackground:SetTexture(texture)

gBackgroundOverlap = (720-508) / 2
gBackground:SetPosition(0,-gBackgroundOverlap)
-- 512 -1024 full image
-- 320 720
--print(texture:GetWidth(), texture:GetHeight())

gTime = 0
--print(System.GetScreenTopLeft())
--print(System.GetScreenBottomRight())

gRenderer:SetFontHorzAlignment("center")
gRenderer:SetFontVertAlignment("center")

local mosPos = Vector.Create()
function update()
  gTime = gTime + GetDeltaTime()
  gRenderer:DrawSprite(gBackground)
  gRenderer:DrawText2d(0,0,"Hello there")
end

