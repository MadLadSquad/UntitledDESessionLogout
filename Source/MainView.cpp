#include "MainView.hpp"
#include <unistd.h>
#include <pwd.h>
#include <sys/sysinfo.h>

ude_session_logout::MainView::MainView() noexcept
{
    const auto uid = geteuid();
    const auto pwd = getpwuid(uid);

    user = pwd ? pwd->pw_name : "user";
    struct sysinfo info{};
    if (sysinfo(&info) == 0 && info.totalswap > 0)
        bHasSwap = true;
}

void ude_session_logout::MainView::begin() noexcept
{
    beginAutohandle();
    UImGui::Window::Platform::setWindowType(X11_WINDOW_TYPE_SPLASH);
    initDBus();
}

void ude_session_logout::MainView::tick(const float deltaTime) noexcept
{
    tickAutohandle(deltaTime);

    ImGui::NewLine();
    ImGui::Text(" Log out the following user: %s", user.c_str());

    // Just create a new line with padding
    ImGui::Text("");
    ImGui::SameLine();
    if (ImGui::Button("Log Out"))
    {
        logout();
        UImGui::Instance::shutdown();
    }

    ImGui::SameLine();
    if (ImGui::Button("Power Off"))
    {
        poweroff();
        UImGui::Instance::shutdown();
    }

    ImGui::SameLine();
    if (ImGui::Button("Restart"))
    {
        restart();
        UImGui::Instance::shutdown();
    }

    ImGui::SameLine();
    if (ImGui::Button("Sleep"))
    {
        suspend();
        UImGui::Instance::shutdown();
    }
    if (bHasSwap)
    {
        ImGui::SameLine();
        if (ImGui::Button("Hibernate"))
        {
            hibernate();
            UImGui::Instance::shutdown();
        }
    }

    // This is the shutdown for the application not to be confused with the actual shutdown above
    // Add padding on the new line here
    ImGui::Text("");
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
        UImGui::Instance::shutdown();
}

void ude_session_logout::MainView::end() noexcept
{
    endAutohandle();

}

ude_session_logout::MainView::~MainView() noexcept
{
    user.clear();
}

void ude_session_logout::MainView::logout() noexcept
{
    const auto uid = geteuid();
    const auto pwd = getpwuid(uid);

    UImGui::FString session_id = getSessionID(pwd->pw_name);
    if (session_id.empty())
        return;

    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "TerminateSession");
    
    char* sid = session_id.data();
    UDBus::MessageBuilder builder(message);
    builder << sid << UDBus::EndMessage;

    // Send the message and get the response
    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();

    reply.pending_call_steal_reply(pending);
    if (reply.get_type() == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when logging out the system! Error: ", ULOG_LOG_TYPE_ERROR, reply.get_error_name());
}

void ude_session_logout::MainView::poweroff() noexcept
{
    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "PowerOff");

    const udbus_bool_t bForce = 1;
    UDBus::MessageBuilder builder(message);
    builder << bForce << UDBus::EndMessage;
    
    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();

    reply.pending_call_steal_reply(pending);

    if (reply.get_type() == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when powering off the system! Error: ", ULOG_LOG_TYPE_ERROR, reply.get_error_name());
}

void ude_session_logout::MainView::restart() noexcept
{
    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Reboot");

    const udbus_bool_t bForce = 1;
    UDBus::MessageBuilder builder(message);
    builder << bForce << UDBus::EndMessage;

    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();

    reply.pending_call_steal_reply(pending);

    if (reply.get_type() == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when restarting the system! Error: ", ULOG_LOG_TYPE_ERROR, reply.get_error_name());
}

void ude_session_logout::MainView::suspend() noexcept
{
    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Suspend");
    
    const udbus_bool_t bForce = 1;
    UDBus::MessageBuilder builder(message);
    builder << bForce << UDBus::EndMessage;

    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();

    reply.pending_call_steal_reply(pending);

    if (reply.get_type() == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when putting the system to sleep! Error: ", ULOG_LOG_TYPE_ERROR, reply.get_error_name());
}


void ude_session_logout::MainView::hibernate() noexcept
{
    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Hibernate");

    const udbus_bool_t bForce = 1;
    UDBus::MessageBuilder builder(message);
    builder << bForce << UDBus::EndMessage;

    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();

    reply.pending_call_steal_reply(pending);

    if (reply.get_type() == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when putting the system to sleep! Error: ", ULOG_LOG_TYPE_ERROR, reply.get_error_name());
}

void ude_session_logout::MainView::initDBus() noexcept
{
    conn.bus_get(DBUS_BUS_SYSTEM, error);

    if (error.is_set())
    {
        Logger::log("Error when connecting to the system bus! Error: ", ULOG_LOG_TYPE_ERROR, error.message());
        error.free();
        UImGui::Instance::shutdown();
    }
}

UImGui::FString ude_session_logout::MainView::getSessionID(const UImGui::FString& username) noexcept
{
    UImGui::FString sessionID;

    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "ListSessions");

    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();
    reply.pending_call_steal_reply(pending);

    if (reply.get_type() != DBUS_MESSAGE_TYPE_ERROR)
    {
        std::vector<UDBus::Struct<const char*, uint32_t, const char*, const char*, const char*>> v;
        auto p = UDBus::Type
        {
            v,
            UDBus::bump()
        };

        auto result = reply.handleMessage(p);
        if (result != UDBus::RESULT_SUCCESS)
            Logger::log("Couldn't handle parsing the message reply. Error: ", ULOG_LOG_TYPE_ERROR, result);

        for (auto& a : v)
        {
            if (username == *a.next()->next()->data)
            {
                sessionID = *a.data;
                break;
            }
        }
        decltype(p)::destroyComplex(v);
        decltype(p)::destroy(p);
    }
    else
        Logger::log("Error getting the session list! Error: ", ULOG_LOG_TYPE_ERROR, error.message());

    message.unref();
    reply.unref();
    pending.unref();

    return sessionID;
}
