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
#include <iostream>

// Global Constants
constexpr int SCREEN_WIDTH  = 1600 / 1.1,
              SCREEN_HEIGHT = 900 / 1.1,
              FPS           = 60,
              // SIZE          = 500 / 2,
              SLIDER_SPEED  = 500,
              INIT_BALL_SPEED = 500;

constexpr char    BG_COLOUR[]    = "#000000ff";
constexpr Vector2 ORIGIN         = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  SLIDER_SIZE      = { (float) 382 / 30, (float) 1557 / 10 },
                  SLIDER1_INIT_POS  = { ORIGIN.x - 680, ORIGIN.y },
                  SLIDER2_INIT_POS  = { ORIGIN.x + 680, ORIGIN.y },
                  DASHED_LINE_SIZE = { (float) 63, (float) 942 },
                  DASHED_LINE_POS = { ORIGIN.x, ORIGIN.y - 20 },
                  BALL_SIZE       = { (float) 198 / 8, (float) 198 / 8 },
                  BALL_INIT_POS   = { ORIGIN.x, ORIGIN.y };
                  
constexpr char SLIDER[] = "assets/game/slider.png";
constexpr char DASHED_LINE[] = "assets/game/dashed_line.png";
constexpr char BALL[] = "assets/game/ball.png";


// Global Variables
AppStatus gAppStatus     = RUNNING;
bool      gGameStarted   = false,
          gPlayerOneBall = false,
          gPlayerTwoBall = false;

enum gGAMEMODE  { TWO_PLAYER, SINGLE_PLAYER };

gGAMEMODE currMODE = TWO_PLAYER;

float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f;

Vector2 gSlider1Position  = SLIDER1_INIT_POS,
        gSlider2Position  = SLIDER2_INIT_POS,
        gSlider1Movement  = { 0.0f, 0.0f },
        gSlider2Movement  = { 0.0f, 0.0f },
        gSliderScale     = SLIDER_SIZE,

        gBallPosition    = BALL_INIT_POS,
        gBallMovement    = { 0.0f, 0.0f },
        gBallScale       = BALL_SIZE,

        gDashedLinePosition = DASHED_LINE_POS,
        gDashedLineScale    = DASHED_LINE_SIZE,

        gMousePosition = GetMousePosition();

Texture2D gSLIDER1;
Texture2D gSLIDER2;
Texture2D gDASHED_LINE;
Texture2D gBALL;

unsigned int startTime;

float gWallSliderBound = gSliderScale.y / 2.0f,
      gWallBallBound   = gBallScale.y / 2.0f;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);
void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale);
void ToggleGameMode();
void updateGAME();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "User Input / Collision Detection");

    startTime = time(NULL);

    gSLIDER1        = LoadTexture(SLIDER);
    gSLIDER2        = LoadTexture(SLIDER);
    gDASHED_LINE    = LoadTexture(DASHED_LINE);
    gBALL           = LoadTexture(BALL);

    SetTargetFPS(FPS);
}

