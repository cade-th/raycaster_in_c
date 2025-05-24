### Raycaster in C

Dependencies:
Raylib:
    git clone https://github.com/raysan5/raylib.git raylib
    cd raylib
    mkdir build && cd build
    cmake -DBUILD_SHARED_LIBS=ON ..
    make
    sudo make install
    sudo ldconfig

Helpful functions:

void DrawRectangle(int posX, int posY, int width, int height, Color color);    
bool IsKeyDown(int key);
void DrawCircle(int centerX, int centerY, float radius, Color color);
void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color);          
void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);
Texture2D LoadTexture("path/to/atlas.png");
void DrawText(const char *text, int posX, int posY, int fontSize, Color color);

Pseudocode for the DDA algorithm:

    int DOF(depth of view) = 8
    ray_pos x and y

    Check Horizontal Lines
        If ray directoin UP
           
        If ray direciton DOWN

        else ray direction is perfectly horizontal

    while DOF < 8
        

    Check Vertical Lines
        if ray direction LEFT

        if ray direction RIGHT

        else ray direction is perfectly vertical

    while DOF < 8
        

    Return Position and distance of longest hit point



Pseudocode for the FOV function:

    iterate over the number of rays
        cast a ray at an offset of some field of view

    save the hitpoint and distance in a tuple

    return an array of those tuples

Pseudocode for the FPS function:

    column_width = ceil(float(screen_width / number of rays)

    iterate of the number of rays:

        wall_height = (screen_height * world_tile_size) / distance of the ray hit

        wall width = i * column_width
    
    + a bunch of other stuff if you're doing a textured raycaster

