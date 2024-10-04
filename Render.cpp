#include "Wordle.hpp"

LRESULT CALLBACK windowMessageHandler(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

//graphic
BITMAPINFO  bmi;
HWND        window;
HDC         WindowDC;
WNDCLASSA   wc;

//my stuff
pixel*      vram;
gameInfo*   g_info;

pixel*  open32BitBMP(std::string fileName)
{
    HANDLE      file;
    std::string buffer;
    char*        buffer2;
    uint32_t    file_size;
    uint32_t    start_dest;
    pixel*      image;

    file = CreateFileA(fileName.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    ReadFile(file, (LPVOID)buffer.c_str(), 14, 0, 0);
    file_size = *(uint32_t*)(buffer.c_str() + 2);
    start_dest = *(uint32_t*)(buffer.c_str() + 10);
    SetFilePointer(file, start_dest, 0, 0);
    image = new pixel[(file_size - start_dest) / 4];
    buffer2 = new char[file_size];
    ReadFile(file, buffer2, file_size, 0, 0);
    for (int i = 0; i < (file_size - start_dest) / 4; i++)
    {
        image[i].b = buffer2[i * 4 + 0];
        image[i].g = buffer2[i * 4 + 1];
        image[i].r = buffer2[i * 4 + 2];
        image[i].a = buffer2[i * 4 + 3];
    }
    CloseHandle(file);
    delete buffer2;
    return (image);
}

pixel** initTextures()
{
    pixel** textures = new pixel * [TEXTURE_COUNT];

    for (int i = 0; i < TEXTURE_COUNT; i++)
    {
        std::string fileName(1, (char)(i + 'A'));
        textures[i] = open32BitBMP("Sprites/" + fileName + ".bmp");
    }
    return textures;
}

void	initRender(uint32_t vramX, uint32_t vramY, uint32_t scrX, uint32_t scrY)
{
    WNDCLASSA wc{};

    wc.lpfnWndProc = windowMessageHandler;                                                                  // Pointer to the window procedure
    wc.hInstance = GetModuleHandle(NULL);                                                                   // Handle to the application instance
    wc.lpszClassName = "class";                                                                             // Name of the window class
    wc.hCursor = LoadCursor(NULL, IDC_HAND);                                                                // Default cursor
    wc.hIcon = (HICON)LoadImageA(NULL, "Wordle.ico", IMAGE_ICON, 256, 256, LR_DEFAULTSIZE | LR_LOADFROMFILE); // Create Icon
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);                                                          // Default background color
    wc.style = CS_HREDRAW | CS_VREDRAW;                                                                     // Window style (optional)

    // Register the window class
    RegisterClassA(&wc);
    vram = new pixel[vramX * vramY]{};
    window = CreateWindowExA(0, "class", "Wordle", WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_EX_CLIENTEDGE, 0, 0, scrX, scrY, 0, 0, wc.hInstance, 0);
    WindowDC = GetDC(window);

    bmi.bmiHeader.biWidth = vramX;
    bmi.bmiHeader.biHeight = vramY;
    bmi.bmiHeader.biBitCount = sizeof(pixel) * 8;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);

    RECT client;
    GetClientRect(window, &client);
    SetWindowPos(window, 0, 150, 50, scrX + (scrX - client.right), scrY + (scrY - client.bottom), 0);
}

//special transparent version
void drawTexture(int x, int y, int size, int screenWidth, pixel* texture)
{
    int iVram;
    int iTexture;
    for (int y2 = 0; y2 < size; y2++)
    {
        for (int x2 = 0; x2 < size; x2++)
        {
            iVram = x + x2 + (y + y2) * screenWidth;
            iTexture = x2 + y2 * size;
            if (texture[iTexture].a == 0xFF)
                vram[iVram] = texture[iTexture];
            else if (texture[iTexture].a)
            {
                vram[iVram].b = (uint8_t)((float)vram[iVram].b * (float)(0xFF - texture[iTexture].a) / 0xFF + (float)texture[iTexture].b * (float)texture[iTexture].a / 0xFF);
                vram[iVram].g = (uint8_t)((float)vram[iVram].g * (float)(0xFF - texture[iTexture].a) / 0xFF + (float)texture[iTexture].g * (float)texture[iTexture].a / 0xFF);
                vram[iVram].r = (uint8_t)((float)vram[iVram].r * (float)(0xFF - texture[iTexture].a) / 0xFF + (float)texture[iTexture].r * (float)texture[iTexture].a / 0xFF);
            }
        }
    }
}

