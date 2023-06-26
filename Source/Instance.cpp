#include "Instance.hpp"

ude_session_logout::Instance::Instance()
{
    initInfo =
    {
        .inlineComponents = { &mainView },
        UIMGUI_INIT_INFO_DEFAULT_DIRS,
    };
}

void ude_session_logout::Instance::begin()
{
    beginAutohandle();

}

void ude_session_logout::Instance::tick(float deltaTime)
{
    tickAutohandle(deltaTime);

}

void ude_session_logout::Instance::end()
{
    endAutohandle();

}

ude_session_logout::Instance::~Instance()
{

}

void ude_session_logout::Instance::onEventConfigureStyle(ImGuiStyle& style, ImGuiIO& io)
{

}

