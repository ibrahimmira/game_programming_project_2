/**
* Author: [Your name here]
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1600 / 1.1,
              SCREEN_HEIGHT = 900 / 1.1,
              FPS           = 60,
              SIZE          = 500 / 2,
              SLIDER_SPEED  = 500;

constexpr char    BG_COLOUR[]    = "#000000ff";
constexpr Vector2 ORIGIN         = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  SLIDER_SIZE      = { (float) SIZE / 4, (float) SIZE / 2  },
                  SLIDER1_INIT_POS  = { ORIGIN.x - 680, ORIGIN.y },
                  SLIDER2_INIT_POS  = { ORIGIN.x + 680, ORIGIN.y },
                  DASHED_LINE_SIZE = { (float) 63, (float) 942 },
                  DASHED_LINE_POS = { ORIGIN.x, ORIGIN.y - 20 };
                  
constexpr char SLIDER[] = "assets/game/slider.png";
constexpr char DASHED_LINE[] = "assets/game/dashed_line.png";


// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;

Vector2 gSlider1Position  = SLIDER1_INIT_POS,
        gSlider2Position  = SLIDER2_INIT_POS,
        gSlider1Movement  = { 0.0f, 0.0f },
        gSlider2Movement  = { 0.0f, 0.0f },
        gSliderScale     = SLIDER_SIZE,

        gDashedLinePosition = DASHED_LINE_POS,
        gDashedLineScale    = DASHED_LINE_SIZE,

        gMousePosition = GetMousePosition();

Texture2D gSLIDER1;
Texture2D gSLIDER2;
Texture2D gDASHED_LINE;

unsigned int startTime;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);
void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale);

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "User Input / Collision Detection");

    startTime = time(NULL);

    gSLIDER1  = LoadTexture(SLIDER);
    gSLIDER2  = LoadTexture(SLIDER);
    gDASHED_LINE = LoadTexture(DASHED_LINE);

    SetTargetFPS(FPS);
}

void processInput() 
{
    gSlider1Movement = { 0.0f, 0.0f };
    gSlider2Movement = { 0.0f, 0.0f };

    if      (IsKeyDown(KEY_W)) gSlider1Movement.y = -1;
    else if (IsKeyDown(KEY_S)) gSlider1Movement.y =  1;

    if     (IsKeyDown(KEY_UP)) gSlider2Movement.y = -1;
    else if (IsKeyDown(KEY_DOWN)) gSlider2Movement.y =  1;

    /*
    This system will cause quite a bit of "shaking" once the game object
    reaches the mouse position. Ideally, we'd stop checking for this
    once the object reaches a general area AROUND the mouse position.
    */
   
    // // to avoid faster diagonal speed
    // if (GetLength(&gSlider1Movement) > 1.0f) Normalise(&gSlider1Movement);

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;
    
    gSlider1Position = {
    gSlider1Position.x + SLIDER_SPEED * gSlider1Movement.x * deltaTime,
    gSlider1Position.y + SLIDER_SPEED * gSlider1Movement.y * deltaTime
    };

    gSlider2Position = {
    gSlider2Position.x + SLIDER_SPEED * gSlider2Movement.x * deltaTime,
    gSlider2Position.y + SLIDER_SPEED * gSlider2Movement.y * deltaTime
    };
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    // Render SLIDER1
    renderObject(&gSLIDER1, &gSlider1Position, &gSliderScale);
    // Render SLIDER2
    renderObject(&gSLIDER1, &gSlider2Position, &gSliderScale);
    // Render DASHED LINE
    renderObject(&gDASHED_LINE, &gDashedLinePosition, &gDashedLineScale);

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); 
    UnloadTexture(gSLIDER1);
    UnloadTexture(gSLIDER2);
    UnloadTexture(gDASHED_LINE);

}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}


// Function Definitions

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 * 
 * @see 
 * 
 * @param positionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *positionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        gAngle, WHITE
    );
}