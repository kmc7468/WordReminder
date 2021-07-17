#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

struct Point final {
	int X, Y;
};

struct Size final {
	int Width, Height;
};

class Component {
private:
	Component* m_Parent = nullptr;
	std::vector<std::unique_ptr<Component>> m_Children;

	bool m_IsCreated = false;
	Point m_Location{ 0, 0 };
	Size m_Size{ 100, 50 };
	std::wstring m_Text;
	bool m_Visibility = true;

public:
	Component() noexcept = default;
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

protected:
	virtual Point GetLocationDirect() const = 0;
	virtual void SetLocationDirect(Point newLocation) = 0;
	virtual Size GetSizeDirect() const = 0;
	virtual void SetSizeDirect(Size newSize) = 0;
	virtual std::wstring GetTextDirect() const = 0;
	virtual void SetTextDirect(const std::wstring& newText) = 0;
	virtual bool GetVisibilityDirect() const = 0;
	virtual void SetVisibilityDirect(bool newVisibility) = 0;

	virtual void CreateComponent(Point location, Size size, const std::wstring& text, bool visibility, std::size_t index) = 0;

private:
	void CreateComponent(std::size_t index);
};

class Window : public virtual Component {
	// TODO
};