#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif

#include <iostream>  // für std::cerr und std::cout
#include <stdio.h>   // für printf
#include "Application.h"
#include "freeimage.h"
#include "../CGVStudio/CGVStudio/MenuManager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <fstream>
#include <thread>
#include <chrono>


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
    const char* fontPath = "fonts/Bold.ttf";

    // Prüfen, ob die Datei existiert:
    std::ifstream fontFile(fontPath);
    if (!fontFile) {
        std::cerr << "[DEBUG] Font-Datei nicht gefunden: " << fontPath << "\n";
    }
    else {
        std::cout << "[DEBUG] Font-Datei erfolgreich gefunden: " << fontPath << "\n";
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault(); // Standardfont (Index 0)

        io.Fonts->AddFontFromFileTTF(fontPath, 32.0f);
    }
    ImGui::StyleColorsDark(); // oder Light/Classic

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");


    float dtime = 0;
    float lastDtime;
    
    {
        MenuManager& menu = MenuManager::instance();
        Application App(window);

        // Sofort starten (default Easy)
        App.initialize(Difficulty::Easy);
        App.start();
        menu.audioReadyForGame = true;

        float dtime = 0;
        float lastDtime = 0;

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            lastDtime = dtime;
            dtime = glfwGetTime();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            menu.updateMusic();

            if (menu.state == MenuState::Loading) {
                menu.Draw();

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                glfwSwapBuffers(window);

                menu.state = MenuState::LoadingStarted;
                continue;
            }

            if (menu.state == MenuState::LoadingStarted) {
                if (menu.multiplayerSelected) {
                    menu.state = MenuState::MultiPlayer;
                }
                else {
                    menu.state = MenuState::Playing;
                }
                App.reinitialize(menu.difficulty);
                menu.loadingTriggered = false;
                menu.audioReadyForGame = true;
               
            }

            if (menu.state == MenuState::Playing || menu.state == MenuState::MultiPlayer) {
                App.update(dtime - lastDtime);
                if (App.isGameOver())
                    menu.state = MenuState::GameWon;
            }

            App.draw();

            if (menu.state == MenuState::Start ||
                menu.state == MenuState::GameWon)
            {
                menu.Draw();
            }

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
