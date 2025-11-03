#include "backend/ruddercontroller.h"
#include <QDebug>
#include <QDateTime>

#ifdef Q_OS_WIN
#include <SDL2/SDL.h>
#include <windows.h>

// 移除原来的 #pragma comment(lib, "XInput.lib")
// 改为动态加载 XInput 函数

// 定义 XInput 函数指针类型
typedef DWORD (WINAPI *XInputGetStatePtr)(DWORD, void*);

class XInputLoader {
private:
    HMODULE m_xinputDll;
    XInputGetStatePtr m_xInputGetState;

public:
    XInputLoader() : m_xinputDll(nullptr), m_xInputGetState(nullptr) {
        // 尝试加载不同版本的 XInput DLL
        const wchar_t* dllNames[] = {
            L"xinput1_4.dll",
            L"xinput1_3.dll",
            L"xinput9_1_0.dll",
            L"xinput1_2.dll",
            L"xinput1_1.dll"
        };

        for (const wchar_t* dllName : dllNames) {
            m_xinputDll = LoadLibraryW(dllName);
            if (m_xinputDll) {
                m_xInputGetState = (XInputGetStatePtr)GetProcAddress(m_xinputDll, "XInputGetState");
                if (m_xInputGetState) {
                    qDebug() << "成功加载 XInput DLL:" << QString::fromWCharArray(dllName);
                    break;
                }
                FreeLibrary(m_xinputDll);
                m_xinputDll = nullptr;
            }
        }

        if (!m_xInputGetState) {
            qWarning() << "无法加载 XInput DLL";
        }
    }

    ~XInputLoader() {
        if (m_xinputDll) {
            FreeLibrary(m_xinputDll);
        }
    }

    bool isAvailable() const { return m_xInputGetState != nullptr; }

    DWORD getState(DWORD dwUserIndex, void* pState) const {
        return m_xInputGetState ? m_xInputGetState(dwUserIndex, pState) : ERROR_DLL_NOT_FOUND;
    }
};

// 全局 XInput 加载器
static XInputLoader g_xInputLoader;

