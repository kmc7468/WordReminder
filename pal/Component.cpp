#include "Component.hpp"

#include <cassert>
#include <utility>

Component::Component(std::unique_ptr<EventHandler>&& eventHandler) noexcept
	: m_EventHandler(std::move(eventHandler)) {
	assert(m_EventHandler != nullptr);
}

const Component* Component::GetParent() const noexcept {
	return m_Parent;
}
Component* Component::GetParent() noexcept {
	return m_Parent;
}
Component* Component::AddChild(std::unique_ptr<Component>&& child) {
	assert(child->m_Parent == nullptr);

	child->m_Parent = this;
	child->SetParentDirect(this, m_Children.size());
	if (m_IsCreated) {
		child->CreateComponent();
	}

	m_Children.push_back(std::move(child));
	return m_Children.back().get();
}
const Component* Component::GetChild(std::size_t index) const noexcept {
	return m_Children[index].get();
}
Component* Component::GetChild(std::size_t index) noexcept {
	return m_Children[index].get();
}
std::size_t Component::GetCountOfChildren() const noexcept {
	return m_Children.size();
}

Point Component::GetLocation() const {
	return m_IsCreated ? GetLocationDirect() : m_Location;
}
void Component::SetLocation(Point newLocation) {
	if (m_IsCreated) {
		SetLocationDirect(newLocation);
	} else {
		m_Location = newLocation;
	}
}
Size Component::GetSize() const {
	return m_IsCreated ? GetSizeDirect() : m_Size;
}
void Component::SetSize(Size newSize) {
	if (m_IsCreated) {
		SetSize(newSize);
	} else {
		m_Size = newSize;
	}
}
std::wstring Component::GetText() const {
	return m_IsCreated ? GetTextDirect() : m_Text;
}
void Component::SetText(const std::wstring& newText) {
	if (m_IsCreated) {
		SetTextDirect(newText);
	} else {
		m_Text = newText;
	}
}
bool Component::GetVisibility() const {
	return m_IsCreated ? GetVisibilityDirect() : m_Visibility;
}
void Component::SetVisibility(bool newVisibility) {
	if (m_IsCreated) {
		SetVisibilityDirect(newVisibility);
	} else {
		m_Visibility = newVisibility;
	}
}

void Component::Show() {
	if (m_IsCreated) {
		SetVisibilityDirect(true);
	} else {
		m_Visibility = true;
		CreateComponent();
	}
}
void Component::Hide() {
	if (m_IsCreated) {
		SetVisibilityDirect(false);
	} else {
		m_Visibility = false;
		CreateComponent();
	}
}

bool Component::IsCreated() const noexcept {
	return m_IsCreated;
}

void Component::RaiseEvent(std::unique_ptr<Event> event) {
	switch (event->GetType()) {
	case Event::Create: m_EventHandler->OnCreate(event.get()); break;
	case Event::Close: m_EventHandler->OnClose(event.get()); break;
	default: assert(false); break;
	}
}

void Component::CreateComponent() {
	assert(!m_IsCreated);

	CreateComponent(m_Location, m_Size, m_Text, m_Visibility);
	m_IsCreated = true;

	for (std::size_t i = 0; i < m_Children.size(); ++i) {
		if (const auto& child = m_Children[i];
			!child->m_IsCreated) {
			child->CreateComponent();
		}
	}
}

Window::Window(Point location, Size size) noexcept {
	SetLocation(location);
	SetSize(size);
}

Size Window::GetMinimumSize() const {
	return IsCreated() ? GetMinimumSizeDirect() : GetMinimumSizeProperty();
}
void Window::SetMinimumSize(Size newMinimumSize) {
	if (IsCreated()) {
		SetMinimumSizeDirect(newMinimumSize);
	} else {
		SetMinimumSizeProperty(newMinimumSize);
	}
}

double Window::GetDisplayScale() const {
	assert(IsCreated());

	return GetDisplayScaleDirect();
}

Size Window::GetMinimumSizeProperty() const noexcept {
	return m_MinimumSize;
}
void Window::SetMinimumSizeProperty(Size newMinimumSize) noexcept {
	m_MinimumSize = newMinimumSize;
}

Button::Button() noexcept {}

void Button::RaiseEvent(std::unique_ptr<Event> event) {
	switch (event->GetType()) {
	case Event::Click: GetEventHandler<ButtonEventHandler>()->OnClick(event.get()); break;
	default: Component::RaiseEvent(std::move(event));
	}
}

Label::Label() noexcept {}