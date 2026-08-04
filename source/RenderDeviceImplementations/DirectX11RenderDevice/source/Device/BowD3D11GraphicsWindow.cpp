#include <DirectX11RenderDevice/Device/BowD3D11GraphicsWindow.h>

#include <DirectX11RenderDevice/BowD3D11RenderDevice.h>
#include <DirectX11RenderDevice/Device/BowD3D11RenderContext.h>

#include <CoreSystems/BowLogger.h>

#include <unordered_map>

namespace bow
{
namespace
{

const wchar_t *WindowClassName = L"LongBowDirectX11Window";

//! Maps a window handle back to the object that owns it.
/*!
    WindowProc is a free function, so the message it receives has to be routed
    to the right instance. The handle is registered before CreateWindow returns
    because messages arrive during creation.
*/
std::unordered_map<HWND, D3D11GraphicsWindow *> &WindowRegistry()
{
    static std::unordered_map<HWND, D3D11GraphicsWindow *> registry;
    return registry;
}

std::wstring Widen(const std::string &text)
{
    if (text.empty())
    {
        return std::wstring();
    }

    const int length = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)text.size(), nullptr, 0);
    std::wstring wide((size_t)length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)text.size(), &wide[0], length);
    return wide;
}

} // namespace

D3D11GraphicsWindow::D3D11GraphicsWindow() : m_windowHandle(nullptr), m_parentDevice(nullptr), m_context(nullptr), m_width(0), m_height(0), m_shouldClose(false), m_focused(true), m_visible(true)
{
    FN("D3D11GraphicsWindow::D3D11GraphicsWindow");
}

D3D11GraphicsWindow::~D3D11GraphicsWindow()
{
    FN("D3D11GraphicsWindow::~D3D11GraphicsWindow");

    VRelease();
}

bool D3D11GraphicsWindow::Initialize(uint32_t width, uint32_t height, const std::string &title, WindowType windowType, D3D11RenderDevice *device)
{
    FN("D3D11GraphicsWindow::Initialize");

    m_parentDevice = device;
    m_width = width;
    m_height = height;

    HINSTANCE instance = GetModuleHandle(nullptr);

    // Registering the same class twice fails with ERROR_CLASS_ALREADY_EXISTS,
    // which is expected from the second window onwards.
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = WindowClassName;
    RegisterClassExW(&windowClass);

    DWORD style = WS_OVERLAPPEDWINDOW;
    if (windowType == WindowType::FullscreenBorderlessWindow || windowType == WindowType::Fullscreen)
    {
        style = WS_POPUP;
        m_width = (uint32_t)GetSystemMetrics(SM_CXSCREEN);
        m_height = (uint32_t)GetSystemMetrics(SM_CYSCREEN);
    }

    // The requested size is the drawable area, not the outer frame.
    RECT windowRect = {0, 0, (LONG)m_width, (LONG)m_height};
    AdjustWindowRect(&windowRect, style, FALSE);

    const std::wstring windowTitle = Widen(title + " (DirectX11)");

    m_windowHandle = CreateWindowExW(0, WindowClassName, windowTitle.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, instance, this);

    if (m_windowHandle == nullptr)
    {
        LOG_ERROR("Could not create a Win32 window (error code %lu).", GetLastError());
        return false;
    }

    WindowRegistry()[m_windowHandle] = this;

    ShowWindow(m_windowHandle, SW_SHOW);

    m_context = D3D11RenderContextPtr(new D3D11RenderContext(m_windowHandle, m_width, m_height));
    if (!m_context->Initialize(device))
    {
        LOG_ERROR("Could not create a DirectX 11 context for the window.");
        return false;
    }

    LOG_TRACE("DirectX11-Window successfully initialized!");
    return true;
}

LRESULT CALLBACK D3D11GraphicsWindow::WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::unordered_map<HWND, D3D11GraphicsWindow *> &registry = WindowRegistry();
    std::unordered_map<HWND, D3D11GraphicsWindow *>::iterator entry = registry.find(windowHandle);
    D3D11GraphicsWindow *window = (entry != registry.end()) ? entry->second : nullptr;

    switch (message)
    {
    case WM_CLOSE:
        if (window != nullptr)
        {
            window->m_shouldClose = true;
        }
        return 0;

    case WM_DESTROY:
        registry.erase(windowHandle);
        return 0;

    case WM_SIZE:
        if (window != nullptr)
        {
            window->m_visible = (wParam != SIZE_MINIMIZED);
            window->OnResize((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
        }
        return 0;

    case WM_SETFOCUS:
        if (window != nullptr)
        {
            window->m_focused = true;
        }
        return 0;

    case WM_KILLFOCUS:
        if (window != nullptr)
        {
            window->m_focused = false;
        }
        return 0;

    default:
        break;
    }

    return DefWindowProcW(windowHandle, message, wParam, lParam);
}

void D3D11GraphicsWindow::OnResize(uint32_t width, uint32_t height)
{
    FN("D3D11GraphicsWindow::OnResize");

    if (width == 0 || height == 0)
    {
        return;
    }

    m_width = width;
    m_height = height;

    if (m_context != nullptr)
    {
        m_context->Resize(width, height);
    }
}

void D3D11GraphicsWindow::VRelease()
{
    FN("D3D11GraphicsWindow::VRelease");

    if (m_context != nullptr)
    {
        m_context->VRelease();
        m_context.reset();
    }

    if (m_windowHandle != nullptr)
    {
        WindowRegistry().erase(m_windowHandle);
        DestroyWindow(m_windowHandle);
        m_windowHandle = nullptr;
    }

    LOG_TRACE("D3D11GraphicsWindow released");
}

RenderContextPtr D3D11GraphicsWindow::VGetContext() const
{
    FN("D3D11GraphicsWindow::VGetContext");

    return m_context;
}

void D3D11GraphicsWindow::VPollWindowEvents() const
{
    FN("D3D11GraphicsWindow::VPollWindowEvents");

    MSG message = {};
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
}

void D3D11GraphicsWindow::VSetWindowTitle(const char *title) const
{
    FN("D3D11GraphicsWindow::VSetWindowTitle");

    if (m_windowHandle != nullptr && title != nullptr)
    {
        SetWindowTextW(m_windowHandle, Widen(title).c_str());
    }
}

void D3D11GraphicsWindow::VHideCursor() const
{
    FN("D3D11GraphicsWindow::VHideCursor");

    ShowCursor(FALSE);
}

void D3D11GraphicsWindow::VShowCursor() const
{
    FN("D3D11GraphicsWindow::VShowCursor");

    ShowCursor(TRUE);
}

int D3D11GraphicsWindow::VGetWidth() const { return (int)m_width; }

int D3D11GraphicsWindow::VGetHeight() const { return (int)m_height; }

bool D3D11GraphicsWindow::VIsVisible() const { return m_visible; }

bool D3D11GraphicsWindow::VIsFocused() const { return m_focused; }

bool D3D11GraphicsWindow::VShouldClose() const
{
    // Messages have to be drained for WM_CLOSE to ever arrive; an application
    // that only asks whether it should close would otherwise never be told.
    VPollWindowEvents();
    return m_shouldClose;
}

} // namespace bow
