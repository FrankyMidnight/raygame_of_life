#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <raylib.h>

void game_init();
void game_setup();
void read_input();
void game_draw();
void game_close();
void game_update();
const char *title = "Conway's Game of Life";

int WIN_WIDTH = 1400;
int WIN_HEIGHT = 700;
int DEFAULT_WIN_WIDTH = 1400;
int DEFAULT_WIN_HEIGHT = 700;
int FULLSCREEN_WIDTH = 1920;
int FULLSCREEN_HEIGHT = 1080;

int FPS_TARGET = 60;
double wait_time = 0.2;

int CELL_SIZE = 10;
int BOARD_COLS = 0;
int BOARD_ROWS = 0;

bool *board_old = NULL;
bool *board_new = NULL;

Color BACKGROUND_COLOR = {.r = 25, .g = 25, .b = 25, .a = 255};
#define CELL_OFFSET 2
Color CELL_COLOR = {.r = 65, .g = 65, .b = 65, .a = 255};

bool is_running = true;
bool is_fps_visible = false;
bool is_fullscreen;

int calculate_nearby_alive_cells(int row, int col);

int main(void)
{
    game_init();
    game_setup();
    while (!WindowShouldClose())
    {
        read_input();
        if (is_running)
        {
            game_update();
        }
        BeginDrawing();
        game_draw();
        EndDrawing();
    }
    game_close();
    return 0;
}

void game_init()
{

    // Init window with flags set
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(WIN_WIDTH, WIN_HEIGHT, title);
    // set target fps
    SetTargetFPS(FPS_TARGET);
    
}
void game_close()
{
    free(board_new);
    free(board_old);
    CloseWindow();
}

void game_setup()
{
    free(board_new);
    free(board_old);

    // calculate columns and rows by window size
    BOARD_COLS = WIN_WIDTH / CELL_SIZE;
    BOARD_ROWS = WIN_HEIGHT / CELL_SIZE;

    // allocate 2 boards for doublebuffering
    board_old = calloc((size_t)(BOARD_COLS * BOARD_ROWS), sizeof(bool));
    board_new = calloc((size_t)(BOARD_COLS * BOARD_ROWS), sizeof(bool));

    // check exit on failure
    if (!board_new || !board_old)
    {
        TraceLog(LOG_INFO, "Error allocating memory for the boards");
        exit(EXIT_FAILURE);
    }

    // Seed random generator with time
    srand((uint32_t)time(NULL));

    // Generate same random cells on all two boards
    // int cell_count = 200;
    for (int i = 0; i < BOARD_COLS * BOARD_ROWS; i++)
    {
        // cell_count--;
        board_old[i] = (bool)(rand() % 2) ? true : false;
        board_new[i] = board_old[i];
        // if(cell_count <= 0) break;
    }
}
void read_input()
{
    // READ INPUT
    if (IsKeyPressed(KEY_R))
    {
        game_setup();
    }
    if (IsKeyPressed(KEY_F))
    {
        // toggle show FPS
        is_fps_visible = !is_fps_visible;
    }
    if (IsKeyPressed(KEY_SPACE))
    {
        // toggle the pause of the game
        is_running = !is_running;
        printf("%s\n", (is_running) ? "PAUSE TRUE" : "PAUSE FALSE");
    }
    if (IsKeyPressed(KEY_UP))
    {
        // Increase speed
        wait_time -= 0.05;
        TraceLog(LOG_INFO, "Wait time : %.2f", wait_time);
        if (FPS_TARGET < 142)
        {
            FPS_TARGET += 2;
            SetTargetFPS(FPS_TARGET);
        }
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        // decrease speed
        wait_time += 0.05;
        TraceLog(LOG_INFO, "Wait time : %.2f", wait_time);
        if (FPS_TARGET > 3)
        {
            FPS_TARGET -= 2;
            SetTargetFPS(FPS_TARGET);
        }
    }
    if (IsKeyPressed(KEY_F11))
    {
        ToggleFullscreen();
        is_fullscreen = !is_fullscreen;
        // if  in fullscreen switch to fullscreen resolution
        if (IsWindowFullscreen())
        {
            WIN_WIDTH = FULLSCREEN_WIDTH;
            WIN_HEIGHT = FULLSCREEN_HEIGHT;
        }
        // else switch to default resolution
        else
        {
            WIN_WIDTH = DEFAULT_WIN_WIDTH;
            WIN_HEIGHT = DEFAULT_WIN_HEIGHT;
        }
        SetWindowSize(WIN_WIDTH, WIN_HEIGHT);
        game_setup();
    }
    // update board with rules of the game
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        printf("X : %i\tY : %i\n", GetMouseX(), GetMouseY());
        // Divide mouse position by cell size to get the index
        int col = GetMouseX() / CELL_SIZE;
        int row = GetMouseY() / CELL_SIZE;
        // translate x y coordinate into monodimensional array
        // change cell in both boards and draw
        board_new[BOARD_COLS * row + col] = !board_new[BOARD_COLS * row + col];
        board_old[BOARD_COLS * row + col] = board_new[BOARD_COLS * row + col];
        game_draw();
    }
}
void game_draw()
{
    // clear screen
    ClearBackground(BACKGROUND_COLOR);
    // draw board_new
    int cell_rect_size = CELL_SIZE - CELL_OFFSET;
    for (int row = 0; row < BOARD_ROWS; row++)
    {
        for (int col = 0; col < BOARD_COLS; col++)
        {
            if (board_new[BOARD_COLS * row + col])
            {
                DrawRectangle(col * CELL_SIZE + CELL_OFFSET, row * CELL_SIZE + CELL_OFFSET, cell_rect_size,
                              cell_rect_size, CELL_COLOR);
            }
        }
    }
    // Print FPS
    if (is_fps_visible)
    {
        DrawFPS(WIN_WIDTH * 0.9f, WIN_HEIGHT * 0.1f);
    }
}

