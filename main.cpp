/**
* Author: Ibrahim Mira
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
              SLIDER_SPEED  = 500,
              INIT_BALL_SPEED = 500;

constexpr char    BG_COLOUR[]    = "#000000ff";
constexpr Vector2 ORIGIN         = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  SLIDER_SIZE      = { (float) 70 / 2, (float) 301 / 2 },
                  SLIDER1_INIT_POS  = { ORIGIN.x - 680, ORIGIN.y },
                  SLIDER2_INIT_POS  = { ORIGIN.x + 680, ORIGIN.y },

                  BLACKHOLE_SIZE = { (float) 400, (float) 360 },
                  BLACKHOLE_POS = { ORIGIN.x, ORIGIN.y },

                  GAMEOVER_SIZE = {(float) 640, (float) 480},

                  BACKGROUND_SIZE = {SCREEN_WIDTH, SCREEN_HEIGHT},
                  BACKGROUND_POS = { ORIGIN.x, ORIGIN.y };
                  
constexpr char SLIDER[] = "assets/game/slider.png";
constexpr char DASHED_LINE[] = "assets/game/blackhole.png";
constexpr char BALL[] = "assets/game/earth_ball.png";
constexpr char GAMEOVER[] = "assets/game/gameover.png";
constexpr char BACKGROUND[] = "assets/game/background.jpg";


// Global Variables
AppStatus gAppStatus     = RUNNING;
bool      gGameStarted   = false,
          gPlayerOneBall = false,
          gPlayerTwoBall = false,
          gGameOver      = false;

enum gGAMEMODE  { TWO_PLAYER, SINGLE_PLAYER };

gGAMEMODE gCurrMode = TWO_PLAYER;

float     gAngle         = 0.0f,
          gPreviousTicks = 0.0f,
          gSpawnOffsets[3] = { -40.0f, 0.0f, 40.0f };

Vector2 gSlider1Position  = SLIDER1_INIT_POS,
        gSlider2Position  = SLIDER2_INIT_POS,
        gSlider1Movement  = { 0.0f, 0.0f },
        gSlider2Movement  = { 0.0f, 0.0f },
        gSliderScale     = SLIDER_SIZE,

        gBlackholePosition = BLACKHOLE_POS,
        gBlackholeScale    = BLACKHOLE_SIZE,
        
        gGameOverPosition = ORIGIN,
        gGameOverScale = GAMEOVER_SIZE,

        gBackgroundPosition = BACKGROUND_POS,
        gBackgroundScale    = BACKGROUND_SIZE;

float   gWallSliderBound = gSliderScale.y / 2.0f;

Texture2D gBACKGROUND;
Texture2D gSLIDER1;
Texture2D gSLIDER2;
Texture2D gDASHED_LINE;
Texture2D gGameOverSign;

unsigned int startTime;

int gNextSpawn = 0,
    gNumActiveBalls = 0;

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
void updateSliders(float deltaTime);
void updateBall(float deltaTime);

// Struct 
struct Ball

{
    /* data */

    Vector2     BALL_SIZE       = { (float) 256 / 7, (float) 256 / 7 },
                BALL_INIT_POS   = { ORIGIN.x, ORIGIN.y },
                BallPosition    = BALL_INIT_POS,
                BallMovement    = { 0.0f, 0.0f },
                BallScale       = BALL_SIZE;

    float       mWallBallBound   = BallScale.y / 2.0f;
    
    Texture2D mBALL;

    enum BallStatus { ACTIVE, INACTIVE };

    BallStatus mBallStatus = INACTIVE;

    void activate()   { 
        mBallStatus  = ACTIVE;  
    }

    void deactivate() { 
        mBallStatus  = INACTIVE; 

        BallPosition = BALL_INIT_POS;
        BallPosition.y += gSpawnOffsets[gNextSpawn];
        BallMovement = { 0.0f, 0.0f };

        gNextSpawn = (gNextSpawn + 1) % 3;
    }
    bool isActive() {
        if (mBallStatus == ACTIVE) return true;
        return false;
    }

    void loadBall() {
        mBALL = LoadTexture(BALL); 
    }

    void unloadBall() {
        UnloadTexture(mBALL); 
    }

    void renderBall() {
        if (mBallStatus == ACTIVE) {
            renderObject(&mBALL, &BallPosition, &BallScale);
        }
    }

};

