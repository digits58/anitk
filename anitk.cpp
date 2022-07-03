#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui_internal.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include <array>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "lib.h"
#include "CelFolder.h"
#include "config.h"

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

fs::path CelsPath;

void drop_callback(GLFWwindow* window [[maybe_unused]], int count, const char **paths) {
  if (count != 1) return;

  fs::path p(paths[0]);
  if (!fs::is_directory(p)) return;

  CelsPath = p;
}

static std::string BUILD_DATE = __DATE__;
static std::string BUILD_TIME = __TIME__;
static std::string BUILD_TIMESTAMP = BUILD_DATE + ' ' + BUILD_TIME;

int main(int argc, char** argv) {
  if (argc > 1) {
    CelsPath = argv[1];
  }

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
  io.IniFilename = NULL;
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
  // bool show_demo_window = false;
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

    ImGui::Text("%s v%s", PROJECT_NAME, PROJECT_VER);
    ImGui::Text("last commit: %s", GIT_COMMIT_SHA1);
    ImGui::Text("commit date: %s", GIT_COMMIT_DATE);
    ImGui::Text("build date: %s", BUILD_TIMESTAMP.c_str());
    ImGui::Separator();

    auto windowWidth = ImGui::GetContentRegionAvail().x;
    auto windowHeight = ImGui::GetContentRegionAvail().y;

    if (CelsPath.empty()) {
      std::string text = "Drag and drop cel folder";
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

      ImGui::Text("%s", text.c_str());
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

  while (!glfwWindowShouldClose(window)) {
    glfwWaitEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::Begin("Layers", nullptr, ImGuiWindowFlags_NoDecoration);

    static CelFolder cels = CelFolder(CelsPath);
    static std::string dest = "output";
    static bool first = true;

    ImGui::Text("Src: %s", CelsPath.string().c_str());
    ImGui::Text("Dest: %s", dest.c_str());
    // ImGui::SameLine();
    // ImGui::InputText("##Destination", &dest);

    if (ImGui::Button("Run")) {
      for (const auto &[layer, paths] : cels.dedupe) {
        auto changes = dedupeImagePaths(paths);
        // for (const auto& [in, out] : changes) {
        //   std::cout<< in << "->" << out << std::endl;
        // }
        executeChanges(changes);
      }
    };
    ImGui::Separator();

    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("Input", ImVec2(contentRegion.x/2.0f -10.0f, -1.0f));
    ImGui::Text("%s", "Input");
    ImGui::Separator();

    for (const auto &[layer, paths] : cels.layers) {
      char buf[256];
      snprintf(buf, 255, "%s (%ld)", layer.c_str(), paths.size());
      if (first) ImGui::SetNextItemOpen(true);
      if (ImGui::TreeNode(buf)) {
        for (const auto& p : paths) {
            ImGui::Text("%s", p.filename().string().c_str());
        }
        ImGui::TreePop();
      }
    }

    ImGui::EndChild();

    ImGui::SameLine();


    ImGui::BeginChild("Output", ImVec2(contentRegion.x/2.0f -10.0f, -1.0f));
    // static int i0 = digitCount(cels.imagePaths.begin()->filename().string());
    // ImGui::InputInt("zero padding", &i0);
    ImGui::Text("%s", "Output");
    ImGui::Separator();

    static std::map<std::string, std::vector<std::pair<fs::path, fs::path>>> dedupes;
    for (const auto &[layer, paths] : cels.dedupe) {
      char buf[256];
      snprintf(buf, 255, "%s (%ld)", layer.c_str(), paths.size());
      if (first) ImGui::SetNextItemOpen(true);
      if (ImGui::TreeNode(buf)) {
          if (dedupes.count(layer) == 0) dedupes[layer] = dedupeImagePaths(paths);
        for (const auto& [in, out] : dedupes[layer]) {
            ImGui::Text("%s", out.filename().string().c_str());
        }
        ImGui::TreePop();
      }
    }
    first = false;
    ImGui::EndChild();
    ImGui::End();

    // if (show_demo_window)
    //   ImGui::ShowDemoWindow(&show_demo_window);

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
