#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

struct Point final {
	int X, Y;
};

struct Size final {
	int Width, Height;
};

class EventHandler;

class Component {
private:
	std::unique_ptr<EventHandler> m_EventHandler;
	Component* m_Parent = nullptr;
	std::vector<std::unique_ptr<Component>> m_Children;

	bool m_IsCreated = false;
	Point m_Location{ 0, 0 };
	Size m_Size{ 100, 50 };
	std::wstring m_Text;
	bool m_Visibility = true;

public:
	Component(std::unique_ptr<EventHandler>&& eventHandler) noexcept;
	Component(const Component&) = delete;
	virtual ~Component() = default;

public:
	Component& operator=(const Component&) = delete;

public:
	const Component* GetParent() const noexcept;
	Component* GetParent() noexcept;
	Component* AddChild(std::unique_ptr<Component>&& child);
	const Component* GetChild(std::size_t index) const noexcept;
	Component* GetChild(std::size_t index) noexcept;
	std::size_t GetCountOfChildren() const noexcept;

	Point GetLocation() const;
	void SetLocation(Point newLocation);
	Size GetSize() const;
	void SetSize(Size newSize);
	std::wstring GetText() const;
	void SetText(const std::wstring& newText);
	bool GetVisibility() const;
	void SetVisibility(bool newVisibility);

	void Show();
	void Hide();

protected:
	virtual void SetParentDirect(Component* parent, std::size_t index) = 0;

	virtual Point GetLocationDirect() const = 0;
	virtual void SetLocationDirect(Point newLocation) = 0;
	virtual Size GetSizeDirect() const = 0;
	virtual void SetSizeDirect(Size newSize) = 0;
	virtual std::wstring GetTextDirect() const = 0;
	virtual void SetTextDirect(const std::wstring& newText) = 0;
	virtual bool GetVisibilityDirect() const = 0;
	virtual void SetVisibilityDirect(bool newVisibility) = 0;

	virtual void CreateComponent(Point location, Size size, const std::wstring& text, bool visibility) = 0;

private:
	void CreateComponent();

protected:
	void OnCreate();
	void OnClose();
};

class Window : public virtual Component {
public:
	Window(Point location, Size size) noexcept;
	Window(const Window&) = delete;
	virtual ~Window() override = default;

public:
	Window& operator=(const Window&) = delete;

public:
	static std::unique_ptr<Window> Create(std::unique_ptr<EventHandler>&& eventHandler, int width, int height);
};

class EventHandler {
public:
	EventHandler() noexcept = default;
	EventHandler(const EventHandler&) = delete;
	virtual ~EventHandler() = default;

public:
	EventHandler& operator=(const EventHandler&) = delete;

public:
	virtual void OnCreate(Component* component);
	virtual void OnClose(Component* component);
};