int calculate_nearby_alive_cells(int row, int col)
{
    int nearby_alive_cells = 0;
    // left
    if (board_old[BOARD_COLS * row + col - 1])
        nearby_alive_cells++;
    // right
    if (board_old[BOARD_COLS * row + col + 1])
        nearby_alive_cells++;

    // top left
    if (board_old[BOARD_COLS * (row - 1) + col - 1])
        nearby_alive_cells++;
    // top center
    if (board_old[BOARD_COLS * (row - 1) + col])
        nearby_alive_cells++;
    // top right
    if (board_old[BOARD_COLS * (row - 1) + col + 1])
        nearby_alive_cells++;

    // bottom left
    if (board_old[BOARD_COLS * (row + 1) + col - 1])
        nearby_alive_cells++;
    // bottom center
    if (board_old[BOARD_COLS * (row + 1) + col])
        nearby_alive_cells++;
    // bottom right
    if (board_old[BOARD_COLS * (row + 1) + col + 1])
        nearby_alive_cells++;

    return nearby_alive_cells;
}

void game_update()
{
    // Apply rule of the game
    // rule 1   if the cell is alive and has  < 2 || > 3 nearby alive cells , cell dies
    // rule 2   if the cell is  dead and has    == 3     nearby alive cells , cell rebirth
    // PROVARE A CICLARE PARTENDO DA ROW 1 e COL 1 FINO A ROWS -2 e COLS -2 praticamente
    // per ignorare la cintura esterna
    for (int row = 1; row < BOARD_ROWS - 1; row++)
    {
        for (int col = 1; col < BOARD_COLS - 1; col++)
        {
            // calculate index for monodimensional array
            int index = BOARD_COLS * row + col;
            // board_new[index] = board_old[index];
            //  CALCULATE NEARBY ALIVE CELLS of board_old
            int nearby_alive_cells = calculate_nearby_alive_cells(row, col);
            // if current cell is alive and has < 2 or > 3 alive cells it dies
            if (board_old[index] && (nearby_alive_cells == 2))
            {
                board_new[index] = true;
            }
            else if (board_old[index] && (nearby_alive_cells == 3))
            {
                board_new[index] = true;
            }
            else if (!board_old[index] && (nearby_alive_cells == 3))
            {
                board_new[index] = true;
            }
            else
            {
                board_new[index] = false;
            }
        }
    }
    // copy  board_new in board_old
    for (int i = 0; i < BOARD_COLS * BOARD_ROWS; i++)
    {
        board_old[i] = board_new[i];
    }
}