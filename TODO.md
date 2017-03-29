# A unordered set of Todos for Dinodeck

- French characters aren't working. (Why?)
- What would be required to add support for Asian languages too. UTF-8 - multibyte characters have implications a few places in the code base
- Add a Quickstart page to the website
- Make the sound streams API work
- Simplfiy build process
    - You should be able to download the source on any OS, run a script and have it build or a useful error message telling you the next step
    - This is hard and I'd definitely need help!
- Try emscriptem out on some of the earlier steps of the project
- Remove FTGL only a small amount of it used, interface with freetype directly
- Shader support
    - Renderer.PushShader() ?
    - Needs to be cross platform (straight GLSL?)