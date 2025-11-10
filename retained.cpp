#include <raylib.h>
#include <raymath.h>

#include <iostream>
#include <string>
#include <vector>
#include <functional>

using namespace std;

// Generic UI component
struct UIComponent
{
    // Rectangle reprsenting the bounds of the UI component
    Rectangle bounds;

    // Draws this particular UI component
    // Set as abstract so that child widgets will implement this for us
    virtual void Draw() = 0;

    // Handles a mouse click event
    // Set as abstract so that child widgets will implement this for us
    // Returns a boolean indicating whether this UI component successfully handled the event
    virtual bool HandleClick(Vector2 click_position) = 0;
};

// Generic UI component that can contain other UI components as children
struct UIContainer : public UIComponent
{
    vector<UIComponent*> children;

    // Adds a child to the container
    void AddChild(UIComponent* child)
    {
        children.push_back(child);
    }

    // Draw
    void Draw() override
    {
        // Since we are just a container for other widgets, we simply
        // call the draw function of other widgets.
        // This results in a pre-order traversal when we also draw child widgets that are also containers
        for (size_t i = 0; i < children.size(); ++i)
        {
            children[i]->Draw();
        }
    }

    // Handles a mouse click event
    // Returns a boolean indicating whether this UI component successfully handled the event
    bool HandleClick(Vector2 click_position) override
    {
        // Since we are just a container for other widgets, we call the HandleClick function of our child widgets
        // Since later children are drawn last, we do the opposite of draw where we start from the last child.
        // This results in a pre-order traversal but in the reverse order.
        for (size_t i = children.size(); i > 0; --i)
        {
            // If a child already handles the click event, we instantly return so no more child widgets handle the click
            if (children[i - 1]->HandleClick(click_position))
            {
                return true;
            }
        }

        return false;
    }
};

// Button widget
struct Button : public UIComponent
{
    // Text displayed by the button
    string text;

    vector<function<void()>> observers;

    void AddObserver(const function<void()>& callback) 
    {
        observers.push_back(callback);
    }

    // Draw
    void Draw() override
    {
        DrawRectangleRec(bounds, GRAY);
        DrawText(text.c_str(), bounds.x, bounds.y, 14, BLACK);
    }

    // Handle mouse click
    // Returns a boolean indicating whether this UI component successfully handled the event
    bool HandleClick(Vector2 click_position) override
    {
        // Check if the mouse click position is within our bounds
        if (CheckCollisionPointRec(click_position, bounds))
        {
            for (auto& callback : observers)
                callback();
            return true;
        }

        return false;
    }
};

// Text display widget
struct Label : public UIComponent
{
    // Text to be displayed
    string text;

    // Draw
    void Draw() override
    {
        DrawText(text.c_str(), bounds.x, bounds.y, 14, BLACK);
    }

    // Handle mouse click
    // Returns a boolean indicating whether this UI component successfully handled the event
    bool HandleClick(Vector2 click_position) override
    {
        // Always return false since we're not going to handle click events for this particular widget
        // (unless you have to)
        return false;
    }
};

struct CheckBox : public UIComponent
{
    string text;
    bool checked = false;
    vector<function<void(bool)>> observers;

    void AddObserver(const function<void(bool)>& cb)
    {
        observers.push_back(cb);
    }

    void Draw() override
    {
        float boxSize = 20.0f;
        Rectangle boxBounds = { bounds.x, bounds.y, boxSize, boxSize};

        DrawRectangleLinesEx(boxBounds, 2, BLACK);

        if (checked) 
        {
            DrawRectangle(boxBounds.x + 4, boxBounds.y + 4, boxSize - 8, boxSize - 8, GREEN);
        }

        DrawText(text.c_str(), bounds.x + boxSize + 10, bounds.y + 2, 16, BLACK);
    }

    bool HandleClick(Vector2 click_position) override
    {
        Rectangle boxBounds = { bounds.x, bounds.y, 20.0f, 20.0f};
        
        if (CheckCollisionPointRec(click_position, boxBounds))
        {
            checked = !checked;

            for (auto& cb : observers)
                cb(checked);
            
            return true;
        }
        return false;
    }

    void SetChecked(bool value)
    {
        checked = value;
        for (auto& cb : observers)
            cb(checked);
    }

    bool GetChecked() const {return checked;}
};

// Struct to encapsulate our UI library
struct UILibrary
{
    // Root container
    UIContainer root_container;

    // Updates the current UI state
    void Update()
    {
        // If the left mouse button was released, we handle the click from the root container
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            root_container.HandleClick(GetMousePosition());
        }
    }

    // Draw
    void Draw()
    {
        root_container.Draw();
    }
};

int main()
{
    int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Retained Mode");
    SetTargetFPS(60);

    UILibrary ui_library;
    ui_library.root_container.bounds = { 10, 10, 600, 500 };

    
    CheckBox checkbox;
    checkbox.text = "Lock screen size";
    checkbox.bounds = {10, 70, 20, 20};
    checkbox.SetChecked(false);
    ui_library.root_container.AddChild(&checkbox);

    Button button_s;
    button_s.text = "800x600";
    button_s.bounds = { 10, 100, 80, 40 };
    button_s.AddObserver([&checkbox](){ if (!checkbox.checked) {SetWindowSize(800, 600);} });
    ui_library.root_container.AddChild(&button_s);
    Button button_m;
    button_m.text = "1280x720";
    button_m.bounds = { 100, 100, 80, 40 };
    button_m.AddObserver([&checkbox](){ if (!checkbox.checked) {SetWindowSize(1280, 720);} });
    ui_library.root_container.AddChild(&button_m);
    Button button_l;
    button_l.text = "1366x768";
    button_l.bounds = { 190, 100, 80, 40 };
    button_l.AddObserver([&checkbox](){ if (!checkbox.checked) {SetWindowSize(1366, 768);} });
    ui_library.root_container.AddChild(&button_l);
    
    
    while (!WindowShouldClose())
    {
        ui_library.Update();

        BeginDrawing();
        ClearBackground(WHITE);
        ui_library.Draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