// XINPUT_STATE 结构定义
typedef struct _XINPUT_GAMEPAD {
    WORD  wButtons;
    BYTE  bLeftTrigger;
    BYTE  bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

typedef struct _XINPUT_STATE {
    DWORD dwPacketNumber;
    XINPUT_GAMEPAD Gamepad;
} XINPUT_STATE, *PXINPUT_STATE;

#endif

RudderController::RudderController(QObject *parent)
    : QThread(parent)
    , m_running(true)
    , m_controllerConnected(false)
{
    qDebug() << "手柄控制器初始化...";
}

RudderController::~RudderController()
{
    stopDetection();
    if (!wait(1000)) { // 等待线程结束，最多1秒
        qWarning() << "手柄检测线程未能正常结束";
    }
}

RudderState RudderController::getCurrentState()
{
    QMutexLocker locker(&m_mutex);
    return m_currentState;
}

bool RudderController::isControllerConnected() const
{
    return m_controllerConnected;
}

void RudderController::stopDetection()
{
    m_running = false;
}

void RudderController::updateState(const RudderState &newState)
{
    QMutexLocker locker(&m_mutex);
    m_currentState = newState;
}

void RudderController::run()
{
    qDebug() << "开始手柄检测线程...";

#ifdef Q_OS_WIN
    // SDL2 初始化
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI, "1");
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_XBOX, "1");
    SDL_SetHint(SDL_HINT_XINPUT_ENABLED, "1");

    bool sdlAvailable = (SDL_Init(SDL_INIT_GAMECONTROLLER) >= 0);
    SDL_GameController *controller = nullptr;
    bool previousConnected = m_controllerConnected;

    if (sdlAvailable) {
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            if (SDL_IsGameController(i)) {
                controller = SDL_GameControllerOpen(i);
                if (controller) {
                    qDebug() << "检测到SDL控制器:" << SDL_GameControllerName(controller);
                    m_controllerConnected = true;
                    break;
                }
            }
        }
    } else {
        qWarning() << "SDL2初始化失败";
    }

    if (!controller) {
        qWarning() << "SDL控制器不可用，尝试使用XInput";
    }

    // 检查XInput是否可用
    bool xinputAvailable = g_xInputLoader.isAvailable();
    if (!xinputAvailable) {
        qWarning() << "XInput也不可用，将无法检测手柄输入";
    }

    bool useXInput = (controller == nullptr) && xinputAvailable;
    RudderState state;
    int zeroCount = 0;

    while (m_running) {
        bool currentConnected = m_controllerConnected;

        if (!useXInput && controller) {
            SDL_GameControllerUpdate();

            state.leftX  = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX)  / 32767.0f;
            state.leftY  = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY)  / 32767.0f;
            state.rightX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f;
            state.rightY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f;

            if (!SDL_GameControllerGetAttached(controller)) {
                qWarning() << "SDL控制器断开连接";
                useXInput = xinputAvailable;
                m_controllerConnected = false;
            }
            else if (qFuzzyIsNull(state.leftX) && qFuzzyIsNull(state.leftY) &&
                     qFuzzyIsNull(state.rightX) && qFuzzyIsNull(state.rightY)) {
                zeroCount++;
                if (zeroCount > 10) {
                    qWarning() << "SDL返回零轴数据，切换到XInput模式";
                    useXInput = xinputAvailable;
                }
            } else {
                zeroCount = 0;
                m_controllerConnected = true;
            }
        }

        if (useXInput && xinputAvailable) {
            XINPUT_STATE xState;
            ZeroMemory(&xState, sizeof(XINPUT_STATE));

            DWORD result = g_xInputLoader.getState(0, &xState);
            if (result == ERROR_SUCCESS) {
                SHORT lx = xState.Gamepad.sThumbLX;
                SHORT ly = xState.Gamepad.sThumbLY;
                SHORT rx = xState.Gamepad.sThumbRX;
                SHORT ry = xState.Gamepad.sThumbRY;

                state.leftX  = lx / 32767.0f;
                state.leftY  = ly / 32767.0f;
                state.rightX = rx / 32767.0f;
                state.rightY = ry / 32767.0f;

                m_controllerConnected = true;
            } else {
                state = RudderState();
                m_controllerConnected = false;
            }
        }

        // 如果没有可用的控制器，状态清零
        if (!sdlAvailable && !xinputAvailable) {
            state = RudderState();
            m_controllerConnected = false;
        }

        if (currentConnected != m_controllerConnected) {
            emit controllerStatusChanged(m_controllerConnected);
            if (m_controllerConnected) {
                qDebug() << "控制器已连接";
            } else {
                qWarning() << "控制器断开连接";
            }
        }

        updateState(state);
        emit rudderUpdated(state);

        static int outputCount = 0;
        if (outputCount++ % 50 == 0) {
            QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
            QString status = m_controllerConnected ? "已连接" : "未连接";
            // qDebug().noquote() << QString("[%1] 手柄状态(%2): %3")
            //                           .arg(timestamp)
            //                           .arg(status)
            //                           .arg(state.toString());
        }

        msleep(16);
    }

    if (controller) {
        SDL_GameControllerClose(controller);
    }
    if (sdlAvailable) {
        SDL_Quit();
    }
#else
    // 非Windows平台的模拟代码
    qWarning() << "非Windows平台，使用模拟数据";
    RudderState state;
    int counter = 0;

    while (m_running) {
        float time = counter++ * 0.1f;
        state.leftX = qSin(time) * 0.5f;
        state.leftY = qCos(time) * 0.5f;
        state.rightX = qSin(time * 1.5f) * 0.3f;
        state.rightY = qCos(time * 1.5f) * 0.3f;

        updateState(state);
        emit rudderUpdated(state);

        qDebug().noquote() << QString("模拟数据: %1").arg(state.toString());

        msleep(100);
    }
#endif

    qDebug() << "手柄检测线程结束";
}
