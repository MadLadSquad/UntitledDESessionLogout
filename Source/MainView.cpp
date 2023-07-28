#include "MainView.hpp"
#include <unistd.h>
#include <pwd.h>
#include <sys/sysinfo.h>

ude_session_logout::MainView::MainView()
{
    auto uid = geteuid();
    auto pwd = getpwuid(uid);

    user = pwd ? pwd->pw_name : "user";
    struct sysinfo info{};
    if (sysinfo(&info) == 0 && info.totalswap > 0)
        bHasSwap = true;
}

void ude_session_logout::MainView::begin()
{
    beginAutohandle();
    UImGui::Window::Platform::setWindowType(X11_WINDOW_TYPE_SPLASH);
    initDBus();
}

void ude_session_logout::MainView::tick(float deltaTime)
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

void ude_session_logout::MainView::end()
{
    endAutohandle();

}

ude_session_logout::MainView::~MainView()
{
    user.clear();
}

void ude_session_logout::MainView::logout() noexcept
{
    auto uid = geteuid();
    auto pwd = getpwuid(uid);

    UDBus::Error error;
    UDBus::Message message;
    UDBus::PendingCall pending;
    UDBus::Message reply;

    std::string session_id = getSessionID(pwd->pw_name);
    if (session_id.empty())
        return;

    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "TerminateSession");

    char* sid = session_id.data();
    // Append the session ID to the message
    message.append_args_simple(&sid);

    // Send the message and get the response
    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();

    reply.pending_call_steal_reply(pending);
    if (reply.get_type() == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when logging out the system! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, reply.get_error_name());
}

void ude_session_logout::MainView::poweroff() noexcept
{
    UDBus::Error error;
    UDBus::Message message;
    UDBus::PendingCall pending;
    UDBus::Message reply;

    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "PowerOff");

    udbus_bool_t bForce = 1;
    message.append_args_simple(&bForce);

    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();

    reply.pending_call_steal_reply(pending);

    if (reply.get_type() == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when powering off the system! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, reply.get_error_name());
}

void ude_session_logout::MainView::restart() noexcept
{
    UDBus::Error error;
    UDBus::Message message;
    UDBus::PendingCall pending;
    UDBus::Message reply;

    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Reboot");

    udbus_bool_t bForce = 1;
    message.append_args_simple(&bForce);

    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();

    reply.pending_call_steal_reply(pending);

    if (reply.get_type() == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when restarting the system! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, reply.get_error_name());
}

void ude_session_logout::MainView::suspend() noexcept
{
    UDBus::Error error;
    UDBus::Message message;
    UDBus::PendingCall pending;
    UDBus::Message reply;

    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Suspend");

    udbus_bool_t bForce = 1;
    message.append_args_simple(&bForce);

    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();

    reply.pending_call_steal_reply(pending);

    if (reply.get_type() == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when putting the system to sleep! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, reply.get_error_name());
}


void ude_session_logout::MainView::hibernate() noexcept
{
    UDBus::Error error;
    UDBus::Message message;
    UDBus::PendingCall pending;
    UDBus::Message reply;
    
    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Hibernate");

    udbus_bool_t bForce = 1;
    message.append_args_simple(&bForce);

    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();

    reply.pending_call_steal_reply(pending);

    if (reply.get_type() == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when putting the system to sleep! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, reply.get_error_name());
}

void ude_session_logout::MainView::initDBus() noexcept
{
    UDBus::Error error;
    conn.bus_get(DBUS_BUS_SYSTEM, error);

    if (error.is_set())
    {
        Logger::log("Error when connecting to the system bus! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, error.message());
        error.free();
        UImGui::Instance::shutdown();
    }
}

std::string ude_session_logout::MainView::getSessionID(const std::string& username) noexcept
{
    std::string sessionID;
    UDBus::Error error;
    UDBus::Message message;
    UDBus::PendingCall pending;
    UDBus::Message reply;

    message.new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "ListSessions");

    conn.send_with_reply(message, pending, -1);
    conn.flush();

    pending.block();
    reply.pending_call_steal_reply(pending);

    if (reply.get_type() != DBUS_MESSAGE_TYPE_ERROR)
    {
        UDBus::Iterator it, arrayIt;
        it.setGet(reply, arrayIt, true);
        
        if (it.get_arg_type() == DBUS_TYPE_ARRAY)
        {
            it.recurse();
            
            while (arrayIt.get_arg_type() == DBUS_TYPE_STRUCT)
            {
                UDBus::Iterator structIt;
                arrayIt.setGet(reply, structIt, false);
                arrayIt.recurse();
                
                char* id;
                char* userName;
                
                structIt.get_basic(&id);
                structIt.next();
                structIt.next();
                structIt.get_basic(&userName);
                
                if (username == userName)
                {
                    sessionID = id;
                    break;
                }
                arrayIt.next();
            }
        }
    }
    else
        Logger::log("Error getting the session list! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, error.message());

    return sessionID;
}