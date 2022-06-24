// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui_internal.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <anitk/lib.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void drop_callback(GLFWwindow* window, int count, const char **paths) {
    for(int n = 0; n < count; n++) {
        std::cout << paths[n] << std::endl;
    }
}

int main(int, char**)
{
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char* glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(1280, 720, "anitk", NULL, NULL);
  if (window == NULL)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSetDropCallback(window, drop_callback);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != NULL);

  io.Fonts->AddFontDefault();
  ImFontConfig config;
  config.MergeMode = true;
  io.Fonts->AddFontFromFileTTF("fonts/NotoSansCJK-Regular.ttc", 18.0f, &config, io.Fonts->GetGlyphRangesJapanese());
  io.Fonts->AddFontFromFileTTF("fonts/NotoSansCJK-Regular.ttc", 18.0f, &config, io.Fonts->GetGlyphRangesKorean());
  io.Fonts->Build();

  // Our state
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Image
  int my_image_width = 0;
  int my_image_height = 0;
  GLuint my_image_texture = 0;
  std::optional<GLuint> ret = LoadTextureFromFile("cels/A0001.jpg", &my_image_width, &my_image_height);
  IM_ASSERT(ret);
  my_image_texture = ret.value();

  auto cpaths = ListDirectory("cels").value();
  std::vector<std::string> v;
  std::vector<GLuint> image_textures;
  for (const auto& cpath : cpaths) {
    v.push_back(cpath.string());
    auto r = LoadTextureFromFile(cpath.string(), &my_image_width, &my_image_height);
    image_textures.push_back(r.value());
  }

  // Main loop
  while (!glfwWindowShouldClose(window))
  {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    // glfwPollEvents();
    glfwWaitEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 0. fill
    {
      // int width, height;
      // glfwGetWindowSize(window, &width, &height);
      // ImGui::SetNextWindowSize(ImVec2(width, height)); // ensures ImGui fits the GLFW window
      // ImGui::SetNextWindowPos(ImVec2(0, 0));

      ImGui::Begin("t", nullptr, ImGuiWindowFlags_MenuBar);

      if (ImGui::BeginMenuBar()) {
          ImGui::MenuItem("About");
      }
      ImGui::EndMenuBar();

      static int selected = 0;

      float iw = 100.f;
      float ih = (float)my_image_height / (float)my_image_width * iw;

      ImGui::BeginChild("Timeline", ImVec2(ImGui::GetContentRegionAvail().x, ih*1.2), false, ImGuiWindowFlags_HorizontalScrollbar);
      for (int n = 0; n < image_textures.size(); n++) {
        ImGui::PushID(n);
        if (n != 0) ImGui::SameLine();

        auto cur = ImGui::GetCursorPos();
        char buf[32];
        sprintf(buf, "##Object %d", n);
        ImGui::SetCursorPos(ImVec2(cur.x, cur.y));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::Selectable(buf, selected == n, 0, ImVec2(iw, ih))) {
          selected = n;
        }
        ImGui::PopStyleColor();

        ImGui::SetItemAllowOverlap();
        ImGui::SetCursorPos(ImVec2(cur.x, cur.y));
        ImGui::Image((void*)(intptr_t)image_textures[n], ImVec2(iw, ih));

        ImGui::PopID();
        if (selected == n) {
          ImGui::SetScrollHereX(0.5f); // 0.0f:left, 0.5f:center, 1.0f:right
        }
      }

      if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && io.WantCaptureMouse) {
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_N))) selected++;
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_P))) selected--;
      }
      if (selected < 0) selected = 0;
      if (selected >= image_textures.size()) selected = image_textures.size()-1;
      ImGui::EndChild();

      ImGui::BeginChild("Viewport",
                        ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
                        false,
                        0);

      ImVec2 avail_size = ImGui::GetContentRegionAvail();
      ImVec2 i_size = ImVec2( (float)my_image_width / (float)my_image_height * avail_size.y, avail_size.y);
      ImGui::SetCursorPos((ImGui::GetContentRegionAvail() - i_size) * 0.5f +
                          ImGui::GetWindowSize() - ImGui::GetContentRegionAvail());
      ImGui::Image((void*)(intptr_t)image_textures[selected], i_size);
      ImGui::EndChild();
      ImGui::End();
    }

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

      ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
      ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
      ImGui::Checkbox("Another Window", &show_another_window);

      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

      if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
      ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me"))
        show_another_window = false;
      ImGui::End();
    }

    // 4. Show Image
    // {
    //   static int counter = 0;
    //   ImGui::Begin("OpenGL Texture Text", NULL, ImGuiWindowFlags_HorizontalScrollbar);
    //   ImGui::Text("pointer = %p", image_textures[counter]);
    //   ImGui::Text("size = %d x %d", my_image_width, my_image_height);
    //   ImGui::Image((void*)(intptr_t)image_textures[counter], ImVec2(my_image_width, my_image_height));
    //   if (ImGui::IsWindowFocused() && io.WantCaptureMouse) {
    //     if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_N))) counter++;
    //     if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_P))) counter--;
    //   }
    //   ImGui::Text("counter = %d", counter);
    //   ImGui::End();
    // }

    // 5. ls and select
    // {
    //   static int selected = -1;
    //   ImGui::Begin("ls");
    //   ImGui::Columns(2);
    //   int n = 0;
    //   for (const auto &cpath: cpaths) {
    //     if (ImGui::Selectable(cpath.c_str(), selected == n)) {
    //       selected = n;
    //     }
    //     n++;
    //   }
    //   ImGui::NextColumn();
    //   ImVec2 avail_size = ImGui::GetContentRegionAvail();
    //   ImGui::Image((void*)(intptr_t)image_textures[selected], avail_size);
    //   ImGui::Columns(1);
    //   ImGui::End();
    // }

    // 5.5 child widows
    {
      static int selected = 0;
      ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
      ImGui::Begin("ayy");
      ImGui::BeginChild(
        "ChildL",
        ImVec2(
          100.0f,
          ImGui::GetContentRegionAvail().y),
        false,
        window_flags);
      int n = 0;
      for (const auto &cpath: cpaths) {
        if (ImGui::Selectable(cpath.string().c_str(), selected == n)) {
          selected = n;
        }
        n++;
      }
      ImGui::EndChild();

      if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && io.WantCaptureMouse) {
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_N))) selected++;
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_P))) selected--;
      }
      if (selected < 0) selected = 0;
      if (selected >= cpaths.size()) selected = cpaths.size()-1;

      ImGui::SameLine();

      ImGui::BeginChild(
        "ChildR",
        ImVec2(
          ImGui::GetContentRegionAvail().x,
          ImGui::GetContentRegionAvail().y),
        false,
        window_flags);
      ImVec2 avail_size = ImGui::GetContentRegionAvail();
      ImGui::Image(
        (void*)(intptr_t)image_textures[selected],
        ImVec2((float)my_image_width / (float)my_image_height * (float)avail_size.y,
               avail_size.y));
      ImGui::EndChild();


      ImGui::End();
    }

    // 6. listbox
    // {
    //   static int current_item = -1;

    //   ImGui::Begin("listbox");
    //   ImGui::Columns(2);
    //   ImGui::ListBox("Cels", &current_item, v.data(), v.size());
    //   ImGui::NextColumn();
    //   ImGui::Image((void*)(intptr_t)image_textures[current_item], ImVec2(my_image_width, my_image_height));
    //   ImGui::Columns(1);
    //   ImGui::End();
    // }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}