#include "Instance.hpp"

ude_session_logout::Instance::Instance() noexcept
{
    initInfo =
    {
        .inlineComponents = { &mainView },
        UIMGUI_INIT_INFO_DEFAULT_DIRS,
    };
}

void ude_session_logout::Instance::begin() noexcept
{
    beginAutohandle();

}

void ude_session_logout::Instance::tick(const float deltaTime) noexcept
{
    tickAutohandle(deltaTime);

}

void ude_session_logout::Instance::end() noexcept
{
    endAutohandle();

}

void ude_session_logout::Instance::onEventConfigureStyle(ImGuiStyle& style, ImGuiIO& io) noexcept
{

}