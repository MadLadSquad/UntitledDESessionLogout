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

        static void logout() noexcept;
        static void poweroff() noexcept;
        static void restart() noexcept;

        static void suspend() noexcept;

        struct DBusData
        {
            DBusError error;
            DBusConnection* conn;
            DBusMessage* message;
            DBusPendingCall* pending;
            DBusMessage* reply;

            bool bErrored = false;
        } data;

        static DBusData initDBus() noexcept;
        static void destroyDBus(DBusData& data) noexcept;
        static std::string getSessionID(DBusData& data) noexcept;
    };
}

