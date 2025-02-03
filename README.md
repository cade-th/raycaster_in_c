### Raycaster in C

Helpful functions:

void DrawRectangle(int posX, int posY, int width, int height, Color color);    
bool IsKeyDown(int key);
void DrawCircle(int centerX, int centerY, float radius, Color color);
void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color);          
void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint);
Texture2D LoadTexture("path/to/atlas.png");

Pseudocode for the DDA algorithm:

Pseudocode for the FOV function:

Pseudocode for the FPS function:


