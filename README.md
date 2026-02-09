# Spider Solitaire

A Spider Solitaire game built from scratch using C++ and OpenGL. This project was created as a learning exercise to understand the fundamentals of graphics programming, shader management, and game logic implementation.

## Libraries Used

This project leverages several industry-standard libraries:

*   **OpenGL**: The core graphics API used for rendering.
*   **GLFW**: For window creation, context management, and handling user input.
*   **GLAD**: To load OpenGL function pointers.
*   **Dear ImGui**: For the user interface (menus, pause screen, etc.).
*   **stb_image**: For loading textures (card faces, backgrounds).

## Features

*   **Spider Solitaire Logic**: Full implementation of Spider Solitaire rules.
*   **Difficulty Levels**:
* * Easy: 1 suit
*  * Normal: 2 suits
*  * Hard: 4 suits
*   **Interactive Gameplay**: Smooth card dragging and placement logic.
*   **UI System**: Functional main menu, pause menu, and "You Win" screens.

## To-Do

Future improvements for this project:

- [ ] Further improve animations
- [ ] Undo/Redo system
- [ ] Sound effects for card flips, dealing, etc.
- [ ] Score system + high scores
- [ ] Add more card backs and background options
- [ ] Make more responsive to window/monitor sizes

## Building the project

1.  Ensure you have a C++17 compatible compiler.
2.  Configure the project:
    ```bash
    cmake -B build
    ```
3.  Build the executable:
    ```bash
    cmake --build build --target learnopengl
    ```

*Note: You may need to adjust the `GLFW_ROOT` path in `CMakeLists.txt` to match your local environment.*

***

Card textures from [Sylly](https://opengameart.org/users/sylly) under CC0.
