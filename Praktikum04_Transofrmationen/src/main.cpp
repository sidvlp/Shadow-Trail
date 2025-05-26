#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include <stdio.h>
#include "Application.h"
#include "freeimage.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//test 2
void PrintOpenGLVersion();


int main () {
    FreeImage_Initialise();
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }
    
#ifdef __APPLE__
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    
    const int WindowWidth = 800;
    const int WindowHeight = 600;
    
    GLFWwindow* window = glfwCreateWindow (WindowWidth, WindowHeight, "Computergrafik - Hochschule Osnabrück", NULL, NULL);
    if (!window) {
        fprintf (stderr, "ERROR: can not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent (window);

#if WIN32
	glewExperimental = GL_TRUE;
	glewInit();
#endif

    PrintOpenGLVersion();

    // IMGUI Setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark(); // oder Light/Classic

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    float dtime = 0;
    float lastDtime;
    
    {
        Application App(window);
        App.start();
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            lastDtime = dtime;
            dtime = glfwGetTime();

            // Start ImGui Frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // GUI Logik
            static bool show_start_menu = true;
            if (show_start_menu) {
                ImGui::SetNextWindowSize(ImVec2(400, 300));
                ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
                ImGui::Begin("Startmenü", nullptr,
                    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                    ImGuiWindowFlags_NoScrollbar);

                ImGui::Spacing();
                ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Willkommen zum Spiel!").x) * 0.5f);
                ImGui::Text("WillkommenS!");
                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.4f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.2f, 1.0f));

                ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 200) * 0.5f);
                if (ImGui::Button("Spiel starten", ImVec2(200, 40))) {
                    show_start_menu = false;
                }

                ImGui::PopStyleColor(3);
                ImGui::End();
            }

            else {
                App.update(dtime - lastDtime);
                App.draw();
            }

            // ImGui Rendern
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }

        App.end();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}


void PrintOpenGLVersion()
{
    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);
}
