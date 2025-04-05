#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <unistd.h>

#include <raylib.h>

void game_init();
void game_setup();
void game_update();
void game_draw();
void game_close();

const char *title = "Conway's Game of Life";
#define WIN_WIDTH   1200
#define WIN_HEIGHT  800
int FPS_TARGET  =   60;

#define CELL_SIZE   10
#define BOARD_COLS  WIN_WIDTH / CELL_SIZE       // 60 
#define BOARD_ROWS  WIN_HEIGHT / CELL_SIZE      // 40

bool board_old[BOARD_COLS * BOARD_ROWS] = {};
bool board_new[BOARD_COLS * BOARD_ROWS] = {};

#define BACKGROUND_COLOR (Color){.r = 25, .g = 25, .b = 25, .a = 255}
#define CELL_OFFSET 2
#define CELL_COLOR (Color){.r = 65, .g = 65, .b = 65, .a = 255}

bool is_running = true;
bool is_fps_visible = false;

int calculate_nearby_alive_cells(int row, int col);


int main(void)
{
    game_init();
    game_setup();
    while (!WindowShouldClose()) 
    {
        game_update();
        if(is_running)
        {
            BeginDrawing();
            game_draw();
            EndDrawing();
        }
    }
    game_close();
    return 0;
}

void game_init()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(WIN_WIDTH, WIN_HEIGHT, title);
    // SetTargetFPS(FPS_TARGET);
}
void game_close()
{
    CloseWindow();
}

void game_setup()
{
    srand((uint32_t)time(NULL));
    // Generate same random cells on all two boards
    int cell_count = 200;
    for (int i = 0; i < BOARD_COLS * BOARD_ROWS; i++) 
    {
        cell_count--;
        board_old[i] = (bool)(rand() % 2) ? true : false;
        board_new[i] = board_old[i];
        if(cell_count <= 0) break;
    }
}
void game_update()
{
    if(IsKeyPressed(KEY_R))
    {
        game_setup();
    }  
    else if (IsKeyPressed(KEY_F))
    {
        // toggle show FPS
        is_fps_visible = !is_fps_visible;
    }  
    else if (IsKeyPressed(KEY_SPACE))
    {
        // toggle the pause of the game
        // is_running = !is_running;
        if(is_running == true) is_running = false;
        else if(is_running == false) is_running = true;
        if(!is_running) printf("%s\n", (is_running) ? "PAUSE TRUE" : "PAUSE FALSE");
    }  
    else if (IsKeyPressed(KEY_UP))
    {
        // Increase speed
        if(FPS_TARGET < 120)
        {
            FPS_TARGET +=10;
            SetTargetFPS(FPS_TARGET);
        }
    }  
    else if (IsKeyPressed(KEY_DOWN))
    {
        // decrease speed
        if(FPS_TARGET > 10)
        {
            FPS_TARGET -=10;
            SetTargetFPS(FPS_TARGET);
        }
    }  
    // update board with rules of the game
    else if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        // printf("X : %i\tY : %i\n", GetMouseX(), GetMouseY());
        // Divide mouse position by cell size to get the index
        int col = GetMouseX() / CELL_SIZE;
        int row = GetMouseY() / CELL_SIZE;
        // translate x y coordinate into monodimensional array
        board_new[BOARD_COLS * row + col] = !board_new[BOARD_COLS * row + col];
    } 
    // Apply rule of the game
    // rule 1   if the cell is alive and has  < 2 || > 3 nearby alive cells , cell dies
    // rule 2   if the cell is  dead and has    == 3     nearby alive cells , cell rebirth   
    // PROVARE A CICLARE PARTENDO DA ROW 1 e COL 1 FINO A ROWS -2 e COLS -2 praticamente 
    // per ignorare la cintura esterna 
    for (int row = 1; row < BOARD_ROWS - 2 ; row++) 
    {
        for (int col = 1; col < BOARD_COLS - 2; col++) 
        {
            // calculate index for monodimensional array
            int index = BOARD_COLS * row + col;
            // CALCULATE NEARBY ALIVE CELLS of board_old
            int nearby_alive_cells = calculate_nearby_alive_cells(row, col);
            // if current cell is alive and has < 2 or > 3 alive cells it dies
            if(board_old[index])
            {
                if(nearby_alive_cells < 2 || nearby_alive_cells > 3)
                {
                    board_old[index] = false;
                }
            }
            else if(!board_old[index] && nearby_alive_cells == 3)
            {
                board_old[index] = true;
            }
        }
    }
    // copy board_old in board_new
    for (int i = 0; i < BOARD_COLS * BOARD_ROWS; i++) 
    {
        board_new[i] = board_old[i];
    }
}
void game_draw()
{
    // clear screen
    ClearBackground(BACKGROUND_COLOR);
    // draw board_new
    int cell_rect_size = CELL_SIZE - CELL_OFFSET;
    for (int row = 0 ; row < BOARD_ROWS ; row++)
    {
        for (int col = 0; col < BOARD_COLS; col++) {
            if(board_new[BOARD_COLS * row + col])       
            {
                DrawRectangle(col * CELL_SIZE + CELL_OFFSET, 
                              row * CELL_SIZE + CELL_OFFSET, 
                             cell_rect_size, 
                            cell_rect_size, 
                             CELL_COLOR);
            }
        }
    }
    // Print FPS
    if(is_fps_visible)
    {
        DrawFPS(WIN_WIDTH * 0.8, WIN_HEIGHT * 0.1);
    }
}

int calculate_nearby_alive_cells(int row, int col)
{
    int alive_cells = 0;
    // left
    if(board_old[BOARD_COLS *  row + col - 1]) alive_cells++;
    // right
    if(board_old[BOARD_COLS *  row + col + 1]) alive_cells++;

    // top left
    if(board_old[BOARD_COLS * (row - 1 ) + col - 1]) alive_cells++;
    // top 
    if(board_old[BOARD_COLS * (row - 1 ) + col   ]) alive_cells++;
    // top right
    if(board_old[BOARD_COLS * (row - 1 ) + col + 1]) alive_cells++;

    // bottom left
    if(board_old[BOARD_COLS * (row + 1 ) + col - 1]) alive_cells++;
    // top 
    if(board_old[BOARD_COLS * (row + 1 ) + col   ]) alive_cells++;
    // top right
    if(board_old[BOARD_COLS * (row + 1 ) + col + 1]) alive_cells++;


    return alive_cells;
}