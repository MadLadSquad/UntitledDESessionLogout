#pragma once
#include <Framework.hpp>
#include <dbus/dbus.h>

namespace ude_session_logout
{
    class UIMGUI_PUBLIC_API MainView : public UImGui::InlineComponent
    {
    public:
        MainView();
        virtual void begin() override;
        virtual void tick(float deltaTime) override;
        virtual void end() override;
        virtual ~MainView() override;
    private:
        UImGui::FString user;
        bool bHasSwap = false;

        void logout() noexcept;
        void poweroff() noexcept;
        void restart() noexcept;

        void suspend() noexcept;
        void hibernate() noexcept;

        UDBus::Connection conn;

        void initDBus() noexcept;
        std::string getSessionID(const std::string& username) noexcept;
    };
}

