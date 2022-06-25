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

#include "lib.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

fs::path CelsPath;

void drop_callback(GLFWwindow* window, int count, const char **paths) {
  if (count != 1) return;

  fs::path p(paths[0]);
  if (!fs::is_directory(p)) return;

  CelsPath = fs::absolute(p);
}

int main(int, char**) {
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

  glfwWindowHint(GLFW_RESIZABLE, false);

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(640, 360, "anitk", NULL, NULL);
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

  // io.Fonts->AddFontDefault();
  // ImFontConfig config;
  // config.MergeMode = true;
  // io.Fonts->AddFontFromFileTTF("fonts/NotoSansCJK-Regular.ttc", 18.0f, &config, io.Fonts->GetGlyphRangesJapanese());
  // io.Fonts->AddFontFromFileTTF("fonts/NotoSansCJK-Regular.ttc", 18.0f, &config, io.Fonts->GetGlyphRangesKorean());
  // io.Fonts->Build();

  // Our state
  bool show_demo_window = true;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Drag and drop loop
  while (!glfwWindowShouldClose(window) && CelsPath.empty()) {
    glfwWaitEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Drag and Drop", nullptr, ImGuiWindowFlags_NoDecoration);
    auto windowWidth = ImGui::GetWindowSize().x;
    auto windowHeight = ImGui::GetWindowSize().y;

    if (CelsPath.empty()) {
      std::string text = "Drag and drop cel folder here";
      auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
      auto textHeight = ImGui::CalcTextSize(text.c_str()).y;
      ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
      ImGui::SetCursorPosY((windowHeight - textHeight) * 0.5f);

      ImGui::Text("%s", text.c_str());
    } else {
      std::string text = "Loading";
      auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
      auto textHeight = ImGui::CalcTextSize(text.c_str()).y;
      ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
      ImGui::SetCursorPosY((windowHeight - textHeight) * 0.5f);

      ImGui::Text(text.c_str());
    }
    ImGui::End();

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

  glfwSetWindowSize(window, 1024, 768);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
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


    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);
  

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).

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
