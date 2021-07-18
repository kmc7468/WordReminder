#include "Event.hpp"

Event::Event(Type type, Component* receiver) noexcept
	: m_Type(type), m_Receiver(receiver) {}

Event::Type Event::GetType() const noexcept {
	return m_Type;
}
Component* Event::GetReceiver() noexcept {
	return m_Receiver;
}

void EventHandler::OnCreate(Event*) {}
void EventHandler::OnClose(Event*) {}

void ButtonEventHandler::OnClick(Event*) {}