#include <raylib.h>
#include <raymath.h>

#include <functional>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Struct to encapsulate our UI library
struct UiLibrary
{
    // ID of the currently hot widget
    int hot = -1;

    // ID of the currently active widget
    int active = -1;

    // Creates a button with the specified text and bounds
    // Returns true if this button was clicked in this frame
    bool Button(int id, const string& text, const Rectangle& bounds)
    {
        bool result = false;

        // If this button is the currently active widget, that means
        // the user is currently interacting with this widget
        if (id == active)
        {
            // If the user released the mouse button while we are active,
            // register as a click
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                // Make sure that we are still the hot widget
                // (User might have dragged mouse to another widget, in which case we will not count as a button click)
                if (id == hot)
                {
                    result = true;
                }

                // Set ourselves to not be active anymore
                active = -1;
            }
        }


        Color buttonColor = GRAY;
        Color textColor = BLACK;
        // If we are currently the hot widget
        if (id == hot)
        {
            buttonColor = LIGHTGRAY;
            // If the user pressed the left mouse button, that means the user started
            // interacting with this widget, so we set this widget as active
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                active = id;                

            }

        }

        // If the mouse cursor is hovering within our boundaries
        if (CheckCollisionPointRec(GetMousePosition(), bounds))
        {
            // Set this widget to be the hot widget
            hot = id;

            // If the left mouse button is held down, color and text change
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                // Button held down color
                buttonColor = RED;
                textColor = WHITE;
                active = id;
            }
            else
            {
                // Releasing reverts back to original
                buttonColor = LIGHTGRAY;
                textColor = BLACK;
            }

        }
        // If the mouse cursor is not on top of this widget, and this widget
        // was previously the hot widget, set the hot widget to -1
        // We check if this widget was the previously hot widget since there is a possibility that
        // the hot widget is now a different widget, and we don't want to overwrite that with -1
        else if (hot == id)
        {
            hot = -1;
        }

        // Draw our button regardless of what happens
        DrawRectangleRec(bounds, buttonColor);
        DrawText(text.c_str(), bounds.x + 7, bounds.y + (bounds.height/2) - 7, 14, textColor);

        return result;
    }

    bool CheckBox(int id, const string& text, Vector2 position, bool& state){

        //Checkbox Size
        float boxSize = 20.0f; 
        Rectangle boxBounds = { position.x, position.y, boxSize, boxSize };

        // Check hover
        bool hovered = CheckCollisionPointRec(GetMousePosition(), boxBounds);

        // Toggle state when clicked making it true and false and vice versa
        if (hovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            state = !state; // toggle
        }

        // Draw checkbox 
        DrawRectangleLinesEx(boxBounds, 2, BLACK);

        // Fill if checked
        if (state)
        {
            DrawRectangle(boxBounds.x + 4, boxBounds.y + 4, boxSize - 8, boxSize - 8, GREEN);
        }

        // Change color when hovered
        if (hovered)
        {
            DrawRectangleLinesEx(boxBounds, 2, LIGHTGRAY);
        }

        // Draw text
        DrawText(text.c_str(), position.x + boxSize + 10, position.y + 2, 16, BLACK);

        return state;


    }

};

int main()
{
    
    int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Immediate Mode");
    SetTargetFPS(60);

    UiLibrary uiLibrary;

    //initial setting of the autosave state
    bool lockScreenSize = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);
        if (uiLibrary.Button(0, "800x600", { 10, 10, 80, 40 }))
        {
            if (!lockScreenSize) {
                SetWindowSize(800, 600);
            }
        }
        if (uiLibrary.Button(1, "1280x720", { 100, 10, 80, 40 }))
        {
            if (!lockScreenSize) {
                SetWindowSize(1280, 720);
            }
        }
        if (uiLibrary.Button(2, "1366x768", { 190, 10, 80, 40 }))
        {
            if (!lockScreenSize) {
                SetWindowSize(1366, 768);
            }
        }

        //drawing checkbox
        uiLibrary.CheckBox(2, "Lock screen size", {10, 70}, lockScreenSize);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
