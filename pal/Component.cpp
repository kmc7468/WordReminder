#include "Component.hpp"

#include <cassert>
#include <utility>

const Component* Component::GetParent() const noexcept {
	return m_Parent;
}
Component* Component::GetParent() noexcept {
	return m_Parent;
}
Component* Component::AddChild(std::unique_ptr<Component>&& child) {
	assert(child->m_Parent == nullptr);

	child->m_Parent = this;
	if (m_IsCreated) {
		child->CreateComponent(m_Children.size());
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

void Component::CreateComponent(std::size_t index) {
	assert(!m_IsCreated);

	CreateComponent(m_Location, m_Size, m_Text, m_Visibility, index);
	m_IsCreated = true;

	for (std::size_t i = 0; i < m_Children.size(); ++i) {
		if (const auto& child = m_Children[i];
			!child->m_IsCreated) {
			child->CreateComponent(index);
		}
	}
}