void processInput() 
{

    if (IsKeyPressed(KEY_T)) ToggleGameMode();

    if (currMODE == SINGLE_PLAYER) {

        // gSlider1Movement = { 0.0f, 0.0f };
        gSlider2Movement = { 0.0f, 0.0f };

        if      (IsKeyDown(KEY_UP)) gSlider2Movement.y = -1;
        else if (IsKeyDown(KEY_DOWN)) gSlider2Movement.y =  1;  
        
        // gSlider1Movement.y = -1;
    }

    else {

        gSlider1Movement = { 0.0f, 0.0f };
        gSlider2Movement = { 0.0f, 0.0f };

        if      (IsKeyDown(KEY_W)) gSlider1Movement.y = -1;
        else if (IsKeyDown(KEY_S)) gSlider1Movement.y =  1;

        if     (IsKeyDown(KEY_UP)) gSlider2Movement.y = -1;
        else if (IsKeyDown(KEY_DOWN)) gSlider2Movement.y =  1;
    }
    

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
    // // Delta time
    // float ticks = (float) GetTime();
    // float deltaTime = ticks - gPreviousTicks;
    // gPreviousTicks  = ticks;
    
    // gSlider1Position = {
    // gSlider1Position.x + SLIDER_SPEED * gSlider1Movement.x * deltaTime,
    // gSlider1Position.y + SLIDER_SPEED * gSlider1Movement.y * deltaTime
    // };

    // gSlider2Position = {
    // gSlider2Position.x + SLIDER_SPEED * gSlider2Movement.x * deltaTime,
    // gSlider2Position.y + SLIDER_SPEED * gSlider2Movement.y * deltaTime
    // };

    // if (gSlider1Position.y < gWallSliderBound) {
    //     gSlider1Position.y = gWallSliderBound;
    // }
    // else if (gSlider1Position.y > SCREEN_HEIGHT - gWallSliderBound) {
    //     gSlider1Position.y = SCREEN_HEIGHT - gWallSliderBound;
    // }

    // if (gSlider2Position.y < gWallSliderBound) {
    //     gSlider2Position.y = gWallSliderBound;
    // }
    // else if (gSlider2Position.y > SCREEN_HEIGHT - gWallSliderBound) {
    //     gSlider2Position.y = SCREEN_HEIGHT - gWallSliderBound;
    // }

    // if (gBallMovement.x == 0.0f && gBallMovement.y == 0.0f) {
    //     if (gPlayerOneBall) {
    //         gBallMovement = { -1.0f, -0.005f };
    //         gPlayerOneBall = false;
    //     }
    //     else if (gPlayerTwoBall) {
    //         gBallMovement = { 1.0f, -0.005f };
    //         gPlayerTwoBall = false;
    //     }
    //     else {
    //         gBallMovement = { 1.0f, -0.005f };
    //     }
        
    //     Normalise(&gBallMovement);
    //     gGameStarted = true;
    // }
    // gBallPosition = {
    //     gBallPosition.x + INIT_BALL_SPEED * gBallMovement.x * deltaTime,
    //     gBallPosition.y + INIT_BALL_SPEED * gBallMovement.y * deltaTime
    // };

    // if (gBallPosition.y < gWallBallBound) {
    //     gBallPosition.y = gWallBallBound;
    //     gBallMovement.y *= -1;
    //     Normalise(&gBallMovement);
    // }
    // else if (gBallPosition.y > SCREEN_HEIGHT - gWallBallBound) {
    //     gBallPosition.y = SCREEN_HEIGHT - gWallBallBound;
    //     gBallMovement.y *= -1;
    //     Normalise(&gBallMovement);
    // }

    // if (gBallPosition.x < 0.0f) {
    //     gGameStarted = false;
    //     gBallPosition = BALL_INIT_POS;
    //     gBallMovement.x = 0.0f;
    //     gBallMovement.y = 0.0f;
    //     gPlayerTwoBall = true;
    // }

    // if (gBallPosition.x > SCREEN_WIDTH) {
    //     gGameStarted = false;
    //     gBallPosition = BALL_INIT_POS;
    //     gBallMovement.x = 0.0f;
    //     gBallMovement.y = 0.0f;
    //     gPlayerOneBall = true;
    // }

    // float offset = 0.0f;
    // if (isColliding(&gBallPosition, &gBallScale, &gSlider1Position, &gSliderScale)) {
    //     offset = (gBallPosition.y - gSlider1Position.y) / (gSliderScale.y / 2.0f);

    //     gBallPosition.x = gSlider1Position.x + (gSliderScale.x / 2.0f) + (gBallScale.x / 2.0f);
    //     gBallMovement.x *= -1;
    //     gBallMovement.y = offset;
    //     Normalise(&gBallMovement);

    // }
    // else if (isColliding(&gBallPosition, &gBallScale, &gSlider2Position, &gSliderScale)) {
    //     offset = (gBallPosition.y - gSlider2Position.y) / (gSliderScale.y / 2.0f);

    //     gBallPosition.x = gSlider2Position.x - (gSliderScale.x / 2.0f) - (gBallScale.x / 2.0f);
    //     gBallMovement.x *= -1;
    //     gBallMovement.y = offset;
    //     Normalise(&gBallMovement);
    // }

    updateGAME();

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
    // Render BALL
    renderObject(&gBALL, &gBallPosition, &gBallScale);

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); 
    UnloadTexture(gSLIDER1);
    UnloadTexture(gSLIDER2);
    UnloadTexture(gDASHED_LINE);
    UnloadTexture(gBALL);

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

