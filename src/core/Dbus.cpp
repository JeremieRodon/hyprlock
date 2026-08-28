#include "Dbus.hpp"
#include "../helpers/Log.hpp"

CDbus::CDbus(bool enable) {
    if (!enable)
        return;
    try {
        m_connection        = sdbus::createSystemBusConnection();
        m_freedesktopLogin1 = sdbus::createProxy(*m_connection, sdbus::ServiceName{"org.freedesktop.login1"}, sdbus::ObjectPath{"/org/freedesktop/login1"});
        m_freedesktopLogin1Session = sdbus::createProxy(*m_connection, sdbus::ServiceName{"org.freedesktop.login1"},
                                                        sdbus::ObjectPath{"/org/freedesktop/login1/session/auto"});
    } catch (sdbus::Error& e) {
        Log::logger->log(Log::ERR, "dbus: failed to create connection or login proxy ({})", e.what());
        m_freedesktopLogin1Session.reset();
        m_freedesktopLogin1.reset();
        m_connection.reset();
    }
}

void CDbus::setLockedHint(bool locked) {
    if (!m_connection || !m_freedesktopLogin1Session) {
        Log::logger->log(Log::WARN, "dbus: SetLockedHint({}) skipped — no DBus connection", locked);
        return;
    }

    try {
        m_freedesktopLogin1Session->callMethod("SetLockedHint").onInterface(sdbus::InterfaceName{"org.freedesktop.login1.Session"}).withArguments(locked);
        Log::logger->log(Log::INFO, "dbus: SetLockedHint({})", locked);
    } catch (sdbus::Error& e) {
        Log::logger->log(Log::ERR, "dbus: SetLockedHint failed ({})", e.what());
    }
}
