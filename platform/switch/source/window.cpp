#include "common/runtime.h"
#include "modules/window/window.h"

using namespace love;

#include "deko3d/init.h"
#include "common/exception.h"

Window::Window() : window(nullptr),
                   open(false)

{
    deko3d::Initialize();

    this->displaySizes =
    {
        {1280, 720}
    };

    this->targets.reserve(1);
}

Window::~Window()
{
    deko3d::DeInitialize();

    this->graphics.Set(nullptr);

    this->open = false;
}

Renderer * Window::GetRenderer()
{
    return this->targets.back();
}

void Window::SetScreen(size_t screen)
{
    // this never goes above 1 but.. not risking it
    this->currentDisplay = std::max(screen - 1, (size_t)0);
}

bool Window::SetMode()
{
    return this->open = true;
}

void Window::Clear(Color * color)
{}

void Window::SetRenderer(Canvas * canvas)
{}

void Window::SetGraphics(Graphics * g)
{
    this->graphics.Set(g);
}

void Window::Present()
{
    // Acquire a framebuffer from the swapchain (and wait for it to be available)
    int slot = deko3d::queue.acquireImage(deko3d::swapchain);

    // Run the command list that attaches said framebuffer to the queue
    deko3d::queue.submitCommands(deko3d::framebuffer_cmdlists[slot]);

    // Run the main rendering command list
    deko3d::queue.submitCommands(deko3d::render_cmdlist);

    // Now that we are done rendering, present it to the screen
    deko3d::queue.presentImage(deko3d::swapchain, slot);
}

std::vector<std::pair<int, int>> & Window::GetFullscreenModes()
{
    return this->displaySizes;
}

int Window::GetDisplayCount()
{
    return 1;
}

bool Window::IsOpen()
{
    return this->open;
}