void updateGAME() {

    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    std:: cout << currMODE << std::endl;
    
    if (currMODE == SINGLE_PLAYER) {

        gSlider2Position = {
            gSlider2Position.x + SLIDER_SPEED * gSlider2Movement.x * deltaTime,
            gSlider2Position.y + SLIDER_SPEED * gSlider2Movement.y * deltaTime
        };

        gSlider1Position = {
            gSlider1Position.x + SLIDER_SPEED * gSlider1Movement.x * deltaTime,
            gSlider1Position.y + SLIDER_SPEED * gSlider1Movement.y * deltaTime
        };

        if (gSlider1Position.y < gWallSliderBound) {
            gSlider1Movement.y = 1;
        }
        else if (gSlider1Position.y > SCREEN_HEIGHT - gWallSliderBound) {
            gSlider1Movement.y = -1;
        }
    }

    else {

        gSlider1Position = {
            gSlider1Position.x + SLIDER_SPEED * gSlider1Movement.x * deltaTime,
            gSlider1Position.y + SLIDER_SPEED * gSlider1Movement.y * deltaTime
        };

        gSlider2Position = {
            gSlider2Position.x + SLIDER_SPEED * gSlider2Movement.x * deltaTime,
            gSlider2Position.y + SLIDER_SPEED * gSlider2Movement.y * deltaTime
        };
    }

    if (gSlider1Position.y < gWallSliderBound) {
        gSlider1Position.y = gWallSliderBound;
    }
    else if (gSlider1Position.y > SCREEN_HEIGHT - gWallSliderBound) {
        gSlider1Position.y = SCREEN_HEIGHT - gWallSliderBound;
    }

    if (gSlider2Position.y < gWallSliderBound) {
        gSlider2Position.y = gWallSliderBound;
    }
    else if (gSlider2Position.y > SCREEN_HEIGHT - gWallSliderBound) {
        gSlider2Position.y = SCREEN_HEIGHT - gWallSliderBound;
    }

    if (gBallMovement.x == 0.0f && gBallMovement.y == 0.0f) {
        if (gPlayerOneBall) {
            gBallMovement = { -1.0f, -0.005f };
            gPlayerOneBall = false;
        }
        else if (gPlayerTwoBall) {
            gBallMovement = { 1.0f, -0.005f };
            gPlayerTwoBall = false;
        }
        else {
            gBallMovement = { 1.0f, -0.005f };
        }
        
        Normalise(&gBallMovement);
        gGameStarted = true;
    }
    gBallPosition = {
        gBallPosition.x + INIT_BALL_SPEED * gBallMovement.x * deltaTime,
        gBallPosition.y + INIT_BALL_SPEED * gBallMovement.y * deltaTime
    };

    if (gBallPosition.y < gWallBallBound) {
        gBallPosition.y = gWallBallBound;
        gBallMovement.y *= -1;
        Normalise(&gBallMovement);
    }
    else if (gBallPosition.y > SCREEN_HEIGHT - gWallBallBound) {
        gBallPosition.y = SCREEN_HEIGHT - gWallBallBound;
        gBallMovement.y *= -1;
        Normalise(&gBallMovement);
    }

    if (gBallPosition.x < 0.0f) {
        gGameStarted = false;
        gBallPosition = BALL_INIT_POS;
        gBallMovement.x = 0.0f;
        gBallMovement.y = 0.0f;
        gPlayerTwoBall = true;
    }

    if (gBallPosition.x > SCREEN_WIDTH) {
        gGameStarted = false;
        gBallPosition = BALL_INIT_POS;
        gBallMovement.x = 0.0f;
        gBallMovement.y = 0.0f;
        gPlayerOneBall = true;
    }

    float offset = 0.0f;
    if (isColliding(&gBallPosition, &gBallScale, &gSlider1Position, &gSliderScale)) {
        offset = (gBallPosition.y - gSlider1Position.y) / (gSliderScale.y / 2.0f);

        gBallPosition.x = gSlider1Position.x + (gSliderScale.x / 2.0f) + (gBallScale.x / 2.0f);
        gBallMovement.x *= -1;
        gBallMovement.y = offset;
        Normalise(&gBallMovement);

    }
    else if (isColliding(&gBallPosition, &gBallScale, &gSlider2Position, &gSliderScale)) {
        offset = (gBallPosition.y - gSlider2Position.y) / (gSliderScale.y / 2.0f);

        gBallPosition.x = gSlider2Position.x - (gSliderScale.x / 2.0f) - (gBallScale.x / 2.0f);
        gBallMovement.x *= -1;
        gBallMovement.y = offset;
        Normalise(&gBallMovement);
    }

}

void ToggleGameMode() {

    if (currMODE == SINGLE_PLAYER) currMODE = TWO_PLAYER;
    
    else  {

        currMODE = SINGLE_PLAYER;
        gSlider1Movement.y = -1;
        
    }
}