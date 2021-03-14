#ifndef _ENUM_STRINGS_H
#define _ENUM_STRINGS_H

#include <string>
#include <A2DPSession.h>

std::string enumToString(A2DPSession::ConnectionState e)
{
  switch (e)
  {
  case A2DPSession::ConnectionState::DISCONNECTED:
    return "Disconnected";
  case A2DPSession::ConnectionState::CONNECTING:
    return "Connecting";
  case A2DPSession::ConnectionState::CONNECTED:
    return "Connected";
  }
}

std::string enumToString(A2DPSession::MediaState e)
{
  switch (e)
  {
  case A2DPSession::MediaState::INACTIVE:
    return "Inactive";
  case A2DPSession::MediaState::ACTIVE:
    return "Active";
  }
}

// enum ConnectionState
// {
//   DISCONNECTED,
//   CONNECTING,
//   CONNECTED
// };

// enum MediaState
// {
//   INACTIVE,
//   ACTIVE
// };

#endif
