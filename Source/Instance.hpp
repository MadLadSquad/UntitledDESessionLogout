#pragma once
#include <Framework.hpp>
#include "MainView.hpp"
#include <Generated/Config.hpp>

namespace ude_session_logout
{
    class UIMGUI_PUBLIC_API Instance : public UImGui::Instance
    {
    public:
        Instance();
        virtual void begin() override;
        virtual void tick(float deltaTime) override;
        virtual void end() override;
        virtual ~Instance() override;

        virtual void onEventConfigureStyle(ImGuiStyle& style, ImGuiIO& io) override;
    private:
        MainView mainView;
    };
}

