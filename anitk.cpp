#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <stdio.h>
#include <vector>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "nfd.hpp"
#include "nlohmann/json.hpp"

#include "CelFolder.h"
#include "config.h"
#include "lib.h"

using json = nlohmann::json;
static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

fs::path CelsPath;

void drop_callback(GLFWwindow *window [[maybe_unused]], int count,
                   const char **paths) {
  if (count != 1) {
    return;
  }

  fs::path p(paths[0]);
  if (!fs::is_directory(p)) {
    return;
  }

  CelsPath = p;
}

int main(int argc, char **argv) {
  if (argc > 1) {
    CelsPath = argv[1];
  }

  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    return 1;
  }

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  glfwWindowHint(GLFW_RESIZABLE, false);

  // Create window with graphics context
  GLFWwindow *window = glfwCreateWindow(640, 360, "anitk", NULL, NULL);
  if (window == NULL) {
    return 1;
  }
  glfwMakeContextCurrent(window);
  glfwSetDropCallback(window, drop_callback);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.IniFilename = NULL;
  // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // Enable Gamepad Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please
  // handle those errors in your application (e.g. use an assertion, or display
  // an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored
  // into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which
  // ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
  // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

  // io.Fonts->AddFontDefault();
  // ImFontConfig config;
  // config.MergeMode = true;
  // io.Fonts->AddFontFromFileTTF("fonts/NotoSansCJK-Regular.ttc", 18.0f,
  // &config, io.Fonts->GetGlyphRangesJapanese());
  // io.Fonts->AddFontFromFileTTF("fonts/NotoSansCJK-Regular.ttc", 18.0f,
  // &config, io.Fonts->GetGlyphRangesKorean()); io.Fonts->Build();

  // Our state
  // bool show_demo_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  NFD::Guard nfdGuard;        // quit after scope
  NFD::UniquePath inputPath;  // free after scope
  NFD::UniquePath outputPath; // free after scope

  // Drag and drop loop
  while (!glfwWindowShouldClose(window)) {
    glfwWaitEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Drag and Drop", nullptr, ImGuiWindowFlags_NoDecoration);

    ImGui::Text("%s v%s by 58", PROJECT_NAME, PROJECT_VER);
    ImGui::Text("last commit: %s", GIT_COMMIT_SHA1);
    ImGui::Text("commit date: %s", GIT_COMMIT_DATE);
    ImGui::Text("build date: %s", BUILD_TIMESTAMP.c_str());
    ImGui::Separator();

    static std::string inp;
    static std::string outp;

    ImGui::Text("Input ");
    ImGui::SameLine();
    ImGui::InputText("##Input", &inp);
    ImGui::SameLine();
    if (ImGui::Button("Browse##InputPath")) {
      // show the dialog
      nfdresult_t result = NFD::PickFolder(inputPath);
      if (result == NFD_OKAY) {
        inp = inputPath.get();
        std::cout << inputPath.get() << std::endl;
      } else if (result == NFD_CANCEL) {
        std::cout << "User pressed cancel." << std::endl;
      } else {
        std::cout << "Error: " << NFD::GetError() << std::endl;
      }
    }
    ImGui::SameLine();
    static CelFolder cels;

    if (ImGui::Button("1. Analyze")) {
      cels.setPath(inp);
      cels.dedupeLayers();
    }
    ImGui::Text("Output");
    ImGui::SameLine();
    ImGui::InputText("##Output", &outp);
    ImGui::SameLine();
    if (ImGui::Button("Browse##OutputPath")) {
      // show the dialog
      nfdresult_t result = NFD::PickFolder(outputPath);
      if (result == NFD_OKAY) {
        outp = outputPath.get();
        std::cout << outputPath.get() << std::endl;
      } else if (result == NFD_CANCEL) {
        std::cout << "User pressed cancel." << std::endl;
      } else {
        std::cout << "Error: " << NFD::GetError() << std::endl;
      }
    }
    ImGui::SameLine();
    bool runButton = false;
    if (ImGui::Button("2. Run    ")) {
      runButton = true;
      for (const auto &[layer, paths] : cels.dedupe) {
        auto changes = dedupeImagePaths(paths, outp);
        for (const auto &[in, out] : changes) {
          std::cout << in << "->" << out << std::endl;
        }
        executeChanges(changes, outp);
      }
    }

    ImGui::Separator();

    if (!inp.empty()) {
      static bool first = true;

      ImVec2 contentRegion = ImGui::GetContentRegionAvail();

      ImGui::BeginChild("Input", ImVec2(contentRegion.x / 2.0f - 10.0f, -1.0f));
      ImGui::Text("%s", "Input");
      ImGui::Separator();

      for (const auto &[layer, paths] : cels.layers) {
        char buf[256];
        snprintf(buf, 255, "%s (%ld)", layer.c_str(), paths.size());
        if (first) {
          ImGui::SetNextItemOpen(true);
        }
        if (ImGui::TreeNode(buf)) {
          for (const auto &p : paths) {
            ImGui::Text("%s", p.filename().string().c_str());
          }
          ImGui::TreePop();
        }
      }

      ImGui::EndChild();

      ImGui::SameLine();

      ImGui::BeginChild("Output",
                        ImVec2(contentRegion.x / 2.0f - 10.0f, -1.0f));
      // static int i0 =
      // digitCount(cels.imagePaths.begin()->filename().string());
      // ImGui::InputInt("zero padding", &i0);
      ImGui::Text("%s", "Output");
      ImGui::Separator();

      static std::map<std::string, std::vector<std::pair<fs::path, fs::path>>>
          dedupes;
      for (const auto &[layer, paths] : cels.dedupe) {
        char buf[256];
        snprintf(buf, 255, "%s (%ld)", layer.c_str(), paths.size());
        if (first) {
          ImGui::SetNextItemOpen(true);
        }
        if (ImGui::TreeNode(buf)) {
          if (dedupes.count(layer) == 0)
            dedupes[layer] = dedupeImagePaths(paths, outp);
          for (const auto &[in, out] : dedupes[layer]) {
            ImGui::Text("%s", out.filename().string().c_str());
          }
          ImGui::TreePop();
        }
      }
      first = false;
      ImGui::EndChild();
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
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
