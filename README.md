# ShadowTrail

Ein 3D-Spiel in **C++ und OpenGL**, entstanden als Teamprojekt im Modul Computergrafik an der Hochschule Osnabrück (Sid Wannink und Paula Matthies).

Die Spielfigur muss über einen Pfad zur Zielplattform laufen, der fast vollständig im Dunkeln liegt. Lichtquellen auf dem Pfad beleuchten ihn in pulsierenden Lichtkreisen, sodass immer nur Teile sichtbar sind. In regelmäßigen Abständen wird der ganze Pfad kurz sichtbar, das ist die Merkphase. Wer vom Weg abkommt, fällt herunter und startet wieder am Anfang.

## Spielmodi

| Modus | Beschreibung |
|---|---|
| Leicht | Einzelspieler, kleines Spielfeld, drei Leben |
| Schwer | Einzelspieler, großes Spielfeld, drei Leben |
| Multiplayer | Zwei Spieler im geteilten Bildschirm, großes Spielfeld, ohne Lebensbegrenzung. Wer zuerst am Ziel ist, gewinnt. |

**Steuerung im Multiplayer:** Spieler 1 (links) mit WASD, Spieler 2 (rechts) mit den Pfeiltasten.

## Technische Umsetzung

- **Rendering mit OpenGL und GLSL:** eigene Shader, darunter die Lichtkreise, die wachsen und schrumpfen und alles außerhalb des Lichts ausblenden
- **Partikelsystem:** Funken über den Feuerkugeln und ein Lichtring am Ziel
- **Zufällige Pfadgenerierung:** jede Runde ein neuer Weg von der Start- zur Zielplattform
- **Kollisionserkennung** mit achsenparallelen Bounding Boxes (AABB)
- **Geteilter Bildschirm** für den Mehrspielermodus
- **Menü und Oberfläche** mit Dear ImGui, Musik mit miniaudio

## Technik

C++ · OpenGL · GLSL · GLEW · Assimp · FreeImage · Dear ImGui · miniaudio · Visual Studio

## Starten

Das Projekt ist eine Visual-Studio-Projektmappe für Windows.

1. `CGVStudio/ShadowTrail.sln` in Visual Studio öffnen.
2. Als Konfiguration x64 wählen und das Projekt starten.

## Quellen

- Spielfiguren: free3d.com ([Vogel](https://free3d.com/3d-model/bird-v1--282209.html), [Hund](https://free3d.com/3d-model/australian-cattle-dog-v1--993323.html))
- Texturen: Freepik (Feuer-, Stein- und Mauertexturen)
- Kollisionserkennung: [LearnOpenGL](https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection), [MDN](https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection)
- Partikelsystem: [LearnOpenGL – Particles](https://learnopengl.com/In-Practice/2D-Game/Particles)
- Pfadgenerierung: angelehnt an das Projekt „ZufallswegGenerator“ aus dem Praktikum Spieleprogrammierung & 3D-Animation 24/25
- Bibliotheken: [Dear ImGui](https://github.com/ocornut/imgui), [miniaudio](https://github.com/mackron/miniaudio)
- Spielidee inspiriert von [diesem Video](https://www.youtube.com/watch?v=gXdHO7XomSA)
