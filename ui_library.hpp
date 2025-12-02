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
        DrawText(text.c_str(), bounds.x + 7, bounds.y + (bounds.height/2) - 7, 32, textColor);

        return result;
    }
};