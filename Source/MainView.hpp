#pragma once
#include <Framework.hpp>
#include <dbus/dbus.h>

namespace ude_session_logout
{
    class UIMGUI_PUBLIC_API MainView final : public UImGui::InlineComponent
    {
    public:
        MainView() noexcept;
        virtual void begin() noexcept override;
        virtual void tick(float deltaTime) noexcept override;
        virtual void end() noexcept override;
        virtual ~MainView() noexcept override;
    private:
        UImGui::FString user;
        bool bHasSwap = false;

        void logout() noexcept;
        void poweroff() noexcept;
        void restart() noexcept;

        void suspend() noexcept;
        void hibernate() noexcept;

        UDBus::Connection conn;

        UDBus::Error error;
        UDBus::Message message;
        UDBus::PendingCall pending;
        UDBus::Message reply;

        void initDBus() noexcept;
        UImGui::FString getSessionID(const UImGui::FString& username) noexcept;
    };
}

