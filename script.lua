-- script.lua  –  optional user script
print("Lua script loaded!")

print(imgui)

function _render()
    imgui.SetNextWindowPos({10, 10}, "Once")
    imgui.SetNextWindowSize({500, 200}, "Once")

    if imgui.BeginWindow("My Lua Window") then
        imgui.Text("Hello from Lua!")
        if imgui.Button("Click me!") then
            print("Button pressed in Lua → C")
            hello_world()          -- calls the C test_call()
        end
    end
    imgui.EndWindow()
end