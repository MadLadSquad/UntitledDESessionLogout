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

}

void ude_session_logout::MainView::tick(float deltaTime)
{
    tickAutohandle(deltaTime);

    ImGui::NewLine();
    ImGui::Text(" Log out the following user: %s", user.c_str());

    // Just create a new line with padding
    ImGui::Text(" ");
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
    ImGui::Text(" ");
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
    auto data = initDBus();
    if (data.bErrored)
        return;

    std::string session_id = getSessionID(data);
    if (session_id.empty())
        return;
    data.message = dbus_message_new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "TerminateSession");

    char* sid = session_id.data();
    // Append the session ID to the message
    dbus_message_append_args(data.message, DBUS_TYPE_STRING, &sid, DBUS_TYPE_INVALID);

    // Send the message and get the response
    dbus_connection_send_with_reply(data.conn, data.message, &data.pending, -1);
    dbus_connection_flush(data.conn);

    dbus_pending_call_block(data.pending);

    data.reply = dbus_pending_call_steal_reply(data.pending);
    if (dbus_message_get_type(data.reply) == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when logging out the system! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, dbus_message_get_error_name(data.reply));

    destroyDBus(data);
}

void ude_session_logout::MainView::poweroff() noexcept
{
    auto data = initDBus();
    if (data.bErrored)
        return;

    data.message = dbus_message_new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "PowerOff");

    dbus_bool_t bForce = 1;
    dbus_message_append_args(data.message, DBUS_TYPE_BOOLEAN, &bForce, DBUS_TYPE_INVALID);

    dbus_connection_send_with_reply(data.conn, data.message, &data.pending, -1);
    dbus_connection_flush(data.conn);

    dbus_pending_call_block(data.pending);

    data.reply = dbus_pending_call_steal_reply(data.pending);

    if (dbus_message_get_type(data.reply) == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when powering off the system! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, dbus_message_get_error_name(data.reply));

    destroyDBus(data);
}

void ude_session_logout::MainView::restart() noexcept
{
    auto data = initDBus();
    if (data.bErrored)
        return;

    data.message = dbus_message_new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Reboot");

    dbus_bool_t bForce = 1;
    dbus_message_append_args(data.message, DBUS_TYPE_BOOLEAN, &bForce, DBUS_TYPE_INVALID);

    dbus_connection_send_with_reply(data.conn, data.message, &data.pending, -1);
    dbus_connection_flush(data.conn);

    dbus_pending_call_block(data.pending);

    data.reply = dbus_pending_call_steal_reply(data.pending);

    if (dbus_message_get_type(data.reply) == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when restarting the system! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, dbus_message_get_error_name(data.reply));
    destroyDBus(data);
}

void ude_session_logout::MainView::suspend() noexcept
{
    auto data = initDBus();
    if (data.bErrored)
        return;

    data.message = dbus_message_new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Suspend");

    dbus_bool_t bForce = 1;
    dbus_message_append_args(data.message, DBUS_TYPE_BOOLEAN, &bForce, DBUS_TYPE_INVALID);

    dbus_connection_send_with_reply(data.conn, data.message, &data.pending, -1);
    dbus_connection_flush(data.conn);

    dbus_pending_call_block(data.pending);

    data.reply = dbus_pending_call_steal_reply(data.pending);

    if (dbus_message_get_type(data.reply) == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when putting the system to sleep! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, dbus_message_get_error_name(data.reply));
    destroyDBus(data);
}


void ude_session_logout::MainView::hibernate() noexcept
{
    auto data = initDBus();
    if (data.bErrored)
        return;

    data.message = dbus_message_new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "Hibernate");

    dbus_bool_t bForce = 1;
    dbus_message_append_args(data.message, DBUS_TYPE_BOOLEAN, &bForce, DBUS_TYPE_INVALID);

    dbus_connection_send_with_reply(data.conn, data.message, &data.pending, -1);
    dbus_connection_flush(data.conn);

    dbus_pending_call_block(data.pending);

    data.reply = dbus_pending_call_steal_reply(data.pending);

    if (dbus_message_get_type(data.reply) == DBUS_MESSAGE_TYPE_ERROR)
        Logger::log("Error when putting the system to sleep! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, dbus_message_get_error_name(data.reply));
    destroyDBus(data);
}

ude_session_logout::MainView::DBusData ude_session_logout::MainView::initDBus() noexcept
{
    DBusData data{};
    dbus_error_init(&data.error);
    data.conn = dbus_bus_get(DBUS_BUS_SYSTEM, &data.error);

    if (dbus_error_is_set(&data.error))
    {
        Logger::log("Error when connecting to the system bus! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, data.error.message);
        dbus_error_free(&data.error);
        data.bErrored = true;
    }
    return data;
}

void ude_session_logout::MainView::destroyDBus(ude_session_logout::MainView::DBusData& data) noexcept
{
    dbus_message_unref(data.reply);
    dbus_pending_call_unref(data.pending);
    dbus_message_unref(data.message);
    dbus_connection_unref(data.conn);
}

std::string ude_session_logout::MainView::getSessionID(ude_session_logout::MainView::DBusData& data) noexcept
{
    std::string sessionID;
    data.message = dbus_message_new_method_call("org.freedesktop.login1", "/org/freedesktop/login1", "org.freedesktop.login1.Manager", "ListSessions");

    dbus_connection_send_with_reply(data.conn, data.message, &data.pending, -1);
    dbus_connection_flush(data.conn);

    dbus_pending_call_block(data.pending);

    data.reply = dbus_pending_call_steal_reply(data.pending);

    if (dbus_message_get_type(data.reply) != DBUS_MESSAGE_TYPE_ERROR)
    {
        DBusMessageIter iter, array_iter;
        dbus_message_iter_init(data.reply, &iter);

        if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_ARRAY)
        {
            dbus_message_iter_recurse(&iter, &array_iter);

            while (dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_STRUCT)
            {
                DBusMessageIter struct_iter;
                dbus_message_iter_recurse(&array_iter, &struct_iter);

                char* id;
                char* user;
                dbus_message_iter_get_basic(&struct_iter, &id);
                dbus_message_iter_next(&struct_iter);
                dbus_message_iter_next(&struct_iter);
                dbus_message_iter_get_basic(&struct_iter, &user);

                //user_name = strdup(user_name);
                std::cout << id << std::endl;
                if (std::string(user) == std::string("i-use-gentoo-btw"))
                {
                    sessionID = id;
                    break;
                }
                dbus_message_iter_next(&array_iter);
            }
        }
    }
    else
        Logger::log("Error getting the session list! Error: ", UVKLog::UVK_LOG_TYPE_ERROR, data.error.message);

    dbus_message_unref(data.reply);
    dbus_pending_call_unref(data.pending);
    dbus_message_unref(data.message);

    return sessionID;
}