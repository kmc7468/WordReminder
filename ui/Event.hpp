#pragma once

class Component;

class Event {
public:
	enum Type {
		Create,
		Close,

		Click,
	};

private:
	Type m_Type;
	Component* m_Receiver;

public:
	Event(Type type, Component* receiver) noexcept;
	Event(const Event&) = delete;
	virtual ~Event() = default;

public:
	Event& operator=(const Event&) = delete;

public:
	Type GetType() const noexcept;
	Component* GetReceiver() noexcept;
};

class EventHandler {
public:
	EventHandler() noexcept = default;
	EventHandler(const EventHandler&) = delete;
	virtual ~EventHandler() = default;

public:
	EventHandler& operator=(const EventHandler&) = delete;

public:
	virtual void OnCreate(Event* event);
	virtual void OnClose(Event* event);
};

class ButtonEventHandler : public EventHandler {
public:
	ButtonEventHandler() noexcept = default;
	ButtonEventHandler(const ButtonEventHandler&) = delete;
	virtual ~ButtonEventHandler() override = default;

public:
	ButtonEventHandler& operator=(const ButtonEventHandler&) = delete;

public:
	virtual void OnClick(Event* event);
};