void drawrectangle(int x, int y, int width, int height, int screenWidth, pixel colour)
{
    int tempwidth = width;
    width += x;
    height += y;
    
    for (; y < height; y++)
    {
        for (; x < width; x++)
        {
            vram[x + y * screenWidth] = colour;
        }
        x -= tempwidth;
    }
}

void    render(block* info, pixel** textures)
{
    pixel colour;
    for (int y = 0; y < MAX_TRY; y++)
    {
        for (int x = 0; x < STR_LEN; x++)
        {
            if (info[y].state[x] == WHITE)
                colour = WHITE_RGB;
            else if (info[y].state[x] == GRAY)
                colour = GRAY_RGB;
            else if (info[y].state[x] == YELLOW)
                colour = YELLOW_RGB;
            else
                colour = GREEN_RGB;
            drawrectangle(x * OFFSET, (MAX_TRY - 1 - y) * OFFSET, SPRITE_SIZE, SPRITE_SIZE, VRAM_X, colour);
            if (info[y].str.length() > x)
                drawTexture(x * OFFSET, (MAX_TRY - 1 - y) * OFFSET, SPRITE_SIZE, VRAM_X, textures[info[y].str[x] - 'A']);
        }
    }
    
}

void    render(gameInfo* info)
{
    memset(vram, D_GRAY, VRAM_X * VRAM_Y * sizeof(pixel));
    render(info->grid, info->textures);
    StretchDIBits(WindowDC, 0, 0, SCREEN_X, SCREEN_Y, 0, 0, VRAM_X, VRAM_Y, vram, &bmi, 0, SRCCOPY);
}

void    endPrompt(std::string title, std::string content, gameInfo* info)
{
    render(info);
    if (MessageBoxA(0, content.c_str(), title.c_str(), MB_YESNO | MB_ICONQUESTION) == IDNO)
        exit(0);
    info->finished = true;
}

void    userInputHell(WPARAM wParam)
{
    static uint8_t  i;
    gameInfo* info = g_info;

    if (wParam == VK_RETURN && validWord(info->grid[i].str, info->wordList, info->listLength))
    {
        if (handleWord(info, i))
        {
            endPrompt("Congrats", "You won! Would you like to try again?", info);
        }
        else if (i >= MAX_TRY - 1)
        {
            endPrompt("Out of moves", "The word was : " + info->answer + ". Would you like to try again?", info);
        }
        if (info->finished)
        {
            i = 0;
            return;
        }
        i++;
    }
    if (wParam == VK_BACK && info->grid[i].str.length() > 0)
    {
        info->grid[i].str.pop_back();
    }
    if ((wParam >= 'A' && wParam <= 'Z') && info->grid[i].str.length() < STR_LEN)
    {
        info->grid[i].str.append(1, (char)wParam);
    }
    if (DEBUG != 0)
    {
        std::cout << info->grid[i].str << "\n";
    }
    render(info);
}

LRESULT CALLBACK windowMessageHandler(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    {
        userInputHell(wParam);
        break;
    }
    case WM_QUIT:
    case WM_CLOSE:
        ExitProcess(0);
    }
    return DefWindowProcA(window, msg, wParam, lParam);
}

void    renderLoop(gameInfo* info)
{
    MSG     msg;

    g_info = info;
    render(info);
    FreeConsole();
    while (1)
    {
        while (PeekMessageA(&msg, window, 0, 0, 0))
        {
            GetMessageA(&msg, window, 0, 0);
            DispatchMessageA(&msg);
        }
        if (info->finished)
            return;
        Sleep(33);
    }
}