Ball gBall1;
Ball gBall2;
Ball gBall3;

// Use an array to store multiple balls
Ball gBalls[3] = { gBall1, gBall2, gBall3 };

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "User Input / Collision Detection");

    startTime = time(NULL);

    gBACKGROUND     = LoadTexture(BACKGROUND);
    gSLIDER1        = LoadTexture(SLIDER);
    gSLIDER2        = LoadTexture(SLIDER);
    gDASHED_LINE    = LoadTexture(DASHED_LINE);
    gGameOverSign   = LoadTexture(GAMEOVER);

    for (int x = 0; x < 3; x++ ) {
        gBalls[x].loadBall();
    }

    SetTargetFPS(FPS);
}

void processInput() 
{

    if (!gGameOver) {

        if (IsKeyPressed(KEY_T)) ToggleGameMode();

        if (IsKeyPressed(KEY_ZERO)) {
            gNumActiveBalls = 0;
        }

        else if (IsKeyPressed(KEY_ONE)) {
            gNumActiveBalls = 1;
        }

        else if (IsKeyPressed(KEY_TWO)) {
            gNumActiveBalls = 2;
        }

        else if (IsKeyPressed(KEY_THREE)) {
            gNumActiveBalls = 3;
        }

        if (gCurrMode == SINGLE_PLAYER) {

            gSlider2Movement = { 0.0f, 0.0f };

            if      (IsKeyDown(KEY_UP)) gSlider2Movement.y = -1;
            else if (IsKeyDown(KEY_DOWN)) gSlider2Movement.y =  1;  
            
        }

        else {

            gSlider1Movement = { 0.0f, 0.0f };
            gSlider2Movement = { 0.0f, 0.0f };

            if      (IsKeyDown(KEY_W)) gSlider1Movement.y = -1;
            else if (IsKeyDown(KEY_S)) gSlider1Movement.y =  1;

            if     (IsKeyDown(KEY_UP)) gSlider2Movement.y = -1;
            else if (IsKeyDown(KEY_DOWN)) gSlider2Movement.y =  1;
        }
    }

    if (gGameOver && IsKeyPressed(KEY_SPACE)) gGameOver = false;

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    if (!gGameOver) {

        // Delta time
        float ticks = (float) GetTime();
        float deltaTime = ticks - gPreviousTicks;
        gPreviousTicks  = ticks;
        
        
        updateSliders(deltaTime);
        updateBall(deltaTime);

        if (gGameOver) {
            gSlider1Position  = SLIDER1_INIT_POS;
            gSlider2Position  = SLIDER2_INIT_POS;
        }

    }   

}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    
    if (gGameOver) renderObject(&gGameOverSign, &gGameOverPosition ,&gGameOverScale);

    else {


        renderObject(&gBACKGROUND, &gBackgroundPosition, &gBackgroundScale);

        renderObject(&gSLIDER1, &gSlider1Position, &gSliderScale);

        renderObject(&gSLIDER1, &gSlider2Position, &gSliderScale);

        renderObject(&gDASHED_LINE, &gBlackholePosition, &gBlackholeScale);

        for (int x = 0; x < 3; x++) {
            gBalls[x].renderBall();
    }
    }

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); 
    UnloadTexture(gSLIDER1);
    UnloadTexture(gSLIDER2);
    UnloadTexture(gDASHED_LINE);
    UnloadTexture(gGameOverSign);
    UnloadTexture(gBACKGROUND);

    for (int x = 0; x < 3; x++ ) {
        gBalls[x].unloadBall();
    }

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

void ToggleGameMode() {

    if (gCurrMode == SINGLE_PLAYER) gCurrMode = TWO_PLAYER;
    
    else  {
        gCurrMode = SINGLE_PLAYER;
        gSlider1Movement.y = -1;
    }
}

void updateBall(float deltaTime) {

    for (int x = 0; x < 3; x++) {
        if ( x < gNumActiveBalls) {
            gBalls[x].activate();
        } 
        else  gBalls[x].deactivate();
    }

    for (int x = 0; x < 3; x++) {

        if (gBalls[x].isActive()){

            if (gBalls[x].BallMovement.x == 0.0f && gBalls[x].BallMovement.y == 0.0f) {

                gBalls[x].BallMovement = { 1.0f, -0.005f };
                Normalise(&gBalls[x].BallMovement);
                gGameStarted = true;
                
            }
            gBalls[x].BallPosition = {
                gBalls[x].BallPosition.x + INIT_BALL_SPEED * gBalls[x].BallMovement.x * deltaTime,
                gBalls[x].BallPosition.y + INIT_BALL_SPEED * gBalls[x].BallMovement.y * deltaTime
            };

            if (gBalls[x].BallPosition.y < gBalls[x].mWallBallBound) {
                gBalls[x].BallPosition.y = gBalls[x].mWallBallBound;
                gBalls[x].BallMovement.y *= -1;
                Normalise(&gBalls[x].BallMovement);
            }
            else if (gBalls[x].BallPosition.y > SCREEN_HEIGHT - gBalls[x].mWallBallBound) {
                gBalls[x].BallPosition.y = SCREEN_HEIGHT - gBalls[x].mWallBallBound;
                gBalls[x].BallMovement.y *= -1;
                Normalise(&gBalls[x].BallMovement);
            }

            if (gBalls[x].BallPosition.x < 0.0f) {
                gGameStarted = false;
                gBalls[x].BallPosition = gBalls[x].BALL_INIT_POS;
                gBalls[x].BallMovement.x = 0.0f;
                gBalls[x].BallMovement.y = 0.0f;
                
                gPlayerTwoBall = true;

                gBalls[x].deactivate();
                gNumActiveBalls -= 1;
                
                // Ensures that if a balls goes off-screen, it does NOT get reactivated
                int lastActive = gNumActiveBalls;          
                if (x < lastActive) {
                std::swap(gBalls[x], gBalls[lastActive]);}

                if (gNumActiveBalls == 0) gGameOver = true;
            }

            if (gBalls[x].BallPosition.x > SCREEN_WIDTH) {
                gGameStarted = false;
                gBalls[x].BallPosition = gBalls[x].BALL_INIT_POS;
                gBalls[x].BallMovement.x = 0.0f;
                gBalls[x].BallMovement.y = 0.0f;

                gPlayerOneBall = true;

                gBalls[x].deactivate();
                gNumActiveBalls -= 1;

                // Ensures that if a balls goes off-screen, it does NOT get reactivated
                int lastActive = gNumActiveBalls;          
                if (x < lastActive) {
                std::swap(gBalls[x], gBalls[lastActive]); }

                if (gNumActiveBalls == 0) gGameOver = true;
            }
            
            // Offset is to help determine which direction (y) the ball bounces to.
            float offset = 0.0f;
            if (isColliding(&gBalls[x].BallPosition, &gBalls[x].BallScale, &gSlider1Position, &gSliderScale)) {
                offset = (gBalls[x].BallPosition.y - gSlider1Position.y) / (gSliderScale.y / 2.0f);

                gBalls[x].BallPosition.x = gSlider1Position.x + (gSliderScale.x / 2.0f) + (gBalls[x].BallScale.x / 2.0f);
                gBalls[x].BallMovement.x *= -1;
                gBalls[x].BallMovement.y = offset;
                Normalise(&gBalls[x].BallMovement);

            }
            else if (isColliding(&gBalls[x].BallPosition, &gBalls[x].BallScale, &gSlider2Position, &gSliderScale)) {
                offset = (gBalls[x].BallPosition.y - gSlider2Position.y) / (gSliderScale.y / 2.0f);

                gBalls[x].BallPosition.x = gSlider2Position.x - (gSliderScale.x / 2.0f) - (gBalls[x].BallScale.x / 2.0f);
                gBalls[x].BallMovement.x *= -1;
                gBalls[x].BallMovement.y = offset;
                Normalise(&gBalls[x].BallMovement);
            }
        }
        
    }
    
}

void updateSliders(float deltaTime) {

    if (gCurrMode == SINGLE_PLAYER && !gGameOver) {

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
}
